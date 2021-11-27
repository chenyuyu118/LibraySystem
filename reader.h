//
// Created by Cherish on 2021/11/26.
//

#ifndef LIBRARYSYSTEM_READER_H
#define LIBRARYSYSTEM_READER_H
#include <stdio.h>

#define DATA_PATH "reader1.txt"


typedef struct ReaderLink
{ char Code[20];  //证件号
    char Name[30];  //姓名
    char Sex[5];    //性别
    char Dept[50];  //工作单位
    int  Tag;       //证件状态：0-正常，1-挂失
    struct ReaderLink *next;
}ReaderLink;

typedef struct {
    ReaderLink *pre;
    ReaderLink *node;
} RecordNode;

enum Field {
    NAME = -2, CODE = -1, TAG = 0, DEPT = 1
};

typedef union {
    char *s;
    int *tag;
}Key;

void InputReader(RecordNode *head); // 录入读者信息，建立读者信息链表
void SaveReader(RecordNode *head); // 将读者信息保存到reader.txt

RecordNode *FindReader_Code(RecordNode *head, char * key); // 通过证件号查找读者
RecordNode *FindReader_Name(RecordNode *head, char * key); // 通过名称查找读者

RecordNode *CreatReaderLink(); // 通过文件读取读者信息创建链表
void AlterReader(RecordNode *,ReaderLink *); // 修改读者信息
void SaveReaderW(); // 读者信息写入文件，删除旧信息

void BrowserReader(RecordNode *head); // 浏览全部读者信息
void LostReader(ReaderLink *); // 读者证件挂失


// 辅助函数
void appendLinkList(RecordNode *head, ReaderLink *reader);
void insertLinkList(ReaderLink *node, ReaderLink *reader);
void deleteLinkList(RecordNode *head, RecordNode *node);
RecordNode *getReader(RecordNode *head,Key key, enum Field f);
void ChangeReader(ReaderLink *changedReader, Key key, enum Field f);
int InitNode(ReaderLink* newNode, FILE *);
Key getPart(ReaderLink* node, enum Field);

void readerMenu();
#endif //LIBRARYSYSTEM_READER_H
