#include <stdlib.h>
#include "../ast.h"
#include "../util/mem.h"
#include "../util/log.h"
#include "type-resolve.h"

void typeResolutionError(char const weak *type) {
    logMsg(LOG_ERROR, 4, "Error Resolving Type of %s", type);
    exit(EXIT_FAILURE);
}

struct Type strong *resolveLiteralExprType(struct LiteralExpr weak *expr) {
    struct Type strong *ret ;
    switch (expr->type) {
        case INT_LITERAL: {
                struct SimpleType strong *simpleType = memAlloc(sizeof(struct SimpleType));
                simpleType->base.type = SIMPLE_TYPE;
                simpleType->name = "Int";
                ret = (struct Type strong *) simpleType;
            }
            break;

        case BOOL_LITERAL: {
                struct SimpleType strong *simpleType = memAlloc(sizeof(struct SimpleType));
                simpleType->base.type = SIMPLE_TYPE;
                simpleType->name = "Bool";
                ret = (struct Type strong *) simpleType;
            }
            break;
        
        default:
            typeResolutionError("Literal Expr");
    }

    return ret;
}

void resolveExprType(struct Expr weak *expr) {
    switch (expr->type) {

        case LITERAL_EXPR:
            expr->evalType = resolveLiteralExprType((struct LiteralExpr weak *) expr);
            break;

        default:
            typeResolutionError("Expr");
    }
}

void resolveTypesInModule(struct Module weak *module) {

}

void resolveTypes(struct Module weak *module) {
    resolveTypesInModule(module);    
}
