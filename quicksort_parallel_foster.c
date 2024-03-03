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
int partitionFoster(int *arr, int high, int low, int pivot)
{
    int i = low - 1;
    for (int j = low; j <= high; j++)
    {
        if (arr[j] < pivot)
        {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    return i;
}

void quickSortFoster(int *arr, int size)
{
    if (size <= 20000 && size > 1)
    {
#pragma omp parallel
        {
#pragma omp single
            {
                quickSort(arr, 0, size - 1);
            }
        }

        return;
    }

    if (size <= 1)
    {
        return;
    }

    int middle = 0;

    int leftPartPos = 0;
    int rightPartPos;
    int start, end, seperator;
    int *tempArray = (int *)malloc(size * sizeof(int));
    int pivot = arr[0];
    int currentLeftSize;
    int currentRightSize;
    int portion;
    int rank;
    int shouldSequential = 0;
    int num_thread;

#pragma omp parallel shared(middle, tempArray, pivot, leftPartPos, rightPartPos) private(start, end, seperator, currentLeftSize, currentRightSize, portion, rank)
    {
        num_thread = omp_get_num_threads();
        // printf("Numthread: %d\n", num_thread);
        portion = ((size) / num_thread) + 1;
        rank = omp_get_thread_num();
        start = portion * rank;
        end = (rank != num_thread - 1) ? portion * (rank + 1) - 1 : size - 1;
        if (start < size && end >= size)
        {
            end = size - 1;
        }

        if (start <= end && start < size && end < size)
        {
            seperator = partitionFoster(arr, end, start, pivot);

            if (seperator < 0)
            {
                seperator = 0;
            }
            currentLeftSize = seperator - start + 1;
            currentRightSize = (end)-seperator;
        }
        else
        {
            currentLeftSize = 0;
            currentRightSize = 0;
        }

        if (portion * num_thread >= size && portion >= 2)
        {
#pragma omp critical
            {
                middle += currentLeftSize;
                rightPartPos = middle;
            }
#pragma omp barrier
#pragma omp critical
            {
                if (currentLeftSize > 0)
                    memcpy(tempArray + leftPartPos, arr + start, currentLeftSize * sizeof(int));
                leftPartPos += currentLeftSize;
                if (currentRightSize > 0)
                    memcpy(tempArray + rightPartPos, arr + seperator + 1, currentRightSize * sizeof(int));
                rightPartPos += currentRightSize;
            }
        }
        else
        {
            shouldSequential = 1;
        }
    }
    if (shouldSequential)
    {

#pragma omp parallel
        {
#pragma omp single
            {
                quickSort(arr, 0, size - 1);
            }
        }

        free(tempArray);

        return;
    }

#pragma omp task
    {
        quickSortFoster(tempArray, middle);
    }

#pragma omp task
    {
        quickSortFoster(tempArray + middle, size - middle);
    }

    memcpy(arr, tempArray, middle * sizeof(int));

    memcpy(arr + middle, tempArray + middle, (size - middle) * sizeof(int));

    free(tempArray);
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

    quickSortFoster(data, count);

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