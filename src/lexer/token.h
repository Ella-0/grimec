#pragma once

enum TokenType {
	NULL_TOKEN = 0,
	EOF_TOKEN,
	STRING_TOKEN,
	INT_TOKEN,
	ID_TOKEN,

	L_PAREN_TOKEN,
	R_PAREN_TOKEN,

	L_BRACKET_TOKEN,
	R_BRACKET_TOKEN,

	L_BRACE_TOKEN,
	R_BRACE_TOKEN,

	COLON_TOKEN,
	ARROW_TOKEN,

	FUNC_TOKEN,
	VAR_TOKEN,
	FOR_TOKEN,
	WHILE_TOKEN,
	IF_TOKEN,
	USE_TOKEN,
	MOD_TOKEN,

	DOUBLE_COLON_TOKEN,
	EQUALS_TOKEN,
	SEMI_COLON_TOKEN,
};

struct Token {
	enum TokenType type;
	const char *raw;
};

struct StringToken {
	struct Token base;
	const char *value;
};

struct IntToken {
	struct Token base;
	long long value;
};

struct IdToken {
	struct Token base;
	const char *value;
};

struct LParenToken {
	struct Token base;
};

struct RParenToken {
	struct Token base;
};

struct LBracketToken {
	struct Token base;
};

struct RBracketToken {
	struct Token base;
};

struct LBraceToken {
	struct Token base;
};

struct RBraceToken {
	struct Token base;
};

struct ColonToken {
	struct Token base;
};

struct ArrowToken {
	struct Token base;
};

struct FuncToken {
	struct Token base;
};

struct VarToken {
	struct Token base;
};

struct ForToken {
	struct Token base;
};

struct WhileToken {
	struct Token base;
};

struct IfToken {
	struct Token base;
};

struct DoubleColonToken {
	struct Token base;
};

struct EqualsToken {
	struct Token base;
};

struct SemiColonToken {
	struct Token base;
};

struct ModToken {
	struct Token base;
};

struct UseToken {
	struct Token base;
};
