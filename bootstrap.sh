#!/usr/bin/env bash

set -e

#fetch local .gitmodule submodule deps
git submodule update --init

#fetch boost (regex) submodule deps
pushd lib/boost > /dev/null
    git submodule update --init tools/build
    git submodule update --init libs/config
    git submodule update --init libs/core
    git submodule update --init libs/detail
    git submodule update --init libs/smart_ptr
    git submodule update --init libs/predef
    git submodule update --init libs/assert
    git submodule update --init libs/throw_exception
    git submodule update --init libs/mpl
    git submodule update --init libs/type_traits
    git submodule update --init libs/static_assert
    git submodule update --init libs/integer
    git submodule update --init libs/preprocessor
    git submodule update --init libs/functional
    git submodule update --init libs/program_options
    git submodule update --init libs/chrono
    git submodule update --init libs/system
    git submodule update --init libs/thread
    git submodule update --init libs/winapi
    git submodule update --init libs/move
    git submodule update --init libs/date_time
    git submodule update --init libs/ratio
    git submodule update --init libs/iterator
    git submodule update --init libs/range
    git submodule update --init libs/any
    git submodule update --init libs/concept_check
    git submodule update --init libs/array
    git submodule update --init libs/timer
    git submodule update --init libs/bind
    git submodule update --init libs/utility
    git submodule update --init libs/io
    git submodule update --init libs/intrusive
    git submodule update --init libs/container
    git submodule update --init libs/tuple
    git submodule update --init libs/exception
    git submodule update --init libs/function
    git submodule update --init libs/type_index
    git submodule update --init libs/lexical_cast
    git submodule update --init libs/numeric
    git submodule update --init libs/math
    git submodule update --init libs/tokenizer
    git submodule update --init libs/optional
    git submodule update --init libs/atomic
    git submodule update --init libs/rational
    git submodule update --init libs/algorithm

    #fetch boost (regex) submodule itself
    git submodule update --init libs/regex
    
    #prepare boost library
    ./bootstrap.sh --with-icu

    #build headers only
    ./b2 headers

    #ensure on develop branch because fuzzing fixes sometimes
    #don't get merged into master for many months, change as needed.
    pushd libs/regex > /dev/null
        git checkout develop > /dev/null
    popd > /dev/null
popd > /dev/null
