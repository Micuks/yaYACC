 # yaYACC: 可根据任意给定文法对任意输入进行语法分析的语法分析程序

yaYACC(yet another YACC)

## Introduction

可以根据任意给定文法对任意输入进行语法分析, 包括LL(1)和LR(1)两种分析方式.

## 运行环境

在macOS上使用clang进行过测试, 其他环境兼容性未知, 可能需要自己对CMakeLists做修改.

## Using

运行如下指令可以获得详细说明.
```bash
cmake -S . -B build
./build/main --help
```

## Testing

使用Google test进行单元测试.

```bash
cmake -S . -B build
./build/test_all
```
