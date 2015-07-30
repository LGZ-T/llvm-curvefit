#!/bin/bash -x

cd ./bin/originbc
dir=$(ls *.bc)
cd ..

clang-3.5 -c ~/Document/projects/llvm-curvefit/BasicBlockTime/getbbtime.c -o getbbtime.o

for i in $dir
do
    opt-3.5 -load ~/Document/projects/llvm-curvefit/build/src/libMemoryModel.so -mfuncTime ./originbc/$i -o ./mainbc/$i".mainbc"
    llc-3.5 -filetype=obj ./mainbc/$i".mainbc" -o $i".main.o"
    mpif90 $i".main.o" getbbtime.o -L/usr/local/lib `pkg-config llvm-prof --variable=profile_rt_lib` -o ./mainexec/$i".main"
    rm *.main.o
done
