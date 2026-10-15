# Exercise 0 - MPI in-class activities

Student ID: IT24103447

Starter repository: https://github.com/SLIIT-FacultyOfComputing/SE4060-MPI-Part1

## Completed activities

| Folder | Activity | Verified result |
| --- | --- | --- |
| 01 - Hello MPI | Run HelloMPI.c with four processes | Ranks 0, 1, 2 and 3 print their messages |
| 02 - Single Value Message | Send one integer from rank 1 to rank 3 using MPI_Ssend | Rank 3 receives 30 |
| 03 - Array Message | Send ten integers from rank 1 to rank 3 using MPI_Ssend | Rank 3 receives 0, 10, 20, 30, 40, 50, 60, 70, 80 and 90 |

Each activity contains its source code and actual EC2 execution output in output.txt.
The Hello MPI output also preserves the earlier terminal result supplied by the student.
The starter spelling messaage2.cc is retained.

## Execution environment

Amazon Linux 2023 with Open MPI 4.1.2 on one EC2 t3.small instance.
All activities were executed successfully with four MPI processes on this single host.
Oversubscription allows four processes on the instance's two vCPUs; these are not four separate nodes.
The lecturer's PBS job script targets a cluster scheduler. These runs use mpirun directly on EC2.

## Compile and run

Transfer the source file to EC2, then run the appropriate pair of commands there:

```bash
mpic++ -Wall -Wextra HelloMPI.c -o hello
mpirun --oversubscribe -np 4 ./hello

mpic++ -Wall -Wextra messages1.cc -o messages1
mpirun --oversubscribe -np 4 ./messages1

mpic++ -Wall -Wextra messaage2.cc -o message2
mpirun --oversubscribe -np 4 ./message2
```

The hostname buffers use MPI_MAX_PROCESSOR_NAME to hold the full EC2 hostname safely.
The message-passing logic and data follow the starter programs.
Output line order may vary between runs.
Compilation succeeded; enabling warnings reported function-pointer cast warnings inside the installed Open MPI C++ headers.
