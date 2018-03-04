echo "Compile"
sh build.sh convol_paral.c
echo "End Compile"

echo "filtre:0 nbiter:1,10,100"
mpirun -n 2 ./convol Images/Sukhothai_4080x6132.ras 0 1
mpirun -n 2 ./convol Images/Sukhothai_4080x6132.ras 0 10
mpirun -n 2 ./convol Images/Sukhothai_4080x6132.ras 0 100

echo "filtre:0 nbiter:1,10,100"
mpirun -n 4 ./convol Images/Sukhothai_4080x6132.ras 0 1
mpirun -n 4 ./convol Images/Sukhothai_4080x6132.ras 0 10
mpirun -n 4 ./convol Images/Sukhothai_4080x6132.ras 0 100

echo "filtre:0 nbiter:1,10,100"
mpirun -n 8 ./convol Images/Sukhothai_4080x6132.ras 0 1
mpirun -n 8 ./convol Images/Sukhothai_4080x6132.ras 0 10
mpirun -n 8 ./convol Images/Sukhothai_4080x6132.ras 0 100

echo "filtre:0 nbiter:1,10,100"
mpirun -n 16 ./convol Images/Sukhothai_4080x6132.ras 0 1
mpirun -n 16 ./convol Images/Sukhothai_4080x6132.ras 0 10
mpirun -n 16 ./convol Images/Sukhothai_4080x6132.ras 0 100