#pragma once
#include <stdbool.h>
#include "../util/mem.h"

enum TokenType {
	NULL_TOKEN = 0,
	EOF_TOKEN,
	STRING_TOKEN,
	INT_TOKEN,
	BYTE_TOKEN,
	CHAR_TOKEN,
	ID_TOKEN,
	BOOL_TOKEN,

	ADD_TOKEN,
	SUB_TOKEN,
	MUL_TOKEN,
	DIV_TOKEN,

	L_PAREN_TOKEN,
	R_PAREN_TOKEN,

	L_BRACKET_TOKEN,
	R_BRACKET_TOKEN,

	L_BRACE_TOKEN,
	R_BRACE_TOKEN,

	COLON_TOKEN,
	ARROW_TOKEN,
	COMMA_TOKEN,

	FUNC_TOKEN,
	FROM_TOKEN,
	DEF_TOKEN,
	VAR_TOKEN,
	FOR_TOKEN,
	WHILE_TOKEN,
	IF_TOKEN,
	ELSE_TOKEN,
	USE_TOKEN,
	MOD_TOKEN,
	CLASS_TOKEN,
	PAD_TOKEN,
    TYPE_TOKEN,
    EXT_TOKEN,

    DOT_TOKEN,
	BUILD_TOKEN,

    G_THAN_TOKEN,
    L_THAN_TOKEN,
    NG_THAN_TOKEN,
    NL_THAN_TOKEN,

	DOUBLE_COLON_TOKEN,
	EQUALS_TOKEN,
    ASSIGN_TOKEN,
    N_EQUALS_TOKEN,
    SEMI_COLON_TOKEN,
    MUT_TOKEN,
};

struct Token {
	enum TokenType type;
    unsigned int line;
    unsigned int column;
    char const strong *raw;
};

void delTokens(struct Token const strong *const strong *tokens);

struct StringToken {
	struct Token base;
	char const strong *value;
};

struct BoolToken {
	struct Token base;
	bool value;
};

struct IntToken {
	struct Token base;
	long long value;
};

struct ByteToken {
	struct Token base;
	long long value;
};

struct CharToken {
	struct Token base;
	char value;
};

struct IdToken {
	struct Token base;
	char const weak *value; // base.raw owns
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

struct AddToken {
	struct Token base;
};

struct SubToken {
	struct Token base;
};

struct MulToken {
	struct Token base;
};

struct DivToken {
	struct Token base;
};

struct FromToken {
	struct Token base;
};

struct DefToken {
	struct Token base;
};

struct CommaToken {
	struct Token base;
};

struct ClassToken {
	struct Token base;
};

struct PadToken {
	struct Token base;
};

struct DotToken {
	struct Token base;
};

struct BuildToken {
	struct Token base;
};

struct TypeToken {
    struct Token base;
};

struct ExtToken {
    struct Token base;
};

struct AssignToken {
    struct Token base;
};

struct GThanToken {
    struct Token base;
};

struct LThanToken {
    struct Token base;
};

struct NEqualsToken {
    struct Token base;
};

struct NGThanToken {
    struct Token base;
};

struct NLThanToken {
    struct Token base;
};

struct ElseToken {
    struct Token base;
};

struct MutToken {
    struct Token base;
};
