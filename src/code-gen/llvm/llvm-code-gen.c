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

LLVMValueRef codeGenExprLLVM(LLVMBuilderRef builder, struct Expr *expr);

LLVMValueRef codeGenBinaryExprLLVM(LLVMBuilderRef builder, struct BinaryExpr *expr) {
	switch (expr->op) {
		case ADD_OP:
			LLVMBuildAdd(builder, codeGenExprLLVM(builder, expr->lhs), codeGenExprLLVM(builder, expr->rhs), "");
			break;
		case SUB_OP:
			LLVMBuildSub(builder, codeGenExprLLVM(builder, expr->lhs), codeGenExprLLVM(builder, expr->rhs), "");
			break;
		case MUL_OP:
			LLVMBuildMul(builder, codeGenExprLLVM(builder, expr->lhs), codeGenExprLLVM(builder, expr->rhs), "");
			break;
		default:
			logMsg(LOG_ERROR, 4, "Unimplemented Op");
			exit(-1);
	}
}

LLVMValueRef codeGenExprLLVM(LLVMBuilderRef builder, struct Expr *expr) {
	LLVMValueRef ret;
	switch (expr->type) {
		case LITERAL_EXPR:
			logMsg(LOG_INFO, 2, "Building Literal Expression!");
			ret = codeGenLiteralExprLLVM(builder, (struct LiteralExpr *) expr);
			break;
		case BINARY_EXPR:
			logMsg(LOG_INFO, 2, "Building Binary Expression!");
			ret = codeGenBinaryExprLLVM(builder, (struct BinaryExpr *) expr);
			break;
		default:
			logMsg(LOG_ERROR, 4, "Invalid Expression Type!");
			exit(-1);
	}
	return ret;
}

LLVMValueRef codeGenExprStmtLLVM(LLVMBuilderRef builder, struct ExprStmt *stmt) {
	return codeGenExprLLVM(builder, stmt->expr);
}

LLVMValueRef codeGenVarStmtLLVM(LLVMBuilderRef builder, struct Tree **localVarSymbols, struct VarStmt *stmt) {
	LLVMValueRef rhs = codeGenExprLLVM(builder, stmt->init);
	*localVarSymbols = treeAdd(*localVarSymbols, stmt->var->name, rhs);
	return rhs; 
}

LLVMValueRef codeGenAssignStmtLLVM(LLVMBuilderRef builder, struct Tree **localVarSymbols, struct AssignStmt *stmt) {
	LLVMValueRef rhs = codeGenExprLLVM(builder, stmt->init);
	*localVarSymbols = treeAdd(*localVarSymbols, stmt->var->name, rhs);
	return rhs;
}

LLVMValueRef codeGenNullStmtLLVM(LLVMBuilderRef builder, struct NullStmt *stmt) {
	return LLVMBuildRet(builder, LLVMBuildAdd(builder, LLVMConstInt(LLVMIntType(32), 0, false), LLVMConstInt(LLVMIntType(32), 0, false), "no-op"));
}

LLVMValueRef codeGenBlockStmt(LLVMBuilderRef builder, struct Tree **localVarSymbols, struct BlockStmt *stmt) {
	return NULL;
}

LLVMValueRef codeGenStmtLLVM(LLVMBuilderRef builder, struct Tree **localVarSymbols, struct Stmt *stmt) {
	logMsg(LOG_INFO, 2, "Started Stmt Code Gen");
	switch (stmt->type) {
		case NULL_STMT:
			logMsg(LOG_INFO, 2, "Building Null Statement!");
			return codeGenNullStmtLLVM(builder, (struct NullStmt *) stmt);
		case VAR_STMT:
			logMsg(LOG_INFO, 2, "Building Var Statement!");
			return codeGenVarStmtLLVM(builder, localVarSymbols, (struct VarStmt *) stmt);
		case ASSIGN_STMT:
			logMsg(LOG_INFO, 2, "Building Assign Statement!");
			return codeGenAssignStmtLLVM(builder, localVarSymbols, (struct AssignStmt *) stmt);
		case EXPR_STMT:
			logMsg(LOG_INFO, 2, "Building Expr Statement!");
			return codeGenExprStmtLLVM(builder, (struct ExprStmt *) stmt);
		case BLOCK_STMT:
			logMsg(LOG_INFO, 2, "Building Block Statement!");
			return codeGenBlockStmt(builder, localVarSymbols, (struct BlockStmt *) stmt);
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

	struct Tree *localVarSymbols = treeCreate();

	for (int i = 0; i < func->paramCount; i -=- 1) {
		localVarSymbols = treeAdd(localVarSymbols, func->params[i]->name, LLVMGetParam(out, i));
	}

	localVarSymbols = treeAdd(localVarSymbols, "ret", NULL);

	codeGenStmtLLVM(builder, &localVarSymbols, func->body);

	LLVMValueRef retValue = treeLookUp(localVarSymbols, "ret");
	if (retValue == NULL) {
		logMsg(LOG_ERROR, 4, "Return Value MUST Be Initalized!");
	}
	LLVMBuildRet(builder, retValue);
	
	
	if (!strcmp(func->name, "main")) {
		codeGenMainFuncLLVM(module, out);
	}
	logMsg(LOG_INFO, 2, "Finished Module Code Gen");
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
