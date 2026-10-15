#include <cstdio>
#include <cstdlib>
#include <mpi.h>

// IT24103447 - Rewrite Exercise 6 with MPI_Bsend instead of MPI_Ssend.
// IT24103447 - Retain MPI_ANY_SOURCE as the receive source and keep tag 0.
// IT24103447 - The array values remain 0, 10, ..., 90, sent from rank 1 to rank 3.

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

        // IT24103447 - Provide enough MPI buffer space for one message and overhead.
        int packed_size;
        MPI_Pack_size(10, MPI_INT, MPI_COMM_WORLD, &packed_size);
        const int buffer_size = packed_size + MPI_BSEND_OVERHEAD;
        void *buffer = std::malloc(buffer_size);
        if (buffer == NULL) {
            std::fprintf(stderr, "Unable to allocate the buffered-send storage.\n");
            MPI_Abort(MPI_COMM_WORLD, 2);
        }
        MPI_Buffer_attach(buffer, buffer_size);

        std::printf("Rank 1: sending x[10] to rank 3 using MPI_Bsend.\n");
        // IT24103447 - Keep the original send array x and its data unchanged.
        MPI_Bsend(x, 10, MPI_INT, 3, 0, MPI_COMM_WORLD);

        // IT24103447 - Wait for buffered sends before releasing their storage.
        void *detached_buffer;
        int detached_size;
        MPI_Buffer_detach(&detached_buffer, &detached_size);
        std::free(detached_buffer);
        std::printf("Rank 1: buffered send completed and buffer detached.\n");
    } else if (rank == 3) {
        MPI_Recv(y, 10, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        std::printf("Rank 3: accepted message from rank %d, tag %d.\n",
                    status.MPI_SOURCE, status.MPI_TAG);
        std::printf("Rank 3: received y[10]:\n");
        for (int r = 0; r < 10; ++r) {
            std::printf("%d%s", y[r], r == 9 ? "\n" : " ");
        }
    } else {
        std::printf("Just a normal process From rank %d machine %s\n", rank, name);
    }

    MPI_Finalize();
    return 0;
}
