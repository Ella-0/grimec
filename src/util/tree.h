#pragma once
#include "mem.h"
//TODO Generic

struct Tree {
	char const weak *key;
	void weak *value;
	struct Tree strong *left;
	struct Tree strong *right;
};

struct Tree strong *treeCreate();

struct Tree strong *treeAdd(struct Tree strong *tree, char const weak *key, void weak *value);
void treeDel(struct Tree strong *tree);

void weak *treeLookUp(struct Tree weak *tree, char const weak *key); //add function pointer for generics

