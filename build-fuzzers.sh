#!/usr/bin/env bash

set -e

AFL_HOME="${PWD}/fuzzers/afl"
AFL_LLVM_MODE="${AFL_HOME}/llvm_mode/afl-llvm-rt.o.c"
AFL_UTILS_HOME="${PWD}/fuzzers/afl-utils"
AFL_COMPILER="${AFL_HOME}/afl-clang-fast++"
AFL_GLUE="${PWD}/fuzzers/afl-glue.cpp"

BOOST_LIB_DIR="${PWD}/lib/boost"
BOOST_REGEX_SRCDIR="${PWD}/lib/regex/src"
BOOST_REGEX_BUILD_DIR="${PWD}/fuzzers/regex/regex-build"

##### REGEX #####

echo "[boostfuzz] Instrumenting boost (regex) library with llvm pass"

pushd $BOOST_REGEX_BUILD_DIR
    $AFL_COMPILER -m32 -O2 -fsanitize=address -I $BOOST_LIB_DIR \
    $BOOST_REGEX_SRCDIR/*.cpp -c
popd

echo "[boostfuzz] Building boost (regex) fuzzers"

pushd fuzzers/regex/generic
    clang -m32 -O2 -I $BOOST_LIB_DIR generic_boost_regex_fuzzer.cc -c

    clang -O2 -m32 $AFL_LLVM_MODE -c -w 

    clang++ -m32 -O2 -fsanitize=address -I $BOOST_LIB_DIR $AFL_GLUE *.o \
    $BOOST_REGEX_BUILD_DIR/*.o -o generic_boost_regex_fuzzer

    rm *.o
popd

pushd fuzzers/regex/narrow
    clang -m32 -O2 -fsanitize=address -I $BOOST_LIB_DIR narrow_boost_regex_fuzzer.cc -c

    clang -O2 -m32 $AFL_LLVM_MODE -c -w 

    clang++ -m32 -O2 -fsanitize=address -I $BOOST_LIB_DIR $AFL_GLUE *.o \
    $BOOST_REGEX_BUILD_DIR/*.o -o narrow_boost_regex_fuzzer

    rm *.o
popd

pushd fuzzers/regex/wide
    clang -m32 -O2 -fsanitize=address -I $BOOST_LIB_DIR wide_boost_regex_fuzzer.cc -c

    clang -O2 -m32 $AFL_LLVM_MODE -c -w 

    clang++ -m32 -O2 -fsanitize=address -I $BOOST_LIB_DIR $AFL_GLUE *.o \
    $BOOST_REGEX_BUILD_DIR/*.o -o wide_boost_regex_fuzzer

    rm *.o
popd

echo "[boostfuzz] boost (regex) fuzzer building complete!"
echo "[boostfuzz] the fuzzers are located in fuzzers/regex/{generic,narrow,wide}"
