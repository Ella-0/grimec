#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../lexer/token.h"
#include "../util/colours.h"
#include "../util/log.h"
#include "../util/mem.h"
#include "parser.h"

void syntaxError(char const weak *location, unsigned int line, unsigned int column, char const *expected, char const *actual) {
    logMsg(LOG_ERROR, 4, RED "Syntax Error while parsing %s: "
            MAGENTA "Unexpected Token at "
            BLUE "%d:%d: "
            DEFAULT "Expected "
            YELLOW "%s "
            DEFAULT "but got "
            YELLOW "'%s'", location, line, column, expected, actual);
    exit(EXIT_FAILURE);
}

struct Token const weak *consumeToken(struct Token const *const **tokens, enum TokenType type, char const *string, char const weak *location) {
    logMsg(LOG_INFO, 1, "Attempting %s Token Consumption", string);
    if ((**tokens)->type != type) {
        syntaxError(location, (**tokens)->line, (**tokens)->column, string, (**tokens)->raw);
    }
    (*tokens)++;
    return *((*tokens)-1);
}

// In the future there will be no buildin types Only types with or without generics. 
// But for bootstrapping String, Int and Bool will be buildin and will be the only types. 
// They will be removed as soon as the compiler is bootstrapped and replaced with classes.
/*struct BuildinType *parseBuildinType(struct Token const *const **tokens) {
	logMsg(LOG_INFO, 2, "Parsing Buildin Type");
	struct BuildinType *ret = (struct BuildinType *) memAlloc(sizeof(struct BuildinType));

	logMsg(LOG_INFO, 1, "Attempting Id Token Consumption");
	if ((**tokens)->type != ID_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected Identifier but got '%s'", (**tokens)->raw);
		exit(-1);
	}
	ret->base.type = BUILDIN_TYPE;
	logMsg(LOG_INFO, 1, "Type Id: '%s'", (**tokens)->raw);
	if (!strcmp((**tokens)->raw, "Void")) {
		ret->type = VOID_BUILDIN_TYPE;
	} 
	(*tokens)++;
	logMsg(LOG_INFO, 1, "Id Token Consumption Successful");

	logMsg(LOG_INFO, 2, "Parsed Buildin Type");
	return ret;
}*/

void pushVar(struct Var ***buffer, unsigned int *count, struct Var *var) {
	(*count)++;
	(*buffer) = memRealloc(*buffer, sizeof(struct Var *) * *count);
	(*buffer)[(*count) - 1 ] = var;
}

struct Type *parseType(struct Token const *const **tokens);

struct Type strong *parseTupleType(struct Token const *const **tokens) {
    logMsg(LOG_INFO, 2, "Parsing Tuple Type");
    
    struct TupleType strong *ret = memAlloc(sizeof(struct TupleType));

    ret->base.type = TUPLE_TYPE;

    consumeToken(tokens, L_PAREN_TOKEN, "'('", "Tuple Type");

    ret->vars = NULL;
    ret->varCount = 0;

    for (unsigned int count = 0; (**tokens)->type != R_PAREN_TOKEN; count++) {
        if (count != 0) {
            consumeToken(tokens, COMMA_TOKEN, "','", "Tuple Type");
        }

        struct Var strong *var = memAlloc(sizeof(struct Var));
        var->name = consumeToken(tokens, ID_TOKEN, "Identifier", "Tuple Type")->raw;

        switch ((**tokens)->type) {
            case COLON_TOKEN: {
                    consumeToken(tokens, COLON_TOKEN, "':'", "Tuple Type");
                    var->type = parseType(tokens);
                }
                break;
            default:
                break;
        }
        pushVar(&ret->vars, &ret->varCount, var);
    }

    consumeToken(tokens, R_PAREN_TOKEN, "')'", "Tuple Type");

    logMsg(LOG_INFO, 2, "Parsed Tuple Type");

    return (struct Type strong *) ret;
}

struct Type strong *parseArrayType(struct Token const *const **tokens) {
    logMsg(LOG_INFO, 2, "Parsing Array Type");

    struct ArrayType strong *ret = (struct ArrayType strong *) memAlloc(sizeof(struct ArrayType));
    ret->base.type = ARRAY_TYPE;
    
    consumeToken(tokens, L_BRACKET_TOKEN, "'['", "Array Type");
    
    switch ((**tokens)->type) {
        case R_BRACKET_TOKEN: {
                ret->typed = false;
                consumeToken(tokens, R_BRACKET_TOKEN, "']'", "Array Type");
            }
            break;
        
        case SEMI_COLON_TOKEN: {
                ret->sized = true;
                consumeToken(tokens, SEMI_COLON_TOKEN, "';'", "Array Type");
                struct IntToken const weak *sizeToken = (struct IntToken const weak *) consumeToken(tokens, INT_TOKEN, "Integer", "Array Type");
                ret->elementCount = sizeToken->value;                  
                consumeToken(tokens, R_BRACKET_TOKEN, "']'", "Array Type");
            }
            break;

        default: {
                ret->typed = true;
                ret->type = parseType(tokens);
                ret->sized = false;
                consumeToken(tokens, R_BRACKET_TOKEN, "']'", "Array Type");
            }
            break;
    }

    return (struct Type strong *) ret;
}

struct Type strong *parseSimpleType(struct Token const strong *const strong **weak tokens) {
    logMsg(LOG_INFO, 2, "Parsing Simple Type");
    
    struct SimpleType strong *ret = (struct SimpleType *) memAlloc(sizeof(struct SimpleType));
    ret->base.type = SIMPLE_TYPE;

    struct Token const weak *c = consumeToken(tokens, ID_TOKEN, "Identifier", "Simple Type");
    ret->name = c->raw;

    logMsg(LOG_INFO, 2, "Parsed Simple Type");
    return (struct Type strong *) ret;
}

struct Type *parseType(struct Token const *const **tokens) {
	logMsg(LOG_INFO, 2, "Parsing Type");

    struct Type strong *ret = NULL;

    switch ((**tokens)->type) {
        case ID_TOKEN: {
                ret = (struct Type strong *) parseSimpleType(tokens);
            }
            break;

        case L_PAREN_TOKEN: {
                ret = (struct Type strong *) parseTupleType(tokens);
            }
            break;

        case L_BRACKET_TOKEN: {
                ret = (struct Type strong *) parseArrayType(tokens);
            }
            break;

        default:
            syntaxError("Type", (**tokens)->line, (**tokens)->column, "Identifier" DEFAULT ", " YELLOW 
                    "'('" DEFAULT " or " YELLOW "'['", (**tokens)->raw);
    }

	//struct SimpleType *ret = (struct SimpleType *) memAlloc(sizeof(struct SimpleType));
	logMsg(LOG_INFO, 2, "Parsed Type");
	return (struct Type *) ret;
}

struct Var *parseParam(struct Token const *const **tokens) {
	logMsg(LOG_INFO, 2, "Parsing Var");
	struct Var *ret = (struct Var *) memAlloc(sizeof(struct Var));

	ret->name = consumeToken(tokens, ID_TOKEN, "Identifier", "Var")->raw;
	
    consumeToken(tokens, COLON_TOKEN, "':'", "Var");

	ret->type = parseType(tokens);

	logMsg(LOG_INFO, 2, "Parsed Var");
    return ret;
}

struct Expr *parseExpr(struct Token const *const **tokens);

void pushExpr(struct Expr ***buffer, unsigned int *count, struct Expr *expr) {
	(*count)++;
	(*buffer) = (struct Expr **) memRealloc(*buffer, sizeof(struct Expr *) * *count);
	(*buffer)[(*count) - 1 ] = expr;
}

// terminals

struct Expr strong *parseFactor(struct Token const *const **tokens) {
	logMsg(LOG_INFO, 2, "Parsing Factor");
	struct Expr strong *ret;
	switch ((**tokens)->type) {
		case INT_TOKEN: {
				struct IntLiteral *lit =(struct IntLiteral *)  memAlloc(sizeof(struct IntLiteral));
                lit->base.base.type = LITERAL_EXPR;
				lit->base.base.evalType = memAlloc(sizeof(struct SimpleType));
                lit->base.base.evalType->type = SIMPLE_TYPE;
                ((struct SimpleType *) lit->base.base.evalType)->name = heapString("Int");
                lit->base.type = INT_LITERAL;
				lit->val = ((struct IntToken *) consumeToken(tokens, INT_TOKEN, "Integer", "Factor"))->value;
				ret = (struct Expr *) lit;
				logMsg(LOG_INFO, 1, "Created Int Literal with value: %d", lit->val);
			}
			break;
		case BOOL_TOKEN: {
				struct BoolLiteral strong *lit = (struct BoolLiteral strong *) memAlloc(sizeof(struct BoolLiteral)); 
				lit->base.base.type = LITERAL_EXPR;
				lit->base.type = BOOL_LITERAL;
				lit->val = ((struct BoolToken *) consumeToken(tokens, BOOL_TOKEN, "Boolean", "Factor"))->value;
				ret = (struct Expr *) lit;
				logMsg(LOG_INFO, 1, "Created Bool Literal with value: %d", lit->val);
			}
			break;
		case STRING_TOKEN: {
				struct StringLiteral *lit = (struct StringLiteral *) memAlloc(sizeof(struct StringLiteral));
				lit->base.base.type = LITERAL_EXPR;
				lit->base.type = STRING_LITERAL;
				lit->val = ((struct StringToken *) consumeToken(tokens, STRING_TOKEN, "String", "Factor"))->value;
				ret = (struct Expr *) lit;
				logMsg(LOG_INFO, 1, "Created String Literal with value: %s", lit->val);
			}
			break;
		case CHAR_TOKEN: {
				struct CharLiteral *lit = (struct CharLiteral *) memAlloc(sizeof(struct CharLiteral));
				lit->base.base.type = LITERAL_EXPR;
				lit->base.type = CHAR_LITERAL;
				lit->val = ((struct CharToken *) consumeToken(tokens, CHAR_TOKEN, "Char", "Factor"))->value;
				ret = (struct Expr *) lit;
				logMsg(LOG_INFO, 1, "Cheated Char Literal with value: %c", lit->val);
			}
			break;
		case BYTE_TOKEN: {
				struct ByteLiteral strong *lit = (struct ByteLiteral strong *) memAlloc(sizeof(struct ByteLiteral));
				lit->base.base.type = LITERAL_EXPR;
				lit->base.type = BYTE_LITERAL;
				lit->val = ((struct ByteToken weak *) consumeToken(tokens, BYTE_TOKEN, "Byte", "Factor"))->value;
				ret = (struct Expr strong *) lit;
				logMsg(LOG_INFO, 1, "Created Byte Literal with value: %d", lit->val);
			}
			break;
        case L_BRACKET_TOKEN: {
                struct ArrayLiteral strong *lit = (struct ArrayLiteral strong *) memAlloc(sizeof(struct ArrayLiteral));
                lit->base.base.type = LITERAL_EXPR;
                lit->base.type = ARRAY_LITERAL;
                consumeToken(tokens, L_BRACKET_TOKEN, "'['", "Factor");
                switch ((**tokens)->type) {
                    case SEMI_COLON_TOKEN: {
                            lit->typed = false;
                            lit->type = NULL;
                        }
                        break;
                    default: {
                            lit->typed = true;
                            lit->type = parseType(tokens);
                        }
                        break;
                }
                consumeToken(tokens, SEMI_COLON_TOKEN, "';'", "Factor");

                lit->count = parseExpr(tokens);
                consumeToken(tokens, R_BRACKET_TOKEN, "']'", "Factor");
                ret = (struct Expr strong *) lit;
                logMsg(LOG_INFO, 1, "Created Array Literal with size: %d", lit->count);
            }
            break;
        case L_PAREN_TOKEN: {
				consumeToken(tokens, L_PAREN_TOKEN, "'('", "Factor");
				ret = parseExpr(tokens);
				consumeToken(tokens, R_PAREN_TOKEN, "')'", "Factor");
			}
			break;
		case ID_TOKEN: {
    			struct Token const weak *nameToken = consumeToken(tokens, ID_TOKEN, "Identifier", "Factor");
    			char const *name = nameToken->raw;
    			
    			switch ((**tokens)->type) { 
    				case L_PAREN_TOKEN: {
    						struct CallExpr *callExpr = (struct CallExpr *) memAlloc(sizeof(struct CallExpr));
    						callExpr->base.type = CALL_EXPR;
    						callExpr->name = name;
    						callExpr->argCount = 0;
    						callExpr->args = NULL;
    						
                            consumeToken(tokens, L_PAREN_TOKEN, "'('", "Factor");

    						while ((**tokens)->type != R_PAREN_TOKEN) {
    							if (callExpr->argCount != 0) {
    								consumeToken(tokens, COMMA_TOKEN, ",", "Factor");
    							}
    							pushExpr(&callExpr->args, &callExpr->argCount, parseExpr(tokens));
    						}
    	                    
                            consumeToken(tokens, R_PAREN_TOKEN, ")", "Factor");

    						ret = (struct Expr *) callExpr;	
    					}
    					break;
    
    				default: {
    						struct VarExpr *varExpr = (struct VarExpr *) memAlloc(sizeof(struct VarExpr));
    						varExpr->base.type = VAR_EXPR;
    						varExpr->name = name;
    						ret = (struct Expr *) varExpr;
    					}
    					break;
    			}
            }
			break;
		default:
			logMsg(LOG_ERROR, 4, "Invalid Factor");
			ret = NULL;
			exit(-1);
			break;
	}

	logMsg(LOG_INFO, 2, "Parsed Factor");
	return ret;
}

struct Expr strong *parseIndex(struct Token strong const *strong const *weak *tokens) {
    logMsg(LOG_INFO, 2, "Parsing Index");
    struct Expr *ret = parseFactor(tokens);
    switch ((**tokens)->type) {
        case L_BRACKET_TOKEN: {
                consumeToken(tokens, L_BRACKET_TOKEN, "'['", "Index");
                struct IndexExpr *indexExpr = memAlloc(sizeof(struct IndexExpr));
                indexExpr->base.type = INDEX_EXPR;
                indexExpr->rhs = ret;
                indexExpr->index = parseExpr(tokens);
                ret = (struct Expr strong *) indexExpr;
                consumeToken(tokens, R_BRACKET_TOKEN, "']'", "Index");
            }
            break;
        
        default:
            break;
    }
    return ret;
}

struct Expr *parseMember(struct Token const *const **tokens) {
	logMsg(LOG_INFO, 2, "Parsing Member");
	struct Expr *ret = parseIndex(tokens);
	if ((**tokens)->type == DOT_TOKEN) {
	    consumeToken(tokens, DOT_TOKEN, "'.'", "Member");
		logMsg(LOG_INFO, 1, "Attempting Id token consumption");
		char const *name = consumeToken(tokens, ID_TOKEN, "Identifier", "Member")->raw;

		struct MethodCallExpr *callExpr = (struct MethodCallExpr *) memAlloc(sizeof(struct MethodCallExpr));
		callExpr->base.type = METHOD_CALL_EXPR;
		callExpr->name = name;
		callExpr->argCount = 0;
		callExpr->args = NULL;
		callExpr->lhs = ret;
    
        consumeToken(tokens, L_PAREN_TOKEN, "'('", "Member");

        while ((**tokens)->type != R_PAREN_TOKEN) {
			if (callExpr->argCount != 0) {
				consumeToken(tokens, COMMA_TOKEN, "','", "Member");
			}
			pushExpr(&callExpr->args, &callExpr->argCount, parseExpr(tokens));
		}
	
		consumeToken(tokens, R_PAREN_TOKEN, "Identifier", "Member");
		ret = (struct Expr *) callExpr;	
	}
	return ret;
}

// * /
struct Expr *parseTerm(struct Token const *const **tokens) {
	logMsg(LOG_INFO, 2, "Parsing Term");
	struct Expr *ret = parseMember(tokens);
	while ((**tokens)->type == MUL_TOKEN || (**tokens)->type == DIV_TOKEN) {
		switch ((**tokens)->type) {
			case MUL_TOKEN: {
					struct BinaryExpr *binaryRet = (struct BinaryExpr *) memAlloc(sizeof(struct BinaryExpr));
					(*tokens)++;
					binaryRet->base.type = BINARY_EXPR;
					binaryRet->lhs = ret;
					binaryRet->rhs = parseTerm(tokens);
					binaryRet->op = MUL_OP;
					ret = (struct Expr *) binaryRet;
				}
				break;
			case DIV_TOKEN: {
					struct BinaryExpr *binaryRet = (struct BinaryExpr *) memAlloc(sizeof(struct BinaryExpr));
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



struct Expr *parseArithExpr(struct Token const *const **tokens) {
	logMsg(LOG_INFO, 2, "Parsing Expr");
	struct Expr *ret = parseTerm(tokens);
	while ((**tokens)->type == ADD_TOKEN || (**tokens)->type == SUB_TOKEN) {
		switch ((**tokens)->type) {
			case ADD_TOKEN: {
					struct BinaryExpr *binaryRet = (struct BinaryExpr *) memAlloc(sizeof(struct BinaryExpr));
					consumeToken(tokens, ADD_TOKEN, "'+'", "Expr");
					binaryRet->base.type = BINARY_EXPR;
					binaryRet->lhs = ret;
					binaryRet->rhs = parseTerm(tokens);
					binaryRet->op = ADD_OP;
					ret = (struct Expr *) binaryRet;
				}
				break;
			case SUB_TOKEN: {
					struct BinaryExpr *binaryRet = (struct BinaryExpr *) memAlloc(sizeof(struct BinaryExpr));
				    consumeToken(tokens, SUB_TOKEN, "'-'", "Expr");
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

struct Expr strong *parseLogicFactor(struct Token const strong *const strong *weak *tokens) {
    logMsg(LOG_INFO, 2, "Parsing Expr");
    struct Expr strong *ret = parseArithExpr(tokens);
 	while ((**tokens)->type == ADD_TOKEN || (**tokens)->type == SUB_TOKEN) {
		switch ((**tokens)->type) {
			case G_THAN_TOKEN: {
					struct BinaryExpr *binaryRet = (struct BinaryExpr *) memAlloc(sizeof(struct BinaryExpr));
					consumeToken(tokens, G_THAN_TOKEN, "'>'", "Expr");
					binaryRet->base.type = BINARY_EXPR;
					binaryRet->lhs = ret;
					binaryRet->rhs = parseTerm(tokens);
					binaryRet->op = G_THAN_OP;
					ret = (struct Expr *) binaryRet;
				}
				break;
			case L_THAN_TOKEN: {
					struct BinaryExpr *binaryRet = (struct BinaryExpr *) memAlloc(sizeof(struct BinaryExpr));
				    consumeToken(tokens, L_THAN_TOKEN, "'<'", "Expr");
					binaryRet->base.type = BINARY_EXPR;
					binaryRet->lhs = ret;
					binaryRet->rhs = parseTerm(tokens);
					binaryRet->op = L_THAN_OP;
					ret = (struct Expr *) binaryRet;
				}
				break;
 
            case EQUALS_TOKEN: {
					struct BinaryExpr *binaryRet = (struct BinaryExpr *) memAlloc(sizeof(struct BinaryExpr));
				    consumeToken(tokens, EQUALS_TOKEN, "'='", "Expr");
					binaryRet->base.type = BINARY_EXPR;
					binaryRet->lhs = ret;
					binaryRet->rhs = parseTerm(tokens);
					binaryRet->op = EQUALS_OP;
					ret = (struct Expr *) binaryRet;
				}
				break;
    
			case NG_THAN_TOKEN: {
					struct BinaryExpr *binaryRet = (struct BinaryExpr *) memAlloc(sizeof(struct BinaryExpr));
				    consumeToken(tokens, NG_THAN_TOKEN, "'!>'", "Expr");
					binaryRet->base.type = BINARY_EXPR;
					binaryRet->lhs = ret;
					binaryRet->rhs = parseTerm(tokens);
					binaryRet->op = NG_THAN_OP;
					ret = (struct Expr *) binaryRet;
				}
				break;
    
            case NL_THAN_TOKEN: {
					struct BinaryExpr *binaryRet = (struct BinaryExpr *) memAlloc(sizeof(struct BinaryExpr));
				    consumeToken(tokens, NL_THAN_TOKEN, "'!<'", "Expr");
					binaryRet->base.type = BINARY_EXPR;
					binaryRet->lhs = ret;
					binaryRet->rhs = parseTerm(tokens);
					binaryRet->op = NL_THAN_OP;
					ret = (struct Expr *) binaryRet;
				}
				break;
    

		    case N_EQUALS_TOKEN: {
					struct BinaryExpr *binaryRet = (struct BinaryExpr *) memAlloc(sizeof(struct BinaryExpr));
				    consumeToken(tokens, N_EQUALS_TOKEN, "'!='", "Expr");
					binaryRet->base.type = BINARY_EXPR;
					binaryRet->lhs = ret;
					binaryRet->rhs = parseTerm(tokens);
					binaryRet->op = N_EQUALS_OP;
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

struct Expr strong *parseExpr(struct Token const strong *const strong *weak *tokens) {
    logMsg(LOG_INFO, 2, "Parsing Expr");
    struct Expr strong *ret = parseLogicFactor(tokens);
    logMsg(LOG_INFO, 2, "Parsed Expr");
    return ret;
}

struct Stmt *parseStmt(struct Token const *const **tokens);

void pushStmt(struct Stmt ***buffer, unsigned int *count, struct Stmt *stmt) {
	(*count)++;
	(*buffer) = (struct Stmt **) memRealloc(*buffer, sizeof(struct Stmt *) * *count);
	(*buffer)[(*count) - 1 ] = stmt;
}

struct Stmt *parseBlockStmt(struct Token const *const **tokens) {
	logMsg(LOG_INFO, 2, "Parsing Block Stmt");
	struct BlockStmt *out = (struct BlockStmt *) memAlloc(sizeof(struct BlockStmt));

	out->base.type = BLOCK_STMT;

    consumeToken(tokens, L_BRACE_TOKEN, "'{'", "Block Stmt");

	out->stmts = NULL;
	out->stmtCount = 0;

	while ((**tokens)->type != R_BRACE_TOKEN) {
		pushStmt(&out->stmts, &out->stmtCount, parseStmt(tokens));
	}
	
    consumeToken(tokens, R_BRACE_TOKEN, "'}'", "Block Stmt");

	logMsg(LOG_INFO, 2, "Parsed Block Stmt");
	return (struct Stmt *) out;
}

struct Stmt *parseVarStmt(struct Token const *const **tokens) {
	logMsg(LOG_INFO, 2, "Parsing Var Stmt");
	struct VarStmt *out = (struct VarStmt *) memAlloc(sizeof(struct VarStmt));
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

	out->var = (struct Var *) memAlloc(sizeof(struct Var));

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

	logMsg(LOG_INFO, 1, "Attempting ':=' token consumption");
	if ((**tokens)->type != ASSIGN_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected ':=' but got '%s'", (**tokens)->raw);
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
	struct AssignStmt *out = (struct AssignStmt *) memAlloc(sizeof(struct AssignStmt));
	out->base.type = ASSIGN_STMT;

	logMsg(LOG_INFO, 1, "Attempting Id token consumption");
	if ((**tokens)->type != ID_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected identifier but got '%s'", (**tokens)->raw);
		exit(-1);
	}
	struct Var *var = (struct Var *) memAlloc(sizeof(struct Var));
	var->name = ((struct IdToken *)(**tokens))->value;
	var->type = memAlloc(0);
	//var->name = heapString("ret");
	out->var = var;
	(*tokens)++;
	logMsg(LOG_INFO, 1, "Id Token Consumption Successful");

	logMsg(LOG_INFO, 1, "Attempting ':=' token consumption");
	if ((**tokens)->type != ASSIGN_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected ':=' but got '%s'", (**tokens)->raw);
		exit(-1);
	}
	(*tokens)++;
	logMsg(LOG_INFO, 1, "':=' Token Consumption Successful");

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
	struct ExprStmt *ret = (struct ExprStmt *) memAlloc(sizeof(struct ExprStmt));
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

struct Stmt strong *parseIfStmt(struct Token const weak *const weak *weak *tokens) {
	logMsg(LOG_INFO, 2, "Parsing If Stmt");
	struct IfStmt strong *ret = (struct IfStmt strong *) memAlloc(sizeof(struct IfStmt));
	ret->base.type = IF_STMT;
	logMsg(LOG_INFO, 1, "Attempting 'if' Token Consumption");
	if ((**tokens)->type != IF_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected 'if' but got '%s'", (**tokens)->raw);
		exit(EXIT_FAILURE);
	}
	(*tokens)++;
	logMsg(LOG_INFO, 1, "'if' token consumption successful");

	logMsg(LOG_INFO, 1, "Attempting '(' Token Consumption");
	if ((**tokens)->type != L_PAREN_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected '(' but got '%s'", (**tokens)->raw);
		exit(EXIT_FAILURE);
	}
	(*tokens)++;
	logMsg(LOG_INFO, 1, "'(' token consumption successful");

	ret->condition = parseExpr(tokens);

	logMsg(LOG_INFO, 1, "Attempting ')' Token Consumption");
	if ((**tokens)->type != R_PAREN_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected ')' but got '%s'", (**tokens)->raw);
		exit(EXIT_FAILURE);
	}
	(*tokens)++;
	logMsg(LOG_INFO, 1, "')' token consumption successful");

	ret->ifBody = parseStmt(tokens);

	if ((**tokens)->type == ELSE_TOKEN) {	
		logMsg(LOG_INFO, 1, "Attempting 'else' Token Consumption");
		if ((**tokens)->type != ELSE_TOKEN) {
			logMsg(LOG_ERROR, 4, "Invalid Token: Expected 'else' but got '%s'", (**tokens)->raw);
			exit(EXIT_FAILURE);
		}
		(*tokens)++;
		logMsg(LOG_INFO, 1, "'else' token consumption successful");

		ret->elseBody = parseStmt(tokens);
	} else {
		ret->elseBody = NULL;
	}
	return (struct Stmt strong *) ret;
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
			if (((*tokens)[1])->type != ASSIGN_TOKEN) {
				stmt = parseExprStmt(tokens);
			} else {
				stmt = parseAssignStmt(tokens);
			}
			break;
		case IF_TOKEN:
			stmt = parseIfStmt(tokens);
			break;
		default:
			logMsg(LOG_ERROR, 4, "Unexpected Token: %s", (**tokens)->raw);
			exit(-1);
			break;
	}
	logMsg(LOG_INFO, 2, "Parsed Statement");
	return stmt;
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

    consumeToken(tokens, L_PAREN_TOKEN, "')'", "Func");

	// parse and consume params.
	
	ret->params = NULL;
	ret->paramCount = 0;

	while ((**tokens)->type != R_PAREN_TOKEN) {
        if (ret->paramCount != 0) {
            consumeToken(tokens, COMMA_TOKEN, "','", "Func");
        }
        pushVar(&ret->params, &ret->paramCount, parseParam(tokens));
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

void pushFunc(struct Func ***buffer, unsigned int *count, struct Func *func) {
	(*count)++;
	(*buffer) = memRealloc(*buffer, sizeof(struct Func *) * *count);
	(*buffer)[(*count) - 1 ] = func;
}

void pushString(char const ***buffer, unsigned int *count, char const *string) {
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

void pushUse(struct Use ***buffer, unsigned int *count, struct Use *use) {
	(*count)++;
	(*buffer) = (struct Use **) memRealloc(*buffer, sizeof(struct Use *) * *count);
	(*buffer)[(*count) - 1 ] = use;
}

struct FuncDef *parseFuncDef(struct Token const *const **tokens) {
	struct FuncDef *funcDef = (struct FuncDef *) memAlloc(sizeof(struct FuncDef));
	
	struct Func *ret = (struct Func *) memAlloc(sizeof(struct Func));
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
		if (ret->paramCount != 0) {
			logMsg(LOG_INFO, 1, "Attempting ',' token consumption");
			if ((**tokens)->type != COMMA_TOKEN) {
				logMsg(LOG_ERROR, 4, "Invalid Token: Expected ',' but got '%s'", (**tokens)->raw);
			}
			(*tokens)++;
			logMsg(LOG_INFO, 1, "',' Token Consumption Successful");
		}

		struct Type *type = parseType(tokens);
		struct Var *var = (struct Var *) memAlloc(sizeof(struct Var));
		var->type = type;
		var->name = "";
		pushVar(&ret->params, &ret->paramCount, var);
	}
			
	logMsg(LOG_INFO, 1, "Attempting ')' Token Consumption");
	if ((**tokens)->type != R_PAREN_TOKEN) {
		logMsg(LOG_ERROR, 4, "Invalid Token: Expected ')' but got '%s'", (**tokens)->raw);
		exit(-1);
	}
	(*tokens)++;
	logMsg(LOG_INFO, 1, "')' Token Consumption Successful");
			
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
	return funcDef;
}

struct Def *parseDef(struct Token const *const **tokens) {
	struct Def def;
	struct Def *out;
	struct Use *use = (struct Use *) memAlloc(sizeof(struct Use));
	logMsg(LOG_INFO, 2, "Parsing Def");

	use->names = NULL;
	use->nameCount = 0;

    switch ((**tokens)->type) {
        case FROM_TOKEN: {
            	consumeToken(tokens, FROM_TOKEN, "'from'", "Def");
        	    pushString(&use->names, &use->nameCount, consumeToken(tokens, ID_TOKEN, "Identifier", "Def")->raw);
     
                while ((**tokens)->type == DOUBLE_COLON_TOKEN) {
     	            consumeToken(tokens, DOUBLE_COLON_TOKEN, "'::'", "Def");
        		    pushString(&use->names, &use->nameCount, consumeToken(tokens, ID_TOKEN, "Identifier", "Def")->raw);
         	    }
            }
            break;
        case EXT_TOKEN: {
                consumeToken(tokens, EXT_TOKEN, "'ext'", "Def");
            }
            break;
        default:
            syntaxError("Def", (**tokens)->line, (**tokens)->column, "'from'" DEFAULT " or " YELLOW "'ext'", (**tokens)->raw);
    }

	def.use = use;

	consumeToken(tokens, DEF_TOKEN, "'def'", "Def");
	switch ((**tokens)->type) {
		case FUNC_TOKEN: {
				struct FuncDef *funcDef = parseFuncDef(tokens);
				funcDef->base = def;
				funcDef->base.type = FUNC_DEF;
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

struct TypeAlias strong *parseTypeAlias(struct Token const strong *const strong *weak *tokens) {
    logMsg(LOG_INFO, 2, "Parsing Type Alias");
    struct TypeAlias strong *ret = memAlloc(sizeof(struct TypeAlias));
    
    consumeToken(tokens, TYPE_TOKEN, "'types'", "Type Alias");
    
    struct Token const weak *id = consumeToken(tokens, ID_TOKEN, "Identifier", "Type Alias");
    ret->name = id->raw;

    consumeToken(tokens, COLON_TOKEN, "':'", "Type Alias");

    ret->type = parseType(tokens);

    logMsg(LOG_INFO, 2, "Parsed Type Alias");
    return ret;
}

void pushDef(struct Def ***buffer, unsigned int *count, struct Def *def) {
	(*count)++;
	(*buffer) = memRealloc(*buffer, sizeof(struct Def *) * *count);
	(*buffer)[(*count) - 1 ] = def;
}

void pushTypeAlias(struct TypeAlias strong *strong *weak *buffer, unsigned int weak *count, struct TypeAlias strong *typeAlias) {
    (*count)++;
    (*buffer) = memRealloc(*buffer, sizeof(struct TypeAlias strong *) * *count);
    (*buffer)[(*count) - 1] = typeAlias;
}

struct Module strong *parseModule(struct Token const *const **tokens) {
	struct Module strong *module = memAlloc(sizeof(struct Module));
	logMsg(LOG_INFO, 2, "Parsing Module");
	module->nameCount = 0;
	module->names = parseModuleName(tokens, &module->nameCount);
    module->typeAliasCount = 0;
    module->typeAliases = NULL;
    module->funcCount = 0;
	module->funcs = NULL;
	module->includes = NULL;
	module->includeCount = 0;
	module->defs = NULL;
	module->defCount = 0;
	while ((**tokens)->type != EOF_TOKEN) {
		switch ((**tokens)->type) {
			case FUNC_TOKEN:
				pushFunc(&module->funcs, &module->funcCount, parseFunc(tokens));
				break;
			case USE_TOKEN:
				pushUse(&module->includes, &module->includeCount, parseUse(tokens));
				break;
            case EXT_TOKEN:
            case FROM_TOKEN:
				pushDef(&module->defs, &module->defCount, parseDef(tokens));
				break;
	        case TYPE_TOKEN:
                pushTypeAlias(&module->typeAliases, &module->typeAliasCount, parseTypeAlias(tokens));
                break;
            default:
				syntaxError("Module", (**tokens)->line, (**tokens)->column, "'func'" DEFAULT ", " YELLOW
                                                                            "'use'" DEFAULT ", " YELLOW 
                                                                            "'type'" DEFAULT ", " YELLOW
                                                                            "'def'" DEFAULT " or " YELLOW
                                                                            "'ext'", (**tokens)->raw);
		}
	}
	logMsg(LOG_INFO, 2, "Parsed Module");
	return module;
}

struct Module strong *parse(struct Token const *const *tokens) {
	return parseModule(&tokens);
}
