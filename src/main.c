#include <stdlib.h>
#include <stdio.h>
#include "util/log.h"
#include "util/mem.h"
#include "code-gen/llvm/llvm-code-gen.h"
#include "lexer/token.h"
#include "lexer/lex.h"
#include "parser/parser.h"

const char * testFile =
"mod test::test::test\n"
"func main() -> Int ret = (13 + 71) * 10;\n"
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

int main() {
	setLogLevel(0);

	struct Token const *const *tokens = lex(testFile);

	logTokens(tokens);
	struct Module tree = parse(tokens);
	codeGenLLVM(&tree);

	logMsg(LOG_ERROR, 1, "Memory Still Allocated: %d", memLeaks());
	return 0;
}
