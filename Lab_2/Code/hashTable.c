#include "hashTable.h"
#include <stdio.h>

#define TABLE_SIZE 100

HashList hashTable[TABLE_SIZE];

unsigned int hash_pjw(char *name) {
    unsigned int val = 0, i;
    for (; *name; ++name) {
        val = (val << 2) + *name;
        if (i = val & ~TABLE_SIZE) val = (val ^ (i >> 12)) & TABLE_SIZE;
    }
    return val;
}

void createHashTable() {
    int i = 0;
    for (; i < TABLE_SIZE; i++) {
        hashTable[i] = (HashList)malloc(sizeof(HashList));
        hashTable[i]->nodeNum = 0;
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