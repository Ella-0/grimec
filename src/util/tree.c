#include <stdlib.h>
#include <string.h>
#include "mem.h"
#include "tree.h"

struct Tree *treeCreate() {
	return NULL;
}

struct Tree *treeAdd(struct Tree *tree, const char *key, void *value) {
	if (tree == NULL) {
		struct Tree *out = memAlloc(sizeof(struct Tree));
		out->key = key;
		out->value = value;
		out->left = NULL;
		out->right = NULL;
		return out;
	} else {
		int cmp = strcmp(tree->key, key);
		if (cmp > 0) {
			tree->right = treeAdd(tree->right, key, value);
			return tree;
		} else if (cmp == 0) {
			tree->value = value;
			return tree;
		} else if (cmp < 0) {
			tree->left = treeAdd(tree->left, key, value);
			return tree;
		}
	}
	return NULL;
}

void treeDel(struct Tree *tree) {
	if (tree != NULL) {
		treeDel(tree->left);
		treeDel(tree->right);
		memFree(tree);
	}
}

void *treeLookUp(struct Tree *tree, const char *key) {
	if (tree == NULL) {
		return NULL;
	} else {
		int cmp = strcmp(tree->key, key);
		if (cmp > 0) {
			return treeLookUp(tree->right, key);
		} else if (cmp == 0) {
			return tree->value;
		} else if (cmp < 0) {
			return treeLookUp(tree->left, key);
		}
	}
	return NULL;
}
