#include <stdlib.h>
#include <stdio.h>
#include "util/log.h"
#include "util/mem.h"
#include "code-gen/llvm/llvm-code-gen.h"
#include "lexer/token.h"
#include "lexer/lex.h"
#include "parser/parser.h"

const char * testFile = "func main() -> Int ret = 10;";

void logTokens(struct Token const *const *tokens) {
	struct Token const *const *token = tokens;
	for (; (*token) == NULL || (*token)->type != EOF_TOKEN; token++) {
		if (*token == NULL) {
			logMsg(LOG_INFO, 2, "NULL");
		} else {
			logMsg(LOG_INFO, 2, "%s", (*token)->raw);
		}
	}
}

int main() {
	setLogLevel(0);

	struct Token const *const *tokens = lex(testFile);
	
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

	logTokens(tokens);
	codeGenLLVM(&module);
	struct Module tree = parse(tokens);
	codeGenLLVM(&tree);

	logMsg(LOG_ERROR, 1, "Memory Still Allocated: %d", memLeaks());
	return 0;
}
