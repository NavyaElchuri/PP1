# PP1
Parallel processing program1

In the modified code it divides the data more finely. Process 0 transfers the first 25 rows of data to process 1, then the second 25 rows, and finally the remaining 50 rows sums. Process 1 gets data in two phases (for rows 0-24 and rows 25-49, respectively). This communicates with three message tags, 1, 2, and 3. Message tag 3 is used to transfer the second batch of data from process 0 to process 1. Because of the granular division of data, this code requires more complicated data processing. Processes 0 and 1 exchange partial data, and row sums are computed and conveyed in sections. It involves in the most frequent communication.
