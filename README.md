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
This is the old syntax and what the compiler can currently handle
```grime
mod test::helloworld

ext def func puts([UByte]) -> Int

func main(argc: Int, argv: [[UByte]]) -> Int {
	var greeting: [UByte] := "Hello, World!"
	puts(greeting)
	ret := 0
}
```
The new syntax that needs to be implemented
```grime
/*
 * `!` means always run at comptime
 * `mod` is a function that tells the compiler
 * what namespace we're in
 */
mod!(test::helloworld)

/*
 * `main` is the entry point. I'll probably make it possible to have "swissarmy-knife" type binary
 * like busybox and toybox
 *
 * `env` is the environment; one thing I dislike is globals and this allows one to avoid that.
 * it contains everything one needs to know from constant file descriptors such as stdin and stderr
 * to environment variables and cmdline args
 * the `&` prefixing `env` tells us this is a "borrow checked pointer" that we don't own. 
 * `main` will not drop `env` `_start` is responsible for that.
 *
 * `NotZero` returns an integer type that can never ever be 0
 * `()` is basically nothing similar to rust
 * `Result` returns a structure type with a bool and a union of `E` and `T`
 * but in some cases it can be packed together for example
 * if one of the type's is a NotZero and the other is () the representation will always be the width
 * of the original integer so the return type here will always be a u32 and if it's a 0 it means there
 * is no error.
 */
pub fn main(env: &std::Env) -> Result!((),NotZero!(u32)) {
	/*
	 * `stdout` is one of the constant file descriptors
	 * `println` does what it says on the tin and prints with
	 * a new line. should probably change to have a cmptime format
	 */
	env.stdout.println("Hello, World!");
	/*
	 * like rust this is returning without an error
	 */
	Ok(())
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
