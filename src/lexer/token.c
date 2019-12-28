#include "../util/mem.h"
#include "token.h"

void delTokens(struct Token const *const *tokens) {
	struct Token const *const *pTokens;
	for (pTokens = tokens; (*pTokens)->type != EOF_TOKEN; pTokens++) {
		memFree(*pTokens);
	}
	memFree(*pTokens);
	memFree(tokens);
}
