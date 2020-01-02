#include <stdlib.h>
#include <stdio.h>
#include "util/log.h"
#include "util/mem.h"
#include "code-gen/llvm/llvm-code-gen.h"
#include "lexer/token.h"
#include "lexer/lex.h"
#include "parser/parser.h"
#include "file/file.h"

const char * testFile =
"mod test::test::test\n"
"use std::io\n"
"func main() -> Int {\n"
	"var x: Int = 10;\n"
	"var y: Int = x * 10\n;"
	"ret = x;\n"
	"x = ret * x * y;\n"
	"ret = x;\n"
"}\n"
"func test() -> Int ret = 5;";

void logTokens(struct Token const *const *tokens) {
	struct Token const *const *token = tokens;
	for (; (*token) == NULL || (*token)->type != EOF_TOKEN; token++) {
		if (*token == NULL) {
			logMsg(LOG_INFO, 2, "NULL");
		} else {
			logMsg(LOG_INFO, 2, "%s", (*token)->raw);
		}
	}
}

int main(int argc, char const **argv) {

	setLogLevel(1);
	while (argc > 1) {
		char const *fileName = argv[argc - 1];
		argc--;

		char const *source = readFile(fileName);
	
		struct Token const *const *tokens = lex(source);
		
		logTokens(tokens);
		struct Module tree = parse(tokens);
		codeGenLLVM(&tree);
		
		delModule(tree);

		delTokens(tokens);
		
		memFree(source);
	}
	
	memLog();

	return 0;
}
