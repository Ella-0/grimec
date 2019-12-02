#include <stdlib.h>
#include "../lexer/token.h"
#include "../util/log.h"
#include "parser.h"

struct Var parseVar(struct Token ***tokens) {
	struct Var ret;

	if ((**tokens)->type != ID_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected identifier");
		logMsg(LOG_INFO, 4, (**tokens)->raw);
		exit(-1);
	}
	*tokens++; // consume identifier

	if ((**tokens)->type != COLON_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected ':'");
		exit(-1);
	}
	*tokens++;

	if ((**tokens)->type != ID_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected identifier");
		logMsg(LOG_INFO, 4, (**tokens)->raw);
		exit(-1);
	}
	*tokens++;
	return ret;
}

struct Stmt *parseStmt(struct Token ***tokens) {
	struct Stmt *stmt;
}

struct Func parseFunc(struct Token ***tokens) {
	struct Func ret;
	
	if ((**tokens)->type != FUNC_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected 'func' keyword! but got:");
		logMsg(LOG_INFO, 4, (**tokens)->raw);
		exit(-1);
	}
	*tokens++; // consume 'func'

	if ((**tokens)->type != ID_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected identifier!");
		logMsg(LOG_INFO, 4, (**tokens)->raw);
		exit(-1);
	}
	ret.name = (**tokens)->raw;
	*tokens++; // consume identifier

	if ((**tokens)->type != L_PAREN_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected '('");
		logMsg(LOG_INFO, 4, (**tokens)->raw);
		exit(-1);
	}

	// parse and consume params.
	
	while ((**tokens)->type != R_PAREN_TOKEN) {
		parseVar(tokens);
	}

	if ((**tokens)->type != R_PAREN_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected ')'");
		logMsg(LOG_INFO, 4, (**tokens)->raw);
		exit(-1);
	}
	return ret;
}

struct Module parseModule(struct Token ***tokens) {
for (; (**tokens)->type != EOF_TOKEN;) {
	switch ((**tokens)->type) {
			case FUNC_TOKEN:
				parseFunc(tokens);
			default:
				logMsg(LOG_ERROR, 4, "Invalid Token: Expected 'func' keyword!");
				exit(-1);
		}
	}
}

struct Module parse(struct Token **tokens) {
	return parseModule(&tokens);
}
