mkdir build
cmake -B build -S .
make -C build
mv build/nitcbase .
# rm -rf build/
