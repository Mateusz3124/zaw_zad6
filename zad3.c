#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mpi.h"

#define SIZE 4
#define LEFT 0
#define RIGHT 1

int main(int argc, char *argv[]) {
    int numtasks, rank, source, dest, i, tag = 1;
    int nbrs[2], dims[1] = {4}, periods[1] = {0}, reorder = 0, coords[1];
    int inbuf[4] = {
        MPI_PROC_NULL,
        MPI_PROC_NULL,
        MPI_PROC_NULL,
        MPI_PROC_NULL,
    };

    MPI_Request reqs[8];
    MPI_Status stats[8];
    MPI_Comm cartcomm;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    if (numtasks != SIZE) {
        printf("Must specify %d processors. Terminating.\n", SIZE);
        MPI_Finalize();
        return 0;
    }

    MPI_Cart_create(MPI_COMM_WORLD, 1, dims, periods, reorder, &cartcomm);
    MPI_Comm_rank(cartcomm, &rank);
    MPI_Cart_coords(cartcomm, rank, 1, coords);
    MPI_Cart_shift(cartcomm, 0, 1, &nbrs[LEFT], &nbrs[RIGHT]);

    int NUM_NEIGHBORS = 1;
    int NUM_MAX_RECEIVED = 5;
    int received = 0;
    int outbuf[] = {1, 2, 3, 4, 5};
    MPI_Status wait_status;
    int last_sum = 0;
    while (received++ < NUM_MAX_RECEIVED) {
        dest = nbrs[RIGHT];
        source = nbrs[LEFT];
        if (nbrs[LEFT] != MPI_PROC_NULL) {
            MPI_Irecv(&inbuf[0], 1, MPI_INT, source, tag, MPI_COMM_WORLD, &reqs[0]);
            MPI_Wait(&reqs[0], &wait_status);
        } else {
            inbuf[0] = outbuf[received - 1];
        }
        if (nbrs[RIGHT] != MPI_PROC_NULL) {
            MPI_Isend(&inbuf[0], 1, MPI_INT, dest, tag, MPI_COMM_WORLD, &reqs[1]);
            MPI_Wait(&reqs[1], &wait_status);
            printf("[%d<-->%d]: [%d]\n", rank, dest, inbuf[0]);
        } else {
            last_sum += inbuf[0];
            printf("[LAST] [RANK:%d]: [%d]\n", rank, inbuf[0]);
        }
    }

    if (nbrs[RIGHT] == MPI_PROC_NULL) {
        printf("TOTAL SUM: %d\n", last_sum);
    }

    MPI_Finalize();
}