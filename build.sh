if [ "$1" == "debug" ]; then
    cmake -S . -B debug -DCMAKE_BUILD_TYPE=Debug
    cmake --build debug
else
    cmake -S . -B release -DCMAKE_BUILD_TYPE=Release
    cmake --build release
fi