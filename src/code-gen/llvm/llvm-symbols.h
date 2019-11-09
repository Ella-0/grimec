struct VarSymbolLLVM {
	const char *name;
	LLVMValueRef *value; // Pointer to allow mutability for SSA.
};

struct VarTableLLVM {
	unsigned int varCount;
	struct VarSymbolLLVM **; // Allow for base struct in the future with different types of vars.
};

void addVarToTableLLVM(struct VarTableLLVM *table, struct VarSymbolLLVM *var);
struct VarSymbolLLVM *getVarFromNameLLVM(struct VarTableLLVM *table, const char *name);
