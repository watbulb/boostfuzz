#!/usr/bin/env bash

AFL_HOME="${PWD}/fuzzers/afl"
AFL_LLVM_MODE="${PWD}/fuzzers/afl/llvm_mode/afl-llvm-rt.o.c"

AFL_UTILS_HOME="${PWD}/fuzzers/afl-utils"

AFL_COMPILER="${AFL_HOME}/afl-clang-fast++"

BOOST_LIB_DIR="${PWD}/lib/boost"
BOOST_REGEX_SRCDIR="${PWD}/lib/regex/src"
BOOST_REGEX_BUILD_DIR="${PWD}/fuzzers/regex-build"

CXXFLAGS="-m32 -O2 -fsanitize=address"

pushd $BOOST_REGEX_BUILD_DIR
    $AFL_COMPILER $CXXFLAGS -I $BOOST_LIB_DIR $BOOST_REGEX_SRCDIR/*.cpp -c
popd

pushd fuzzers/generic
    clang -m32 -O2 -I $BOOST_LIB_DIR generic_boost_regex_fuzzer.cc -c

    clang -O2 -m32 $AFL_LLVM_MODE -c -w 

    clang++ $CXXFLAGS -I $BOOST_LIB_DIR ../afl-glue.cpp afl-llvm-rt.o.o \
        generic_boost_regex_fuzzer.o $BOOST_REGEX_BUILD_DIR/*.o -o generic_boost_regex_fuzzer

    rm *.o
popd

pushd fuzzers/narrow
    clang $CXXFLAGS -I $BOOST_LIB_DIR narrow_boost_regex_fuzzer.cc -c

    clang -O2 -m32 $AFL_LLVM_MODE -c -w 

    clang++ $CXXFLAGS -I $BOOST_LIB_DIR ../afl-glue.cpp afl-llvm-rt.o.o \
        narrow_boost_regex_fuzzer.o $BOOST_REGEX_BUILD_DIR/*.o -o narrow_boost_regex_fuzzer

    rm *.o
popd

pushd fuzzers/wide
    clang $CXXFLAGS -I $BOOST_LIB_DIR wide_boost_regex_fuzzer.cc -c

    clang -O2 -m32 $AFL_LLVM_MODE -c -w 

    clang++ $CXXFLAGS -I $BOOST_LIB_DIR ../afl-glue.cpp afl-llvm-rt.o.o \
        wide_boost_regex_fuzzer.o $BOOST_REGEX_BUILD_DIR/*.o -o wide_boost_regex_fuzzer

    rm *.o
popd



