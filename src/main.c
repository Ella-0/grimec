#include <stdlib.h>
#include <stdio.h>
#include "code-gen/llvm/llvm-code-gen.h"

int main() {
	struct Func test;

	struct BuildinType retType;
	retType.base.type = BUILDIN_TYPE;
	retType.type = INT_BUILDIN_TYPE;

	struct IntLiteral expr;
	expr.base.base.type = LITERAL_EXPR;
	expr.base.type = INT_LITERAL;
	expr.val = 1;

	struct Var var;
	var.type = &retType.base;
	var.name = "ret";

	struct AssignStmt body;
	body.base.type = ASSIGN_STMT;
	body.var = &var;
	body.init = &expr.base.base;

	test.name = "xyz_grime_example_main";
	test.paramCount = 0;
	test.params = NULL;
	test.retType = &retType.base;
	test.body = &body.base;

	struct Func *testp = &test;

	struct Module module;
	module.name = "xyz_grime_example";
	module.funcCount = 1;
	module.funcs = &testp;

	codeGenLLVM(&module);

	return 0;
}
