#include <stdlib.h>
#include <stdio.h>
#include "ast.h"


int main() {
	struct Func test;

	struct BuildinType retType;
	retType.base.type = BUILDIN_TYPE;
	retType.type = INT_BUILDIN_TYPE;

	struct IntLiteral expr;
	expr.base.base.type = LITERAL_EXPR;
	expr.base.type = INT_LITERAL;
	expr.val = 1;

	struct ExprStmt body;
	body.base.type = EXPR_STMT;
	body.expr = &expr.base.base;

	test.name = "xyz.grime.example";
	test.paramCount = 0;
	test.params = NULL;
	test.retType = &retType.base;
	test.body = &body.base;
	return 0;
}
