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
    make sp NPROCS=$[$i*$i] CLASS=D llvm=1
    make bt NPROCS=$[$i*$i] CLASS=D llvm=1
done
mv sp.D.*.bc bt.D.*.bc ./bin/originbc
mv sp.D.* bt.D.* ./bin/originexec

for(( i=2; i<=256; i=i*2 ))
do
    make cg NPROCS=$i CLASS=D
    make ft NPROCS=$i CLASS=D
    make lu NPROCS=$i CLASS=D
    make is NPROCS=$i CLASS=D
    make cg NPROCS=$i CLASS=D llvm=1
    make ft NPROCS=$i CLASS=D llvm=1
    make lu NPROCS=$i CLASS=D llvm=1
    make is NPROCS=$i CLASS=D llvm=1
done
mv cg.D.*.bc ft.D.*.bc lu.D.*.bc is.D.*.bc ./bin/originbc
mv sp.D.* bt.D.* lu.D.* is.D.* ./bin/originexec

for(( i=2; i<=250; i++ ))
do
    make ep NPROCS=$i CLASS=D
    make ep NPROCS=$i CLASS=D llvm=1
done
mv ep.D.*.bc ./bin/originbc
mv ep.D.* ./bin/originexec

dir=$(ls ./bin/originexec/)

for i in $dir
do
    var=${i//./ }
    arr=($var)
    input=""
    output=""
    if [ -z "${arr[2]}" ]; then
        make ${arr[0]} NPROCS=1 CLASS=${arr[1]} llvm=1
        input=${arr[0]}"."${arr[1]}
        output=${arr[0]}"."${arr[1]}
    else
        make ${arr[0]} NPROCS=${arr[2]} CLASS=${arr[1]} llvm=1
        input=${arr[0]}"."${arr[1]}"."${arr[2]}
        output=${arr[0]}"."${arr[1]}"."${arr[2]}
    fi
    cd ./bin
    opt-3.5 -load ~/Document/projects/llvm-pred/build/src/libLLVMPred.so -insert-edge-profiling -insert-mpi-profiling ./originbc/${input}".bc" -o ./edgempibc/${output}".edgempi.bc"
    llc-3.5 -filetype=obj ./edgempibc/${output}".edgempi.bc" -o ${output}".edgempi.o"
    mpif90 ${output}".edgempi.o" -L/usr/local/lib `pkg-config llvm-prof --variable=profile_rt_lib` -o ./edgempiexec/${output}".edgempi"
    rm *.o
    cd ..
done
