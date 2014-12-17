#!/bin/sh
#build the pyhidlib.so

if ! [ -d "build" ]; then
mkdir build
fi

gcc  -c -fpic  src/pyhidlib.c -o build/pyhidlib.o

gcc -shared -lc  -o build/pyhidlib.so  build/pyhidlib.o

echo if everything went good, find the result in: build/pyhidlib.so
echo execute:
echo sudo cp build/pyhidlib.so /opt/lib
echo to use it comfortable
