Complete the table below with your results, and then fill in the final two sections at the end.

Please do not edit or remove table or section headings, as the autograder script uses these to
locate the start and end of the table.

Each row of the table will be split into columns using the Python "split()" method. Therefore,
- fill in each column with values;
- only use whitespace between columns;
- do not add any non-numeric characters (which would cause the value to be read as zero);
- do not worry if your columns are not perfectly aligned.

For the parallel speed-up S, please note that:
- the time you should use is already output by the provided code;
- take as the serial execution time the time output by the code when run with a single process.
  Hence, the speed-up for 1 process in the table below must be 1.00.


No. Machines:   Total No. Processes:     Mean time (average of 3 runs) in seconds:        Parallel speed-up, S:
=============   ====================     =========================================        =====================
1                       1                             0.008944546667                                1                        
1                       2                             0.008078103333                            1.107258263                                                                        
1                       4                             0.007126893333                            1.255041467                                                      
1                       8                             0.004142713333                            2.159103454                                     
2                       16                            0.01211116667                             0.7385371625                                                              
2                       32                            0.02279393333                             0.3924090914                                                   

Please state the number of cores per machine (for Bragg 2.05, this is typically 12): 12

A brief interpretation of your results:

Initial Speed-up with 2, 4, and 8 Processes

There is a gradual improvement in execution time when increasing the number of processes from 1 to 8.
The parallel speed-up (S) improves as expected, with 8 processes yielding a significant speed-up of 2.16x.
This aligns with the theoretical expectation that more processes help distribute the workload, reducing overall execution time.
Performance Drop with 16 and 32 Processes

At 16 processes across 2 machines, the execution time increases instead of decreasing.
At 32 processes, performance degrades even further.
This suggests communication overhead starts to outweigh the benefits of parallelism.
Since there are 12 cores per machine, adding more than 12 processes per machine leads to process oversubscription, meaning some processes must wait for CPU time, slowing performance.

Shared Memory Efficiency: For small process counts (≤8), shared memory parallelism is efficient, leading to good speed-up.
Communication Overhead in Distributed Memory: When spreading processes across multiple machines (16+ processes), MPI communication overhead increases, limiting performance.
Process Oversubscription: When using 32 processes across 2 machines (16 per machine), each machine has more processes than physical cores, leading to significant slowdowns.