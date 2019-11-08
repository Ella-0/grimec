enum LogLevel {
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,
};

void logMsg(enum LogLevel level, const char *msg);
