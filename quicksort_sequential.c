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
        if (arr[j] <= pivot)
        {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }

    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}


void quickSortSequential(int arr[], int low, int high)
{
    if (low < high)
    {
        int pi = partition(arr, low, high); // Partition index

        quickSortSequential(arr, low, pi - 1);  // Sort before partition
        quickSortSequential(arr, pi + 1, high); // Sort after partition
    }
}

int main(int argc, char *argv[])
{
    int num_thread;
    sscanf(argv[1], "%d", &num_thread);
    omp_set_num_threads(num_thread);

    // Read file
    FILE *fp;
    fp = fopen("input.txt", "r");
    int count;
    fscanf(fp, "%d ", &count);
    int *data = (int *)malloc(count * sizeof(int));

    for (int i = 0; i < count; i++)
    {
        fscanf(fp, "%d ", &data[i]);
    }

    fclose(fp);

    // set time start
    double start = omp_get_wtime();

    quickSortSequential(data, 0, count - 1);

    // set time end
    double end = omp_get_wtime();

    printf("Time: %f\n", end - start);
    // Write file
    FILE *fo;
    fo = fopen("output.txt", "w");
    fprintf(fo, "%d\n", count);
    for (int i = 0; i < count; i++)
    {
        fprintf(fo, "%d ", data[i]);
    }
    fclose(fo);

    free(data);

    return 0;
}