#pragma once

enum TokenType {
	EOF_TOKEN,
	STRING_TOKEN,
	INT_TOKEN,
};

struct Token {
	enum TokenType type;
	const char *raw;
};

struct StringToken {
	struct Token base;
	const char *value;
};

struct IntToken {
	struct Token base;
	long long value;
};
