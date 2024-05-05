#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mpi.h"

#define SIZE 4
#define LEFT 0
#define RIGHT 1
#define MESSAGE_SIZE 1000
#define OUTBUF_SIZE 100000

int main(int argc, char *argv[]) {
  int numtasks, rank, source, dest, i, tag = 1;
  int nbrs[2], dims[1] = {4}, periods[1] = {0}, reorder = 0, coords[1];
  int inbuf[MESSAGE_SIZE];

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
  int NUM_MAX_RECEIVED = OUTBUF_SIZE / MESSAGE_SIZE;
  int received = 0;
  int outbuf[OUTBUF_SIZE];
  if (rank == 0) {
    for (int i = 0; i < OUTBUF_SIZE; i++) {
      outbuf[i] = 1;
    }
  }

  MPI_Status wait_status;
  int last_sum = 0;
  while (received++ < NUM_MAX_RECEIVED) {
    dest = nbrs[RIGHT];
    source = nbrs[LEFT];
    if (nbrs[LEFT] != MPI_PROC_NULL) {
      MPI_Irecv(&inbuf, MESSAGE_SIZE, MPI_INT, source, tag, MPI_COMM_WORLD,
                &reqs[0]);
      MPI_Wait(&reqs[0], &wait_status);
    } else {
      memcpy(inbuf, &outbuf[(received - 1) * MESSAGE_SIZE],
             MESSAGE_SIZE * sizeof(int));
    }
    if (nbrs[RIGHT] != MPI_PROC_NULL) {
      MPI_Isend(&inbuf, MESSAGE_SIZE, MPI_INT, dest, tag, MPI_COMM_WORLD,
                &reqs[1]);
      MPI_Wait(&reqs[1], &wait_status);
    } else {
      for (int i = 0; i < MESSAGE_SIZE; i++) {
        last_sum += inbuf[i];
      }
    }
  }

  if (nbrs[RIGHT] == MPI_PROC_NULL) {
    printf("TOTAL SUM: %d\n", last_sum);
  }

  MPI_Finalize();
}