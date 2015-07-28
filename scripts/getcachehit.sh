#/bin/bash 


if [ ! -f "../build/src/l1tca" ]; then
    echo "Please copy cache programs to llvm-curvefit/build/src/"
    exit 0
fi

rm cachehitpapi
for ((i=50;i<201;i=i+10)) 
do
    ../build/src/l1tca 100 100 $i 1>/dev/null 2>>../build/src/cachehitpapi
    ../build/src/l1dch_l2tch 100 100 $i 1>/dev/null 2>>../build/src/cachehitpapi
    rm *.yaml
done


