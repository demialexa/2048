#!/bin/bash

cd $(dirname $0)
mkdir -p build
cd build
cmake ../src
make -j4
cp ./bin/2048 ../2048
