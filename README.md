![Build](https://github.com/Edward-0/grimec/workflows/Build/badge.svg)
# Grime Compiler
The new static compiler for the grime programming language.

## Building
### Dependencies
 * LLVM
#### Make
 * CMake
```sh
mkdir build
cd build
cmake -G Ninja ..
ninja
```
## Getting Started
### Hello, World
```grime
mod test::helloworld

ext def func puts([UByte]) -> Int

func main(argc: Int, argv: [[UByte]]) -> Int {
	var greeting: [UByte] := "Hello, World!"
	puts(greeting)
	ret := 0
}
```
### Usage

```sh
grimec helloworld.gm | clang -x ir
./a.out
```
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
