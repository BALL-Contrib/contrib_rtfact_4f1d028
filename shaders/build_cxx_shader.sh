#!/bin/bash

# This script can be only executed in the DEVENV environment with
# activated rtfact_anysl, llvm, and llvm_gcc modules.

# $1 - input C++ shader file name
# $2 - output BC module file name
# $3 - optional error output file

# Following environment variables need to be set
#
# JITRT_BITCODE_PATH       paths where to load shaders and gadgets
# JITRT_SHADER_BUILD_DIR   path where build new shaders
# JITRT_CXX_SHADER_INCLUDE paths where includes for C++ shaders are located

fatal() {
    echo >&2 "build_cxx_shader.sh: Fatal error: $@"
    exit 1
}

msg() {
    echo "$@"
}

test -n "$1" || fatal "C++ shader file name missing"
test -n "$2" || fatal "BC module file name missing"

test -r "$1" || fatal "C++ shader file '$1' is not readable"

if [ -n "$3" ]; then
    ERROR_OUT_OPTS="2> \"$3\""
else
    ERROR_OUT_OPTS=
fi

msg "Building shader $1 -> $2."

OPTS="-c -O3 -Wall -emit-llvm -mfpmath=sse -msse -mmmx -msse2 -fpermissive -mpreferred-stack-boundary=4 -m32"

#  -I/home/rubinste/proj_rtsg2/rtfact-anysl/include -Isrc/include

oldIFS=$IFS
IFS=":"
for p in $JITRT_CXX_SHADER_INCLUDE; do
    OPTS="$OPTS -I\"$p\""
done
IFS=$oldIFS

msg "llvm-g++ -x c++ -o \"$2\" $OPTS \"$1\""
eval llvm-g++ -x c++ -o "$2" $OPTS "$1" $ERROR_OUT_OPTS
