#!/bin/bash
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
RET="$?"
if [ "$RET" -eq 0 ]
then
    make
    RET="$?"
else
    echo "cmake failed"
fi
cd ..
#return "$RET"
