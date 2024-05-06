#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>

int *primeFind(int iteration, int num_numbers, int NUM_THREADS, int CHUNKSIZE)
{
    int search_table_size = num_numbers * 20 * iteration;
    int primes_found = 0;
    int* numbers = malloc(sizeof(int[search_table_size]));
    int *table_of_prime_numbers;
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        #pragma omp single
        {
            for (int i = 0; i < search_table_size; i++)
            {
                numbers[i] = i + 2;
            }
            int max_value = numbers[search_table_size - 1];
            for (int i = 0; i < search_table_size; i++)
            {
                if(primes_found == num_numbers)
                {
                    break;
                }
                if (numbers[i] == -1)
                {
                    continue;
                }
                int number = numbers[i];
                primes_found++;
                int default_number_of_iterations = ((max_value - number + number) + (number - 1)) / number;
                int offset = (default_number_of_iterations + (NUM_THREADS - 1)) / NUM_THREADS;
                for(int k = 0; k < NUM_THREADS; k++)
                {
                    #pragma omp task
                    for (int j = number + number + offset * k; j < number + number + offset * (k + 1) && j < max_value; j += number)
                    {
                        numbers[j - 2] = -1;
                    }
                }
                #pragma omp taskwait
            }

            table_of_prime_numbers = malloc(primes_found * sizeof(int));
            int count = 0;
            for (int i = 0; i < search_table_size; i++)
            {
                if(count == primes_found)
                {
                    break;
                }
                if (numbers[i] != -1)
                {
                    table_of_prime_numbers[count] = numbers[i];
                    count++;
                }
            }
            free(numbers);
        }
    }
    return table_of_prime_numbers;
}

int main()
{
    int best_num_threads = 0, best_numt_best_chunksize = 0;
    double best_num_threads_time = 100.0, best_numt_best_chunksize_time = 100.0;
    for(int num_threads = 1; num_threads < 16; ++num_threads) {
        printf("NUM_THREADS: %d\n", num_threads);
        int best_chunksize = 0;
        double best_chunksize_time = 100.0;
        for(int chunksize = 1; chunksize < 100; ++chunksize) {
            clock_t start = clock();
            int *primes = primeFind(1, 50000, num_threads, chunksize);
            clock_t end = clock();
            double dt = (double)(end - start) / CLOCKS_PER_SEC;   
            //printf("\t CHUNKSIZE: %d took %.5f[s]\n", chunksize, dt);

            if(dt < best_num_threads_time) {
                best_num_threads = num_threads;
                best_num_threads_time = dt;
            }
            if(dt < best_chunksize_time) {
                best_chunksize = chunksize;
                best_chunksize_time = dt;
            }
        }

        if(best_num_threads == num_threads) {
            best_numt_best_chunksize = best_chunksize;
            best_numt_best_chunksize_time = best_chunksize_time;
        }

        printf("BEST CHUNKSIZE FOR NUM THREADS(%d) is %d with time %f[s]\n", num_threads, best_chunksize, best_chunksize_time);
    }
    printf("BEST NUMTHREADS IS %d WITH %d CHUNKSIZE AND TIME: %f[s]", best_num_threads, best_numt_best_chunksize, best_num_threads_time);
    return 0;
}
