#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    const long long total_iterations = 10000000LL;
    const double reference_pi = 3.14159265358979323846;
    int rank, size;
    char processor[MPI_MAX_PROCESSOR_NAME];
    int name_length;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(processor, &name_length);

    // IT24103447 - Share exactly 10,000,000 trials, including any remainder.
    const long long local_iterations = total_iterations / size
                                     + (rank < total_iterations % size);
    long long local_hits = 0;

    // IT24103447 - Each process uses a distinct, reproducible random seed.
    const unsigned int seed = 12345u + 104729u * (unsigned int)rank;
    srand(seed);

    MPI_Barrier(MPI_COMM_WORLD);
    const double start = MPI_Wtime();

    // IT24103447 - Sample the unit square and count points in the quarter circle.
    for (long long trial = 0; trial < local_iterations; ++trial) {
        const double x = rand() / ((double)RAND_MAX + 1.0);
        const double y = rand() / ((double)RAND_MAX + 1.0);
        if (x * x + y * y <= 1.0) {
            ++local_hits;
        }
    }

    // IT24103447 - Sum both trial counts and circle hits across every process.
    long long local_counts[2] = {local_iterations, local_hits};
    long long global_counts[2] = {0, 0};
    MPI_Reduce(local_counts, global_counts, 2, MPI_LONG_LONG_INT,
               MPI_SUM, 0, MPI_COMM_WORLD);

    const double elapsed = MPI_Wtime() - start;
    double parallel_time = 0.0;
    MPI_Reduce(&elapsed, &parallel_time, 1, MPI_DOUBLE,
               MPI_MAX, 0, MPI_COMM_WORLD);

    printf("Rank %d on %s: iterations = %lld, circle hits = %lld, seed = %u\n",
           rank, processor, local_iterations, local_hits, seed);

    int valid = 1;
    if (rank == 0) {
        valid = (global_counts[0] == total_iterations
                 && global_counts[1] >= 0
                 && global_counts[1] <= global_counts[0]);
        const double estimated_pi = 4.0 * (double)global_counts[1]
                                 / (double)global_counts[0];
        printf("Processes: %d\n", size);
        printf("Total iterations: %lld\n", global_counts[0]);
        printf("Total circle hits: %lld\n", global_counts[1]);
        printf("Estimated Pi: %.10f\n", estimated_pi);
        printf("Reference Pi: %.10f\n", reference_pi);
        printf("Absolute error: %.10f\n", fabs(estimated_pi - reference_pi));
        printf("Iteration/count check: %s\n", valid ? "PASS" : "FAIL");
        printf("Parallel execution time: %.9f seconds\n", parallel_time);
    }

    MPI_Bcast(&valid, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Finalize();
    return valid ? 0 : 1;
}
