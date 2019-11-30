#include <stdlib.h>
#include <stdbool.h>
#include <regex.h>
#include "../util/mem.h"
#include "../util/log.h"
#include "token.h"
#include <stdio.h>

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

struct Pattern PATTERNS[] = {
	{INT_TOKEN, "^[0-9]+$", &genInt},
	{STRING_TOKEN, "^[a-z]+$", &null},
	{1000, "^[ \n\t\r\v]+$", &null}
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

struct Token **pushToken(struct Token ***buffer, int *bufferCount, struct Token *token) {
	logMsg(LOG_INFO, "Pushing Token");
	(*buffer) = memRealloc(*buffer, (*bufferCount + 1) * sizeof(struct Token *));
	(*buffer)[*bufferCount] = token;
	(*bufferCount)++;
	logMsg(LOG_INFO, "Pushed Token");
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

struct Token **lex(const char *input) {
	struct Token **out = NULL;
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
				logMsg(LOG_ERROR, "Reached End Of File");
				logMsg(LOG_ERROR, buffer);
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
			
		fprintf(stderr, "%d\n", bufferCount);
		logMsg(LOG_INFO, buffer);
		pushToken(&out, &tokenCount, pattern.gen(buffer));
	}
	
	logMsg(LOG_INFO, (*out)->raw);
	return out;
}
