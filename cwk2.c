//
// Starting code for the MPI coursework.
//
// See lectures and/or the worksheet corresponding to this part of the module for instructions
// on how to build and launch MPI programs. A simple makefile has also been included (usage optional).
//


//
// Includes.
//

// Standard includes.
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// The MPI library.
#include <mpi.h>

// Some extra routines for this coursework. DO NOT MODIFY OR REPLACE THESE ROUTINES,
// as this file will be replaced with a different version for assessment.
#include "cwk2_extra.h"


//
// Main.
//
int main( int argc, char **argv )
{
    int i;

    //
    // Initialisation.
    //

    // Initialise MPI and get the rank of this process, and the total number of processes.
    int rank, numProcs;
    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &numProcs );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank     );

    // Check that the number of processes is a power of 2, but <=256, so the data set, which is a multiple of 256 in length,
    // is also a multiple of the number of processes. If using OpenMPI, you may need to add the argument '--oversubscribe'
    // when launching the executable, to allow more processes than you have cores.
    if( (numProcs&(numProcs-1))!=0 || numProcs>256 )
    {
        // Only display the error message from one processes, but finalise and quit all of them.
        if( rank==0 ) printf( "ERROR: Launch with a number of processes that is a power of 2 (i.e. 2, 4, 8, ...) and <=256.\n" );

        MPI_Finalize();
        return EXIT_FAILURE;
    }

    // Load the full data set onto rank 0.
    float *globalData = NULL;
    int globalSize = 0;
    if( rank==0 )
    {
        globalData = readDataFromFile( &globalSize );           // globalData must be free'd on rank 0 before quitting.
        if( globalData==NULL )
        {
            MPI_Finalize();                                     // Should really communicate to all other processes that they need to quit as well ...
            return EXIT_FAILURE;
        }

        printf( "Rank 0: Read in data set with %d floats.\n", globalSize );
    }

    // Calculate the number of floats per process. Note that only rank 0 has the correct value of localSize
    // at this point in the code. This will somehow need to be communicated to all other processes. Note also
    // that we can assume that globalSize is a multiple of numProcs.
              // = 0 at this point of the code for all processes except rank 0.

    // Broadcast globalsize to all the processes
    MPI_Bcast(&globalSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int localSize = globalSize / numProcs;
    float *localData = (float*) malloc(localSize*sizeof(float));
    
    // Start the timing now, after the data has been loaded (will only output on rank 0)
    double startTime = MPI_Wtime();

    // Task 1: Calculate the mean using all available processes.   
    // Scatter the data among all processes
    MPI_Scatter(globalData, localSize, MPI_FLOAT, localData, localSize, MPI_FLOAT, 0, MPI_COMM_WORLD);


    // Compute local sums and get global mean from it
    float localSum = 0.0f;
    for (int i = 0; i < localSize; i++) localSum += localData[i];
    
    float globalSum;
    MPI_Reduce(&localSum, &globalSum, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    float mean = 0.0f;
    if (rank == 0) {
        mean = globalSum / globalSize;
    }

    // Task 2. Calculate the variance using all processes.
    int step;
    for (step = 1; step < numProcs; step *= 2) {
        if (rank < step) {
            if (rank + step < numProcs) {
                MPI_Send(&mean, 1, MPI_FLOAT, rank + step, 0, MPI_COMM_WORLD);
            }
        } 
        else if (rank >= step && rank < step * 2) {
            MPI_Recv(&mean, 1, MPI_FLOAT, rank - step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    // Calculate local squared differences and then global variance
    float localSquaredSum = 0.0f;
    for (int i = 0; i < localSize; i++) {
        localSquaredSum += (localData[i] - mean) * (localData[i] - mean);
    }

    float globalSuaredSum;
    MPI_Reduce(&localSquaredSum, &globalSuaredSum, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    float variance = 0.0f;
    if (rank == 0) {
        variance = globalSuaredSum / globalSize;
    }

    //
    // Output the results alongside a serial check.
    //
    if( rank==0 )
    {
        // Output the results of the timing now, before moving onto other calculations.
        printf( "Total time taken: %g s\n", MPI_Wtime() - startTime );

        // Your code MUST call this function after the mean and variance have been calculated using your parallel algorithms.
        // Do not modify the function itself (which is defined in 'cwk2_extra.h'), as it will be replaced with a different
        // version for the purpose of assessing. Also, don't just put the values from serial calculations here or you will lose marks.
        finalMeanAndVariance( mean, variance );

        // Check the answers against the serial calculations. This also demonstrates how to perform the calculations
        // in serial, which you may find useful. Note that small differences in floating point calculations between
        // equivalent parallel and serial codes are possible, as explained in Lecture 11.

        // Mean.
        float sum = 0.0;
        for( i=0; i<globalSize; i++ ) sum += globalData[i];
        float mean = sum / globalSize;

        // Variance.
        float sumSqrd = 0.0;
        for( i=0; i<globalSize; i++ ) sumSqrd += ( globalData[i]-mean )*( globalData[i]-mean );
        float variance = sumSqrd / globalSize;

        printf( "SERIAL CHECK: Mean=%g and Variance=%g.\n", mean, variance );

   }

    //
    // Free all resources (including any memory you have dynamically allocated), then quit.
    //
    if( rank==0 ) free( globalData );

    MPI_Finalize();

    return EXIT_SUCCESS;
}
