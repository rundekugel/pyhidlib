#!/bin/sh
# Copyright (c) 2014 lifesim.de
#
# URL: https://github.com/rundekugel/pyhidlib
#
# Licensed under the MIT license:
#    http://www.opensource.org/licenses/mit-license.php

echo brought to you by lifesim.de
echo building pyhidlib.so...

if ! [ -d "build" ]; then
mkdir build
fi

gcc  -c -fpic  src/pyhidlib.c -o build/pyhidlib.o
gcc -shared -lc  -o build/pyhidlib.so  build/pyhidlib.o

echo done. if everything went good, find the result in: build/pyhidlib.so
echo execute:
echo sudo cp build/pyhidlib.so /opt/lib
echo to use it comfortable
