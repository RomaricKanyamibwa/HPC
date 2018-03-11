gcc -fopenmp -o exo $1 -g #-Wall -Werror 
if [ "$#" -eq 2 ]; then
    export OMP_NUM_THREADS=$2
    echo "OMP_NUM_THREADS=$2"
fi
export OMP_NUM_THREADS=