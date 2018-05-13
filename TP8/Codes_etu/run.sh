#!/bin/bash
#

if [ $# -eq 0 ] ; then 
echo "Usage : $0 a.out "; 
exit 1
fi

EXEC=$1
OUTPUT_FILE="data_${EXEC}_5.txt"

echo "matmul with cblas " > $OUTPUT_FILE
echo "#N	GFLOP/S " >> $OUTPUT_FILE

for ((N=16; N <= 1024 ; N = N +16)); do 
    echo -n "For N=$N ... " 
    PERF=$( ./$EXEC $N | grep performance | awk '{ print $(NF-1)}')
    echo "$N	$PERF	$PERF_CALCUL" >> $OUTPUT_FILE
    echo " done!"  
done 


