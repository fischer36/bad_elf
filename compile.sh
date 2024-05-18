#!/bin/zsh

set -e

export LD_LIBRARY_PATH=/home/thinkerpad/Desktop/hahaXd/build/:$LD_LIBRARY_PATH
# gcc -fPIC -shared src/init.c -o build/libinit.so - Now statically linked in build.rs
cargo build --release
cp -f target/release/librusty.so build/librusty.so
gcc target_dummy/dummy.c -o build/dummy
cd build
LD_PRELOAD=./librusty.so ./dummy 
cd ..

echo "Build completed successfully."
