#ifndef LIBRARYSYSTEM_BOOKS_H
#define LIBRARYSYSTEM_BOOKS_H

#include <stdio.h>
#include <string.h>
#include <windows.h>

#define MAX_NODE 64  //最多结点个数
#define MAX_TREE_SIZE 100
#define BOOK_TREE_FILE "catalog3.dat"

typedef struct Catalog    //目录结构类型
{
    char Key[20];      //编号
    char Caption[80];  //标题，类名
    char Parent[20];   //父类编号
} Catalog, *PCatalog;

typedef struct CatalogTree    //目录顺序存储结构类型
{
    Catalog node[MAX_TREE_SIZE];
    int root;   //根节点位置，根节点默认在0号单元；
    int num;    //最后一个结点的数组编号
} CatalogTree, *PCatalogTree;



void AddCatalog(PCatalogTree tree); // 增加新的分类，保存到文件
void AlterCatalog(); // 修改分类信息，保存到文件
void DeleteCatalog(); // 从文件中删除分类信息
void PreOrder (CatalogTree *tree, int i, int level); // 递归输出分类树
void PrintCatalog(); // 调用preOrder函数
FILE* CreatCatalogTree(PCatalogTree tree, int *i, FILE *fp); // 将分类信息用双亲表示法存储，调用printCatalogTree
void PrintCatalogTree(PCatalogTree tree, int *i, int level);

void initCatalogTree(PCatalogTree); // 初始化目录树
void initNode(PCatalog log, FILE *fp); // 初始化一个节点

void bookMenu();

#endif //LIBRARYSYSTEM_BOOKS_H
