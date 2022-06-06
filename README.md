# SysYCompiler

A toy compiler of SysY (a subset of C) written in C++17.

## Build
```bash
mkdir build
cd build
cmake ..
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