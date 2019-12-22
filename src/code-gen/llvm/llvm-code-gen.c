#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include "../../util/log.h"
#include "../../util/mem.h"
#include "../../util/tree.h"
#include "llvm-code-gen.h"

struct TypeLLVM {
	LLVMTypeRef type;
	LLVMValueRef init;
	struct Tree **methods;
};

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


LLVMTypeRef codeGenTypeLLVM(LLVMModuleRef module, struct Tree **localTypes, struct Type *type) {
	logMsg(LOG_INFO, 2, "Started Type Code Gen");
	LLVMTypeRef ret;
	switch (type->type) {
		case BUILDIN_TYPE:;
			struct BuildinType *buildinType = (struct BuildinType *) type;
			switch(buildinType->type) {
				case INT_BUILDIN_TYPE:
					ret = LLVMPointerType(((struct TypeLLVM *) treeLookUp(*localTypes, "Int"))->type, false);
					break;
				case STRING_BUILDIN_TYPE:
					//ret = LLVMPointerType(LLVMIntType(8), 0);
					ret = LLVMPointerType(((struct TypeLLVM *) treeLookUp(*localTypes, "String"))->type, false);
					//ret = LLVMIntType(32);
					break;
				case VOID_BUILDIN_TYPE:
					ret = LLVMVoidType();
					break;
				default:
					logMsg(LOG_ERROR, 4, "Unimplemented Buildin Type");
					exit(-1);
			}
			break;
		case SIMPLE_TYPE: {
				struct SimpleType *simpleType = (struct SimpleType *) type;
				if (!strcmp(simpleType->name, "Void")) {
					ret = LLVMVoidType();
				} else {
					ret = LLVMPointerType(((struct TypeLLVM *) treeLookUp(*localTypes, simpleType->name))->type, false);
				}
				break;
			}
		default:
			logMsg(LOG_ERROR, 4, "Invalid Type!");
			exit(-1);
	}
	logMsg(LOG_INFO, 2, "Finished Type Code Gen");
	return ret;
}

LLVMValueRef codeGenIntLiteralLLVM(LLVMBuilderRef builder, struct Tree **localTypes, struct IntLiteral *lit) {
	logMsg(LOG_INFO, 1, "Generating Int Literal with value %d", lit->val);
	LLVMValueRef cInt = LLVMConstInt(LLVMIntType(32), lit->val, false);
	struct TypeLLVM *intType = treeLookUp(*localTypes, "Int");
	LLVMValueRef intObj = LLVMBuildCall(builder, intType->init, NULL, 0, "");

	LLVMValueRef litFunc = LLVMBuildCall(builder, treeLookUp(*intType->methods, "_literal"), &intObj, 1, "");

	//LLVMValueRef litFunc = LLVMBuildLoad(builder, LLVMBuildStructGEP(builder, intObj, 2, ""), "");

	LLVMValueRef literalArgs[2] = {intObj, cInt};

	/*LLVMTypeRef literalArgTypes[2] = {LLVMPointerType(intType->type, false), LLVMIntType(32)};

	litFunc = LLVMBuildBitCast(builder, litFunc, LLVMPointerType(LLVMFunctionType(LLVMVoidType(), literalArgTypes, 2, false), false), "");*/
	LLVMBuildCall(builder, litFunc, literalArgs, 2, "");
	logMsg(LOG_INFO, 1, "Generated Int Literal", lit->val);
	return intObj;
}


// Spaghetti code fix later
LLVMValueRef codeGenStringLiteralLLVM(LLVMBuilderRef builder, struct Tree **localTypes, struct StringLiteral *lit) {
	logMsg(LOG_INFO, 1, "Generating String Literal with value %s", lit->val);
	LLVMValueRef cString = LLVMBuildMalloc(builder, LLVMArrayType(LLVMIntType(8), strlen(lit->val) + 1), "");
	LLVMBuildStore(builder, LLVMConstString(lit->val, strlen(lit->val), false), cString);
	cString = LLVMBuildBitCast(builder, cString, LLVMPointerType(LLVMIntType(8), false), "");

	struct TypeLLVM *stringType = treeLookUp(*localTypes, "String");
	LLVMValueRef stringObj = LLVMBuildCall(builder, stringType->init, NULL, 0, "");

	LLVMValueRef litFunc = LLVMBuildCall(builder, treeLookUp(*stringType->methods, "_literal"), &stringObj, 1, "");

	LLVMValueRef literalArgs[2] = {stringObj, cString};

	LLVMBuildCall(builder, litFunc, literalArgs, 2, "");
	
	logMsg(LOG_INFO, 1, "Generated String Literal");
	return stringObj;
}

LLVMValueRef codeGenLiteralExprLLVM(LLVMBuilderRef builder, struct Tree **localTypes, struct LiteralExpr *expr) {
	switch (expr->type) {
		case INT_LITERAL:
			return codeGenIntLiteralLLVM(builder, localTypes, (struct IntLiteral *) expr);
		case STRING_LITERAL:
			return codeGenStringLiteralLLVM(builder, localTypes, (struct StringLiteral *) expr);
		default:
			logMsg(LOG_ERROR, 4, "Invalid Literal Type!");
			exit(-1);
	}
}

LLVMValueRef codeGenExprLLVM(LLVMBuilderRef builder, struct TreeList localVarSymbols, struct Tree **funcs, struct Tree **types, struct Expr *expr);

LLVMValueRef codeGenBinaryExprLLVM(LLVMBuilderRef builder, struct TreeList localVarSymbols, struct Tree **funcs, struct Tree **types, struct BinaryExpr *expr) {
	switch (expr->op) {
		case ADD_OP:
			LLVMBuildAdd(builder, codeGenExprLLVM(builder, localVarSymbols, funcs, types, expr->lhs), codeGenExprLLVM(builder, localVarSymbols, funcs, types, expr->rhs), "");
			break;
		case SUB_OP:
			LLVMBuildSub(builder, codeGenExprLLVM(builder, localVarSymbols, funcs, types, expr->lhs), codeGenExprLLVM(builder, localVarSymbols, funcs, types, expr->rhs), "");
			break;
		case MUL_OP:
			LLVMBuildMul(builder, codeGenExprLLVM(builder, localVarSymbols, funcs, types, expr->lhs), codeGenExprLLVM(builder, localVarSymbols, funcs, types, expr->rhs), "");
			break;
		case DIV_OP:
			LLVMBuildSDiv(builder, codeGenExprLLVM(builder, localVarSymbols, funcs, types, expr->lhs), codeGenExprLLVM(builder, localVarSymbols, funcs, types, expr->rhs), "");
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

LLVMValueRef codeGenCallExprLLVM(LLVMBuilderRef builder, struct TreeList varList, struct Tree **funcs, struct Tree **types, struct CallExpr *expr) {
	LLVMValueRef ret = treeLookUp(*funcs, expr->name);
	if (ret == NULL) {
		logMsg(LOG_ERROR, 4, "Function '%s' not defined in this scope!", expr->name);
	}
	LLVMValueRef args[expr->argCount];
	for (int i = 0; i < expr->argCount; i++) {
		args[i] = codeGenExprLLVM(builder, varList, funcs, types,  expr->args[i]);
	}

	return LLVMBuildCall(builder, ret, args, expr->argCount, "");
}

LLVMValueRef codeGenExprLLVM(LLVMBuilderRef builder, struct TreeList localVarSymbols, struct Tree **funcs, struct Tree **types, struct Expr *expr) {
	LLVMValueRef ret;
	switch (expr->type) {
		case LITERAL_EXPR:
			logMsg(LOG_INFO, 2, "Building Literal Expression!");
			ret = codeGenLiteralExprLLVM(builder, types, (struct LiteralExpr *) expr);
			break;
		case BINARY_EXPR:
			logMsg(LOG_INFO, 2, "Building Binary Expression!");
			ret = codeGenBinaryExprLLVM(builder, localVarSymbols, funcs, types, (struct BinaryExpr *) expr);
			break;
		case VAR_EXPR:
			logMsg(LOG_INFO, 2, "Building Var Expression!");
			ret = codeGenVarExprLLVM(builder, localVarSymbols, (struct VarExpr *) expr);
			logMsg(LOG_INFO, 2, "Finished Building Var Expression!");
			break;
		case CALL_EXPR:
			ret = codeGenCallExprLLVM(builder, localVarSymbols, funcs, types, (struct CallExpr *) expr);
			break;
		default:
			logMsg(LOG_ERROR, 4, "Invalid Expression Type!");
			exit(-1);
	}
	return ret;
}

LLVMValueRef codeGenExprStmtLLVM(LLVMBuilderRef builder, struct TreeList localVarSymbols, struct Tree **funcs, struct Tree **types, struct ExprStmt *stmt) {
	return codeGenExprLLVM(builder, localVarSymbols, funcs, types, stmt->expr);
}

LLVMValueRef codeGenVarStmtLLVM(LLVMBuilderRef builder, struct TreeList localVarSymbols, struct Tree **funcs, struct Tree **types, struct VarStmt *stmt) {
	logMsg(LOG_INFO, 1, "Creating Var with name %s", stmt->var->name);
	LLVMValueRef rhs = codeGenExprLLVM(builder, localVarSymbols, funcs, types, stmt->init);
	localVarSymbols.trees[localVarSymbols.treeCount - 1] = treeAdd(localVarSymbols.trees[localVarSymbols.treeCount - 1], stmt->var->name, rhs);
	return rhs; 
}

LLVMValueRef codeGenAssignStmtLLVM(LLVMBuilderRef builder, struct TreeList localVarSymbols, struct Tree **funcs, struct Tree **types, struct AssignStmt *stmt) {
	LLVMValueRef rhs = codeGenExprLLVM(builder, localVarSymbols, funcs, types, stmt->init);
	localVarSymbols.trees[localVarSymbols.treeCount - 1] = treeAdd(localVarSymbols.trees[localVarSymbols.treeCount - 1], stmt->var->name, rhs);
	return rhs;
}

LLVMValueRef codeGenNullStmtLLVM(LLVMBuilderRef builder, struct NullStmt *stmt) {
	return LLVMBuildRet(builder, LLVMBuildAdd(builder, LLVMConstInt(LLVMIntType(32), 0, false), LLVMConstInt(LLVMIntType(32), 0, false), "no-op"));
}

LLVMValueRef codeGenStmtLLVM(LLVMBuilderRef builder, struct TreeList localVarSymbols, struct Tree **funcs, struct Tree **types, struct Stmt *stmt);

LLVMValueRef codeGenBlockStmtLLVM(LLVMBuilderRef builder, struct TreeList list, struct Tree **funcs, struct Tree **types, struct BlockStmt *stmt) {
	struct Tree *newTree = treeCreate();

	treeListPush(&list, newTree);

	list.trees[list.treeCount - 1] = treeAdd(list.trees[list.treeCount - 1], "ret", NULL);

	for (int i = 0; i < stmt->stmtCount; i++) {
		logMsg(LOG_INFO, 1, "Building sub stmt #%d", i);
		codeGenStmtLLVM(builder, list, funcs, types, stmt->stmts[i]);
	}
	LLVMValueRef ret = treeLookUp(list.trees[list.treeCount - 1], "ret");
	treeListPop(&list);
	return ret;
}

LLVMValueRef codeGenStmtLLVM(LLVMBuilderRef builder, struct TreeList localVarSymbols, struct Tree **funcs, struct Tree **types, struct Stmt *stmt) {
	logMsg(LOG_INFO, 2, "Started Stmt Code Gen");
	switch (stmt->type) {
		case NULL_STMT:
			logMsg(LOG_INFO, 2, "Building Null Statement!");
			return codeGenNullStmtLLVM(builder, (struct NullStmt *) stmt);
		case VAR_STMT:
			logMsg(LOG_INFO, 2, "Building Var Statement!");
			return codeGenVarStmtLLVM(builder, localVarSymbols, funcs, types, (struct VarStmt *) stmt);
		case ASSIGN_STMT:
			logMsg(LOG_INFO, 2, "Building Assign Statement!");
			return codeGenAssignStmtLLVM(builder, localVarSymbols, funcs, types, (struct AssignStmt *) stmt);
		case EXPR_STMT:
			logMsg(LOG_INFO, 2, "Building Expr Statement!");
			return codeGenExprStmtLLVM(builder, localVarSymbols, funcs, types, (struct ExprStmt *) stmt);
		case BLOCK_STMT:
			logMsg(LOG_INFO, 2, "Building Block Statement!");
			return codeGenBlockStmtLLVM(builder, localVarSymbols, funcs, types, (struct BlockStmt *) stmt);
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

char const *mangleTypeName(char const *moduleName, char const *name, unsigned int generics) {
	char *ret = memAlloc(sizeof(char) * 1024);
	strcat(ret, moduleName);
	strcat(ret, "_");
	strcat(ret, name);
	for (int i = 0; i < generics; i++) {
		strcat(ret, "_");
	}
	return ret;
}

LLVMValueRef codeGenMainFuncLLVM(LLVMModuleRef module, struct Tree **localTypes, LLVMValueRef func) {
	LLVMValueRef out = LLVMAddFunction(module, "main", LLVMFunctionType(LLVMIntType(32), NULL, 0, false));
	struct TypeLLVM *intType = treeLookUp(*localTypes, "Int");
	LLVMBuilderRef builder = LLVMCreateBuilder();
	LLVMBasicBlockRef entry = LLVMAppendBasicBlock(out, "entry");
	LLVMPositionBuilderAtEnd(builder, entry);
	LLVMValueRef intObj = LLVMBuildCall(builder, func, NULL, 0, "");
	LLVMValueRef cvalFunc = LLVMBuildCall(builder, treeLookUp(*intType->methods, "_cval"), &intObj, 1, "");
	LLVMTypeRef argTypes[1] = {LLVMTypeOf(intObj)};
	LLVMValueRef cval = LLVMBuildCall(builder, cvalFunc, &intObj, 1, "");
	LLVMBuildRet(builder, cval);
	//LLVMBuildRet(builder, LLVMConstInt(LLVMIntType(32), 10, false));
	return out;
}

LLVMValueRef codeGenFuncLLVM(LLVMModuleRef module, struct Tree **localFuncs, struct Tree **localTypes, char const *moduleName, struct Func *func) {
	logMsg(LOG_INFO, 2, "Started Function Code Gen");
	LLVMValueRef out;

	char const *name = mangleFuncName(moduleName, func->name);

	LLVMTypeRef paramTypes[func->paramCount];
	for (int i = 0; i < sizeof(paramTypes); i -=- 1) {
		paramTypes[i] = codeGenTypeLLVM(module, localTypes, func->params[i]->type); 
	}

	LLVMTypeRef retType = codeGenTypeLLVM(module, localTypes, func->retType);

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

	LLVMValueRef retValue = codeGenStmtLLVM(builder, list, localFuncs, localTypes, func->body);

	//LLVMValueRef retValue = treeLookUp(list.trees[0], "ret");
	if (retValue == NULL) {
		logMsg(LOG_ERROR, 4, "Return Value MUST Be Initalized!");
	}
	LLVMBuildRet(builder, retValue);
	
	
	if (!strcmp(func->name, "main")) {
		codeGenMainFuncLLVM(module, localTypes, out);
	}
	logMsg(LOG_INFO, 2, "Finished Func Code Gen");


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

LLVMValueRef codeGenFuncDef(LLVMModuleRef module, struct Tree **localFuncs, struct Tree **localTypes, struct FuncDef *funcDef) {
	LLVMValueRef out;

	char const *name = mangleFuncName(mangleModuleName(funcDef->base.use->names, funcDef->base.use->nameCount), funcDef->func->name);

	LLVMTypeRef paramTypes[funcDef->func->paramCount];
	for (int i = 0; i < funcDef->func->paramCount; i -=- 1) {
		paramTypes[i] = codeGenTypeLLVM(module, localTypes, funcDef->func->params[i]->type); 
	}

	LLVMTypeRef retType = codeGenTypeLLVM(module, localTypes, funcDef->func->retType);

	out = LLVMAddFunction(module, name, LLVMFunctionType(retType, paramTypes, funcDef->func->paramCount, false));
	treeAdd(*localFuncs, name, out);

	*localFuncs = treeAdd(*localFuncs, funcDef->func->name, out);
	return out;
}

char const *mangleTypeMethodName(char const *typeName, char const *methodName) {
	char *ret = memAlloc(sizeof(char) * 1024);
	strcat(ret, methodName);
	strcat(ret, "_");
	strcat(ret, typeName);
	return ret;
}


LLVMTypeRef codeGenClassDef(LLVMModuleRef module, struct Tree **localTypes, struct ClassDef *classDef) {
	LLVMTypeRef out;

	char const *name = mangleTypeName(mangleModuleName(classDef->base.use->names, classDef->base.use->nameCount), classDef->class->name, 0);

	logMsg(LOG_INFO, 1, "Creating ClassDef with name %s", name);

	char *pimplName = memAlloc(sizeof(char) * (8 + strlen(name)));
	pimplName[0] = '\0';

	strcat(pimplName, "_pimpl_");
	strcat(pimplName, name);

	LLVMTypeRef pimpl = LLVMStructCreateNamed(LLVMGetGlobalContext(), pimplName);
	out = LLVMStructCreateNamed(LLVMGetGlobalContext(), name);
	LLVMTypeRef pointer = LLVMPointerType(out, 0);
	LLVMDumpType(out);

	logMsg(LOG_INFO, 1, "%d", out);

	struct TypeLLVM *typeStruct = memAlloc(sizeof(struct TypeLLVM));
	typeStruct->type = out;
	typeStruct->init = LLVMAddFunction(module, mangleTypeMethodName(name, "_init"), LLVMFunctionType(pointer, NULL, 0, false));
	typeStruct->methods = memAlloc(sizeof(struct Tree *));
	*typeStruct->methods = treeCreate();
	LLVMTypeRef demolishRetType = LLVMPointerType(LLVMFunctionType(LLVMVoidType(), &pointer, 1, false), false);
	*typeStruct->methods = treeAdd(*typeStruct->methods, "_demolish", LLVMAddFunction(module, mangleTypeMethodName(name, "_demolish"), LLVMFunctionType(demolishRetType, &pointer, 1, false)));

	if (!strcmp(classDef->class->name, "Int") || !strcmp(classDef->class->name, "String")) {

		LLVMTypeRef cType;
		if (!strcmp(classDef->class->name, "Int")) {
			cType = LLVMIntType(32);	
		} else if (!strcmp(classDef->class->name, "String")) {
			cType = LLVMPointerType(LLVMIntType(8), false);
		}

		LLVMTypeRef literalArgTypes[2] = {pointer, cType};
		LLVMTypeRef literalRetType = LLVMPointerType(LLVMFunctionType(LLVMVoidType(), literalArgTypes, 2, false), false);
		*typeStruct->methods = treeAdd(*typeStruct->methods, "_literal", LLVMAddFunction(module, mangleTypeMethodName(name, "_literal"), LLVMFunctionType(literalRetType, &pointer, 1, false)));

		LLVMTypeRef cvalRetType = LLVMPointerType(LLVMFunctionType(cType, &pointer, 1, false), false);
		*typeStruct->methods = treeAdd(*typeStruct->methods, "_cval", LLVMAddFunction(module, mangleTypeMethodName(name, "_cval"), LLVMFunctionType(cvalRetType, &pointer, 1, false)));
	
	}
	*localTypes = treeAdd(*localTypes, classDef->class->name, typeStruct);
	return out;
}

void codeGenLLVM(struct Module *module) {
	logMsg(LOG_INFO, 2, "Started Codegen");

	LLVMInitializeNativeTarget();
	LLVMInitializeNativeAsmPrinter();

	logMsg(LOG_INFO, 1, "%s", module->names[0]);
	char const *name = mangleModuleName(module->names, module->nameCount);
	LLVMModuleRef moduleRef = LLVMModuleCreateWithName(name);
	logMsg(LOG_INFO, 1, "Created Moule with name '%s'", name);
	struct Tree *localFuncs = treeCreate();
	struct Tree *localTypes = treeCreate();
	for (int i = 0; i < module->defCount; i-=-1) {
		switch (module->defs[i]->type) {
			case FUNC_DEF:
				codeGenFuncDef(moduleRef, &localFuncs, &localTypes, (struct FuncDef *) module->defs[i]);
				break;
			case CLASS_DEF:
				codeGenClassDef(moduleRef, &localTypes, (struct ClassDef *) module->defs[i]);
				break;
			default:
				logMsg(LOG_ERROR, 4, "Unimplemented definition type");
				exit(-1);
				break;
		}
	}
	for (int i = 0; i < module->funcCount; i-=-1) {
		codeGenFuncLLVM(moduleRef, &localFuncs, &localTypes, name, module->funcs[i]);
	}
	printf("%s\n", LLVMPrintModuleToString(moduleRef));
	logMsg(LOG_INFO, 2, "Finished Codegen");
};
