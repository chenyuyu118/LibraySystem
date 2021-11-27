//
// Created by Cherish on 2021/11/26.
//

#include "reader.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <process.h>


void appendLinkList(RecordNode *head, ReaderLink *reader) {
    // ���뵽��β��
    if (head->node == NULL)
        head->node = reader;
    else
        head->pre->next = reader;
    head->pre = reader;
}

void insertLinkList(ReaderLink *node, ReaderLink *reader) {
    // ���뵽�ض���λ
    reader->next = node->next;
    node->next = reader;
}

void deleteLinkList(RecordNode *head, RecordNode *node) {
    // ɾ��һ���ڵ�
    if (node->node == head->node)
        // ɾ��ͷ���
        head->node = node->node->next;
    else
        node->pre->next = node->node->next;
    free(node->node); // �ͷſռ�
}

RecordNode *getReader(RecordNode *head,Key key, enum Field f) {
    ReaderLink *current = head->node;
    ReaderLink *pre = head->pre;
    short flag = 0;
    while (current) {
        char *cmpPart = f+1 ? current->Name : current->Code;
        if (!strcmp(cmpPart, key.s)) {
            RecordNode *node = (RecordNode *) malloc(sizeof(RecordNode));
            node->node = current;
            node->pre = pre;
            return node;
        }
        pre = current;
        current = current->next;
    }
    return NULL;
}

void ChangeReader(ReaderLink *changedReader, Key key, enum Field f) {
    void *realKey = f ? key.s : key.tag;
    switch (f) {
        case CODE:
        case NAME:
        case DEPT: {
            char *filed = f + 1 ? (f - 1 ? changedReader->Name : changedReader->Dept) : changedReader->Code;
            size_t len = strlen((char*)realKey);
            strncpy(filed, (char *)realKey, len+1);
            break;
        }
        case TAG:
            changedReader->Tag = *(int *)realKey;
            break;
    }
}

int InitNode(ReaderLink* newNode, FILE *fp) {
    return fscanf(fp, "%s %s %s %s %d", newNode->Code, newNode->Name, newNode->Sex, newNode->Dept, &newNode->Tag);
}

Key getPart(ReaderLink* node, enum Field f) {
    Key key;
    void *value = f ? (void *)(f+1 ? (f-1 ? node->Name: node->Dept): node->Code) : (void *)&node->Tag;
    if (!f)
        key.tag = (int *)value;
    else
        key.s = (char *)value;
    return key;
}

void InputReader(RecordNode *head) {
    ReaderLink *newNode = (ReaderLink *)malloc(sizeof (ReaderLink));
    InitNode(newNode, stdin);
    appendLinkList(head, newNode);
}

void SaveReader(RecordNode *head) {
    ReaderLink *current = head->node;
    FILE *fp = fopen(DATA_PATH, "w");
    if (!fp) {
        exit(-1);
    }
    while (current) {
        fprintf(fp, "%s %s %s %s %d\n", current->Code, current->Name, current->Sex, current->Dept, current->Tag);
        current = current->next;
    }
    fclose(fp);
}

void BrowserReader(RecordNode *head) {
    ReaderLink *current = head->node;
    ReaderLink *history[10] = {head->node};
    int count = 0, count1 = 1;
    while (current) {
        fprintf(stdout, "%-4d %-10s %-10s %-4s %-15s %-4d\n", count++, current->Code, current->Name, current->Sex, current->Dept, current->Tag);
        if (count % 10 == 0) {
            printf("%-10s %-10s %-10s\n", "q�˳�", "n��һҳ", "pǰһҳ");
            char c;
            scanf("%c", &c);
            while (c != 'q') {
                if (c == 'n') {
                    history[count1++] = current->next;
                    current  = current->next;
                    break;
                }
                else if (c == 'p' && count1 != 1) {
                    current = history[count1-- - 2];
                    count -= 10;
                    break;
                } else if (c == 'p') {
                    current = history[0];
                    count -= 10;
                    break;
                }
                scanf("%c", &c);
            }
            if (c == 'q')
                return;
        }
        else
            current = current->next;
    }
}

RecordNode *CreatReaderLink() {
    RecordNode *head = (RecordNode *) malloc(sizeof (RecordNode));
    head->node = NULL;
    FILE *fp = fopen(DATA_PATH, "r");
    while (1) {
        ReaderLink *node = (ReaderLink *) malloc(sizeof (ReaderLink));
        if (InitNode(node, fp) == EOF) {
            free(node);
            break;
        }
        node->next = NULL;
        appendLinkList(head, node);
    }
    fclose(fp);
    return head;
}

void LostReader(ReaderLink *reader) {
    Key k;
    int a = 1;
    k.tag = &a;
    ChangeReader(reader, k, TAG);
}

void AlterReader(RecordNode *head, ReaderLink *node) {
    // �򵥵�ȫ��д��
    SaveReader(head);
}

RecordNode *FindReader_Code(RecordNode* head,char * key) {
    Key key1;
    key1.s = key;
    RecordNode *node = getReader(head, key1, CODE);
    if (!node) {
        printf("δѰ�ҵ���\n");
        return NULL;
    } else
        return node;
}

RecordNode *FindReader_Name(RecordNode *head, char * key) {
    Key key1;
    key1.s = key;
    RecordNode *node = getReader(head, key1, NAME);
    if (!node) {
        printf("δѰ�ҵ���\n");
        return NULL;
    } else
        return node;
}

int browserMenu(RecordNode *head) {
    printf("%-4s %-10s %-10s %-4s %-15s %-4s\n", "���", "֤����", "����", "�Ա�", "ѧԺ", "��ǩ");
    BrowserReader(head);
    return 1;
}

int lostMenu(RecordNode *head) {
    printf("�������ʧ��֤����:\n");
    char s[100];
    scanf("%s", s);
    Key key;
    key.s = s;
    RecordNode* node = getReader(head, key, CODE);
    if (node == NULL) {
        printf("δ�ҵ���֤����!����q�˳� c����\n");
        char c;
        scanf(" %c", &c);
        if (c == 'q')
            return -1;
        while (c != 'c') scanf("%c", &c);
    } else {
        LostReader(node->node);
        printf("��ʧ�ɹ�!����������˳�\n");
        SaveReader(head);
        getchar();
        if (getchar()) putchar('\n');
        return 1;
    }
}

int returnMenu(RecordNode *head) {
    return -100;
}

int addMenu(RecordNode *head) {
    // printf("����������Ӷ���" "֤����" "����" "�Ա�" "ѧԺ(�м��ÿո����)\n");
    while (1) {
        printf("����֤���ţ��������Ա𣬹�����λ��״̬���м��Կո��������\n");
        InputReader(head);
        SaveReader(head);
        printf("�˳�������q������������c\n");
        char c;
        scanf(" %c", &c);
        if (c == 'q')
            return -1;
        while (c != 'c')
            scanf("%c", &c);
    }
    return 1;
}

int searchMenu(RecordNode *head) {
    while (1) {
        printf("�������ѯ���ֶ�:0.���� 1.֤����\n");
        int choice;
        scanf("%d", &choice);
        RecordNode *(*fun)(RecordNode *, char*) = choice ? FindReader_Code : FindReader_Name;
        char s[100];
        scanf("%s", s);
        RecordNode *node = fun(head, s);
        if (node == NULL) {
            printf("δ�ҵ�����!����q�˳� c����\n");
            char c;
            scanf(" %c", &c);
            if (c == 'q')
                return -1;
            while (c != 'c') scanf("%c", &c);
        } else {
            ReaderLink *result = node->node;
            printf("%-10s %-10s %-4s %-15s %-4s\n", "֤����", "����", "�Ա�", "ѧԺ", "��ǩ");
            printf("%-10s %-10s %-4s %-15s %-4d\n", result->Code, result->Name, result->Sex, result->Dept, result->Tag);
            printf("����q�˳� c����\n");
            free(node);
            getchar();
            int c = getchar();
            if (c == 'q')
                break;
            while (c != 'c') c = getchar();
        }
    }
}

int alterMenu(RecordNode *head) {
    while (1) {
        printf("������Ҫ�޸Ķ��ߵ�֤����\n");
        char s[100];
        scanf("%s", s);
        Key key;
        key.s = s;
        RecordNode *node = getReader(head, key, CODE);
        if (node == NULL) {
            printf("û�в��ҵ�������Ϣ�������ԣ��˳�����q��\n");
            char c;
            scanf(" %c", &c);
            if (c == 'q')
                return -1;
        } else {
            printf("������Ҫ�޸ĵĲ���:\n");
            printf("0.֤���� 1.���� 2.ѧԺ\n");
            int choice;
            scanf("%d" , &choice);
            enum Field f = choice ? (choice-1 ? DEPT : NAME): CODE;
            Key value = getPart(node->node, f);
            Key newValue;
            int temp;
            char temp1[100];
            if (!f) {
                printf("��ǰ��Ϣ��%d�������޸ĺ���ֶ�\n", *value.tag);
                scanf("%d" , &temp);
                newValue.tag = &temp;
            }
            else {
                printf("��ǰ��Ϣ��%s;�������޸ĺ���ֶ�\n", value.s);
                scanf("%s", temp1);
                newValue.s = temp1;
            }
            ChangeReader(node->node, newValue, f);
            // д��
            AlterReader(head, node->node);
            printf("�����޸� c �˳� q\n");
            char c;
            scanf(" %c", &c);
            if (c == 'q')
                break;
            while (c != 'c') scanf(" %c", &c);
        }
    }
    return 1;
}

void readerMenu() {
    int (*menus [])(RecordNode *) = {
            returnMenu,
            browserMenu,
            addMenu,
            searchMenu,
            alterMenu,
            lostMenu
    };
    int choice, state = 1;
    RecordNode *head;
    head = CreatReaderLink();
    while (state != -100) {
        printf("                                         **********************************************\n");
        printf("                                                             ���߹���˵�\n");
        printf("                                         **********************************************\n");
        printf("                                         *                   1.������ж���            *\n");
        printf("                                         *                   2.���Ӷ���                *\n");
        printf("                                         *                   3.��ѯ����                *\n");
        printf("                                         *                   4.�޸Ķ���                *\n");
        printf("                                         *                   5.֤����ʧ                *\n");
        printf("                                         *                   0.�����ϼ�                *\n");
        printf("                                         ***********************************************\n");
        printf("���������ѡ��0~5\n");
        scanf("%d", &choice);
        state = menus[choice](head);
        system("cls");
    }
    ReaderLink *current = head->node;
    while (current) {
        ReaderLink *temp = current;
        current = current->next;
        free(temp);
    }
    free(head);
}