# Simple WebAssembly Benchmarks for WARDuino

This repo contains some simple C programs, to be used
for benchmarking the WARDuino interpreter.


On macOS, to compile simple Wasm benchmarks for execution on WARDuino, you need to have an appropriate
LLVM installation (the default macOS clang does not
have wasm backend support).

```
brew install llvm
brew install lld
```

Then to use this alternative LLVM, you need to set 
some environment variables:

```
export PATH="/opt/homebrew/opt/llvm/bin:$PATH"                          export LDFLAGS="-L/opt/homebrew/opt/llvm/lib"
export CPPFLAGS="-I/opt/homebrew/opt/llvm/include"
```
  
  
Now we can compile wasm benchmarks (which include WARDuino-specific intrinsics like print_int) with
a command like this:

```
clang --target=wasm32 -nostdlib -Wl,--no-entry -Wl,--export-all -Wno-implicit-function-declaration -Wl,--allow-undefined -o test.wasm test.c
```
For -O2 version:
```
clang --target=wasm32 -nostdlib -Wl,--no-entry -Wl,--export-all -Wno-implicit-function-declaration -Wl,--allow-undefined -O2 -o test.wasm test.c
```

This should generate a test.wasm file, which we
can execute on a pre-built WARDuino interpreter
in our PATH with:

```
wdcli ./test.wasm --invoke foo --no-debug
```
where `foo` is the entry function for the benchmark.


This folder contains the following benchmarks, ready
for execution on WARDuino:

* binarytrees
* richards

Each of these has been adapted for WARDuino as follows.

1. strip out `#include` header files
2. replace `printf` calls with WARDuino `print_int` and `print_string` (a bit messy!)
3. use user-defined `malloc` and `free` calls, with a fixed
static memory (which will need to be sized appropriately)


