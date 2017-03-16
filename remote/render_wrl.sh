#!/bin/bash

# TODO:
# - resolution
# - viewpoints
# - lightsources

set -e

nrReqShaders=1

if [ $# -lt 1 ]; then
    echo >&2 "Usage: $0 fileName [ -s shaderName ] [ -p ] [ -e ] [ -r N ]"
    exit 1
fi

# check if required programs exist
#if [ -x rtsgRTremoteTest ]; then
if which rtsgRTremoteTest >/dev/null; then
	echo ""
else
	echo >&2 "ERROR: rtsgRTremoteTest executable not found in PATH! (forgot to source devenv.sh?)"
	exit 1
fi
if which replace_node >/dev/null; then
	echo ""
else
	echo >&2 "ERROR: replace_node executable not found in PATH! (RTSG compiled without python support?)"
	exit 1
fi


shaderIdx=0
packetize=0
use_editor=0
resolution=512

fileName=$1
shift

# read commandline arguments (s = shader, p = packetize)
# this silently ignores additional shaders if more than 'nrReqShaders' are supplied
while getopts s:perh: arg
do case "$arg" in
    s)		shaderNames[$shaderIdx]="$OPTARG"
            ((++shaderIdx));;
    p)      packetize=1;;
	e)		use_editor=1;;
    r)      resolution="$OPTARG";;
    h)      print >&2 "Usage: $0 fileName -s shaderName [ -p ] [ -r N ]"  # ?) instead of h) ?
            echo "  -p            = enable automatic packetization of shader (disables GUI)"
            echo "  -e            = activate puppetmaster GUI (dynamic changing of shader code)"
            echo "  -r N          = set resolution to N x N pixels"
            echo "  -s shaderName = brick, checker, dented, glass, granite, mandelbrot, parquetplank, phong, screenaa, starball, whitted, wood"
            exit 1;;
    esac
done


# test if enough shaders were supplied
if [ $shaderIdx -lt $nrReqShaders ]; then
    echo "Error: need to supply $nrReqShaders shaders ($shaderIdx supplied)!"
    exit 1
fi


# now create temporary scene file
# loop replaces occurrences of "shaders SHADERi PackagedShader {}" by supplied shader at index i

if [ $packetize -eq 1 ]; then
	use_editor=0 #hack: if packetization is enabled, disable usage of editor (prevent segfaults)
    VARIANT="SHADER_"
else
    VARIANT="SHADER_SCALAR_"
fi
CMD=replace_node
for ((i=0; "$i" != "$shaderIdx"; ++i)); do
    shaderName=${shaderNames[$i]}
    CMD="$CMD -n SHADER$i PackagedShader scenes/shadersWRL/$VARIANT$shaderName.wrl"
done
TMPFILE="__tmp.x3dv"
$CMD "$fileName" > "$TMPFILE"

# run RTfact
if [ $use_editor -eq 1 ]; then
	../../puppetmaster/run_with_editor.sh "$TMPFILE" -r "$resolution"x"$resolution"
else
	rtsgRTremoteTest "$TMPFILE" -r "$resolution"x"$resolution" -t 32 -c "RTfact/AnySL"
fi


# delete temporary scene file
rm "$TMPFILE"
