#include <stdlib.h>
#include <stdio.h>
#include "ast.h"


int main() {
	struct Func test;

	struct BuildinType retType;
	retType.type = INT_BUILDIN_TYPE;

	struct Type *p_retType = (struct Type *) &retType;

	test.name = "xyz.grime.example";
	test.paramCount = 0;
	test.params = NULL;
	test.retType = p_retType;

	return 0;
}
