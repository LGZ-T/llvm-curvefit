#!/bin/bash

for i in $(seq 10 30); do
#    echo $i
#    ./l1tch 10 10 $i 1>/dev/null 2>>info
#    ./l2tch 10 10 $i 1>/dev/null 2>>info
    ./a.out 10 10 $i >> info
done

rm *.yaml
