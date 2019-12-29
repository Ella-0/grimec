#pragma once
#include <stdint.h>

struct Var {
	const char weak *name;
	struct Type strong *type;
};

enum TypeType {
	BUILDIN_TYPE,
	SIMPLE_TYPE,
	GENERIC_TYPE,
};

struct Type {
	enum TypeType type;	
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
	CALL_EXPR,
	METHOD_CALL_EXPR,
};

struct Expr {
	enum ExprType type;
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
	int val;
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
	BLOCK_STMT,
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
	struct Expr *expr;
};

struct BlockStmt {
	struct Stmt base;
	unsigned int stmtCount;
	struct Stmt strong *strong *stmts;
};

struct Func {
	const char weak	*name;
	unsigned int paramCount;
	// Different sub types have different sizes
	struct Var strong *strong *params;
	struct Type strong *retType;
	struct Stmt strong *body;
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

struct Module {
	unsigned int nameCount;
	char const weak *strong *names; // name0::name1::name2
	
	unsigned int includeCount;
	struct Use strong *strong *includes;

	unsigned int funcCount;
	struct Func strong *strong *funcs;

	unsigned int defCount;
	struct Def strong *strong *defs;
};

void delModule(struct Module module);
