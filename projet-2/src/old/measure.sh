#!/bin/bash
for i in 2 6 10 14 16 18 20 22 24 26 27 28
do
        echo "./upfact ... --maxthreads" $i >> measures.txt
       ./upfact data/gen400-8-0001 data/gen400-8-0002 data/gen400-8-0003 data/gen400-8-0004 data/gen400-8-0005 data/gen400-8-0006 data/gen400-8-0007 data/gen400-8-0000 --maxthreads $i >> measures.txt
done

