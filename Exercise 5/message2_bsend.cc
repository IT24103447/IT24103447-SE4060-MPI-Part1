#include <cstdio>
#include <cstdlib>
#include <mpi.h>

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

    // IT24103447 - Keep the starter program's original send and receive arrays.
    int x[10], y[10];

    if (rank == 1) {
        for (int r = 0; r < 10; ++r) {
            x[r] = 10 * r;
        }

        // IT24103447 - Attach separate MPI buffering storage, including overhead.
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
        // IT24103447 - Send directly from x; do not replace or overwrite its data.
        MPI_Bsend(x, 10, MPI_INT, 3, 0, MPI_COMM_WORLD);

        // IT24103447 - Detach waits for buffered messages before storage is freed.
        void *detached_buffer;
        int detached_size;
        MPI_Buffer_detach(&detached_buffer, &detached_size);
        std::free(detached_buffer);
        std::printf("Rank 1: buffered send completed and buffer detached.\n");
    } else if (rank == 3) {
        MPI_Recv(y, 10, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
        std::printf("Rank 3: received y[10] from rank %d:\n", status.MPI_SOURCE);
        for (int r = 0; r < 10; ++r) {
            std::printf("%d%s", y[r], r == 9 ? "\n" : " ");
        }
    } else {
        std::printf("Just a normal process From rank %d machine %s\n", rank, name);
    }

    MPI_Finalize();
    return 0;
}
