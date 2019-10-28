#include "hashTable.h"
#include <stdio.h>
#include <string.h>
#define TABLE_SIZE 100

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
        hashTable[i]->hashList = NULL;
    }
}

HashNode createHashNode(char *name, NodeType kind) {
    HashNode newNode = (HashNode)malloc(sizeof(HashNode));
    newNode->name = name;
    newNode->kind = kind;
    newNode->next = NULL;
    return newNode;
}

HashNode hashCheck(HashNode checkNode) {
    int index = hash_pjw(checkNode->name);
    if (hashTable[index]->hashList == NULL) {
        return NULL;
    } else {
        HashNode p = hashTable[index]->hashList;
        while (!p) {
            if (strcmp(p->name, checkNode->name) == 0) {
                return p;
            }
            p = p->next;
        }
    }
    return NULL;
}

void insertNode(HashNode newNode) {
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
       p = newNode;
   } else {
       while (!p->next) {
           p = p->next;
       }
       p->next = newNode;
   }
}