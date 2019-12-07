#pragma once
#include <stdarg.h>
#include "../lexer/token.h"

enum LogType {
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,
};

void setLogLevel(int level);

// 0 - individual mem allocations
// 1 - log steps for each cyle of the lexer, parser, code-gen, etc.
// 2 - log when lexer, parser, code-gen, etc steps are completed.
// 3 - basic verbose logging. E.G "Started Parsing", "Finished Parsing".
// 4 - required logging like fatal errors.
void logMsg(enum LogType type, int level, const char *msg, ...);
void vlogMsg(enum LogType type, int level, const char *msg, va_list vp);
