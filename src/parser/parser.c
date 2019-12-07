#include <stdlib.h>
#include "../lexer/token.h"
#include "../util/log.h"
#include "parser.h"

struct Var parseVar(struct Token const *const **tokens) {
	logMsg(LOG_INFO, 2, "Parsing Var");
	struct Var ret;

	if ((**tokens)->type != ID_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected identifier");
		logMsg(LOG_INFO, 4, (**tokens)->raw);
		exit(-1);
	}
	(*tokens)++; // consume identifier

	if ((**tokens)->type != COLON_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected ':'");
		exit(-1);
	}
	(*tokens)++;

	if ((**tokens)->type != ID_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected identifier");
		logMsg(LOG_INFO, 4, (**tokens)->raw);
		exit(-1);
	}
	(*tokens)++;
	return ret;
}

struct Stmt *parseStmt(struct Token const *const **tokens) {
	logMsg(LOG_INFO, 2, "Parsing Statement");
	struct Stmt *stmt;
	return stmt;
}

struct Func parseFunc(struct Token const *const **tokens) {
	logMsg(LOG_INFO, 2, "Parsing Function");
	struct Func ret;
	
	logMsg(LOG_INFO, 1, "Attempting Func Token Consumption");
	if ((**tokens)->type != FUNC_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected 'func' keyword! but got:");
		logMsg(LOG_INFO, 4, (**tokens)->raw);
		exit(-1);
	}
	(*tokens)++; // consume 'func'
	logMsg(LOG_INFO, 1, "Func Token Consumption Successful");

	logMsg(LOG_INFO, 1, "Attempting Id Token Consumption");
	logMsg(LOG_INFO, 1, tokens == NULL ? "Null":"Not Null");
	logMsg(LOG_INFO, 1, *tokens == NULL ? "Null":"Not Null");
	logMsg(LOG_INFO, 1, **tokens == NULL ? "Null":"Not Null");
	if ((**tokens)->type != ID_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected identifier!");
		logMsg(LOG_INFO, 4, (**tokens)->raw);
		exit(-1);
	}
	logMsg(LOG_INFO, 1, "Id Token Found");
	ret.name = (**tokens)->raw;
	(*tokens)++; // consume identifier
	logMsg(LOG_INFO, 1, "Id Token Consumption Successful");

	logMsg(LOG_INFO, 1, "Attempting '(' Token Consumption");
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

struct Module parseModule(struct Token const *const **tokens) {
	struct Module module;
	logMsg(LOG_INFO, 2, "Parsing Module");
	while ((**tokens)->type != EOF_TOKEN) {
		switch ((**tokens)->type) {
			case FUNC_TOKEN:
				parseFunc(tokens);
				break;
			default:
				logMsg(LOG_ERROR, 4, "Invalid Token: Expected 'func' keyword!");
				exit(-1);
		}
	}
	return module;
}

struct Module parse(struct Token const *const *tokens) {
	return parseModule(&tokens);
}
