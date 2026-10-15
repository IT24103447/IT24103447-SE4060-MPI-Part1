#include <cstdio>
#include <mpi.h>

// IT24103447 - Answer: rank 1 sends to rank 3, but rank 3 expects rank 0.
// IT24103447 - MPI_Ssend waits for a matching receive; MPI_Recv waits for rank 0.
// IT24103447 - Neither operation can complete, so this program hangs (deadlock).
// IT24103447 - The test was stopped after 10 seconds; timeout exit 124 is expected.

int main(int argc, char *argv[])
{
    int rank, size;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 4) {
        if (rank == 0) {
            std::fprintf(stderr, "Run this program with at least four MPI processes.\n");
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    char name[MPI_MAX_PROCESSOR_NAME];
    int len;
    MPI_Get_processor_name(name, &len);
    int x = 30;
    int y;

    if (rank == 1) {
        std::printf("Rank 1: synchronously sending x = %d to rank 3, tag 0.\n", x);
        std::fflush(stdout);
        MPI_Ssend(&x, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
        std::printf("Rank 1: send completed.\n");
    } else if (rank == 3) {
        // IT24103447 - Deliberate mismatch: the sender is rank 1, not rank 0.
        std::printf("Rank 3: waiting for rank 0, tag 0, although the sender is rank 1.\n");
        std::fflush(stdout);
        MPI_Recv(&y, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        std::printf("Rank 3: received y = %d.\n", y);
    } else {
        std::printf("Just a normal process From rank %d machine %s\n", rank, name);
        std::fflush(stdout);
    }

    // IT24103447 - The unmatched send and receive prevent normal completion.
    MPI_Finalize();
    return 0;
}
