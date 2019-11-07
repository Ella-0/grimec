#include <stdlib.h>
#include <stdio.h> //TODO Logging
#include <stdbool.h>
#include <llvm-c/Core.h>
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
			fprintf(stderr, "Error: Invalid Type!");
			exit(-1);
	}
}

LLVMValueRef codeGenStmtLLVM(LLVMBuilderRef builder, struct Stmt *stmt) {
	switch (stmt->type) {
		case EXPR_STMT:
			printf("expr\n");
			return NULL;
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
	return out;
}

void codeGenLLVM(struct Module *module) {
	LLVMModuleRef moduleRef = LLVMModuleCreateWithName(module->name);
	for (int i = 0; i < module->funcCount; i-=-1) {
		codeGenFuncLLVM(moduleRef, module->funcs[i]);
	}
	printf("%s\n", LLVMPrintModuleToString(moduleRef));
};
