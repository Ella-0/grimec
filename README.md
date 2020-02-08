![Build](https://github.com/Edward-0/grimec/workflows/Build/badge.svg)
# Grime Compiler
The new static compiler for the grime programming language.

## Building
### Dependencies
 * LLVM
#### Make
 * CMake
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

## Usage

```grimec <file>```

This will output LLVM-IR for the grime file passed to the compiler. To turn this into an executable you can do the following
```grimec <file> | llc | cc -x assembler -``` This will output an executable named `a.out`
