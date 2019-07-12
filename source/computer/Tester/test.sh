#!/bin/sh
echo "seq"
./seqComp in.txt >> results/seqCompResults.txt
echo "omp2"
./ompComp2 in.txt >> results/ompCompResults2.txt
echo "omp4"
./ompComp4 in.txt >> results/ompCompResults4.txt
echo "omp8"
./ompComp8 in.txt >> results/ompCompResults8.txt

echo "seqRKComp"
./seqRKComp in.txt >> results/seqRKCompResults.txt

echo "ompRKComp2"
./ompRKComp2 in.txt >> results/ompRKCompResults2.txt
echo "ompRKComp4"
./ompRKComp4 in.txt >> results/ompRKCompResults4.txt
echo "ompRKComp8"
./ompRKComp8 in.txt >> results/ompRKCompResults8.txt

echo "seqBHComp"
./seqBHComp in.txt >> results/seqBHCompResults.txt

echo "ompBHComp2"
./ompBHComp2 in.txt >> results/ompBHCompResults2.txt
echo "ompBHComp4"
./ompBHComp4 in.txt >> results/ompBHCompResults4.txt
echo "ompBHComp8"
./ompBHComp8 in.txt >> results/ompBHCompResult8.txt

echo "MPI2"
mpirun -np 2 ./mpiSeqComp >> results/mpiSeqResults2.txt
echo "MPI4"
mpirun -np 4 ./mpiSeqComp >> results/mpiSeqResults4.txt
