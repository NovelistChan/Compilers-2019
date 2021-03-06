#include "hashTable.h"
#include <stdio.h>
#include <string.h>

// HashList hashTable[TABLE_SIZE];
TableNode *hashTable = NULL;

unsigned int hash_pjw(char *name) {
    unsigned int val = 0, i;
    for (; *name; ++name) {
        val = (val << 2) + *name;
        if (i = val & ~TABLE_SIZE) val = (val ^ (i >> 12)) & TABLE_SIZE;
    }
    return val;
}

void createHashTable() {
    hashTable = (TableNode *)malloc(sizeof(TableNode) * TABLE_SIZE);
    int i = 0;
    for (; i < TABLE_SIZE; i++) {
//        hashTable[i]->nodeNum = 0;
        hashTable[i] = (TableNode)malloc(sizeof(struct TableNode_));
        hashTable[i]->hashList = NULL;
    }

    // type int
    Type intType = (Type)malloc(sizeof(struct Type_));
    intType->kind = BASIC;
    intType->u.basic = 0;

    // insert  int read()
    HashNode readFuc = createHashNode("read", FUNC);
    Func readTemp = (Func)malloc(sizeof(struct Func_));
    readTemp->paramNum = 0;
    readTemp->ret = intType;
    readTemp->paramList = NULL;
    readTemp->ifReal = true;
    readTemp->ifDef = false;
    readTemp->decLines = NULL;
    readFuc->info->func = readTemp;
    insertHashNode(readFuc);

    // insert  int write(int)
    HashNode writeFuc = createHashNode("write", FUNC);
    FieldList writeParam = (FieldList)malloc(sizeof(struct FieldList_));
    writeParam->name = "";
    writeParam->type = intType;
    writeParam->tail = NULL;
    Func writeTemp = (Func)malloc(sizeof(struct Func_));
    writeTemp->paramNum = 1;
    writeTemp->ret = intType;
    writeTemp->paramList = writeParam;
    writeTemp->ifReal = true;
    writeTemp->ifDef = false;
    writeTemp->decLines = NULL;
    writeFuc->info->func = writeTemp;
    insertHashNode(writeFuc);
}

void printHashTable() {
    int i = 0;
    printf("HashTable: \n");
    for (; i <TABLE_SIZE; i++) {
        if (hashTable[i]->hashList != NULL) {
            HashNode p = hashTable[i]->hashList;
            while (p) {
                printf("NodeName: %s\n", p->name);
                p = p->next;
            }
        }
    }
}

HashNode createHashNode(char *name, NodeType kind) {
    HashNode newNode = (HashNode)malloc(sizeof(struct HashNode_));
    Info newInfo = (Info)malloc(sizeof(struct Info_));
    newNode->name = name;
    newInfo->kind = kind;
    newInfo->next = NULL;
    newNode->info = newInfo;
    newNode->next = NULL;
    return newNode;
}

HashNode hashCheck(char* name) {
    int index = hash_pjw(name);
    if (hashTable[index]->hashList == NULL) {
        return NULL;
    } else {
        HashNode p = hashTable[index]->hashList;
        while (p) {
            if (strcmp(p->name, name) == 0) {
                return p;
            }
            p = p->next;
        }
    }
    return NULL;
}

void insertHashNode(HashNode newNode) {
    int index = hash_pjw(newNode->name);
    /*
    if (hashTable[index]->nodeNum == 0) {
        hashTable[index]->nodeNum++;
        hashTable[index]->hashList = (HashNode)malloc(sizeof(HashNode) * hashTable[index]->nodeNum);
        hashTable[index]->hashList = newNode;
    } else {
        hashTable[index]->nodeNum++;
        HashNode *hashList = (HashNode *)malloc(sizeof(HashNode) * hashTable[index]->nodeNum);
        int i = 0;
        for (; i < hashTable[index]->nodeNum - 1; i++) {
            hashList[i] = hashTable[index]->hashList[i];
        }
        hashList[i] = newNode;
        hashTable[index]->hashList = hashList;
    }
    */
   newNode->next = NULL;
   HashNode p = hashTable[index]->hashList;
   if (!p) {
       hashTable[index]->hashList = newNode;
   } else {
       while (p->next) {
           p = p->next;
       }
       p->next = newNode;
   }
}

int fieldCmp(FieldList f1, FieldList f2) {
    if (f1 == NULL && f2 == NULL) {
        return 0;
    } else if (f1 == NULL && f2 != NULL) {
        return 1;
    } else if (f1 != NULL && f2 == NULL) {
        return 1;
    } else {
        if (!strcmp(f1->name, f2->name) && !typeCmp(f1->type, f2->type)) {
            return fieldCmp(f1->tail, f2->tail);
        } else return 1;
    }
    return 0;
}

// 2 = 类型不匹配 1 = 不相等 0 = 相等 
int structCmp(Type s1, Type s2) {
    if (s1->kind != s2->kind) {
        return 2;
    } 
    FieldList f1 = s1->u.structure;
    FieldList f2 = s2->u.structure;
    int res = fieldCmp(f1, f2);
    return res;
}

// 若有该类型则返回该info项
Info checkInfo(HashNode checkNode, NodeType kind) {
    Info p = checkNode->info;
    while (p) {
        if (p->kind == kind) {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

// 0 = equal
int typeCmp(Type t1, Type t2) {
    if (t1->kind != t2->kind) return 1;
    if (t1->kind == STRUCTURE) {
        return structCmp(t1, t2);
    } else if (t1->kind == BASIC) {
        return (t1->u.basic == t2->u.basic) ? 0 : 1;
    } else {
        if (t1->u.array.size != t2->u.array.size) return 1;
        else return typeCmp(t1->u.array.elem, t2->u.array.elem);
    }
}

// 0 = equal
int funcCmp(Func f1, Func f2){
    if(f1->paramNum != f2->paramNum){
        return 1;
    }
    if(typeCmp(f1->ret, f2->ret) || fieldCmp(f1->paramList, f2->paramList)){
        return 1;
    }
    return 0;
}

void checkFunction(){
    HashNode p = NULL;
    for(int i=0;i<TABLE_SIZE;i++){
        p = hashTable[i]->hashList;
        while(p){
            Info q = p->info;
            int firstDec = 0;
            while(q){
                if(q->kind == FUNC){
                    if(q->func->ifReal) break;
                    if(!firstDec && q->func->ifDef) firstDec = q->func->decLines->lineno;
                }
                q = q->next;
            }
            if(!q && firstDec){
                fprintf(stderr, "Error type 18 at Line %d: Undefined function \"%s\".\n", firstDec, p->name);
            }
            p = p->next;
        }
    }
}