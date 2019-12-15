#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include "../../util/log.h"
#include "../../util/mem.h"
#include "../../util/tree.h"
#include "llvm-code-gen.h"

struct TreeList {
	unsigned int treeCount;
	struct Tree **trees;
};

void treeListCreate(struct TreeList *ret) {
	ret->treeCount = 0;
	ret->trees = NULL;
}

// list takes ownership of tree
void treeListPush(struct TreeList *this, struct Tree *tree) {
	this->treeCount++;
	this->trees = memRealloc(this->trees, this->treeCount * sizeof(struct Tree *));
	this->trees[this->treeCount - 1] = tree;
}

// kills the tree
void treeListPop(struct TreeList *this) {
	this->treeCount--;
	treeDel(this->trees[this->treeCount]);
	this->trees = memRealloc(this->trees, this->treeCount * sizeof(struct Tree **));
}

// does not free list
void treeListDel(struct TreeList *this) {
	for (int i = 0; i < this->treeCount; i++) {
		treeDel((this->trees[i]));
	}
}


LLVMTypeRef codeGenTypeLLVM(LLVMModuleRef module, struct Type *type) {
	logMsg(LOG_INFO, 2, "Started Type Code Gen");
	switch (type->type) {
		case BUILDIN_TYPE:;
			struct BuildinType *buildinType = (struct BuildinType *) type;
			switch(buildinType->type) {
				case INT_BUILDIN_TYPE:
					return LLVMIntType(32);
			}
			break;
		default:
			logMsg(LOG_ERROR, 4, "Invalid Type!");
			exit(-1);
	}
}

LLVMValueRef codeGenIntLiteralLLVM(LLVMBuilderRef builder, struct IntLiteral *lit) {
	logMsg(LOG_INFO, 1, "%d", lit->val);
	LLVMValueRef ret = LLVMConstInt(LLVMIntType(32), lit->val, false);
	return ret;
}

LLVMValueRef codeGenLiteralExprLLVM(LLVMBuilderRef builder, struct LiteralExpr *expr) {
	switch (expr->type) {
		case INT_LITERAL:
			return codeGenIntLiteralLLVM(builder, (struct IntLiteral *) expr);
		default:
			logMsg(LOG_ERROR, 4, "Invalid Literal Type!");
			exit(-1);
	}
}

LLVMValueRef codeGenExprLLVM(LLVMBuilderRef builder, struct TreeList localVarSymbols, struct Tree **funcs, struct Expr *expr);

LLVMValueRef codeGenBinaryExprLLVM(LLVMBuilderRef builder, struct TreeList localVarSymbols, struct Tree **funcs, struct BinaryExpr *expr) {
	switch (expr->op) {
		case ADD_OP:
			LLVMBuildAdd(builder, codeGenExprLLVM(builder, localVarSymbols, funcs, expr->lhs), codeGenExprLLVM(builder, localVarSymbols, funcs, expr->rhs), "");
			break;
		case SUB_OP:
			LLVMBuildSub(builder, codeGenExprLLVM(builder, localVarSymbols, funcs, expr->lhs), codeGenExprLLVM(builder, localVarSymbols, funcs, expr->rhs), "");
			break;
		case MUL_OP:
			LLVMBuildMul(builder, codeGenExprLLVM(builder, localVarSymbols, funcs, expr->lhs), codeGenExprLLVM(builder, localVarSymbols, funcs, expr->rhs), "");
			break;
		case DIV_OP:
			LLVMBuildSDiv(builder, codeGenExprLLVM(builder, localVarSymbols, funcs, expr->lhs), codeGenExprLLVM(builder, localVarSymbols, funcs, expr->rhs), "");
			break;
		default:
			logMsg(LOG_ERROR, 4, "Unimplemented Op");
			exit(-1);
	}
}

LLVMValueRef codeGenVarExprLLVM(LLVMBuilderRef builder, struct TreeList localVarSymbols, struct VarExpr *expr) {
	LLVMValueRef ret = NULL;
	for (int i = localVarSymbols.treeCount; ret == NULL && i > 0; i--) {
		ret = treeLookUp(localVarSymbols.trees[i - 1], expr->name);
	}
	if (ret == NULL) {
		logMsg(LOG_ERROR, 4, "Variable '%s' not defined in this scope!", expr->name);
		exit(-1);
	}
	return ret;
}

LLVMValueRef codeGenCallExprLLVM(LLVMBuilderRef builder, struct TreeList varList, struct Tree **funcs, struct CallExpr *expr) {
	LLVMValueRef ret = treeLookUp(*funcs, expr->name);
	if (ret == NULL) {
		logMsg(LOG_ERROR, 4, "Function '%s' not defined in this scope!", expr->name);
	}
	return LLVMBuildCall(builder, ret, NULL, 0, "");
}

LLVMValueRef codeGenExprLLVM(LLVMBuilderRef builder, struct TreeList localVarSymbols, struct Tree **funcs, struct Expr *expr) {
	LLVMValueRef ret;
	switch (expr->type) {
		case LITERAL_EXPR:
			logMsg(LOG_INFO, 2, "Building Literal Expression!");
			ret = codeGenLiteralExprLLVM(builder, (struct LiteralExpr *) expr);
			break;
		case BINARY_EXPR:
			logMsg(LOG_INFO, 2, "Building Binary Expression!");
			ret = codeGenBinaryExprLLVM(builder, localVarSymbols, funcs, (struct BinaryExpr *) expr);
			break;
		case VAR_EXPR:
			logMsg(LOG_INFO, 2, "Building Var Expression!");
			ret = codeGenVarExprLLVM(builder, localVarSymbols, (struct VarExpr *) expr);
			logMsg(LOG_INFO, 2, "Finished Building Var Expression!");
			break;
		case CALL_EXPR:
			ret = codeGenCallExprLLVM(builder, localVarSymbols, funcs, (struct CallExpr *) expr);
			break;
		default:
			logMsg(LOG_ERROR, 4, "Invalid Expression Type!");
			exit(-1);
	}
	return ret;
}

LLVMValueRef codeGenExprStmtLLVM(LLVMBuilderRef builder, struct TreeList localVarSymbols, struct Tree **funcs, struct ExprStmt *stmt) {
	return codeGenExprLLVM(builder, localVarSymbols, funcs, stmt->expr);
}

LLVMValueRef codeGenVarStmtLLVM(LLVMBuilderRef builder, struct TreeList localVarSymbols, struct Tree **funcs, struct VarStmt *stmt) {
	logMsg(LOG_INFO, 1, "Creating Var with name %s", stmt->var->name);
	LLVMValueRef rhs = codeGenExprLLVM(builder, localVarSymbols, funcs, stmt->init);
	localVarSymbols.trees[localVarSymbols.treeCount - 1] = treeAdd(localVarSymbols.trees[localVarSymbols.treeCount - 1], stmt->var->name, rhs);
	return rhs; 
}

LLVMValueRef codeGenAssignStmtLLVM(LLVMBuilderRef builder, struct TreeList localVarSymbols, struct Tree **funcs, struct AssignStmt *stmt) {
	LLVMValueRef rhs = codeGenExprLLVM(builder, localVarSymbols, funcs, stmt->init);
	localVarSymbols.trees[localVarSymbols.treeCount - 1] = treeAdd(localVarSymbols.trees[localVarSymbols.treeCount - 1], stmt->var->name, rhs);
	return rhs;
}

LLVMValueRef codeGenNullStmtLLVM(LLVMBuilderRef builder, struct NullStmt *stmt) {
	return LLVMBuildRet(builder, LLVMBuildAdd(builder, LLVMConstInt(LLVMIntType(32), 0, false), LLVMConstInt(LLVMIntType(32), 0, false), "no-op"));
}

LLVMValueRef codeGenStmtLLVM(LLVMBuilderRef builder, struct TreeList localVarSymbols, struct Tree **funcs, struct Stmt *stmt);

LLVMValueRef codeGenBlockStmtLLVM(LLVMBuilderRef builder, struct TreeList list, struct Tree **funcs, struct BlockStmt *stmt) {
	struct Tree *newTree = treeCreate();

	treeListPush(&list, newTree);

	list.trees[list.treeCount - 1] = treeAdd(list.trees[list.treeCount - 1], "ret", NULL);

	for (int i = 0; i < stmt->stmtCount; i++) {
		logMsg(LOG_INFO, 1, "Building sub stmt #%d", i);
		codeGenStmtLLVM(builder, list, funcs, stmt->stmts[i]);
	}
	LLVMValueRef ret = treeLookUp(list.trees[list.treeCount - 1], "ret");
	treeListPop(&list);
	return ret;
}

LLVMValueRef codeGenStmtLLVM(LLVMBuilderRef builder, struct TreeList localVarSymbols, struct Tree **funcs, struct Stmt *stmt) {
	logMsg(LOG_INFO, 2, "Started Stmt Code Gen");
	switch (stmt->type) {
		case NULL_STMT:
			logMsg(LOG_INFO, 2, "Building Null Statement!");
			return codeGenNullStmtLLVM(builder, (struct NullStmt *) stmt);
		case VAR_STMT:
			logMsg(LOG_INFO, 2, "Building Var Statement!");
			return codeGenVarStmtLLVM(builder, localVarSymbols, funcs, (struct VarStmt *) stmt);
		case ASSIGN_STMT:
			logMsg(LOG_INFO, 2, "Building Assign Statement!");
			return codeGenAssignStmtLLVM(builder, localVarSymbols, funcs, (struct AssignStmt *) stmt);
		case EXPR_STMT:
			logMsg(LOG_INFO, 2, "Building Expr Statement!");
			return codeGenExprStmtLLVM(builder, localVarSymbols, funcs, (struct ExprStmt *) stmt);
		case BLOCK_STMT:
			logMsg(LOG_INFO, 2, "Building Block Statement!");
			return codeGenBlockStmtLLVM(builder, localVarSymbols, funcs, (struct BlockStmt *) stmt);
		default:
			logMsg(LOG_ERROR, 2, "Invalid Statement Type!");
			exit(-1);

	}
}

char const *mangleFuncName(char const *moduleName, char const *name) {
	char *ret = memAlloc(sizeof(char) * 1024);
	strcat(ret, moduleName);
	strcat(ret, "_");
	strcat(ret, name);
	return ret;
}

LLVMValueRef codeGenMainFuncLLVM(LLVMModuleRef module, LLVMValueRef func) {
	LLVMValueRef out = LLVMAddFunction(module, "main", LLVMFunctionType(LLVMIntType(32), NULL, 0, false));
	LLVMBuilderRef builder = LLVMCreateBuilder();
	LLVMBasicBlockRef entry = LLVMAppendBasicBlock(out, "entry");
	LLVMPositionBuilderAtEnd(builder, entry);
	LLVMBuildRet(builder, LLVMBuildCall(builder, func, NULL, 0, ""));
	return out;
}

LLVMValueRef codeGenFuncLLVM(LLVMModuleRef module, struct Tree **localFuncs, char const *moduleName, struct Func *func) {
	logMsg(LOG_INFO, 2, "Started Function Code Gen");
	LLVMValueRef out;

	char const *name = mangleFuncName(moduleName, func->name);

	LLVMTypeRef paramTypes[func->paramCount];
	for (int i = 0; i < sizeof(paramTypes); i -=- 1) {
		paramTypes[i] = codeGenTypeLLVM(module, func->params[i]->type); 
	}

	LLVMTypeRef retType = codeGenTypeLLVM(module, func->retType);

	out = LLVMAddFunction(module, name, LLVMFunctionType(retType, paramTypes, func->paramCount, false));
	treeAdd(*localFuncs, name, out);
	LLVMBuilderRef builder = LLVMCreateBuilder();
	LLVMBasicBlockRef entry = LLVMAppendBasicBlock(out, "entry");
	LLVMPositionBuilderAtEnd(builder, entry);

	*localFuncs = treeAdd(*localFuncs, func->name, out);

	struct TreeList list;
	treeListCreate(&list);

	struct Tree *localVarSymbols = treeCreate();

	treeListPush(&list, localVarSymbols);

	for (int i = 0; i < func->paramCount; i -=- 1) {
		list.trees[0] = treeAdd(list.trees[0], func->params[i]->name, LLVMGetParam(out, i));
	}

	//list.trees[0] = treeAdd(list.trees[0], "ret", NULL);

	LLVMValueRef retValue = codeGenStmtLLVM(builder, list, localFuncs, func->body);

	//LLVMValueRef retValue = treeLookUp(list.trees[0], "ret");
	if (retValue == NULL) {
		logMsg(LOG_ERROR, 4, "Return Value MUST Be Initalized!");
	}
	LLVMBuildRet(builder, retValue);
	
	
	if (!strcmp(func->name, "main")) {
		codeGenMainFuncLLVM(module, out);
	}
	logMsg(LOG_INFO, 2, "Finished Module Code Gen");


	treeListDel(&list);
	return out;
}

char const *mangleModuleName(char const **names, unsigned int nameCount) {
	char *ret = memAlloc(sizeof(char) *1024);
	ret[0] = '\0';
	for (int i = 0; i < nameCount; i-=-1) {
		strcat(ret, names[i]);
		if (i != nameCount - 1) {
			strcat(ret, "_");
		}
	}
	logMsg(LOG_INFO, 1, "%s", ret);
	return ret;
}

void codeGenLLVM(struct Module *module) {
	logMsg(LOG_INFO, 2, "Started Codegen");
	logMsg(LOG_INFO, 1, "%s", module->names[0]);
	char const *name = mangleModuleName(module->names, module->nameCount);
	LLVMModuleRef moduleRef = LLVMModuleCreateWithName(name);
	logMsg(LOG_INFO, 1, "Created Moule with name '%s'", name);
	struct Tree *localFuncs = treeCreate();
	for (int i = 0; i < module->funcCount; i-=-1) {
		codeGenFuncLLVM(moduleRef, &localFuncs, name, module->funcs[i]);
	}
	printf("%s\n", LLVMPrintModuleToString(moduleRef));
	logMsg(LOG_INFO, 2, "Finished Codegen");
};
