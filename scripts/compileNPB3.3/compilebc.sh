#!/bin/bash -x

mkdir -p ./bin/originexec
mkdir -p ./bin/originbc
mkdir -p ./bin/edgebc
mkdir -p ./bin/edgeexec
mkdir -p ./bin/bbtimebc
mkdir -p ./bin/bbtimeexec

for(( i=2; i<=15; i++ ))
do
    make sp NPROCS=$[$i*$i] CLASS=D
    make bt NPROCS=$[$i*$i] CLASS=D
    rm ./SP/*.o ./BT/*.o ./common/*.o
    make sp NPROCS=$[$i*$i] CLASS=D llvm=1
    make bt NPROCS=$[$i*$i] CLASS=D llvm=1
    rm ./SP/*.o ./BT/*.o ./common/*.o
done
mv sp.D.*.bc bt.D.*.bc ./bin/originbc
mv sp.D.* bt.D.* ./bin/originexec

for(( i=2; i<=256; i=i*2 ))
do
    make cg NPROCS=$i CLASS=D
    make ft NPROCS=$i CLASS=D
    make lu NPROCS=$i CLASS=D
    make is NPROCS=$i CLASS=D
    rm ./CG/*.o ./FT/*.o ./LU/*.o ./IS/*.o ./common/*.o
    make cg NPROCS=$i CLASS=D llvm=1
    make ft NPROCS=$i CLASS=D llvm=1
    make lu NPROCS=$i CLASS=D llvm=1
    make is NPROCS=$i CLASS=D llvm=1
    rm ./CG/*.o ./FT/*.o ./LU/*.o ./IS/*.o ./common/*.o
done
mv cg.D.*.bc ft.D.*.bc lu.D.*.bc is.D.*.bc ./bin/originbc
mv sp.D.* bt.D.* lu.D.* is.D.* ./bin/originexec

for(( i=2; i<=250; i++ ))
do
    make ep NPROCS=$i CLASS=D
    make ep NPROCS=$i CLASS=D llvm=1
    rm ./ep/*.o ./common/*.o
done
mv ep.D.*.bc ./bin/originbc
mv ep.D.* ./bin/originexec

dir=$(ls ./bin/originbc/)

for i in $dir
do
    var=${i//./ }
    arr=($var)
    output=${arr[0]}"."${arr[1]}"."${arr[2]}
    cd ./bin
    opt-3.5 -load ~/Document/projects/llvm-pred/build/src/libLLVMPred.so -insert-edge-profiling -insert-mpi-profiling ./originbc/$i -o ./edgebc/${output}".edge.bc"
    opt-3.5 -load ~/Documents/projects/llvm-curvefit/build/src/libMemoryModel.so -BBTime ./originbc/$i -o ./bbtimebc/${output}".bbtime.bc"
    llc-3.5 -filetype=obj ./edgebc/${output}".edge.bc" -o ${output}".edge.o"
    mpif90 ${output}".edge.o" -L/usr/local/lib `pkg-config llvm-prof --variable=profile_rt_lib` -o ./edgeexec/${output}".edge"
    clang++-3.5 ~/Documents/projects/llvm-curvefit/BasicBlockTime/getbbtime.cpp ./bbtimebc/${output}".bbtime.bc" -o ./bbtimeexec/${output}".bbtime"
    rm *.o
    cd ..
done
