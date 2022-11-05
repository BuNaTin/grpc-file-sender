
mkdir -p build &&\
cd build &&\
cmake -DCMAKE_BUILD_TYPE=Release -DTEST=ON .. &&\
cmake --build . --parallel 4 &&\
ctest --test-dir . -VV --output-on-failure
