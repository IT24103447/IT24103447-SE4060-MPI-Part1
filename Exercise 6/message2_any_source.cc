#include <cstdio>
#include <mpi.h>

// IT24103447 - Interpret the sheet's Exercise 3 as the third MPI starter activity,
// IT24103447 - messaage2.cc; the Monte Carlo Pi program has no MPI_Recv to modify.
// IT24103447 - MPI_ANY_SOURCE is a source wildcard, not a message tag.
// IT24103447 - Comparison: both versions receive 0, 10, ..., 90 from rank 1.
// IT24103447 - The original accepts only rank 1; this version accepts any source
// IT24103447 - with tag 0 in MPI_COMM_WORLD. Only rank 1 sends in this example.

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
    int x[10], y[10];

    if (rank == 1) {
        for (int r = 0; r < 10; ++r) {
            x[r] = 10 * r;
        }
        std::printf("Sending message to computer 3 from computer 1\n");
        MPI_Ssend(x, 10, MPI_INT, 3, 0, MPI_COMM_WORLD);
    } else if (rank == 3) {
        // IT24103447 - Change only the source restriction; retain message tag 0.
        MPI_Recv(y, 10, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        std::printf("Rank 3: accepted message from rank %d, tag %d.\n",
                    status.MPI_SOURCE, status.MPI_TAG);
        std::printf("in computer 3 the value of y is printed\n");
        for (int r = 0; r < 10; ++r) {
            std::printf("%d%s", y[r], r == 9 ? "\n" : " ");
        }
    } else {
        std::printf("Just a normal process From rank %d machine %s\n", rank, name);
    }

    MPI_Finalize();
    return 0;
}
