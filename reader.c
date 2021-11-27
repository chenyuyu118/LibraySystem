//
// Created by Cherish on 2021/11/26.
//

#include "reader.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <process.h>


void appendLinkList(RecordNode *head, ReaderLink *reader) {
    // 插入到表尾部
    if (head->node == NULL)
        head->node = reader;
    else
        head->pre->next = reader;
    head->pre = reader;
}

void insertLinkList(ReaderLink *node, ReaderLink *reader) {
    // 插入到特定部位
    reader->next = node->next;
    node->next = reader;
}

void deleteLinkList(RecordNode *head, RecordNode *node) {
    // 删除一个节点
    if (node->node == head->node)
        // 删除头结点
        head->node = node->node->next;
    else
        node->pre->next = node->node->next;
    free(node->node); // 释放空间
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
            printf("%-10s %-10s %-10s\n", "q退出", "n下一页", "p前一页");
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
    // 简单的全部写回
    SaveReader(head);
}

RecordNode *FindReader_Code(RecordNode* head,char * key) {
    Key key1;
    key1.s = key;
    RecordNode *node = getReader(head, key1, CODE);
    if (!node) {
        printf("未寻找到！\n");
        return NULL;
    } else
        return node;
}

RecordNode *FindReader_Name(RecordNode *head, char * key) {
    Key key1;
    key1.s = key;
    RecordNode *node = getReader(head, key1, NAME);
    if (!node) {
        printf("未寻找到！\n");
        return NULL;
    } else
        return node;
}

int browserMenu(RecordNode *head) {
    printf("%-4s %-10s %-10s %-4s %-15s %-4s\n", "序号", "证件号", "姓名", "性别", "学院", "标签");
    BrowserReader(head);
    return 1;
}

int lostMenu(RecordNode *head) {
    printf("请输入挂失的证件号:\n");
    char s[100];
    scanf("%s", s);
    Key key;
    key.s = s;
    RecordNode* node = getReader(head, key, CODE);
    if (node == NULL) {
        printf("未找到该证件号!输入q退出 c继续\n");
        char c;
        scanf(" %c", &c);
        if (c == 'q')
            return -1;
        while (c != 'c') scanf("%c", &c);
    } else {
        LostReader(node->node);
        printf("挂失成功!输入任意键退出\n");
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
    // printf("请输入新添加读者" "证件号" "姓名" "性别" "学院(中间用空格隔开)\n");
    while (1) {
        printf("输入证件号，姓名，性别，工作单位和状态（中间以空格隔开）：\n");
        InputReader(head);
        SaveReader(head);
        printf("退出请输入q，继续请输入c\n");
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
        printf("请输入查询的字段:0.名称 1.证件号\n");
        int choice;
        scanf("%d", &choice);
        RecordNode *(*fun)(RecordNode *, char*) = choice ? FindReader_Code : FindReader_Name;
        char s[100];
        scanf("%s", s);
        RecordNode *node = fun(head, s);
        if (node == NULL) {
            printf("未找到对象!输入q退出 c继续\n");
            char c;
            scanf(" %c", &c);
            if (c == 'q')
                return -1;
            while (c != 'c') scanf("%c", &c);
        } else {
            ReaderLink *result = node->node;
            printf("%-10s %-10s %-4s %-15s %-4s\n", "证件号", "姓名", "性别", "学院", "标签");
            printf("%-10s %-10s %-4s %-15s %-4d\n", result->Code, result->Name, result->Sex, result->Dept, result->Tag);
            printf("输入q退出 c继续\n");
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
        printf("请输入要修改读者的证件号\n");
        char s[100];
        scanf("%s", s);
        Key key;
        key.s = s;
        RecordNode *node = getReader(head, key, CODE);
        if (node == NULL) {
            printf("没有查找到读者信息！请重试（退出输入q）\n");
            char c;
            scanf(" %c", &c);
            if (c == 'q')
                return -1;
        } else {
            printf("请输入要修改的部分:\n");
            printf("0.证件号 1.姓名 2.学院\n");
            int choice;
            scanf("%d" , &choice);
            enum Field f = choice ? (choice-1 ? DEPT : NAME): CODE;
            Key value = getPart(node->node, f);
            Key newValue;
            int temp;
            char temp1[100];
            if (!f) {
                printf("当前信息：%d请输入修改后的字段\n", *value.tag);
                scanf("%d" , &temp);
                newValue.tag = &temp;
            }
            else {
                printf("当前信息：%s;请输入修改后的字段\n", value.s);
                scanf("%s", temp1);
                newValue.s = temp1;
            }
            ChangeReader(node->node, newValue, f);
            // 写回
            AlterReader(head, node->node);
            printf("继续修改 c 退出 q\n");
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
        printf("                                                             读者管理菜单\n");
        printf("                                         **********************************************\n");
        printf("                                         *                   1.浏览所有读者            *\n");
        printf("                                         *                   2.增加读者                *\n");
        printf("                                         *                   3.查询读者                *\n");
        printf("                                         *                   4.修改读者                *\n");
        printf("                                         *                   5.证件挂失                *\n");
        printf("                                         *                   0.返回上级                *\n");
        printf("                                         ***********************************************\n");
        printf("请输入你的选择！0~5\n");
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