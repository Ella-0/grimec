#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "util/mem.h"

enum TypeType {
	BUILDIN_TYPE,
    ARRAY_TYPE,
    BOX_TYPE,
    TUPLE_TYPE,
    SIMPLE_TYPE,
	GENERIC_TYPE,
};

struct Type {
	enum TypeType type;	
};

struct Var {
	char const weak *name;
	struct Type strong *type;
};


enum BuildinTypeType {
	INT_BUILDIN_TYPE,
	STRING_BUILDIN_TYPE,
	VOID_BUILDIN_TYPE,
};

struct BuildinType {
	struct Type base;
	enum BuildinTypeType type;
};

struct SimpleType {
	struct Type base;
	char const weak *name;
};

struct ArrayType {
    struct Type base;
    bool typed;
    bool owning;
    bool mut;
    struct Type strong *type;
    bool sized;
    unsigned int elementCount;
};

struct TupleType {
    struct Type base;
    unsigned int varCount;
    struct Var **vars;
};

struct GenericType {
	char const weak *name;
	unsigned int typeCount;
	struct Type strong *strong *types;
};

enum ExprType {
	LITERAL_EXPR,
	UNARY_EXPR,
	BINARY_EXPR,
	VAR_EXPR,
    INDEX_EXPR,
	CALL_EXPR,
	METHOD_CALL_EXPR,
    IF_EXPR,
    BLOCK_EXPR,
};

struct Expr {
	enum ExprType type;
    struct Type *evalType;
};

struct IndexExpr {
    struct Expr base;
    struct Expr *rhs;
    struct Expr *index;
};

struct CallExpr {
	struct Expr base;
	char const weak *name;
	unsigned int argCount;
	struct Expr strong *strong *args;
};

struct MethodCallExpr {
	struct Expr base;
	struct Expr *lhs;
	char const *name;
	unsigned int argCount;
	struct Expr strong *strong *args;
};

struct VarExpr {
	struct Expr base;
	char const weak *name;
};

enum LiteralType {
	NULL_LITERAL,
	INT_LITERAL,
	BOOL_LITERAL,
	STRING_LITERAL,
	CHAR_LITERAL,
	BYTE_LITERAL,
    ARRAY_LITERAL,
};

struct LiteralExpr {
	struct Expr base;
	enum LiteralType type;
};

struct NullLiteral {
	struct LiteralExpr base;
};

struct IntLiteral {
	struct LiteralExpr base;
	int val;
};

struct ByteLiteral {
	struct LiteralExpr base;
	int8_t val;
};

struct CharLiteral {
	struct LiteralExpr base;
	char val;
};

struct BoolLiteral {
	struct LiteralExpr base;
	bool val;
};

struct ArrayLiteral {
    struct LiteralExpr base;
    bool typed;
    struct Type strong *type;
    struct Expr *count;
};

struct StringLiteral {
	struct LiteralExpr base;
	char const strong *val;
};

enum UnaryOp {
	NEG_OP,
};

struct UnaryExpr {
	struct Expr base;
	enum UnaryOp op;
	struct Expr *rhs;
};

enum BinaryOp {
	ADD_OP,
	SUB_OP,
	MUL_OP,
	DIV_OP,
    EQUALS_OP,
    N_EQUALS_OP,
    G_THAN_OP,
    L_THAN_OP,
    NG_THAN_OP,
    NL_THAN_OP,
};

struct BinaryExpr {
	struct Expr base;
	struct Expr strong *lhs;
	enum BinaryOp op;
	struct Expr strong *rhs;
};

enum StmtType {
	NULL_STMT,
	VAR_STMT,
	ASSIGN_STMT,
	EXPR_STMT,
};

struct Stmt {
	enum StmtType type;
};

struct NullStmt {
	struct Stmt base;
};

struct VarStmt {
	struct Stmt base;
	struct Var strong *var;
	struct Expr strong *init;
};

struct AssignStmt {
	struct Stmt base;
	struct Var strong *var;
	struct Expr strong *init;
};

struct ExprStmt {
	struct Stmt base;
	struct Expr strong *expr;
};

struct BlockExpr {
	struct Expr base;
	unsigned int stmtCount;
	struct Stmt strong *strong *stmts;
};

struct ConditionalBlock {
	struct Expr strong *condition;
	struct Stmt strong *body;
};

struct IfExpr {
	struct Expr base;
	struct Expr strong *condition;
	struct Expr strong *ifBody;
	struct Expr strong *elseBody;
};

struct Func {
	const char weak	*name;
	unsigned int paramCount;
	// Different sub types have different sizes
	struct Var strong *strong *params;
	struct Type strong *retType;
	struct Expr strong *body;
};

struct Use {
	unsigned int nameCount;
	const char weak *strong *names;
};

struct Class {
	const char weak *name;
	unsigned int buildCount;
	struct Func strong *strong *builds;
	unsigned int funcCount;
	struct Func strong *strong *funcs;
};

enum DefType {
	FUNC_DEF,
	CLASS_DEF,
	TRAIT_DEF,
};

struct Def {
	enum DefType type;
	struct Use strong *use;
};

struct ClassDef  {
	struct Def base;
	struct Class strong *class;
};

struct FuncDef {
	struct Def base;
	struct Func strong *func;
};

struct TypeAlias {
    char const weak *name;
    struct Type strong *type;
};

struct Module {
	unsigned int nameCount;
	char const weak *strong *names; // name0::name1::name2

	unsigned int includeCount;
	struct Use strong *strong *includes;

    unsigned int typeAliasCount;
    struct TypeAlias strong *strong *typeAliases;

	unsigned int funcCount;
	struct Func strong *strong *funcs;

    unsigned int defCount;
	struct Def strong *strong *defs;
};

void delModule(struct Module strong *module);
