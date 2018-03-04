echo "Compile"
sh build.sh convol_paral.c
#gcc -Wall -Werror convol.c -o convol_seq -lm
echo "End Compile"

echo "filtre:0 nbiter:1,10,100"
./convol_seq Images/Sukhothai_4080x6132.ras 0 1
./convol_seq Images/Sukhothai_4080x6132.ras 0 10
./convol_seq Images/Sukhothai_4080x6132.ras 0 100

echo "filtre:1 nbiter:1,10,100"
./convol_seq Images/Sukhothai_4080x6132.ras 1 1
./convol_seq Images/Sukhothai_4080x6132.ras 1 10
./convol_seq Images/Sukhothai_4080x6132.ras 1 100

echo "filtre:2 nbiter:1,10,100"
./convol_seq Images/Sukhothai_4080x6132.ras 2 1
./convol_seq Images/Sukhothai_4080x6132.ras 2 10
./convol_seq Images/Sukhothai_4080x6132.ras 2 100

echo "filtre:3 nbiter:1,10,100"
./convol_seq Images/Sukhothai_4080x6132.ras 3 1
./convol_seq Images/Sukhothai_4080x6132.ras 3 10
./convol_seq Images/Sukhothai_4080x6132.ras 3 100

echo "filtre:4 nbiter:1,10,100"
./convol_seq Images/Sukhothai_4080x6132.ras 4 1
./convol_seq Images/Sukhothai_4080x6132.ras 4 10
./convol_seq Images/Sukhothai_4080x6132.ras 4 100

# echo "NP:2 filtre:0 nbiter:1,10,100"
# mpirun -n 2 ./convol Images/Sukhothai_4080x6132.ras 0 1
# mpirun -n 2 ./convol Images/Sukhothai_4080x6132.ras 0 10
# mpirun -n 2 ./convol Images/Sukhothai_4080x6132.ras 0 100

# echo "NP:4 filtre:0 nbiter:1,10,100"
# mpirun -n 4 ./convol Images/Sukhothai_4080x6132.ras 0 1
# mpirun -n 4 ./convol Images/Sukhothai_4080x6132.ras 0 10
# mpirun -n 4 ./convol Images/Sukhothai_4080x6132.ras 0 100

# echo "NP:8 filtre:0 nbiter:1,10,100"
# mpirun -n 8 ./convol Images/Sukhothai_4080x6132.ras 0 1
# mpirun -n 8 ./convol Images/Sukhothai_4080x6132.ras 0 10
# mpirun -n 8 ./convol Images/Sukhothai_4080x6132.ras 0 100

# echo "NP:16 filtre:0 nbiter:1,10,100"
# mpirun -n 16 ./convol Images/Sukhothai_4080x6132.ras 0 1
# mpirun -n 16 ./convol Images/Sukhothai_4080x6132.ras 0 10
# mpirun -n 16 ./convol Images/Sukhothai_4080x6132.ras 0 100