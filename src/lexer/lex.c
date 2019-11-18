#include "../util/log.h"
#include "token.h"

struct Token **lex(const char *input) {
	for (char c = *input; c != '\0'; c = *++input) switch (c) {
		default:
			logMsg(LOG_ERROR, "Unexpected Character While Performing Lexical Analysis");
			logMsg(LOG_ERROR, input);
			break;
	
	}
}
