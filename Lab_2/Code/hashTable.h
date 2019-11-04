#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct DecLine_* DecLine;
typedef struct Func_* Func;
typedef struct Info_* Info;
// Node组成List List组成Table
typedef struct HashNode_* HashNode;
typedef struct TableNode_* TableNode;

typedef enum {
    BASIC, ARRAY, STRUCTURE
} TypeKind;

typedef enum {
    FUNC, VARI
} NodeType;

struct Type_ {
    TypeKind kind; // 变量类型
    union {
        // 基本类型 int = 0, float = 1
        int basic;
        // 数组类型信息包括元素类型与数组大小构成
        struct { Type elem; int size; } array;
        // 结构体类型信息是一个链表
        FieldList structure;
    } u;
};

struct FieldList_ {
    char *name; // 域的名字
    Type type; // 域的类型
    FieldList tail; // 下一个域
};

struct DecLine_{
    int lineno;
    DecLine next;
};

struct Func_ {
//    char *name; // 函数名
    int paramNum; // 参数个数
    Type ret; // 返回值类型
    FieldList paramList; // 参数列表
    bool ifDef; // 是否已被声明
    bool ifReal; // 是否已被实现
    DecLine decLines; // 存储声明行号
};

struct Info_{
    NodeType kind; // 区分是变量类型还是函数类型
    union {
        Type type;
        Func func;
    };
    Info next;  // 用以指示是否有重名类型
};

struct HashNode_ {
    char *name; // 节点名
    Info info;
    // 开散列
    HashNode next;
};

struct TableNode_ {
//    int nodeNum; // 当前负载节点数量
    HashNode hashList; // 开散列链表
};

unsigned int hash_pjw(char *name);
void createHashTable();
HashNode createHashNode(char *name, NodeType kind);
HashNode hashCheck(char* name);
void insertHashNode(HashNode newNode);
int structCmp(Type s1, Type s2);
Info checkInfo(HashNode checkNode, NodeType kind);
int typeCmp(Type t1, Type t2);
int funcCmp(Func f1, Func f2);
void printHashTable();

#endif