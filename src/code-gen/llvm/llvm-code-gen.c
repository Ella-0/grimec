#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <llvm-c/Core.h>
#include "../../util/log.h"
#include "../../util/tree.h"
#include "llvm-code-gen.h"

LLVMTypeRef codeGenTypeLLVM(LLVMModuleRef module, struct Type *type) {
	switch (type->type) {
		case BUILDIN_TYPE:;
			struct BuildinType *buildinType = (struct BuildinType *) type;
			switch(buildinType->type) {
				case INT_BUILDIN_TYPE:
					return LLVMIntType(32);
			}
			break;
		default:
			logMsg(LOG_ERROR, "Invalid Type!");
			exit(-1);
	}
}

LLVMValueRef codeGenIntLiteralLLVM(LLVMBuilderRef builder, struct IntLiteral *lit) {
	return LLVMConstInt(LLVMIntType(32), lit->val, false);
}

LLVMValueRef codeGenLiteralExprLLVM(LLVMBuilderRef builder, struct LiteralExpr *expr) {
	switch (expr->type) {
		case INT_LITERAL:
			codeGenIntLiteralLLVM(builder, (struct IntLiteral *) expr);
			return NULL;
		default:
			fprintf(stderr, "Error: Invalid Expr Type!");
			exit(-1);
	}
}

LLVMValueRef codeGenExprLLVM(LLVMBuilderRef builder, struct Expr *expr) {
	switch (expr->type) {
		case LITERAL_EXPR:
			return codeGenLiteralExprLLVM(builder, (struct LiteralExpr *) expr);
		default:
			fprintf(stderr, "Error: Invalid Expr Type!");
			exit(-1);
	}
}

LLVMValueRef codeGenExprStmtLLVM(LLVMBuilderRef builder, struct ExprStmt *stmt) {
	return codeGenExprLLVM(builder, stmt->expr);
}

LLVMValueRef codeGenVarStmtLLVM(LLVMBuilderRef builder, struct Tree **localVarSymbols, struct VarStmt *stmt) {
	LLVMValueRef rhs = codeGenExprLLVM(builder, stmt->init);
	*localVarSymbols = treeAdd(*localVarSymbols, stmt->var->name, rhs);
	return rhs; 
}

LLVMValueRef codeGenNullStmtLLVM(LLVMBuilderRef builder, struct NullStmt *stmt) {
	return LLVMBuildRet(builder, LLVMBuildAdd(builder, LLVMConstInt(LLVMIntType(32), 0, false), LLVMConstInt(LLVMIntType(32), 0, false), "no-op"));
}

LLVMValueRef codeGenStmtLLVM(LLVMBuilderRef builder, struct Tree **localVarSymbols, struct Stmt *stmt) {
	switch (stmt->type) {
		case NULL_STMT:
			logMsg(LOG_INFO, "Building Null Statement!");
			return codeGenNullStmtLLVM(builder, (struct NullStmt *) stmt);
		case VAR_STMT:
			logMsg(LOG_INFO, "Building Var Statement!");
			codeGenVarStmtLLVM(builder, localVarSymbols, (struct VarStmt *) stmt);
			return NULL;
		case EXPR_STMT:
			logMsg(LOG_INFO, "Building Expr Statement!");
			return codeGenExprStmtLLVM(builder, (struct ExprStmt *) stmt);
		default:
			fprintf(stderr, "Error: Invalid Statement Type!");
			exit(-1);

	}
}

LLVMValueRef codeGenFuncLLVM(LLVMModuleRef module, struct Func *func) {
	LLVMValueRef out;

	LLVMTypeRef paramTypes[func->paramCount];
	for (int i = 0; i < sizeof(paramTypes); i -=- 1) {
		paramTypes[i] = codeGenTypeLLVM(module, func->params[i]->type); 
	}

	LLVMTypeRef retType = codeGenTypeLLVM(module, func->retType);

	out = LLVMAddFunction(module, func->name, LLVMFunctionType(retType, paramTypes, func->paramCount, false));
	LLVMBuilderRef builder = LLVMCreateBuilder();
	LLVMBasicBlockRef entry = LLVMAppendBasicBlock(out, "entry");
	LLVMPositionBuilderAtEnd(builder, entry);

	struct Tree *localVarSymbols = treeCreate();

	codeGenStmtLLVM(builder, &localVarSymbols, func->body);
	return out;
}

void codeGenLLVM(struct Module *module) {
	LLVMModuleRef moduleRef = LLVMModuleCreateWithName(module->name);
	for (int i = 0; i < module->funcCount; i-=-1) {
		codeGenFuncLLVM(moduleRef, module->funcs[i]);
	}
	printf("%s\n", LLVMPrintModuleToString(moduleRef));
};
