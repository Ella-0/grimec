struct Var {
	const char *name;
	struct Type *type;
};

enum TypeType {
	BUILDIN_TYPE
};

struct Type {
	enum TypeType type;	
};

enum BuildinTypeType {
	INT_BUILDIN_TYPE,
};

struct BuildinType {
	struct Type base;
	enum BuildinTypeType type;
};

enum ExprType {
	LITERAL_EXPR,
	UNARY_EXPR,
	BINARY_EXPR,
	VAR_EXPR,
	CALL_EXPR,
};

struct Expr {
	enum ExprType type;
};

enum LiteralType {
	NULL_LITERAL,
	INT_LITERAL,
	BOOL_LITERAL,
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

struct BoolLiteral {
	struct LiteralExpr base;
	int val;
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
};

struct BinaryExpr {
	struct Expr base;
	struct Expr *lhs;
	enum BinaryOp op;
	struct Expr *rhs;
};

enum StmtType {
	NULL_STMT,
	VAR_STMT,
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
	struct Var *var;
	struct Expr *init;
};

struct ExprStmt {
	struct Stmt base;
	struct Expr *expr;
};

struct BlockStmt {
	struct Stmt base;
	unsigned int stmtCount;
	struct Stmt **stmts;
};

struct Func {
	const char *name;
	unsigned int paramCount;
	// Different sub types have different sizes
	struct Var **params;
	struct Type *retType;
	struct Stmt *body;
};

struct Module {
	const char *name;
	unsigned int funcCount;
	struct Func **funcs;
};
