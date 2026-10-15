#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    const long long n = 10000000LL;
    int rank, size;
    char processor[MPI_MAX_PROCESSOR_NAME];
    int name_length;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(processor, &name_length);

    // IT24103447 - Give every rank a non-overlapping range, including any remainder.
    const long long first = (n * rank) / size + 1;
    const long long last = (n * (rank + 1LL)) / size;
    long long local_sum = 0;
    long long total_sum = 0;

    // IT24103447 - Synchronize before measuring the parallel calculation.
    MPI_Barrier(MPI_COMM_WORLD);
    const double start = MPI_Wtime();

    for (long long value = first; value <= last; ++value) {
        local_sum += value;
    }

    // IT24103447 - Combine all partial sums at rank 0 using a 64-bit integer type.
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_LONG_LONG_INT,
               MPI_SUM, 0, MPI_COMM_WORLD);

    const double elapsed = MPI_Wtime() - start;
    double parallel_time = 0.0;
    MPI_Reduce(&elapsed, &parallel_time, 1, MPI_DOUBLE,
               MPI_MAX, 0, MPI_COMM_WORLD);

    // IT24103447 - Print the host and range to show where each rank performed work.
    printf("Rank %d on %s: range %lld to %lld, partial sum = %lld\n",
           rank, processor, first, last, local_sum);

    int correct = 1;
    if (rank == 0) {
        const long long expected = n * (n + 1) / 2;
        correct = (total_sum == expected);
        printf("Processes: %d\n", size);
        printf("Total sum: %lld\n", total_sum);
        printf("Expected sum: %lld\n", expected);
        printf("Verification: %s\n", correct ? "PASS" : "FAIL");
        printf("Parallel execution time: %.9f seconds\n", parallel_time);
    }

    MPI_Bcast(&correct, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Finalize();
    return correct ? 0 : 1;
}
