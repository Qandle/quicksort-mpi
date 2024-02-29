#include <stdio.h>
#include <stdlib.h>
// #include <mpi.h>
#include <omp.h>
#include <string.h>

void swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

int partition(int *arr, int low, int high)
{
    int pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++)
    {
        if (arr[j] < pivot)
        {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }

    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quickSort(int *arr, int low, int high)
{

    if (low < high)
    {
        int pi = partition(arr, low, high);

#pragma omp task
        {
            quickSort(arr, low, pi - 1);
        }
#pragma omp task
        {
            quickSort(arr, pi + 1, high);
        }
    }
}

/*
    Input:
        arr: array of integers
        size: size of array
        rank: rank of thread
        num_thread: number of threads

*/

int partitionJarnGas(int *arr, int high, int low, int pivot)
{
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++)
    {
        if (arr[j] < pivot)
        {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    return (i + 1);
}

void quickSortJarnGas(int *arr, int size)
{
    if (size <= 1)
    {
        return;
    }

    int middle = 0;

    int leftPartPos = 0;
    int rightPartPos;
    int start, end, seperator;
    printf("Size: %d\n", size);
    int *tempArray = (int *)malloc(size * sizeof(int));
#pragma omp parallel shared(middle) private(start, end, seperator)
    {
        int num_thread = omp_get_num_threads();
        printf("Numthread: %d\n", num_thread);
        int portion = (size / num_thread) + 1;
        int rank = omp_get_thread_num();
        printf("Rank: %d\n", rank);
        start = portion * rank;
        end = portion * (rank + 1) - 1;

        if (rank == num_thread - 1)
            end = size - 1;

        int pivot = 0;

        seperator = partitionJarnGas(arr, end, start, arr[pivot]);

        printf("Seperator: %d, start: %d, rank: %d\n", seperator, start, rank);
#pragma omp critical
        {
            middle += (seperator - start);
            rightPartPos = middle;
        }
#pragma omp barrier
        printf("Middle: %d, rightPartPos: %d\n", middle, rightPartPos);

#pragma omp critical
        {
            memcpy(tempArray + leftPartPos, arr + start, (seperator) * sizeof(int));
            leftPartPos += seperator;
            memcpy(tempArray + rightPartPos, arr + start + seperator, (end - start) - seperator * sizeof(int));
            rightPartPos += (end - start) - seperator;
            printf("thread %d finish: leftPartPos: %d, rightPartPos: %d\n", rank, leftPartPos, rightPartPos);
        }
        printf("not explosion\n");
    }
    for (int i = 0; i < size; i++)
    {
        printf("%d ", tempArray[i]);
    }

    quickSortJarnGas(tempArray, middle);
    quickSortJarnGas(tempArray + middle, size - middle);

    // merge
    memcpy(arr, tempArray, middle * sizeof(int));
    memcpy(arr + middle, tempArray + middle, (size - middle) * sizeof(int));

    free(tempArray);
}

void quickSortSequential(int arr[], int low, int high)
{
    if (low < high)
    {
        int pi = partition(arr, low, high); // Partition index

        quickSort(arr, low, pi - 1);  // Sort before partition
        quickSort(arr, pi + 1, high); // Sort after partition
    }
}

int main(int argc, char *argv[])
{
    int num_thread;
    sscanf(argv[1], "%d", &num_thread);
    omp_set_num_threads(num_thread);

    // Read file
    FILE *fp;
    fp = fopen("input100.txt", "r");
    int count;
    fscanf(fp, "%d ", &count);
    int *data = (int *)malloc(count * sizeof(int));

    for (int i = 0; i < count; i++)
    {
        fscanf(fp, "%d ", &data[i]);
    }

    fclose(fp);

    printf("Data: ", data[0]);
    // set time start
    double start = omp_get_wtime();

    // #pragma omp parallel
    //     {
    // #pragma omp single
    //         {
    //             quickSort(data, 0, count - 1);
    //         }
    //     }

    quickSortJarnGas(data, count);

    // set time end
    double end = omp_get_wtime();

    printf("Time: %f\n", end - start);
    // Write file
    FILE *fo;
    fo = fopen("output100.txt", "w");
    fprintf(fo, "%d\n", count);
    for (int i = 0; i < count; i++)
    {
        fprintf(fo, "%d ", data[i]);
    }
    fclose(fo);

    free(data);

    return 0;
}