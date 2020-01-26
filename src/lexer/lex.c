#include <stdlib.h>
#include <stdbool.h>
#include <regex.h>
#include <string.h>
#include <assert.h>
#include "../util/mem.h"
#include "../util/log.h"
#include "token.h"

struct Pattern {
	enum TokenType type;
	char const strong *partPattern;
	char const strong *pattern;
	struct Token strong *(strong *gen)(char const strong *value);
};

static struct Token strong *genInt(char const strong *value) {
	struct IntToken strong *token = memAlloc(sizeof(struct IntToken));
	token->base.type = INT_TOKEN;
	token->base.raw = value;
	token->value = atoi(value);
	return (struct Token strong *) token;
}

static struct Token strong *genId(char const strong *value) {
	struct IdToken strong *token = memAlloc(sizeof(struct IdToken));
	token->base.type = ID_TOKEN;
	token->base.raw = value;
	token->value = value;
	return (struct Token strong *) token;
}

static struct Token strong *genLParen(char const strong *value) {
	struct LParenToken strong *token = memAlloc(sizeof(struct LParenToken));
	token->base.type = L_PAREN_TOKEN;
	token->base.raw = value;
	return (struct Token strong *) token;
}

static struct Token strong *genRParen(char const strong *value) {
	struct RParenToken strong *token = memAlloc(sizeof(struct RParenToken));
	token->base.type = R_PAREN_TOKEN;
	token->base.raw = value;
	return (struct Token strong *) token;
}

static struct Token strong *genLBracket(char const strong *value) {
	struct LBracketToken strong *token = memAlloc(sizeof(struct LBracketToken));
	token->base.type = L_BRACKET_TOKEN;
	token->base.raw = value;
	return (struct Token strong *) token;
}

static struct Token strong *genRBracket(char const strong *value) {
	struct RBracketToken strong *token = memAlloc(sizeof(struct RBracketToken));
	token->base.type = R_BRACKET_TOKEN;
	token->base.raw = value;
	return (struct Token strong *) token;
}

static struct Token strong *genLBrace(char const strong *value) {
	struct LBraceToken *token = memAlloc(sizeof(struct LBraceToken));
	token->base.type = L_BRACE_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

static struct Token strong *genRBrace(char const strong *value) {
	struct RBraceToken *token = memAlloc(sizeof(struct RBraceToken));
	token->base.type = R_BRACE_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

static struct Token strong *genFunc(char const strong *value) {
	struct FuncToken strong *token = memAlloc(sizeof(struct FuncToken));
	token->base.type = FUNC_TOKEN;
	token->base.raw = value;
	return (struct Token strong *) token;
}

static struct Token strong *genVar(char const strong *value) {
	struct VarToken strong *token = memAlloc(sizeof(struct VarToken));
	token->base.type = VAR_TOKEN;
	token->base.raw = value;
	return (struct Token strong *) token;
}

static struct Token strong *genFor(char const strong *value) {
	struct ForToken strong *token = memAlloc(sizeof(struct ForToken));
	token->base.type = FOR_TOKEN;
	token->base.raw = value;
	return (struct Token strong *) token;
}

static struct Token strong *genColon(char const strong *value) {
	struct ColonToken strong *token = memAlloc(sizeof(struct ColonToken));
	token->base.type = COLON_TOKEN;
	token->base.raw = value;
	return (struct Token strong *) token;
}

static struct Token strong *genArrow(char const *value) {
	struct ArrowToken strong *token = memAlloc(sizeof(struct ArrowToken));
	token->base.type = ARROW_TOKEN;
	token->base.raw = value;
	return (struct Token strong *) token;
}

static struct Token weak *null(char const strong *value) {
	logMsg(LOG_ERROR, 4, "Unimplemened Token '%s' Matched", value);
	memFree(value);
	return NULL;
}

static struct Token weak *genWhitespace(char const strong *value) {
	logMsg(LOG_ERROR, 1, "Whitespace '%s'", value);
	memFree(value);
	return NULL;
}

static struct Token strong *genDoubleColon(char const strong *value) {
	struct DoubleColonToken strong *token = memAlloc(sizeof(struct DoubleColonToken));
	token->base.type = DOUBLE_COLON_TOKEN;
	token->base.raw = value;
	return (struct Token strong *) token;
}

static struct Token strong *genEquals(char const strong *value) {
	struct EqualsToken strong *token = memAlloc(sizeof(struct EqualsToken));
	token->base.type = EQUALS_TOKEN;
	token->base.raw = value;
	return (struct Token strong *) token;
}

static struct Token strong *genSemiColon(char const strong *value) {
	struct SemiColonToken strong *token = memAlloc(sizeof(struct SemiColonToken));
	token->base.type = SEMI_COLON_TOKEN;
	token->base.raw = value;
	return (struct Token strong *) token;
}

static struct Token strong *genMod(char const strong *value) {
	struct ModToken strong *token = memAlloc(sizeof(struct ModToken));
	token->base.type = MOD_TOKEN;
	token->base.raw = value;
	return (struct Token strong *) token;
}

static struct Token strong *genUse(char const strong *value) {
	struct UseToken strong *token = memAlloc(sizeof(struct UseToken));
	token->base.type = USE_TOKEN;
	token->base.raw = value;
	return (struct Token strong *) token;
} 

static struct Token strong *genAdd(char const strong *value) {
	struct AddToken strong *token = memAlloc(sizeof(struct AddToken));
	token->base.type = ADD_TOKEN;
	token->base.raw = value;
	return (struct Token strong *) token;
}

static struct Token strong *genSub(char const strong *value) {
	struct SubToken *token = memAlloc(sizeof(struct SubToken));
	token->base.type = SUB_TOKEN;
	token->base.raw = value;
	return (struct Token strong *) token;
}

static struct Token strong *genMul(char const strong *value) {
	struct MulToken strong *token = memAlloc(sizeof(struct MulToken));
	token->base.type = MUL_TOKEN;
	token->base.raw = value;
	return (struct Token strong *) token;
}

static struct Token strong *genDiv(char const strong *value) {
	struct DivToken strong *token = memAlloc(sizeof(struct DivToken));
	token->base.type = DIV_TOKEN;
	token->base.raw = value;
	return (struct Token strong *) token;
}

static struct Token strong *genFrom(char const strong *value) {
	struct FromToken strong *token = memAlloc(sizeof(struct FromToken));
	token->base.type = FROM_TOKEN;
	token->base.raw = value;
	return (struct Token  strong *) token;
}

static struct Token strong *genDef(char const strong *value) {
	struct DefToken strong *token = memAlloc(sizeof(struct FromToken));
	token->base.type = DEF_TOKEN;
	token->base.raw = value;
	return (struct Token strong *) token;
}

static struct Token strong *genComma(char const strong *value) {
	struct CommaToken strong *token = memAlloc(sizeof(struct CommaToken));
	token->base.type = COMMA_TOKEN;
	token->base.raw = value;
	return (struct Token strong *) token;
}

static char const strong *substring(char const weak *string, int position, int length) {
	int c;

	char *pointer = memAlloc(length+1);
 
	for (c = 0; c < length; c++) {
		pointer[c] = string[position + c];
	}
 
	pointer[c] = '\0';
 
	return pointer;
}

static struct Token *genString(char const *value) {
	struct StringToken *token = memAlloc(sizeof(struct StringToken));
	token->base.type = STRING_TOKEN;
	token->base.raw = value;
	token->value = substring(value, 1, strlen(value) - 2);
	return (struct Token *) token;
}

static struct Token *genClass(char const *value) {
	struct ClassToken *token = memAlloc(sizeof(struct ClassToken));
	token->base.type = CLASS_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

static struct Token *genPad(char const *value) {
	struct PadToken *token = memAlloc(sizeof(struct PadToken));
	token->base.type = PAD_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

static struct Token *genChar(char const *value) {
	struct CharToken *token = memAlloc(sizeof(struct CharToken));
	token->base.type = CHAR_TOKEN;
	token->base.raw = value;
	token->value = value[1];
	return (struct Token *) token;
}

static struct Token *genDot(char const *value) {
	struct DotToken *token = memAlloc(sizeof(struct DotToken));
	token->base.type = DOT_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

static struct Token *genBuild(char const *value) {
	struct BuildToken *token = memAlloc(sizeof(struct BuildToken));
	token->base.type = BUILD_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

static struct Token strong *genByte(char const strong *value) {
	struct ByteToken strong *token = memAlloc(sizeof(struct ByteToken));
	token->base.type = BYTE_TOKEN;
	token->base.raw = value;
	char const strong *subString = substring(value, 0, strlen(value) - 1);
	token->value = atoi(subString);
	memFree(subString);
	return (struct Token strong *) token;
}

static struct Token strong *genIf(char const strong *value) {
	struct IfToken strong *token = memAlloc(sizeof(struct IfToken));
	token->base.type = IF_TOKEN;
	token->base.raw = value;
	return (struct Token strong *) token;
}

static struct Token strong *genBool(char const strong *value) {
	struct BoolToken strong *token = memAlloc(sizeof(struct BoolToken));
	token->base.type = BOOL_TOKEN;
	token->base.raw = value;
	if (!strcmp(value, "true")) {
		token->value = true;
	} else if (!strcmp(value, "false")) {
		token->value = false;
	} else {
		logMsg(LOG_ERROR, 4, "Invalid Bool match");
		exit(EXIT_FAILURE);
	}
	return (struct Token strong *) token;
}

static struct Token strong *genTypeToken(char const strong *value) {
    struct TypeToken strong *token = memAlloc(sizeof(struct TypeToken));
    token->base.type = TYPE_TOKEN;
    token->base.raw = value;
    return (struct Token strong *) token;
}

static struct Token strong *genExtToken(char const strong *value) {
    struct ExtToken strong *token = memAlloc(sizeof(struct ExtToken));
    token->base.type = EXT_TOKEN;
    token->base.raw = value;
    return (struct Token strong *) token;
}

static struct Pattern PATTERNS[] = {
	{INT_TOKEN, "", "^[0-9_]+$", &genInt},
	{STRING_TOKEN, "", "^\"[^\"]*\"$", &genString},
	{ID_TOKEN, "", "^[a-zA-Z_][a-zA-Z0-9_]*$", &genId},
	{L_PAREN_TOKEN, "", "^\\($", &genLParen},
	{R_PAREN_TOKEN, "", "^\\)$", &genRParen},
	{L_BRACKET_TOKEN, "", "^\\[$", &genLBracket},
	{R_BRACKET_TOKEN, "", "^\\]$", &genRBracket},
	{L_BRACE_TOKEN, "", "^\\{$", &genLBrace},
	{R_BRACE_TOKEN, "", "^\\}$", &genRBrace},
	{EQUALS_TOKEN, "", "^:=$", &genEquals},
	{DOUBLE_COLON_TOKEN, "", "^::$", &genDoubleColon},
	{COLON_TOKEN, "", "^:$", &genColon},
	{ARROW_TOKEN, "", "^->$", &genArrow},
	{FUNC_TOKEN, "", "^func$", &genFunc},
	{BUILD_TOKEN, "", "^build$", &genBuild},
	{VAR_TOKEN, "", "^var$", &genVar},
	{FOR_TOKEN, "", "^for$", &genFor},
	{IF_TOKEN, "", "^if$", &genIf},
	{BOOL_TOKEN, "", "^true$", &genBool},
	{BOOL_TOKEN, "", "^false$", &genBool},
	{SEMI_COLON_TOKEN, "", "^;$", &genSemiColon},
	{MOD_TOKEN, "", "^mod$", &genMod},
	{USE_TOKEN, "", "^use$", &genUse},
	{FROM_TOKEN, "", "^from$", &genFrom},
	{DEF_TOKEN, "", "^def$", &genDef},
	{CLASS_TOKEN, "", "^class$", &genClass},
	{PAD_TOKEN, "", "^pad$", &genPad},
	{COMMA_TOKEN, "", "^,$", &genComma},
	{ADD_TOKEN, "", "^\\+$", &genAdd},
	{SUB_TOKEN, "", "^\\-$", &genSub},
	{MUL_TOKEN, "", "^\\*$", &genMul},
	{DIV_TOKEN, "", "^\\/$", &genDiv},
	{CHAR_TOKEN, "", "^'.'$", &genChar},
	{DOT_TOKEN, "", "^\\.$", &genDot},
	{BYTE_TOKEN, "", "^[0-9_]+B$", &genByte},
	{TYPE_TOKEN, "", "^type$", &genTypeToken},
    {EXT_TOKEN, "", "^ext$", &genExtToken},
    //{1000, "", "^//.*\n", &genWhitespace},
	{1000, "", "^[ \n\t\r\v]+$", &genWhitespace},
};

struct Pattern NULL_PATTERN = {NULL_TOKEN, NULL, NULL, &null};

// From regcomp docs
bool match(const char weak *string, const char weak *pattern) {
	int status;
	regex_t re;

	if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) {
		logMsg(LOG_ERROR, 4, "Regex Compilation Failure");
		exit(EXIT_FAILURE);
	}
	status = regexec(&re, string, (size_t) 0, NULL, 0);
	regfree(&re);
	if (status != 0) {
		return false;      /* Report error. */
	}
	return true;
}

char strong *pushChar(char strong *weak *buffer, int weak *bufferCount, char c, unsigned int weak *line, unsigned int weak *column) {
	(*buffer) = (char strong *) memRealloc(*buffer, (*bufferCount + 2) * sizeof(char)); // NULL terminator
    if (c == '\n') {
        (*line)++;
        *column = 1;
    } else {
        (*column)++;
    }
	(*buffer)[*bufferCount] = c;
	(*bufferCount)++;
	(*buffer)[*bufferCount] = '\0';
	return *buffer;
}

struct Token const **pushToken(struct Token const strong *strong *weak *buffer, int *bufferCount, struct Token strong *token, unsigned int line, unsigned int column) {
	if (token == NULL) {
		logMsg(LOG_INFO, 1, "Not Pushing WhiteSpace");
	} else {
		logMsg(LOG_INFO, 1, "Pushing Token %s %u", token->raw, *bufferCount);
		//struct Token const strong *strong *old = *buffer;
		//memFree(old);
		//(*buffer) = memAlloc((*bufferCount + 1) * sizeof(struct Token *));
		//memcpy(*buffer, old, *bufferCount * sizeof(struct Token *));
		(*buffer) = memRealloc(*buffer, (*bufferCount + 1) * sizeof(struct Token *));
        token->line = line;
        token->column = column;
		(*buffer)[*bufferCount] = token;
		(*bufferCount)++;
		logMsg(LOG_INFO, 1, "Pushed Token %u", (*bufferCount));
	}
	return *buffer;
}

struct Pattern matchingPattern(const char weak *buffer) {
	struct Pattern ret = NULL_PATTERN;
	for (unsigned int i = 0; i < sizeof(PATTERNS) / sizeof(struct Pattern); i ++) {
		bool thisMatch = match(buffer, PATTERNS[i].pattern);
		if (thisMatch) {
			ret = PATTERNS[i];
		}
	}
	return ret;
}

struct Token const strong *const strong *lex(const char weak *input) {
	struct Token const strong *strong *out = NULL;
	int tokenCount = 0;
    unsigned int line = 1;
    unsigned int column = 1;
    //char *buffer = memAlloc(1 * sizeof(char)); // NULL terminated
	//int bufferCount = 0;
	//buffer[bufferCount] = '\0';
	
	while (*input != '\0') {
        unsigned int startLine = line;
        unsigned int startColumn = column;
		char strong *buffer = memAlloc(1 * sizeof(char)); // NULL terminated
		int bufferCount = 0;
		buffer[bufferCount] = '\0';

		bool matched = false;
		struct Pattern pattern;
		//struct Token *(*tokenGenFunc)(const char *);
	
		while (!matched) {
			if (*input == '\0') {
				logMsg(LOG_ERROR, 4, "Reached End Of File");
				logMsg(LOG_ERROR, 4, buffer);
				exit(-1);
			}
			
			pushChar(&buffer, &bufferCount, *input, &line, &column);
			input++;

			pattern = matchingPattern(buffer);
			matched = pattern.type != NULL_TOKEN;
		}
		
		while (matched) {
			bool eof = *input == '\0';
			pushChar(&buffer, &bufferCount, *input, &line, &column);
			input++;

			struct Pattern testingPattern;
			if (!eof) {
				testingPattern = matchingPattern(buffer);
			} else {
				testingPattern = NULL_PATTERN;
			}	
			matched = testingPattern.type != NULL_TOKEN;
			if (matched) {
				pattern = testingPattern;
			}
		}
		assert(bufferCount > 0);
		buffer[bufferCount - 1] = '\0';
		input--;
			
		logMsg(LOG_INFO, 2, "%d\n", bufferCount);
		logMsg(LOG_INFO, 2, buffer);
		logMsg(LOG_INFO, 2, pattern.pattern);
		pushToken(&out, &tokenCount, pattern.gen(buffer), startLine, startColumn);
	}

	
	struct Token strong *eofToken = memAlloc(sizeof(struct Token));
	eofToken->type = EOF_TOKEN;
	eofToken->raw = heapString("");
	pushToken(&out, &tokenCount, eofToken, line, column);

	logMsg(LOG_INFO, 1, "End of lex");

	return out;
}
