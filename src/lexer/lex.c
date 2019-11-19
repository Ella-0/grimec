#include <stdlib.h>
#include <stdbool.h>
#include <regex.h>
#include "../util/mem.h"
#include "../util/log.h"
#include "token.h"

struct Pattern {
	enum TokenType type;
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

struct Token *null(const char *value) {
	return NULL;
}

#define PATTERN_COUNT 1
struct Pattern PATTERNS[PATTERN_COUNT] = {
	{INT_TOKEN, "^[0-9]+$", &genInt},
	{STRING_TOKEN, "^[a-z]+$", &null},
	{1000, "^[ ]$", &null}
};

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

struct Token **pushToken(struct Token ***buffer, int *bufferCount, struct Token *token) {
	(*buffer) = memRealloc(*buffer, (*bufferCount + 1) * sizeof(struct Token *));
	(*buffer)[*bufferCount] = token;
	(*bufferCount)++;
	return *buffer;
}

struct Token **lex(const char *input) {
	struct Token **out = NULL;
	int tokenCount = 0;
	char *buffer = memAlloc(1 * sizeof(char)); // NULL terminated
	int bufferCount = 0;
	buffer[bufferCount] = '\0';
	bool matched = false;
	bool eof = false;
	while (!eof) {
		struct Token *(*tokenGenFunc)(const char *);
		while (!matched && !eof) {
			pushChar(&buffer, &bufferCount, *input);
			input++;
			for (int i = 0; i < PATTERN_COUNT; i ++) {
				bool thisMatch = match(buffer, PATTERNS[i].pattern);
				if (thisMatch) {
					tokenGenFunc = PATTERNS[i].gen;
					matched = true;
				}
			}
			logMsg(LOG_INFO, buffer);
			if (*input == '\0') {
				eof = true;
				logMsg(LOG_ERROR, "Reached End Of File");
				exit(-1);
			}
		}
		logMsg(LOG_INFO, "Phase 2");
		while (matched && !eof) {
			pushChar(&buffer, &bufferCount, *input);
			input++;
			for (int i = 0; i < PATTERN_COUNT; i ++) {
				bool thisMatch = match(buffer, PATTERNS[i].pattern);
				if (thisMatch) {
					tokenGenFunc = PATTERNS[i].gen;
				} else {
					matched = false;
				}
			}
			logMsg(LOG_INFO, buffer);
			if (*input == '\0') {
				eof = true;
				logMsg(LOG_INFO, "Reached End Of File");
			}
		}
		buffer[bufferCount] = '\0';
		logMsg(LOG_INFO, buffer);
		pushToken(&out, &tokenCount, tokenGenFunc(buffer));	

		buffer = memAlloc(1 * sizeof(char));
		bufferCount = 0;
		buffer[bufferCount] = '\0';
	}
	logMsg(LOG_INFO, (*out)->raw);
	return out;
}
