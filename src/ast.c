#include "util/mem.h"
#include "util/log.h"
#include "ast.h"

void delVar(struct Var *var) {
	logMsg(LOG_INFO, 1, "Deleting Var %s", var->name);
	memFree(var->type);
	memFree(var);
}

void delExpr(struct Expr strong *expr) {
	logMsg(LOG_INFO, 1, "Deleting Expr");
	switch (expr->type) {
		case VAR_EXPR: {	
			}
			break;
		case METHOD_CALL_EXPR: {
				struct MethodCallExpr strong *methodCallExpr = (struct MethodCallExpr strong *) expr;
				for (unsigned int i = 0; i < methodCallExpr->argCount; i++) {
					delExpr(methodCallExpr->args[i]);
				}
				memFree(methodCallExpr->args);
				delExpr(methodCallExpr->lhs);
			}
			break;
		case CALL_EXPR: {
				struct CallExpr strong *callExpr = (struct CallExpr strong *) expr;
				for (unsigned int i = 0; i < callExpr->argCount; i++) {
					delExpr(callExpr->args[i]);
				}
				memFree(callExpr->args);
			}
			break;
		case BINARY_EXPR: {
				struct BinaryExpr strong *binaryExpr = (struct BinaryExpr strong *) expr;
				delExpr(binaryExpr->lhs);
				delExpr(binaryExpr->rhs);
			}
			break;
		case LITERAL_EXPR: {
				struct LiteralExpr strong *literalExpr = (struct LiteralExpr strong *) expr;
				switch (literalExpr->type) {
					case INT_LITERAL: {
						}
						break;
					case STRING_LITERAL: {
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

void delStmt(struct Stmt strong *stmt);

void delConditionalBlock(struct ConditionalBlock strong *conditionalBlock) {
	delExpr(conditionalBlock->condition);
	delStmt(conditionalBlock->body);
	memFree(conditionalBlock);
}

void delStmt(struct Stmt strong *stmt) {
	logMsg(LOG_INFO, 1, "Deleting Stmt");
	switch (stmt->type) {
		case BLOCK_STMT: {
				struct BlockStmt strong *blockStmt = (struct BlockStmt strong *) stmt;
				for (unsigned int i = 0; i < blockStmt->stmtCount; i++) {
					delStmt(blockStmt->stmts[i]);	
				}
				memFree(blockStmt->stmts);
			}
			break;
		case EXPR_STMT: {
				struct ExprStmt strong *exprStmt = (struct ExprStmt strong *) stmt;
				delExpr(exprStmt->expr);
			}
			break;
		case VAR_STMT: {
				struct VarStmt strong *varStmt = (struct VarStmt strong *) stmt;
				delVar(varStmt->var);
				delExpr(varStmt->init);
			}
			break;
		case ASSIGN_STMT: {
				struct AssignStmt strong *assignStmt = (struct AssignStmt strong *) stmt;
				delExpr(assignStmt->init);
				//delVar(assignStmt->var);
			}
			break;
		case IF_STMT: {
				struct IfStmt strong *ifStmt = (struct IfStmt strong *) stmt;
				delExpr(ifStmt->condition);
				delStmt(ifStmt->ifBody);
				if (ifStmt->elseBody != NULL) {
					delStmt(ifStmt->elseBody);
				}
			}
			break;
		default:
			logMsg(LOG_ERROR, 4, "Unexpected Type when deleting stmt");
			break;	
	}
	memFree(stmt);
}

void delFunc(struct Func strong *func) {
	logMsg(LOG_INFO, 1, "Deleting Func");

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

void delClass(struct Class strong *class) {
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

void delUse(struct Use strong *use) {
	logMsg(LOG_INFO, 1, "Deleting Use");
	memFree(use->names);
	memFree(use);
}

void delDef(struct Def strong *def) {
	logMsg(LOG_INFO, 1, "Deleting Def");
	delUse(def->use);
	switch (def->type) {
		case FUNC_DEF: {
				struct FuncDef strong *funcDef = (struct FuncDef strong *) def;
				delFunc(funcDef->func);
			}
			break;
		case CLASS_DEF: {
				struct ClassDef strong *classDef = (struct ClassDef strong *) def;
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
		//memFree(module.names[i]);
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
