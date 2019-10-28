#ifndef __SEMANTIC_H__
#define __SEMANTIC_H__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hashTable.h"
#include "syntaxTree.h"

extern TreeNode *root;
void semanticAnalysis();    // entrance of semantic analysis

/* Each non-terminal symbol */
void Program(TreeNode *node);
void ExtDefList(TreeNode *node);
void ExtDef(TreeNode *node);
void ExtDecList(TreeNode *node);
void FunDeclaration(TreeNode *node);

void Specifier(TreeNode *node);
void StructSpecifier(TreeNode *node);
void OptTag(TreeNode *node);
void Tag(TreeNode *node);

void VarDec(TreeNode *node);
void FunDec(TreeNode *node);
void VarList(TreeNode *node);
void ParamDec(TreeNode *node);

void CompSt(TreeNode *node);
void StmtList(TreeNode *node);
void Stmt(TreeNode *node);

void DefList(TreeNode *node);
void Def(TreeNode *node);
void DecList(TreeNode *node);
void Dec(TreeNode *node);

void Exp(TreeNode *node);
void Args(TreeNode *node);

#endif