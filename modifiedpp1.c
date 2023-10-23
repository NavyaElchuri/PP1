#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

// Function to generate data for the matrix
#define generate_data(i,j)  (i)+(j)*(j)

int main(int argc, char** argv)
{
    int i, j, pid, np, mtag, count;
    double t0, t1;
    int data[100][100], row_sum[100];
    MPI_Status status, status_r;
    MPI_Request req_s, req_r;

    // Initialize MPI
    MPI_Init(&argc, &argv);

    // Get the process ID (rank) and the total number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    if (pid == 0) { // Process 0

        // Generate data for rows 0 to 49
        for (i = 0; i < 50; i++) {
            for (j = 0; j < 100; j++) {
                data[i][j] = generate_data(i, j);
            }
            
            // When reaching row 24, initiate non-blocking send to process 1
            if (i == 24) {
                mtag = 1;
                MPI_Isend(data, 2500, MPI_INT, 1, mtag, MPI_COMM_WORLD, &req_s);
            }
        }

        // Continue sending data (row 25) to process 1
        mtag = 3;
        MPI_Isend(data[25], 2500, MPI_INT, 1, mtag, MPI_COMM_WORLD, &req_r);

        // Generate data for rows 50 to 99
        for (i = 50; i < 100; i++)
            for (j = 0; j < 100; j++)
                data[i][j] = generate_data(i, j);

        // Compute row sums for rows 50 to 99
        for (i = 50; i < 100; i++) {
            row_sum[i] = 0;
            for (j = 0; j < 100; j++)
                row_sum[i] += data[i][j];
        }

        // Wait for the data to be received from process 1
        MPI_Wait(&req_r, &status);

        // Receive computed row sums from process 1
        mtag = 2;
        MPI_Recv(row_sum, 50, MPI_INT, 1, mtag, MPI_COMM_WORLD, &status);

        // Print the computed row sums
        for (i = 0; i < 100; i++) {
            printf(" %d  ", row_sum[i]);
            if ((i + 1) % 5 == 0) printf("\n");
        }
    }
    else { // Process 1

        // Receive data for rows 0 to 24 from process 0
        mtag = 1;
        MPI_Recv(data, 2500, MPI_INT, 0, mtag, MPI_COMM_WORLD, &status);

        // Continue receiving data for row 25
        mtag = 3;
        MPI_Irecv(data[25], 2500, MPI_INT, 0, mtag, MPI_COMM_WORLD, &req_r);

        // Compute row sums for rows 0 to 49 (including row 25)
        for (i = 0; i < 50; i++) {
            row_sum[i] = 0;
            for (j = 0; j < 100; j++)
                row_sum[i] += data[i][j];
            
            // When reaching row 24, wait for row 25 data
            if (i == 24)
                MPI_Wait(&req_r, &status_r);
        }

        // Send computed row sums for rows 0 to 49 to process 0
        mtag = 2;
        MPI_Send(row_sum, 50, MPI_INT, 0, mtag, MPI_COMM_WORLD);
    }

    // Finalize MPI
    MPI_Finalize();

    return 1;
}