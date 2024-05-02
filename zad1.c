#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int NWD(int a, int b){
    while(a!=b){
        if(a>b)
            a-=b;
        else
            b-=a;
    }
    return a; 
}

int main(int argc, char** argv) {
    MPI_Init(NULL, NULL);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    if ((world_size & -world_size) != world_size){
        if(world_rank == 0){
            printf("numer procesow musi byc potega 2\n");
        }
        MPI_Finalize();
        return -1;
    } 
    //int a []= {20,20,20,20,40,30,50,40,20,20,20,20,40,30,50,40};
    srand(world_rank);
    int number = rand() % 50 + 1;
    //int number = a[world_rank];
    printf("number : %d\n",number);
    float max_count = log2f(world_size);
    int offset = 1;
    for(int i =0; i < max_count; i++){
        MPI_Send(&number, 1, MPI_INT, (world_rank + offset) % world_size, 0, MPI_COMM_WORLD);
        int receive;
        MPI_Recv(&receive, 1, MPI_INT, (((world_rank - offset) % world_size) + world_size) % world_size, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        number = NWD(number, receive);
        MPI_Barrier(MPI_COMM_WORLD);
        offset *= 2;
    }
    if(world_rank == 0){
        printf("wynik: %d\n", number);
    }
    MPI_Finalize();
    return 0;
}