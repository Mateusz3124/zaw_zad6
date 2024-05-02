#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"

#define SIZE 4
#define LEFT 0
#define RIGHT 1

int main(int argc, char *argv[])
{
    int numtasks, rank, source, dest, outbuf, i, tag = 1;
    int nbrs[2], dims[1] = {4}, periods[1] = {1}, reorder = 0, coords[1];
    int inbuf[4] = {
        MPI_PROC_NULL,
        MPI_PROC_NULL,
        MPI_PROC_NULL,
        MPI_PROC_NULL,
    };

    MPI_Request reqs[8];
    MPI_Status stats[8];
    MPI_Comm cartcomm; // required variable

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    if (numtasks == SIZE)
    {
        MPI_Cart_create(MPI_COMM_WORLD, 1, dims, periods, reorder, &cartcomm);
        MPI_Comm_rank(cartcomm, &rank);
        MPI_Cart_coords(cartcomm, rank, 1, coords);
        MPI_Cart_shift(cartcomm, 0, 1, &nbrs[LEFT], &nbrs[RIGHT]);

        printf("rank= %d coords= %d  neighbors(l,r)= %d %d\n",
               rank, coords[0], nbrs[LEFT], nbrs[RIGHT]);

        outbuf = rank;

        int NUM_NEIGHBORS = 2;
        for (i = 0; i < NUM_NEIGHBORS; i++)
        {
            dest = nbrs[i];
            source = nbrs[i];
            MPI_Isend(&outbuf, 1, MPI_INT, dest, tag,
                      MPI_COMM_WORLD, &reqs[i]);
            MPI_Irecv(&inbuf[i], 1, MPI_INT, source, tag,
                      MPI_COMM_WORLD, &reqs[i + NUM_NEIGHBORS]);
        }

        MPI_Waitall(NUM_NEIGHBORS * 2, reqs, stats);

        printf("rank= %d inbuf(l,r)= %d %d\n",
               rank, inbuf[LEFT], inbuf[RIGHT]);
    }
    else
        printf("Must specify %d processors. Terminating.\n", SIZE);

    MPI_Finalize();
}