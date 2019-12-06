#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "log.h"
#define NORMAL "\x1b[0m"
#define INFO_COLOUR "\x1b[34m"
#define WARNING_COLOUR "\x1b[35m" 
#define ERROR_COLOUR "\x1b[31m"

int logLevel = 4;

void setLogLevel(int level) {
	logLevel = level;
}

void logMsg(enum LogType type, int level, const char *msg, ...) {
	if (level >= logLevel) {
		switch (type) {
			case LOG_INFO:
				fprintf(stderr, "grimec: " INFO_COLOUR "Info: " NORMAL "%s\n", msg);
				break;
			case LOG_WARNING:
				fprintf(stderr, "grimec: " WARNING_COLOUR "Warning: " NORMAL "%s\n", msg);
				break;
			case LOG_ERROR:
				fprintf(stderr, "grimec: " ERROR_COLOUR "Error: " NORMAL "%s\n", msg);
				break;
			default:
				logMsg(LOG_ERROR, 4, "Invalid Log Level!");
				exit(-1);
		}
	}
}
