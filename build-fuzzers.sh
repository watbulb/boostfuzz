#!/usr/bin/env bash
####################################################################
# American Fuzzy Lop (AFL) harness builder for boost (regex) fuzzers
# Dayton Pidhirney <dapids> <deepsixsec@protonmail.com> 2017
####################################################################

set -e

AFL_HOME="${PWD}/fuzzers/afl"
AFL_LLVM_MODE="${AFL_HOME}/llvm_mode/afl-llvm-rt.o.c"
AFL_COMPILER="${AFL_HOME}/afl-clang-fast++"
AFL_GLUE="${PWD}/fuzzers/afl-glue.cpp"

FUZZER_DIR="${PWD}/fuzzers/regex"

BOOST_LIB_DIR="${PWD}/lib/boost"
BOOST_REGEX_SRCDIR="${PWD}/lib/boost/libs/regex/src"
BOOST_REGEX_BUILD_DIR="${PWD}/fuzzers/regex/build"

CC=clang
CXX=clang++

##### AFL #####

if [[ ! -f $AFL_COMPILER ]]; then
    pushd $AFL_HOME
        make
        pushd llvm_mode
            make
        popd
    popd
fi

##### FUZZERS #####

function build_regex_fuzzer() {
    pushd $FUZZER_DIR/$1
        $CC -m32 -O2 -fsanitize-coverage=trace-pc-guard -fsanitize=address -I $BOOST_LIB_DIR \
        $1_boost_regex_fuzzer.cc -c

        $CC -O2 -m32 $AFL_LLVM_MODE -c -w 

        $CXX -m32 -O2 -fsanitize=address \
        -I $BOOST_LIB_DIR $AFL_GLUE *.o \
        $BOOST_REGEX_BUILD_DIR/*.o -o $1_boost_regex_fuzzer

        rm ./*.o
    popd
}

##### BOOST #####

if [[ ! -d $BOOST_REGEX_BUILD_DIR ]]; then
    mkdir "$BOOST_REGEX_BUILD_DIR"
elif [[ -z $(ls -p $BOOST_REGEX_BUILD_DIR) ]]; then
    echo "[boostfuzz] Instrumenting boost (regex) library with llvm pass"

    pushd "$BOOST_REGEX_BUILD_DIR"
        $AFL_COMPILER -m32 -O2 -fsanitize-coverage=trace-pc-guard \
        -fno-optimize-sibling-calls -fno-omit-frame-pointer -fsanitize=address -I $BOOST_LIB_DIR \
        $BOOST_REGEX_SRCDIR/*.cpp -c
    popd
else
    echo "[boostfuzz] boost (regex) library already instrumented, skipping"
fi

echo "[boostfuzz] building boost (regex) fuzzers"

build_regex_fuzzer generic
build_regex_fuzzer wide
build_regex_fuzzer narrow

echo "[boostfuzz] boost (regex) fuzzer building complete!"
echo "[boostfuzz] the fuzzers are located in ${PWD}/fuzzers/regex/{generic,narrow,wide}"
