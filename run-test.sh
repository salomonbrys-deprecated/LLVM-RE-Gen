#!/bin/sh

if [ "$1" == "clean" ]; then
    rm -f re1.* test.llvm.*
else
    clang -x c -O3 -c -emit-llvm test.clang -o test.llvm.bc

    Debug/LLVM-RE-Gen > /dev/null 2>&1
    llvm-dis re1.llvm.bc -o re1.llvm

    opt -O3 -S re1.llvm -o re1.opt.llvm
    llvm-as re1.opt.llvm -o re1.opt.llvm.bc

    llvm-ld -native test.llvm.bc re1.opt.llvm.bc
    chmod -x a.out.bc
    ./a.out
fi
