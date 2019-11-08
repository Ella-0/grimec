#include <stdlib.h>
#include <stdio.h>
#include "log.h"
#define NORMAL "\x1b[0m"
#define INFO_COLOUR "\x1b[34m"
#define WARNING_COLOUR "\x1b[35m" 
#define ERROR_COLOUR "\x1b[31m"


void logMsg(enum LogLevel level, const char *msg) {
	switch (level) {
		case LOG_INFO:
			fprintf(stderr, INFO_COLOUR "Info: " NORMAL "%s\n", msg);
			break;
		case LOG_WARNING:
			fprintf(stderr, WARNING_COLOUR "Warning: " NORMAL "%s\n", msg);
			break;
		case LOG_ERROR:
			fprintf(stderr, ERROR_COLOUR "Error: " NORMAL "%s\n", msg);
			break;
		default:
			logMsg(LOG_ERROR, "Invalid Log Level!");
			exit(-1);
	}
}
