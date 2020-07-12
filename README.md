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
### Compiling

```sh
grimec helloworld.gm | clang -x ir
./a.out
```

## Usage

This will output LLVM-IR for the grime file passed to the compiler. To turn this into an executable you can do the following
```grimec <file> | clang -x ir -``` This will output an executable named `a.out`
