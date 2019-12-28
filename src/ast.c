#include "util/mem.h"
#include "util/log.h"
#include "ast.h"

void delVar(struct Var *var) {
	logMsg(LOG_INFO, 1, "Deleting Var %s", var->name);
	memFree(var->type);
	logMsg(LOG_INFO, 1, "Deleted Var");
	memFree(var);
}

void delExpr(struct Expr *expr) {
	logMsg(LOG_INFO, 1, "Deleting Expr");
	switch (expr->type) {
		case VAR_EXPR: {	
			}
			break;
		case METHOD_CALL_EXPR: {
				struct MethodCallExpr *methodCallExpr = (struct MethodCallExpr *) expr;
				for (unsigned int i = 0; i < methodCallExpr->argCount; i++) {
					delExpr(methodCallExpr->args[i]);
				}
				memFree(methodCallExpr->args);
				delExpr(methodCallExpr->lhs);
			}
			break;
		case CALL_EXPR: {
				struct CallExpr *callExpr = (struct CallExpr *) expr;
				for (unsigned int i = 0; i < callExpr->argCount; i++) {
					delExpr(callExpr->args[i]);
				}
				memFree(callExpr->args);
			}
			break;
		case BINARY_EXPR: {
				struct BinaryExpr *binaryExpr = (struct BinaryExpr *) expr;
				delExpr(binaryExpr->lhs);
				delExpr(binaryExpr->rhs);
			}
			break;
		case LITERAL_EXPR: {
				struct LiteralExpr *literalExpr = (struct LiteralExpr *) expr;
				switch (literalExpr->type) {
					case INT_LITERAL: {
						}
						break;
					case STRING_LITERAL: {
							struct StringLiteral *stringLiteral = (struct StringLiteral *) literalExpr;
							memFree(stringLiteral->val);
						}
						break;
					default:
						logMsg(LOG_ERROR, 1, "Unexpected Type when deleting literal expr");
				}
			}
			break;
		default:
			logMsg(LOG_ERROR, 1, "Unexpected Type when deleting expr");
			break;
	}
	memFree(expr);
}

void delStmt(struct Stmt *stmt) {
	logMsg(LOG_INFO, 1, "Deleting Stmt");
	switch (stmt->type) {
		case BLOCK_STMT: {
				struct BlockStmt *blockStmt = (struct BlockStmt *) stmt;
				for (unsigned int i = 0; i < blockStmt->stmtCount; i++) {
					delStmt(blockStmt->stmts[i]);	
				}
				memFree(blockStmt->stmts);
			}
			break;
		case EXPR_STMT: {
				struct ExprStmt *exprStmt = (struct ExprStmt *) stmt;
				delExpr(exprStmt->expr);
			}
			break;
		case VAR_STMT: {
				struct VarStmt *varStmt = (struct VarStmt *) stmt;
				delVar(varStmt->var);
				delExpr(varStmt->init);
			}
		default:
			logMsg(LOG_ERROR, 4, "Unexpected Type when deleting stmt");
			break;	
	}
	memFree(stmt);
}

void delFunc(struct Func *func) {
	logMsg(LOG_INFO, 1, "Deleting Func");
	//memFree(func->name);
	for (unsigned int i = 0; i < func->paramCount; i++) {
		delVar(func->params[i]);
	}
	memFree(func->params);
	memFree(func->retType);
	if (func->body != NULL) {
		delStmt(func->body);
	} else {
		logMsg(LOG_INFO, 1, "NULL body");
	}
	memFree(func);
}

void delClass(struct Class *class) {
	logMsg(LOG_INFO, 1, "Deleting Class %s: %u, %u", class->name, class->buildCount, class->funcCount);
	for (unsigned int i = 0; i < class->buildCount; i++) {
		delFunc(class->builds[i]);
	}
	memFree(class->builds);
	for (unsigned int i = 0; i < class->funcCount; i++) {
		delFunc(class->funcs[i]);
	}
	memFree(class->funcs);
	memFree(class);
}

void delDef(struct Def *def) {
	logMsg(LOG_INFO, 1, "Deleting Def");
	memFree(def->use);
	switch (def->type) {
		case FUNC_DEF: {
				struct FuncDef *funcDef = (struct FuncDef *) def;
				delFunc(funcDef->func);
			}
			break;
		case CLASS_DEF: {
				struct ClassDef *classDef = (struct ClassDef *) def;
				delClass(classDef->class);
			}
			break;
		default:
			logMsg(LOG_ERROR, 4, "Unexpected Type When deleting def");
	}
	memFree(def);
}

void delModule(struct Module module) {
	logMsg(LOG_INFO, 3, "Deleting AST");
	for (unsigned int i = 0; i < module.nameCount; i ++) {
		memFree(module.names[i]);
	}
	memFree(module.names);
	for (unsigned int i = 0; i < module.includeCount; i++) {
		memFree(module.includes[i]);
	}
	memFree(module.includes);
	for (unsigned int i = 0; i < module.funcCount; i++) {
		delFunc(module.funcs[i]);
	}
	memFree(module.funcs);
	for (unsigned int i = 0; i < module.defCount; i++) {
		delDef(module.defs[i]);
	}
	memFree(module.defs);
}
