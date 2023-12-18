#!/bin/bash
#
# Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
# build-gl.sh - Generate small OpenGL 3.3 API haders
set -euo pipefail

# Prepare the output directory
mkdir -p out/gl
cd out/gl

# Download the big header files from khronos
if [ ! -f khrplatform.h ]; then
    wget 'https://www.khronos.org/registry/EGL/api/KHR/khrplatform.h' \
         'https://www.khronos.org/registry/OpenGL/api/GL/glcorearb.h' \
         'https://www.khronos.org/registry/EGL/api/EGL/egl.h' \
         'https://www.khronos.org/registry/EGL/api/EGL/eglext.h'
fi

OUT_FILE=gl_api.h
OUT_TEMP=gl_tmp.h

# Remove everything from version 4 onwards
rg -B10000 -r '' '^#ifndef GL_VERSION_4_1' glcorearb.h \
    | rg -A10000 '^#ifndef GL_VERSION_1_0' \
    > $OUT_TEMP

rg 'typedef .*GLDEBUGPROCARB'  glcorearb.h  >> $OUT_TEMP
rg 'glDebugMessageCallbackARB' glcorearb.h  >> $OUT_TEMP
rg 'GL_DEBUG_SEVERITY_\w*'     glcorearb.h  >> $OUT_TEMP
rg 'GL_DEBUG_OUTPUT_\w*'       glcorearb.h  >> $OUT_TEMP

echo '#pragma once' > $OUT_FILE

echo '#ifndef APIENTRY' >> $OUT_FILE
echo '#define APIENTRY' >> $OUT_FILE
echo '#endif' >> $OUT_FILE

rg -v '^GLAPI' $OUT_TEMP | \
    rg -v 'APIENTRYP' | \
    sed -e 's|<KHR/khrplatform.h>|"khrplatform.h"|g' \
    >> $OUT_FILE

rg -B10000 '^#ifndef GL_VERSION_1_2$' $OUT_TEMP \
    | rg '^.* APIENTRY (\w+) .*$' \
    | sed 's/^GLAPI //g' \
    >> $OUT_FILE

echo 'typedef struct gl_api gl_api;' >> $OUT_FILE
echo 'struct gl_api {' >> $OUT_FILE
rg -r 'APIENTRY (*$1)' 'APIENTRY (\w+)' $OUT_TEMP \
    | sed 's/^GLAPI //g' \
    >> $OUT_FILE
echo '};' >> $OUT_FILE

echo '' >> $OUT_FILE

echo 'static void gl_api_load(gl_api *gl, void *load(const char *)) {' >> $OUT_FILE
echo "#define load_new(k) gl->k = load(#k); assert(gl->k);"  >> $OUT_FILE

# TODO: why do we need this for windows??
echo "#if OS_IS_WINDOWS" >> $OUT_FILE
echo "#define load_old(k) gl->k = k; assert(gl->k);"  >> $OUT_FILE
echo "#else" >> $OUT_FILE
echo "#define load_old(k) load_new(k)"  >> $OUT_FILE
echo "#endif" >> $OUT_FILE

rg -B10000 '^#ifndef GL_VERSION_1_2$' $OUT_TEMP | rg -r '    load_old($1);' '^.* APIENTRY (\w+) .*$' >> $OUT_FILE
rg -A10000 '^#ifndef GL_VERSION_1_2$' $OUT_TEMP | rg -r '    load_new($1);' '^.* APIENTRY (\w+) .*$' >> $OUT_FILE
echo '}' >> $OUT_FILE
