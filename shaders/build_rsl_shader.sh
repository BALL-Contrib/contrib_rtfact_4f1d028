#!/bin/bash

# This script can be only executed in the DEVENV environment with
# activated rtfact_anysl, llvm, llvm_gcc, and sdr2bc modules.

# $1 - input RenderMan shader file name
# $2 - output BC module file name
# #3 - optional error output file

# Following environment variables need to be set
#
# JITRT_BITCODE_PATH       paths where to load shaders and gadgets
# JITRT_SHADER_BUILD_DIR   path where build new shaders
# ANYSL_RSL_SHADER_INCLUDE paths where includes for RSL shaders are located (optional)

fatal() {
    echo >&2 "build_rsl_shader.sh: Fatal error: $@"
    exit 1
}

msg() {
    echo "$@"
}

[ -n "$1" ] || fatal "RenderMan shader file name missing"
[ -n "$2" ] || fatal "BC module file name missing"

[ -r "$1" ] || fatal "RenderMan shader file '$1' is not readable"

if [ -n "$3" ]; then
    ERROR_OUT_OPTS="2> \"$3\""
    ERROR_APPEND_OUT_OPTS="2>> \"$3\""
else
    ERROR_OUT_OPTS=
    ERROR_APPEND_OUT_OPTS=
fi

[ -z "$(type -t sdrc)" ] && fatal "Could not find sdrc parser"
[ -z "$(type -t sdr2bc)" ] && fatal "Could not find sdr2bc converter"

SDRC=sdrc
SDR2BC=sdr2bc

RSL_FILE=$1
BC_FILE=$2

msg "Building shader $1 -> $2."

OPTS=""

#  -I/home/rubinste/proj_rtsg2/rtfact-anysl/include -Isrc/include

oldIFS=$IFS
IFS=":"
for p in $ANYSL_RSL_SHADER_INCLUDE; do
    OPTS="$OPTS -I\"$p\""
done
IFS=$oldIFS

TMP_SDR_FILE=$(basename "$RSL_FILE" .sl).sdr

msg "$SDRC $RSL_FILE -o $TMP_SDR_FILE"
eval "$SDRC" "$RSL_FILE" -o "$TMP_SDR_FILE" $ERROR_OUT_OPTS

if [ $? -ne 0 ]; then
    fatal "Conversion to SDR format failed"
fi

msg "$SDR2BC -o $BC_FILE $TMP_SDR_FILE"

"$SDR2BC" -o "$BC_FILE" "$TMP_SDR_FILE" $ERROR_APPEND_OUT_OPTS
exit_code=$?

rm "$TMP_SDR_FILE"

exit $exit_code
