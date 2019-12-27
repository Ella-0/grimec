#include <stdlib.h>
#include <stdbool.h>
#include <regex.h>
#include <string.h>
#include "../util/mem.h"
#include "../util/log.h"
#include "token.h"

struct Pattern {
	enum TokenType type;
	const char *partPattern;
	const char *pattern;
	struct Token *(*gen)(const char *value);
};

struct Token *genInt(const char *value) {
	struct IntToken *token = memAlloc(sizeof(struct IntToken));
	token->base.type = INT_TOKEN;
	token->base.raw = value;
	token->value = atoi(value);
	return (struct Token *) token;
}

struct Token *genId(const char *value) {
	struct IdToken *token = memAlloc(sizeof(struct IdToken));
	token->base.type = ID_TOKEN;
	token->base.raw = value;
	token->value = value;
	return (struct Token *) token;
}

struct Token *genLParen(const char *value) {
	struct LParenToken *token = memAlloc(sizeof(struct LParenToken));
	token->base.type = L_PAREN_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

struct Token *genRParen(const char *value) {
	struct RParenToken *token = memAlloc(sizeof(struct RParenToken));
	token->base.type = R_PAREN_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

struct Token *genLBracket(const char *value) {
	struct LBracketToken *token = memAlloc(sizeof(struct LBracketToken));
	token->base.type = L_BRACKET_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

struct Token *genRBracket(const char *value) {
	struct RBracketToken *token = memAlloc(sizeof(struct RBracketToken));
	token->base.type = R_BRACKET_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

struct Token *genLBrace(const char *value) {
	struct LBraceToken *token = memAlloc(sizeof(struct LBraceToken));
	token->base.type = L_BRACE_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

struct Token *genRBrace(const char *value) {
	struct RBraceToken *token = memAlloc(sizeof(struct RBraceToken));
	token->base.type = R_BRACE_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

struct Token *genFunc(const char *value) {
	struct FuncToken *token = memAlloc(sizeof(struct FuncToken));
	token->base.type = FUNC_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

struct Token *genVar(const char *value) {
	struct VarToken *token = memAlloc(sizeof(struct VarToken));
	token->base.type = VAR_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

struct Token *genFor(const char *value) {
	struct ForToken *token = memAlloc(sizeof(struct ForToken));
	token->base.type = FOR_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

struct Token *genColon(char const *value) {
	struct ColonToken *token = memAlloc(sizeof(struct ColonToken));
	token->base.type = COLON_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

struct Token *genArrow(char const *value) {
	struct ArrowToken *token = memAlloc(sizeof(struct ArrowToken));
	token->base.type = ARROW_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

struct Token *null(const char *value) {
	logMsg(LOG_ERROR, 4, "Unimplemened Token Matched");
	return NULL;
}

struct Token *genWhitespace(const char *value) {
	return NULL;
}

struct Token *genDoubleColon(char const *value) {
	struct DoubleColonToken *token = memAlloc(sizeof(struct DoubleColonToken));
	token->base.type = DOUBLE_COLON_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

struct Token *genEquals(char const *value) {
	struct EqualsToken *token = memAlloc(sizeof(struct EqualsToken));
	token->base.type = EQUALS_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

struct Token *genSemiColon(char const *value) {
	struct SemiColonToken *token = memAlloc(sizeof(struct SemiColonToken));
	token->base.type = SEMI_COLON_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

struct Token *genMod(char const *value) {
	struct ModToken *token = memAlloc(sizeof(struct ModToken));
	token->base.type = MOD_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

struct Token *genUse(char const *value) {
	struct UseToken *token = memAlloc(sizeof(struct UseToken));
	token->base.type = USE_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
} 

struct Token *genAdd(char const *value) {
	struct AddToken *token = memAlloc(sizeof(struct AddToken));
	token->base.type = ADD_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

struct Token *genSub(char const *value) {
	struct SubToken *token = memAlloc(sizeof(struct SubToken));
	token->base.type = SUB_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

struct Token *genMul(char const *value) {
	struct MulToken *token = memAlloc(sizeof(struct MulToken));
	token->base.type = MUL_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

struct Token *genDiv(char const *value) {
	struct DivToken *token = memAlloc(sizeof(struct DivToken));
	token->base.type = DIV_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

struct Token *genFrom(char const *value) {
	struct FromToken *token = memAlloc(sizeof(struct FromToken));
	token->base.type = FROM_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

struct Token *genDef(char const *value) {
	struct DefToken *token = memAlloc(sizeof(struct FromToken));
	token->base.type = DEF_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

struct Token *genComma(char const *value) {
	struct CommaToken *token = memAlloc(sizeof(struct CommaToken));
	token->base.type = COMMA_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

char const *substring(char const *string, int position, int length) {
	char *pointer;
	int c;

	pointer = memAlloc(length+1);

	if (pointer == NULL) {
		logMsg(LOG_ERROR, 4, "Unable to allocate memory");
		exit(-1);
	}
 
	for (c = 0; c < length; c++) {
		*(pointer+c) = *(string+position-1);      
		string++;  
	}
 
	*(pointer+c) = '\0';
 
	return pointer;
}

struct Token *genString(char const *value) {
	struct StringToken *token = memAlloc(sizeof(struct StringToken));
	token->base.type = STRING_TOKEN;
	token->base.raw = value;
	token->value = substring(value, 2, strlen(value) - 2);
	return (struct Token *) token;
}

struct Token *genClass(char const *value) {
	struct ClassToken *token = memAlloc(sizeof(struct ClassToken));
	token->base.type = CLASS_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

struct Token *genPad(char const *value) {
	struct PadToken *token = memAlloc(sizeof(struct PadToken));
	token->base.type = PAD_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

struct Token *genChar(char const *value) {
	struct CharToken *token = memAlloc(sizeof(struct CharToken));
	token->base.type = CHAR_TOKEN;
	token->base.raw = value;
	token->value = value[1];
	return (struct Token *) token;
}

struct Token *genDot(char const *value) {
	struct DotToken *token = memAlloc(sizeof(struct DotToken));
	token->base.type = DOT_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

struct Token *genBuild(char const *value) {
	struct BuildToken *token = memAlloc(sizeof(struct BuildToken));
	token->base.type = BUILD_TOKEN;
	token->base.raw = value;
	return (struct Token *) token;
}

struct Pattern PATTERNS[] = {
	{INT_TOKEN, "", "^[0-9_]+$", &genInt},
	{STRING_TOKEN, "", "^\"[^\"]*\"$", &genString},
	{ID_TOKEN, "", "^[a-zA-Z_][a-zA-Z0-9_]*$", &genId},
	{L_PAREN_TOKEN, "", "^\\($", &genLParen},
	{R_PAREN_TOKEN, "", "^\\)$", &genRParen},
	{L_BRACKET_TOKEN, "", "^\\[$", &genLBracket},
	{R_BRACKET_TOKEN, "", "^\\]$", &genRBracket},
	{L_BRACE_TOKEN, "", "^\\{$", &genLBrace},
	{R_BRACE_TOKEN, "", "^\\}$", &genRBrace},
	{COLON_TOKEN, "", "^:$", &genColon},
	{DOUBLE_COLON_TOKEN, "", "^::$", &genDoubleColon},
	{ARROW_TOKEN, "", "^->$", &genArrow},
	{FUNC_TOKEN, "", "^func$", &genFunc},
	{BUILD_TOKEN, "", "^build$", &genBuild},
	{VAR_TOKEN, "", "^var$", &genVar},
	{FOR_TOKEN, "", "^for$", &genFor},
	{EQUALS_TOKEN, "", "^=$", &genEquals},
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
	{1000, "", "^[ \n\t\r\v]+$", &genWhitespace},
};

struct Pattern NULL_PATTERN = {NULL_TOKEN, NULL, NULL};

// From regcomp docs
bool match(const char *string, const char *pattern) {
	int status;
	regex_t re;

	if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) {
		return false;      /* Report error. */
	}
	status = regexec(&re, string, (size_t) 0, NULL, 0);
	regfree(&re);
	if (status != 0) {
		return false;      /* Report error. */
	}
	return true;
}

char *pushChar(char **buffer, int *bufferCount, char c) {
	(*buffer) = memRealloc(*buffer, (*bufferCount + 2) * sizeof(char)); // NULL terminator
	(*buffer)[*bufferCount] = c;
	(*bufferCount)++;
	(*buffer)[*bufferCount] = '\0';
	return *buffer;
}

struct Token const **pushToken(struct Token const ***buffer, int *bufferCount, struct Token *token) {
	if (token == NULL) {
		logMsg(LOG_INFO, 1, "Not Pushing WhiteSpace");
	} else {
		logMsg(LOG_INFO, 1, "Pushing Token %s", token->raw);
		(*buffer) = memRealloc(*buffer, (*bufferCount + 1) * sizeof(struct Token *));
		(*buffer)[*bufferCount] = token;
		(*bufferCount)++;
		logMsg(LOG_INFO, 1, "Pushed Token");
	}
	return *buffer;
}

struct Pattern matchingPattern(const char *buffer) {
	struct Pattern ret = NULL_PATTERN;
	for (int i = 0; i < sizeof(PATTERNS) / sizeof(struct Pattern); i ++) {
		bool thisMatch = match(buffer, PATTERNS[i].pattern);
		if (thisMatch) {
			ret = PATTERNS[i];
		}
	}
	return ret;
}

struct Token const *const *lex(const char *input) {
	struct Token const **out = NULL;
	int tokenCount = 0;
	//char *buffer = memAlloc(1 * sizeof(char)); // NULL terminated
	//int bufferCount = 0;
	//buffer[bufferCount] = '\0';
	
	while (*input != '\0') {
		char *buffer = memAlloc(1 * sizeof(char)); // NULL terminated
		int bufferCount = 0;
		buffer[bufferCount] = '\0';

		bool matched = false;
		struct Pattern pattern;
		struct Token *(*tokenGenFunc)(const char *);
	
		while (!matched) {
			if (*input == '\0') {
				logMsg(LOG_ERROR, 4, "Reached End Of File");
				logMsg(LOG_ERROR, 4, buffer);
				exit(-1);
			}
			
			pushChar(&buffer, &bufferCount, *input);
			input++;

			pattern = matchingPattern(buffer);
			matched = pattern.type != NULL_TOKEN;
		}
		
		while (matched) {
			bool eof = *input == '\0';
			pushChar(&buffer, &bufferCount, *input);
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

		buffer[bufferCount - 1] = '\0';
		input--;
			
		logMsg(LOG_INFO, 2, "%d\n", bufferCount);
		logMsg(LOG_INFO, 2, buffer);
		logMsg(LOG_INFO, 2, pattern.pattern);
		pushToken(&out, &tokenCount, pattern.gen(buffer));
	}

	logMsg(LOG_INFO, 1, "First Token: ");	
	logMsg(LOG_INFO, 1, (*out)->raw);
	
	struct Token *eofToken = malloc(sizeof(struct Token));
	eofToken->type = EOF_TOKEN;
	eofToken->raw = "";
	pushToken(&out, &tokenCount, eofToken);

	return out;
}
