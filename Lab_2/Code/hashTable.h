#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef struct Type_ *Type;
typedef struct FieldList_ *FieldList;
typedef struct Func_ *Func;
typedef struct HashNode_ *HashNode;
typedef struct HashTable_ *HashTable;

struct Type_ {
    enum {
        BASIC,
        ARRAY,
        STRUCTURE
    } kind; // 变量类型
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

struct Func_ {
    char *name; // 函数名
    int paramNum; // 参数个数
    Type ret; // 返回值类型
    FieldList paramList; // 参数列表
    bool ifDef; // 是否已被声明
    bool ifReal; // 是否已被实现
};

struct HashNode_ {
    char *name; // 节点名
    enum {
        FUNC,
        TYPE
    } kind; // 区分是变量类型还是函数类型
    union {
        Type type;
        Func func;
    }; // 节点内容
    // 开散列
    HashNode next;
    // bool ifTaken; // 该节点是否已被占用
};

struct HashTable_ {
    int nodeNum; // 当前负载节点数量
    HashNode *hashList; // 开散列链表
};

unsigned int hash_pjw(char *name);
#endif