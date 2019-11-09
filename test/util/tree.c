#include <assert.h>
#include <stddef.h>
#include <string.h>
#include "../../src/util/tree.h"

void treeCreateTest() {
	struct Tree *tree;
	tree = treeCreate();
	assert(tree == NULL);
}

void treeAddTest() {
	struct Tree *tree;
	tree = treeCreate();
	tree = treeAdd(tree, "test0", "test0");
	
	assert(strcmp(treeLookUp(tree, "test0"), "test0") == 0);
}

int main() {
	treeCreateTest();
	treeAddTest();
}
