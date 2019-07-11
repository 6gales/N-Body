#!/bin/sh

mpicxx externalComputer.cpp -o ec -lboost_bind -lboost_asio

mpirun -np $1 ./ec $2 $3
