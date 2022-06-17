# SysYCompiler

A toy compiler of SysY (a subset of C) written in C++17.

For chinese documentation, see: [docs/doc-chinese.pdf](docs/doc-chinese.pdf).

## Dependencies

* [llvm 14.0+](https://github.com/llvm/llvm-project)

## Build
```bash
mkdir build
cd build
cmake .. -DLLVM_DIR=$(llvm-config --cmakedir)
make -j $(nproc)
```

## Usage
```
./compiler <option> <input_file> -o <output_file>

option:
    -l      dump Lexer output (Tokens)
    -p      dump Parser output (AST)
    -i      dump LLVM IR
    -s      write assembly
```