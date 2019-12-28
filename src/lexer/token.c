#include "../util/mem.h"
#include "token.h"

void delTokens(struct Token const strong *const strong *tokens) {
	struct Token const strong *const weak *pTokens;
	for (pTokens = tokens; (*pTokens)->type != EOF_TOKEN; pTokens++) {
		switch ((*pTokens)->type) {
			case STRING_TOKEN: {
					struct StringToken weak *stringToken = (struct StringToken weak *) *pTokens;
					memFree(stringToken->value);
				}
				break;
			case ID_TOKEN: {
					struct IdToken weak *idToken = (struct IdToken weak *) *pTokens;
					memFree(idToken->value);
				}
				break;


			default:
				break;
		}
		memFree(*pTokens);
	}
	memFree(*pTokens);
	memFree(tokens);
}
