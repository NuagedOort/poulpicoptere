#!/bin/bash

cd sfmlGraphicsPipeline/extlib
make -j6

rm -rf ../build
mkdir ../build
cd ../build
cmake ..
make -j6
cd ../../sampleProject

rm -rf build
mkdir build
cd build
cmake ..
make -j6

./main&
