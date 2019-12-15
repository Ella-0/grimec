#include <stdlib.h>
#include "../lexer/token.h"
#include "../util/log.h"
#include "../util/mem.h"
#include "parser.h"

// In the future there will be no buildin types Only types with or without generics. 
// But for bootstrapping String, Int and Bool will be buildin and will be the only types. 
// They will be removed as soon as the compiler is bootstrapped and replaced with classes.
struct BuildinType *parseBuildinType(struct Token const *const **tokens) {
	logMsg(LOG_INFO, 2, "Parsing Buildin Type");
	struct BuildinType *ret = memAlloc(sizeof(struct BuildinType));

	logMsg(LOG_INFO, 1, "Attempting Id Token Consumption");
	if ((**tokens)->type != ID_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected Identifier but got '%s'", (**tokens)->raw);
		exit(-1);
	}
	(*tokens)++;
	ret->base.type = BUILDIN_TYPE;
	ret->type = INT_BUILDIN_TYPE;
	logMsg(LOG_INFO, 1, "Id Token Consumption Successful");

	logMsg(LOG_INFO, 2, "Parsed Buildin Type");
	return ret;
}

struct Type *parseType(struct Token const *const **tokens) {
	logMsg(LOG_INFO, 2, "Parsing Type");
	struct Type *ret = (struct Type *) parseBuildinType(tokens);
	logMsg(LOG_INFO, 2, "Parsed Type");
	return ret;
}

struct Var *parseVar(struct Token const *const **tokens) {
	logMsg(LOG_INFO, 2, "Parsing Var");
	struct Var *ret = memAlloc(sizeof(struct Var));

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

struct Expr *parseExpr(struct Token const *const **tokens);

// terminals

struct Expr *parseFactor(struct Token const *const **tokens) {
	logMsg(LOG_INFO, 2, "Parsing Factor");
	struct Expr *ret;
	switch ((**tokens)->type) {
		case INT_TOKEN: {
				struct IntLiteral *lit = memAlloc(sizeof(struct IntLiteral));
				lit->base.base.type = LITERAL_EXPR;
				lit->base.type = INT_LITERAL;
				lit->val = ((struct IntToken *) (**tokens))->value;
				ret = (struct Expr *) lit;
				logMsg(LOG_INFO, 1, "%d", lit->val);
				(*tokens)++; 
			}
			break;
		case L_PAREN_TOKEN: {
				logMsg(LOG_INFO, 1, "Attempting '(' token consumption");
		    		if ((**tokens)->type != L_PAREN_TOKEN) {
					logMsg(LOG_ERROR, 4, "Invalid Token: Expected '(' but got '%s'", (**tokens)->raw);
					exit(-1);
				}
				(*tokens)++;
				logMsg(LOG_INFO, 1, "'(' Token Consumption Successful");
				ret = parseExpr(tokens);
				logMsg(LOG_INFO, 1, "Attempting ')' token consumption");
		    		if ((**tokens)-> type != R_PAREN_TOKEN) {
					logMsg(LOG_ERROR, 4, "Invalid Token: Expected ')' but got '%s'", (**tokens)->raw);
					exit(-1);
				}
				(*tokens)++;
				logMsg(LOG_INFO, 1, "')' Token Consumption Successful");
			}
			break;
		case ID_TOKEN:
			logMsg(LOG_INFO, 1, "Attempting Id token consumption");
			if ((**tokens)->type != ID_TOKEN) {
				logMsg(LOG_ERROR, 4, "Invalid Token: Expected identifier but got '%s'", (**tokens)->raw);
				exit(-1);
			}
			char const *name = (**tokens)->raw;
			(*tokens)++;
			logMsg(LOG_INFO, 1, "Id Token Consumption Successful");
			
			if ((**tokens)->type == L_PAREN_TOKEN) {
				struct CallExpr *callExpr = memAlloc(sizeof(struct CallExpr));
				callExpr->base.type = CALL_EXPR;
				callExpr->name = name;
				callExpr->argCount = 0;
				callExpr->args = NULL;
				(*tokens)++;
				(*tokens)++;
				ret = (struct Expr *) callExpr;

			} else {
				struct VarExpr *varExpr = memAlloc(sizeof(struct VarExpr));
				varExpr->base.type = VAR_EXPR;
				varExpr->name = name;
				ret = (struct Expr *) varExpr;
			}
			break;
		default:
			logMsg(LOG_ERROR, 4, "Invalid Expression '%s'", (**tokens)->raw);
			exit(-1);
			break;
	}

	logMsg(LOG_INFO, 2, "Parsed Factor");
	return ret;
}

// * /
struct Expr *parseTerm(struct Token const *const **tokens) {
	logMsg(LOG_INFO, 2, "Parsing Term");
	struct Expr *ret = parseFactor(tokens);
	while ((**tokens)->type == MUL_TOKEN || (**tokens)->type == DIV_TOKEN) {
		switch ((**tokens)->type) {
			case MUL_TOKEN: {
					struct BinaryExpr *binaryRet = memAlloc(sizeof(struct BinaryExpr));
					(*tokens)++;
					binaryRet->base.type = BINARY_EXPR;
					binaryRet->lhs = ret;
					binaryRet->rhs = parseTerm(tokens);
					binaryRet->op = MUL_OP;
					ret = (struct Expr *) binaryRet;
				}
				break;
			case DIV_TOKEN: {
					struct BinaryExpr *binaryRet = memAlloc(sizeof(struct BinaryExpr));
					(*tokens)++;
					binaryRet->base.type = BINARY_EXPR;
					binaryRet->lhs = ret;
					binaryRet->rhs = parseTerm(tokens);
					binaryRet->op = DIV_OP;
					ret = (struct Expr *) binaryRet;
				}
				break;

			default:
				logMsg(LOG_ERROR, 4, "Unimplemented Operation '%s'", (**tokens)->raw);
				exit(-1);
		}
	}
	logMsg(LOG_INFO, 2, "Parsed");
	return ret;
}

struct Expr *parseExpr(struct Token const *const **tokens) {
	logMsg(LOG_INFO, 2, "Parsing Expr");
	struct Expr *ret = parseTerm(tokens);
	while ((**tokens)->type == ADD_TOKEN || (**tokens)->type == SUB_TOKEN) {
		switch ((**tokens)->type) {
			case ADD_TOKEN: {
					struct BinaryExpr *binaryRet = memAlloc(sizeof(struct BinaryExpr));
					(*tokens)++;
					binaryRet->base.type = BINARY_EXPR;
					binaryRet->lhs = ret;
					binaryRet->rhs = parseTerm(tokens);
					binaryRet->op = ADD_OP;
					ret = (struct Expr *) binaryRet;
				}
				break;
			case SUB_TOKEN: {
					struct BinaryExpr *binaryRet = memAlloc(sizeof(struct BinaryExpr));
					(*tokens)++;
					binaryRet->base.type = BINARY_EXPR;
					binaryRet->lhs = ret;
					binaryRet->rhs = parseTerm(tokens);
					binaryRet->op = SUB_OP;
					ret = (struct Expr *) binaryRet;
				}
				break;

			default:
				logMsg(LOG_ERROR, 4, "Unimplemented Operation '%s'", (**tokens)->raw);
				exit(-1);
		}
	}
	logMsg(LOG_INFO, 2, "Parsed Expr");
	return ret;
}

struct Stmt *parseStmt(struct Token const *const **tokens);

void pushStmt(struct Stmt ***buffer, int *count, struct Stmt *stmt) {
	(*count)++;
	(*buffer) = memRealloc(*buffer, sizeof(struct Stmt *) * *count);
	(*buffer)[(*count) - 1 ] = stmt;
}

struct Stmt *parseBlockStmt(struct Token const *const **tokens) {
	logMsg(LOG_INFO, 2, "Parsing Block Stmt");
	struct BlockStmt *out = memAlloc(sizeof(struct BlockStmt));

	out->base.type = BLOCK_STMT;

	logMsg(LOG_INFO, 1, "Attempting '{' Token Consumption");
	if ((**tokens)->type != L_BRACE_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected '{' but got '%s'", (**tokens)->raw);
		exit(-1);
	}	
	(*tokens)++;
	logMsg(LOG_INFO, 1, "'{' Token Consumption Successful");

	out->stmts = NULL;
	out->stmtCount = 0;

	while ((**tokens)->type != R_BRACE_TOKEN) {
		pushStmt(&out->stmts, &out->stmtCount, parseStmt(tokens));
	}
	(*tokens)++;
	logMsg(LOG_INFO, 1, "'}' Token Consumption Successful");

	logMsg(LOG_INFO, 2, "Parsed Block Stmt");
	return (struct Stmt *) out;
}

struct Stmt *parseVarStmt(struct Token const *const **tokens) {
	logMsg(LOG_INFO, 2, "Parsing Var Stmt");
	struct VarStmt *out = memAlloc(sizeof(struct VarStmt));
	out->base.type = VAR_STMT;

	logMsg(LOG_INFO, 1, "Attempting 'var' Token Consumption");
	if ((**tokens)->type != VAR_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected 'var' but got '%s'", (**tokens)->raw);
		exit(-1);
	}
	(*tokens)++;
	logMsg(LOG_INFO, 1, "'var' Token Consumption Successful");

	logMsg(LOG_INFO, 1, "Attempting Id token consumption");
	if ((**tokens)->type != ID_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected identifier but got '%s'", (**tokens)->raw);
		exit(-1);
	}

	out->var = memAlloc(sizeof(struct Var));

	out->var->name = (**tokens)->raw;
	(*tokens)++;
	logMsg(LOG_INFO, 1, "Id Token Consumption Successful");

	logMsg(LOG_INFO, 1, "Attempting ':' token consumption");
	if ((**tokens)->type != COLON_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected ':' but got '%s'", (**tokens)->raw);
		exit(-1);
	}
	(*tokens)++;
	logMsg(LOG_INFO, 1, "':' Token Consumption Successful");

	out->var->type = parseType(tokens);

	logMsg(LOG_INFO, 1, "Attempting '=' token consumption");
	if ((**tokens)->type != EQUALS_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected '=' but got '%s'", (**tokens)->raw);
		exit(-1);
	}
	(*tokens)++;

	out->init = parseExpr(tokens);
	logMsg(LOG_INFO, 1, "Attempting ';' token consumption");
	if ((**tokens)->type != SEMI_COLON_TOKEN) {
		logMsg(LOG_ERROR, 1, "Invalid Token: Expected ';' but got '%s'", (**tokens)->raw);
		exit(-1);
	}
	(*tokens)++;
	logMsg(LOG_INFO, 1, "';' Token consumption successful");
	logMsg(LOG_INFO, 2, "Parsed Var Stmt");
	return (struct Stmt *) out;
}

// ID = Expr
struct Stmt *parseAssignStmt(struct Token const *const **tokens) {
	logMsg(LOG_INFO, 2, "Parsing Assign Stmt");
	struct AssignStmt *out = memAlloc(sizeof(struct AssignStmt));
	out->base.type = ASSIGN_STMT;

	logMsg(LOG_INFO, 1, "Attempting Id token consumption");
	if ((**tokens)->type != ID_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected identifier but got '%s'", (**tokens)->raw);
		exit(-1);
	}
	(*tokens)++;
	struct Var *var = memAlloc(sizeof(struct Var));
	var->name = ((struct IdToken *)(**tokens))->value;
	var->name = "ret";
	out->var = var;
	logMsg(LOG_INFO, 1, "Id Token Consumption Successful");

	logMsg(LOG_INFO, 1, "Attempting '=' token consumption");
	if ((**tokens)->type != EQUALS_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected '=' but got '%s'", (**tokens)->raw);
		exit(-1);
	}
	(*tokens)++;
	logMsg(LOG_INFO, 1, "'=' Token Consumption Successful");

	out->init = parseExpr(tokens);
	logMsg(LOG_INFO, 1, "Attempting ';' token consumption");
	if ((**tokens)->type != SEMI_COLON_TOKEN) {
		logMsg(LOG_ERROR, 1, "Invalid Token: Expected ';' but got '%s'", (**tokens)->raw);
		exit(-1);
	}
	(*tokens)++;
	logMsg(LOG_INFO, 1, "';' Token consumption successful");
	logMsg(LOG_INFO, 2, "Parsed Assign Stmt");
	return (struct Stmt *) out;
}

struct Stmt *parseExprStmt(struct Token const *const **tokens) {
	logMsg(LOG_INFO, 2, "Parsing Expr");
	struct ExprStmt *ret = memAlloc(sizeof(struct ExprStmt));
	ret->base.type = EXPR_STMT;
	ret->expr = parseExpr(tokens);

	logMsg(LOG_INFO, 1, "Attempting ';' token consumption");
	if ((**tokens)->type != SEMI_COLON_TOKEN) {
		logMsg(LOG_ERROR, 1, "Invalid Token: Expected ';' but got '%s'", (**tokens)->raw);
		exit(-1);
	}
	(*tokens)++;
	logMsg(LOG_INFO, 1, "';' Token consumption successful");
	logMsg(LOG_INFO, 2, "Parsed Expr");
	return (struct Stmt *) ret;
}

struct Stmt *parseStmt(struct Token const *const **tokens) {
	logMsg(LOG_INFO, 2, "Parsing Statement");
	struct Stmt *stmt;
	switch ((**tokens)->type) {
		case L_BRACE_TOKEN:
			stmt = parseBlockStmt(tokens);
			break;
		case VAR_TOKEN:
			stmt = parseVarStmt(tokens);
			break;
		case ID_TOKEN:
			stmt = parseAssignStmt(tokens);
			break;
		default:
			logMsg(LOG_ERROR, 4, "Unexpected Token: %s", (**tokens)->raw);
			exit(-1);
			break;
	}
	logMsg(LOG_INFO, 2, "Parsed Statement");
	return stmt;
}

void pushVar(struct Var ***buffer, int *count, struct Var *var) {
	(*count)++;
	(*buffer) = memRealloc(*buffer, sizeof(struct Var *) * *count);
	(*buffer)[(*count) - 1 ] = var;
}

struct Func *parseFunc(struct Token const *const **tokens) {
	logMsg(LOG_INFO, 2, "Parsing Function");
	struct Func *ret = memAlloc(sizeof(struct Func));
	
	logMsg(LOG_INFO, 1, "Attempting Func Token Consumption");
	if ((**tokens)->type != FUNC_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected 'func' keyword but got '%s'", (**tokens)->raw);
		exit(-1);
	}
	(*tokens)++; // consume 'func'
	logMsg(LOG_INFO, 1, "Func Token Consumption Successful");

	logMsg(LOG_INFO, 1, "Attempting Id Token Consumption");
	if ((**tokens)->type != ID_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected identifier but got '%s'", (**tokens)->raw);
		logMsg(LOG_INFO, 4, (**tokens)->raw);
		exit(-1);
	}
	ret->name = (**tokens)->raw;
	(*tokens)++; // consume identifier
	logMsg(LOG_INFO, 1, "Id Token Consumption Successful");

	logMsg(LOG_INFO, 1, "Attempting '(' Token Consumption");
	if ((**tokens)->type != L_PAREN_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected '(' but got '%s'", (**tokens)->raw);
		exit(-1);
	}
	(*tokens)++;
	logMsg(LOG_INFO, 1, "'(' Token Consumption Successful");

	// parse and consume params.
	
	ret->params = NULL;
	ret->paramCount = 0;

	while ((**tokens)->type != R_PAREN_TOKEN) {
		pushVar(&ret->params, &ret->paramCount, parseVar(tokens));
	}

	logMsg(LOG_INFO, 1, "Attempting ')' Token Consumption");
	if ((**tokens)->type != R_PAREN_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected ')'");
		logMsg(LOG_INFO, 4, (**tokens)->raw);
		exit(-1);
	}
	logMsg(LOG_INFO, 1, "'(' Token Consumption Successful");
	(*tokens)++;

	logMsg(LOG_INFO, 1, "Attempting '->' Token Consumption");
	if ((**tokens)->type != ARROW_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected '->' but got '%s'", (**tokens)->raw);
		exit(-1);
	}
	(*tokens)++;
	logMsg(LOG_INFO, 1, "'->' Token Conumption Successful");

	ret->retType = parseType(tokens);
	ret->body = parseStmt(tokens);

	logMsg(LOG_INFO, 2, "Parsed Function");
	return ret;
}

void pushFunc(struct Func ***buffer, int *count, struct Func *func) {
	(*count)++;
	(*buffer) = memRealloc(*buffer, sizeof(struct Func *) * *count);
	(*buffer)[(*count) - 1 ] = func;
}

void pushString(char const ***buffer, int *count, char const *string) {
	(*count)++;
	(*buffer) = memRealloc(*buffer, sizeof(char const *) * *count);
	(*buffer)[(*count) - 1 ] = string;
}

char const **parseModuleName(struct Token const *const **tokens, unsigned int *count) {
	char const **ret = NULL;
	logMsg(LOG_INFO, 2, "Parsing Module Name");

	logMsg(LOG_INFO, 1, "Attempting 'mod' token consumption");
	if ((**tokens)->type != MOD_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected 'mod' but got %s", (**tokens)->raw);
		exit(-1);
	}
	(*tokens)++;
	logMsg(LOG_INFO, 1, "'mod' Token Consumption Successful");

	logMsg(LOG_INFO, 1, "Attempting Id token consumption");
	if ((**tokens)->type != ID_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected identifier but got '%s'", (**tokens)->raw);
		exit(-1);
	}
	pushString(&ret, count, (**tokens)->raw);
	(*tokens)++;
	logMsg(LOG_INFO, 1, "Id token consumption successful");

	while ((**tokens)->type == DOUBLE_COLON_TOKEN) {
		logMsg(LOG_INFO, 1, "Attempting '::' token consumption");
		if ((**tokens)->type != DOUBLE_COLON_TOKEN) {
			logMsg(LOG_ERROR, 4, "Invalid Token: Expected '::' but got ''%s");
			exit(-1);
		}
		(*tokens)++;
		logMsg(LOG_INFO, 1, "'::' Token Consumption Successful");
		logMsg(LOG_INFO, 1, "Attempting Id token consumption");
		if ((**tokens)->type != ID_TOKEN) {
			logMsg(LOG_ERROR, 4, "Invalid Token: Expected identifier but got '%s'", (**tokens)->raw);
			exit(-1);
		}
		pushString(&ret, count, (**tokens)->raw);
		(*tokens)++;
		logMsg(LOG_INFO, 1, "Id token consumption successful");
	}

	logMsg(LOG_INFO, 2, "Parsed Module Name");
	return ret;
}

struct Use *parseUse(struct Token const *const **tokens) {
	struct Use *ret = memAlloc(sizeof(struct Use));
	logMsg(LOG_INFO, 2, "Parsing Use");

	logMsg(LOG_INFO, 1, "Attempting 'use' token consumption");
	if ((**tokens)->type != USE_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected 'use' but got '%s'", (**tokens)->raw);
		exit(-1);
	}
	(*tokens)++;
	logMsg(LOG_INFO, 1, "'use' token consumption successful");

	ret->names = NULL;
	ret->nameCount = 0;

	logMsg(LOG_INFO, 1, "Attempting Id token consumption");
	if ((**tokens)->type != ID_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected identifier but got '%s'", (**tokens)->raw);
		exit(-1);
	}
	pushString(&ret->names, &ret->nameCount, (**tokens)->raw);
	(*tokens)++;
	logMsg(LOG_INFO, 1, "Id token consumption successful");

	while ((**tokens)->type == DOUBLE_COLON_TOKEN) {
		logMsg(LOG_INFO, 1, "Attempting '::' token consumption");
		if ((**tokens)->type != DOUBLE_COLON_TOKEN) {
			logMsg(LOG_ERROR, 4, "Invalid Token: Expected '::' but got ''%s");
			exit(-1);
		}
		(*tokens)++;
		logMsg(LOG_INFO, 1, "'::' Token Consumption Successful");
		logMsg(LOG_INFO, 1, "Attempting Id token consumption");
		if ((**tokens)->type != ID_TOKEN) {
			logMsg(LOG_ERROR, 4, "Invalid Token: Expected identifier but got '%s'", (**tokens)->raw);
			exit(-1);
		}
		pushString(&ret->names, &ret->nameCount, (**tokens)->raw);
		(*tokens)++;
		logMsg(LOG_INFO, 1, "Id token consumption successful");
	}

	logMsg(LOG_INFO, 2, "Parsed Use");
	return ret;
}

void pushUse(struct Use ***buffer, int *count, struct Use *use) {
	(*count)++;
	(*buffer) = memRealloc(*buffer, sizeof(struct Use *) * *count);
	(*buffer)[(*count) - 1 ] = use;
}

struct Def *parseDef(struct Token const *const **tokens) {
	struct Def def;
	struct Def *out;
	struct Use *use = memAlloc(sizeof(struct Use));
	logMsg(LOG_INFO, 2, "Parsing Use");

	logMsg(LOG_INFO, 1, "Attempting 'from' token consumption");
	if ((**tokens)->type != FROM_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected 'from' but got '%s'", (**tokens)->raw);
		exit(-1);
	}
	(*tokens)++;
	logMsg(LOG_INFO, 1, "'from' token consumption successful");

	use->names = NULL;
	use->nameCount = 0;

	logMsg(LOG_INFO, 1, "Attempting Id token consumption");
	if ((**tokens)->type != ID_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected identifier but got '%s'", (**tokens)->raw);
		exit(-1);
	}
	pushString(&use->names, &use->nameCount, (**tokens)->raw);
	(*tokens)++;
	logMsg(LOG_INFO, 1, "Id token consumption successful");

	while ((**tokens)->type == DOUBLE_COLON_TOKEN) {
		logMsg(LOG_INFO, 1, "Attempting '::' token consumption");
		if ((**tokens)->type != DOUBLE_COLON_TOKEN) {
			logMsg(LOG_ERROR, 4, "Invalid Token: Expected '::' but got ''%s");
			exit(-1);
		}
		(*tokens)++;
		logMsg(LOG_INFO, 1, "'::' Token Consumption Successful");
		logMsg(LOG_INFO, 1, "Attempting Id token consumption");
		if ((**tokens)->type != ID_TOKEN) {
			logMsg(LOG_ERROR, 4, "Invalid Token: Expected identifier but got '%s'", (**tokens)->raw);
			exit(-1);
		}
		pushString(&use->names, &use->nameCount, (**tokens)->raw);
		(*tokens)++;
		logMsg(LOG_INFO, 1, "Id token consumption successful");
	}

	def.use = use;

	logMsg(LOG_INFO, 1, "Attempting 'def' token consumption");
	if ((**tokens)->type != DEF_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected 'def' but got '%s'", (**tokens)->raw);
	}
	(*tokens)++;
	logMsg(LOG_INFO, 1, "'def' token consumption successful");

	switch ((**tokens)->type) {
		case FUNC_TOKEN: {
				struct FuncDef *funcDef = memAlloc(sizeof(struct FuncDef));
				funcDef->base = def;
				funcDef->base.type = FUNC_DEF;
				struct Func *ret = memAlloc(sizeof(struct Func));
	
				logMsg(LOG_INFO, 1, "Attempting Func Token Consumption");
				if ((**tokens)->type != FUNC_TOKEN) {
					logMsg(LOG_ERROR, 4, "Invalid Token: Expected 'func' keyword but got '%s'", (**tokens)->raw);
					exit(-1);
				}
				(*tokens)++; // consume 'func'
				logMsg(LOG_INFO, 1, "Func Token Consumption Successful");
		
				logMsg(LOG_INFO, 1, "Attempting Id Token Consumption");
				if ((**tokens)->type != ID_TOKEN) {
					logMsg(LOG_ERROR, 4, "Invalid Token: Expected identifier but got '%s'", (**tokens)->raw);
					logMsg(LOG_INFO, 4, (**tokens)->raw);
					exit(-1);
				}
				ret->name = (**tokens)->raw;
				(*tokens)++; // consume identifier
				logMsg(LOG_INFO, 1, "Id Token Consumption Successful");
			
				logMsg(LOG_INFO, 1, "Attempting '(' Token Consumption");
				if ((**tokens)->type != L_PAREN_TOKEN) {
					logMsg(LOG_ERROR, 4, "Invalid Token: Expected '(' but got '%s'", (**tokens)->raw);
					exit(-1);
				}
				(*tokens)++;
				logMsg(LOG_INFO, 1, "'(' Token Consumption Successful");
			
				// parse and consume params.
				
				ret->params = NULL;
				ret->paramCount = 0;
			
				while ((**tokens)->type != R_PAREN_TOKEN) {
					pushVar(&ret->params, &ret->paramCount, parseVar(tokens));
				}
			
				logMsg(LOG_INFO, 1, "Attempting ')' Token Consumption");
				if ((**tokens)->type != R_PAREN_TOKEN) {
					logMsg(LOG_ERROR, 4, "Invalid Token: Expected ')' but got '%s'", (**tokens)->raw);
					exit(-1);
				}
				(*tokens)++;
				logMsg(LOG_INFO, 1, "'(' Token Consumption Successful");
			
				logMsg(LOG_INFO, 1, "Attempting '->' Token Consumption");
				if ((**tokens)->type != ARROW_TOKEN) {
					logMsg(LOG_ERROR, 4, "Invalid Token: Expected '->' but got '%s'", (**tokens)->raw);
					exit(-1);
				}
				(*tokens)++;
				logMsg(LOG_INFO, 1, "'->' Token Conumption Successful");
			
				ret->retType = parseType(tokens);
				ret->body = NULL;
				
				funcDef->func = ret;

				out = (struct Def *) funcDef;
			}
			break;
		default:
			logMsg(LOG_ERROR, 4, "Unimplemented definition type '%s'", (**tokens)->raw);
			exit(-1);
	}

	logMsg(LOG_INFO, 2, "Parsed Def");
	return out;
}

void pushDef(struct Def ***buffer, int *count, struct Def *def) {
	(*count)++;
	(*buffer) = memRealloc(*buffer, sizeof(struct Def *) * *count);
	(*buffer)[(*count) - 1 ] = def;
}

struct Module parseModule(struct Token const *const **tokens) {
	struct Module module;
	logMsg(LOG_INFO, 2, "Parsing Module");
	module.nameCount = 0;
	module.names = parseModuleName(tokens, &module.nameCount);
	module.funcCount = 0;
	module.funcs = NULL;
	module.includes = NULL;
	module.includeCount = 0;
	module.defs = NULL;
	module.defCount = 0;
	while ((**tokens)->type != EOF_TOKEN) {
		switch ((**tokens)->type) {
			case FUNC_TOKEN:
				pushFunc(&module.funcs, &module.funcCount, parseFunc(tokens));
				break;
			case USE_TOKEN:
				pushUse(&module.includes, &module.includeCount, parseUse(tokens));
				break;
			case FROM_TOKEN:
				pushDef(&module.defs, &module.defCount, parseDef(tokens));
				break;
			default:
				logMsg(LOG_ERROR, 4, "Invalid Token: Expected 'func' keyword not '%s'", (**tokens)->raw);
				exit(-1);
		}
	}
	logMsg(LOG_INFO, 2, "Parsed Module");
	return module;
}

struct Module parse(struct Token const *const *tokens) {
	return parseModule(&tokens);
}
