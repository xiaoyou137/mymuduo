#!/bin/bash

set -e 

if [ ! -d `pwd`/build ]; then
    mkdir `pwd`/build
fi
rm -rf `pwd`/build/*
cd `pwd`/build
cmake ..
make

cd ..

# 把头文件拷贝到 /usr/include/mymuduo 
if [ ! -d /usr/include/mymuduo ]; then
    mkdir /usr/include/mymuduo
fi
for header in `ls ./include/*.h`
do 
    cp $header /usr/include/mymuduo
done

# so库拷贝到/usr/lib
cp `pwd`/lib/*.so /usr/lib

ldconfig