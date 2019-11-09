//TODO Generic

struct Tree {
	const char *key;
	void *value;
	struct Tree *left;
	struct Tree *right;
};

struct Tree *treeCreate();

struct Tree *treeAdd(struct Tree *tree, const char *key, void *value);
void treeDel(struct Tree *tree);

void *treeLookUp(struct Tree *tree, const char *key); //add function pointer for generics

