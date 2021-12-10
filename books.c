//
// Created by Cherish on 2021/12/9.
//

#include "books.h"

/**
 * 向文件输入新增加的分类信息，写入文件
 * @param tree 分类信息指针
 */
void AddCatalog(PCatalogTree tree) {
    /**
     * 写入程序运行数据首先
     */
    Catalog log;
    printf("输入分类编号(以#结束):");
    scanf("%s #", log.Key);
    printf("输入类名:");
    scanf("%s", log.Caption);
    printf("输入父类名:");
    scanf("%s", log.Parent);
    tree->node[tree->num++] = log;
    /**
     * 写入文件
     */
    FILE *fp = fopen(BOOK_TREE_FILE, "ab+"); // 直接追加
    /**
     * 使用定位方式
     * FILE *fp = fopen(BOOK_TREE_FILE, "wb+");
     * fseek(fp, 0, SEEK_END);
     */
    fwrite(log.Key, 20, 1, fp);
    fwrite(log.Caption, 80, 1, fp);
    fwrite(log.Parent, 20, 1, fp);
    fclose(fp);
}

/**
 * 定位到某一位置，然后修改需要修改的记录
 */
void AlterCatalog() {
    printf("请输入要修改的条目信息（类别或者编号):");
    char searchArea[80];
    scanf("%s", searchArea);
    /**
     * 通过文件直接查找
     */
    FILE *fp = fopen(BOOK_TREE_FILE, "rb+");
    fseek(fp, 0L, SEEK_SET);
    int flag = 0;
    while (!feof(fp) && !flag) {
        char Key[20];      //编号
        char Caption[80];  //标题，类名
        fread(Key, sizeof(char) * 20, 1, fp);
        fread(Caption, sizeof(char) * 80, 1, fp);
        flag = strncmp(Key, searchArea, 20) == 0 || strncmp(Caption, searchArea, 80) == 0;
        fread(Key, sizeof(char) * 20, 1, fp);
    }
    // 找到了查询字段
    if (flag == 1) {
        fseek(fp, -120L, SEEK_CUR); // 回溯120个字节
        Catalog log;
        printf("输入分类编号(以#结束):");
        scanf("%s #", log.Key);
        printf("输入类名:");
        scanf("%s", log.Caption);
        printf("输入父类名:");
        scanf("%s", log.Parent);
        fwrite(log.Key, 20, 1, fp);
        fwrite(log.Caption, 80, 1, fp);
        fwrite(log.Parent, 20, 1, fp);
    } else
        printf("未查找到该类\n");
    fclose(fp);
}


/**
 * 删除一个目录条目
 */
void DeleteCatalog() {
    printf("请输入分类编号");
    char Key[20];
    scanf("%s", Key);
    FILE *fp = fopen(BOOK_TREE_FILE, "rb+");
    // 得到文件尾部位置，下面要用
    fseek(fp, 0L, SEEK_END);
    long end = ftell(fp);
    // 重定位到开头
    fseek(fp, 0L, SEEK_SET);
    int flag = 0;
    /**遍历文件查找分类编号对应部分*/
    while (!feof(fp) && !flag) {
        char Key1[20];      //编号
        char Caption[80];  //标题，类名
        fread(Key1, sizeof(char) * 20, 1, fp);
        fread(Caption, sizeof(char) * 80, 1, fp);
        flag = strncmp(Key1, Key, 20) == 0;
        fread(Key1, sizeof(char) * 20, 1, fp);
    }
    /** 找到了查询字段*/
    if (flag == 1) {
        long now = ftell(fp); // 得到当前有效数据位置
        /**当前字段为文件末尾，需要重写整个文件覆盖*/
        if (now == end) {
            char s[10000];
            fseek(fp, -120L, SEEK_CUR);
            int r = ftell(fp);
            fseek(fp, 0L, SEEK_SET);
            fread(s, r, 1, fp);
            fclose(fp);
            fp = fopen(BOOK_TREE_FILE, "wb");
            fwrite(s, r, 1, fp);
            fclose(fp);
            return;
        }
        /**当前字段不为文件末尾，将后面的有效字节前移*/
        char temp[end-now+1];
        fread(temp, end-now+1, 1, fp); // 读出剩余所有字段
        fseek(fp, -(end-now+120), SEEK_END); // 返回到查找字段部分
        fwrite(temp, end-now+1, 1, fp); // 写回
    } else
        printf("未查找到该类\n");
    fclose(fp);
}


/**
 * 先序遍历目录树
 * @param tree 树的指针
 * @param i 索引为i的节点
 * @param level 遍历的层次
 */
void PreOrder (PCatalogTree tree, int i, int level) {
    /**
     * 在数组中逐个比对找到结点i的所有子结点
     */
    for (int j = 0; j < level; ++j)
        printf("%c", '\t');
    Catalog log = tree->node[i];
    printf("|-----%s %s\n", log.Caption, log.Key);
    for (int j = i + 1; j < tree->num; j++)
    {
        if (strncmp(tree->node[j].Parent, tree->node[i].Key, 20) == 0)
            PreOrder(tree, j, level+1);        //探索该子结点的子结点
    }
}


/**
 * 从文件中初始化目录树
 * @param tree pointer to Catalog Tree
 */
void initCatalogTree(PCatalogTree tree) {
    FILE *fp = fopen(BOOK_TREE_FILE, "rb");
    tree->num = 0;
    int i = 0;
    do {
        PCatalog temp = &tree->node[i++];
        initNode(temp, fp);
        tree->num++;
        if (strncmp(temp->Parent, "-1", 3) == 0)
            tree->root = i-1;
    } while (!feof(fp));
    tree->num--; // 因为feof会延后，所以长度减一
    fclose(fp);
}

/**
 * 初始化一个类型节点
 * @param log 需要初始化化的类型节点指针
 * @param fp 文件流
 */
void initNode(PCatalog log, FILE *fp) {
    char Key[20];      //编号
    char Caption[80];  //标题，类名
    char Parent[20];   //父类编号
    fread(Key, sizeof(char) * 20, 1, fp);
    fread(Caption, sizeof(char) * 80, 1, fp);
    fread(Parent, sizeof(char) * 20, 1, fp);
    strncpy(log->Key, Key, 20);
    strncpy(log->Caption, Caption, 80);
    strncpy(log->Parent, Parent, 20);
}

/**
 * 顺序打印文件中的所有条目
 */
void PrintCatalog() {
    FILE *fp = fopen(BOOK_TREE_FILE, "rb");
    while (1) {
        char Key[20];      //编号
        char Caption[80];  //标题，类名
        char Parent[20];   //父类编号
        /**当读取到文件尾部时fread返回-1，跳出循环*/
        unsigned int flag = fread(Key, sizeof(char) * 20, 1, fp);
        if (flag == 0)
            break;
        fread(Caption, sizeof(char) * 80, 1, fp);
        fread(Parent, sizeof(char) * 20, 1, fp);
        printf("%s %s %s\n", Key, Caption, Parent);
    }
}

/**
 * 本函数将文件中的记录，以双亲表示法的形式存入结构体数组C.Node[i]中，为下一步输出树形目录结构做准备，接受需要
 * tree.num为0才能进行递归，而且对于fp为空的情况也可以正常运行
 * @param tree 目录树 num字段需要为0
 * @param i 当前索引指针
 * @param fp 文件指针，为空时自动读取catalog.dat文件
 */
FILE* CreatCatalogTree(PCatalogTree tree, int *i, FILE *fp) {
    Catalog log = tree->node[(*i) - 1];
    if (fp == NULL)
        fp = fopen(BOOK_TREE_FILE, "rb");
    rewind(fp); // 重定向到文件头
    do {
        Catalog node;
        unsigned int ui = fread(node.Key, sizeof(char) * 20, 1, fp);
        if (ui == 0)
            break;
        fread(node.Caption, sizeof(char) * 80, 1, fp);
        fread(node.Parent, sizeof(char) * 20, 1, fp);
        /**首次使用，找到祖先节点然后开始递归*/
        if (tree->num == 0) {
            if (strncmp("-1", node.Parent, 20) == 0){
                tree->node[(*i)++] = node;
                tree->num++;
                tree->root = 0;
                CreatCatalogTree(tree, i, fp);
            } else
                continue;
        }
        // 循环比较找到儿子节点
        if (strncmp(node.Parent, log.Key, 20) == 0) {
            tree->node[(*i)++] =  node;
            tree->num++;
            long loc = ftell(fp);
            CreatCatalogTree(tree, i, fp);
            fseek(fp, loc, SEEK_SET);
        }
    } while (!feof(fp));
    return fp;
}

/**
 * 打印目录树，基于排序完好的数组，使用CreateCatalogTree创建的列表才可以，
 * 它的儿子节点的序号总是紧跟父亲节点！
 * @param tree 目录树的指针
 * @param i 遍历会以递归进行，传递当前遍历到得索引i
 * @param level 当前节点的层次
 */
void PrintCatalogTree(PCatalogTree tree, int *i, int level) {
    for (int j = 0; j < level; ++j)
        printf("\t");
    printf("|-----%s %s\n", tree->node[*i].Key, tree->node[*i].Caption);
    int now = *i;
    while (*i < tree->num){
        if (strncmp(tree->node[*i + 1].Parent, tree->node[now].Key, 20) == 0) {
            (*i)++;
            PrintCatalogTree(tree, i, level + 1);
        } else
            return;
    }
}

/**
 * 菜单函数
 */
void bookMenu() {
    while (1) {
        system("cls");
        char starts[] = "**********************************";
        printf("%20s%-s\n", "", starts);
        printf("%30s%-s\n", "", "分类管理菜单");
        printf("%20s%20s\n", "*", starts);
        printf("%20s%10s%-20s%5s\n", "*", "", "1.增加分类", "*");
        printf("%20s%10s%-20s%5s\n", "*", "", "2.输出分类", "*");
        printf("%20s%10s%-20s%5s\n", "*", "", "3.输出树形分类树", "*");
        printf("%20s%10s%-20s%5s\n", "*", "", "4.修改分类", "*");
        printf("%20s%10s%-20s%5s\n", "*", "", "5.删除分类", "*");
        printf("%20s%10s%-20s%5s\n", "*", "", "0.返回上级", "*");
        printf("%20s%20s\n", "*", starts);
        printf("请输入你的选择！0-5：");
        int choice;
        CatalogTree tree;
        initCatalogTree(&tree);
        scanf("%d", &choice);
        switch (choice) {
            case 0:
                exit(0);
            case 1:
                AddCatalog(&tree);
                break;
            case 2:
                PrintCatalog();
                break;
            case 3: {
                /** CreateCatalogTree基于空目录树，所以置tree.num=0, 然后返回函数中创建的文件指针，需要释放*/
                tree.num = 0;
                int i = 0, j = 0;
                FILE *fp = CreatCatalogTree(&tree, &i, NULL);
                tree.num--;
                fclose(fp);
                PrintCatalogTree(&tree, &j, 0);
                break;
            }
            case 4:
                AlterCatalog();
                break;
            case 5:
                DeleteCatalog();
                break;
        }
    }
}