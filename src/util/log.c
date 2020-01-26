#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "colours.h"
#include "log.h"
#define NORMAL DEFAULT
#define INFO_COLOUR BLUE
#define WARNING_COLOUR MAGENTA 
#define ERROR_COLOUR RED

static int logLevel = 4;

void setLogLevel(int level) {
	logLevel = level;
}

void logMsg(enum LogType type, int level, char const weak *msg, ...) {
	va_list arglist;
	va_start(arglist, msg);
	vlogMsg(type, level, msg, arglist);
	va_end(arglist);
}

void vlogMsg(enum LogType type, int level, char const weak *msg, va_list ap) {
	if (level >= logLevel) {
		switch (type) {
			case LOG_INFO:
				(void) fprintf(stderr, "grimec: " INFO_COLOUR "Info: " NORMAL);
				break;
			case LOG_WARNING:
				(void) fprintf(stderr, "grimec: " WARNING_COLOUR "Warning: " NORMAL);
				break;
			case LOG_ERROR:
				(void) fprintf(stderr, "grimec: " ERROR_COLOUR "Error: " NORMAL);
				break;
			default:
				logMsg(LOG_ERROR, 4, "Invalid Log Level!");
				exit(EXIT_FAILURE);
		}
		(void) vfprintf(stderr, msg, ap);
		(void) fprintf(stderr, "\n");
		fflush(stderr);
	}
}
