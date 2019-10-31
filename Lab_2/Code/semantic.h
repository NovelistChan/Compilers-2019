#ifndef __SEMANTIC_H__
#define __SEMANTIC_H__
#include "hashTable.h"
#include "syntaxTree.h"

extern TreeNode *root;
void semanticAnalysis();    // entrance of semantic analysis

/* Each non-terminal symbol */
void Program(TreeNode *node);
void ExtDefList(TreeNode *node);
void ExtDef(TreeNode *node);
void ExtDecList(TreeNode *node, Type type);
void FunDeclaration(TreeNode *node);

Type Specifier(TreeNode *node);
Type StructSpecifier(TreeNode *node);
char* OptTag(TreeNode *node);
char* Tag(TreeNode *node);

char* VarDec(TreeNode *node, Type type);
void FunDec(TreeNode *node, Type type, bool isDef);
FieldList VarList(TreeNode *node);
FieldList ParamDec(TreeNode *node);

void CompSt(TreeNode *node);
void StmtList(TreeNode *node);
void Stmt(TreeNode *node);

void DefList(TreeNode *node, bool isStruct, FieldList fieldList);
void Def(TreeNode *node, bool isStruct, FieldList fieldList);
void DecList(TreeNode *node, bool isStruct, FieldList fieldList, Type type);
void Dec(TreeNode *node, bool isStruct, FieldList fieldList, Type type);

Type Exp(TreeNode *node);
FieldList Args(TreeNode *node);

int checkArgs(FieldList paramList1, FieldList paramList2);
// 赋值号
Type AssignOp(Type left, Type right, int line);
// 算术运算符
Type ArithmeticCheck(Type left, Type right, int line);
// 逻辑运算符
Type LogicCheck(Type left, Type right, int line);

#endif