#include "../util/mem.h"
#include "token.h"

void delTokens(struct Token const strong *const strong *tokens) {
	struct Token const strong *const weak *pTokens;
	for (pTokens = tokens; (*pTokens)->type != EOF_TOKEN; pTokens++) {
		memFree((*pTokens)->raw);
		switch ((*pTokens)->type) {
			case STRING_TOKEN: {
					struct StringToken weak *stringToken = (struct StringToken weak *) *pTokens;
					memFree(stringToken->value);
				}
				break;
			case ID_TOKEN: {
					// weak
				}
				break;


			default:
				break;
		}
		memFree(*pTokens);
	}
	memFree((*pTokens)->raw);
	memFree(*pTokens);
	memFree(tokens);
}
