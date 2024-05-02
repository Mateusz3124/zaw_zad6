#!/bin/bash
condition=$1

if [ "$condition" -eq 1 ]; then
  mpicc -o file zad1.c -lm
  mpirun -n 8 ./file
fi
if [ "$condition" -eq 2 ]; then
  mpicc -o file zad2.c -lm
  mpirun -n 4 ./file
fi
if [ "$condition" -eq 3 ]; then
  mpicc -o file zad3.c -lm
  mpirun -n 4 ./file
fi