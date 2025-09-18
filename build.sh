#!/bin/bash

if [ "$1" == "debug" ]; then
    cmake -S . -B debug -DCMAKE_BUILD_TYPE=Debug
    cmake --build debug
    cd debug; cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_STANDARD=17 ..
    mv compile_commands.json ..
else
    cmake -S . -B release -DCMAKE_BUILD_TYPE=Release
    cmake --build release
    cd release; cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_STANDARD=17 ..
    mv compile_commands.json ..
fi