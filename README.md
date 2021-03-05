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

```
grimec <file> | clang -x ir -
```

## TODO
 - lexer is fundamentally broken and needs a total rewrite (most likely use FSM)
 - restructure semantic stage
 - add custom ir
 - change syntax
 - change build system to a single POSIX Makefile and a Ninja file or write a
   very short configure POSIX shell script to generate one of your choice
   `./configure --ninja` or `./configure --make`
