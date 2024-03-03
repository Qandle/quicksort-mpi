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

void quickSortJarnGas(int *arr, int size)
{
    // printf("arr\n");

    for (int i = 0; i < size; i++)
    {
        // printf("%d ", arr[i]);
    }
    // printf("\n");

    if (size <= 5000 && size > 1) {
        
          
          quickSort(arr, 0, size - 1);
            
        
        // quickSortSequential(arr, 0, size - 1);
        return;
    }  


    // if (size == 2 ) {
    //     if (arr[0] > arr[1]) {
    //         swap(&arr[0], &arr[1]);
    //     }
    //     return;
    // }



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
    // printf("Size: %d, pivot: %d\n", size, pivot);

#pragma omp parallel shared(middle,tempArray,pivot,leftPartPos,rightPartPos) private(start, end, seperator, currentLeftSize, currentRightSize, portion, rank)
    {
        num_thread = omp_get_num_threads();
        // printf("Numthread: %d\n", num_thread);
        portion = ((size) / num_thread) +1;
        rank = omp_get_thread_num();
        start = portion * rank;
        end = (rank != num_thread - 1) ? portion * (rank + 1) - 1 : size - 1;
        // printf("Rank: %d, portion:%d,start: %d, end: %d\n", rank, portion, start, end);
        if (start < size && end >= size) {
          end = size - 1;
        }

        if (start <= end && start <size && end < size) {
          seperator = partitionJarnGas(arr, end, start, pivot);
          // if (start == seperator) {
          //   seperator++;
          // }
          if (seperator < 0) {
            seperator = 0;
          }
          currentLeftSize = seperator - start + 1;
          currentRightSize =(end) - seperator ;

        }else {
          currentLeftSize = 0;
          currentRightSize = 0;
        }
          
          if (portion * num_thread >= size && portion  >= 2) {
            
          // // printf("Seperator: %d, start: %d, end: %d, rank: %d\n", seperator, start, end, rank);
          #pragma omp critical
          {
              middle += currentLeftSize;
              rightPartPos = middle;
          }
        #pragma omp barrier
        // printf("Middle: %d\n", middle);
        #pragma omp critical
        {
            // printf("---------------------------------------\n");

            // for (int i = 0; i < size; i++)
            // {
                // printf("%d ", arr[i]);
            // }
            // printf("\n");
            // printf("rank: %d, seperator: %d, start: %d, end: %d, leftPartPos: %d, rightPartPos: %d---------------------------------------\n", rank,seperator,start ,end , leftPartPos, rightPartPos);
            // printf("currentLeftSize: %d, currentRightSize: %d\n", currentLeftSize, currentRightSize);
            // printf("leftPartPos: %d, rightPartPos: %d\n", leftPartPos, rightPartPos);
            // printf("first item: %d, last item: %d\n", arr[start], arr[end]);
            if (currentLeftSize > 0)
            memcpy(tempArray + leftPartPos, arr + start, currentLeftSize * sizeof(int));
              // for (int i = 0; i < currentLeftSize; i++)
              // {
              //     // printf("%d ", tempArray[leftPartPos + i]);
              // }
            
          
            // for (int i = 0; i < currentLeftSize; i++)
            // {
            //     tempArray[leftPartPos + i] = arr[start + i];
            // }
            // printf("first part: ");
            // for (int i = 0; i < size; i++)
            // {
                // printf("%d ", tempArray[i]);
            // }
            // printf("\nexplode yet? rank:%d\n", rank);
            leftPartPos += currentLeftSize;
            if (currentRightSize > 0)
            memcpy(tempArray + rightPartPos, arr + seperator+1, currentRightSize * sizeof(int));
            // for (int i = 0; i < currentRightSize; i++)
            // {
            //     tempArray[rightPartPos + i] = arr[seperator + 1 + i];
            // }
            rightPartPos += currentRightSize;
            // printf("second part: ");
            // for (int i = 0; i < size; i++)
            // {
                // printf("%d ", tempArray[i]);
            // }
            // printf("\nthread %d finish: leftPartPos: %d, rightPartPos: %d\n", rank, leftPartPos, rightPartPos);
        }
        // printf("not explosion middle: %d\n", middle);
        } else {
          shouldSequential = 1;
        }
        

    // #pragma omp barrier
    }
    // if (!shouldSequential) {
    //   // printf("tempArr\n");
    //   // for (int i = 0; i < size; i++)
    //   // {
    //       // printf("%d ", tempArray[i]);
    //   // }
    //   // printf("\n");
    // }else {

    // }
    
        // return;
    
    // printf("\n");
    // printf("\n");
    if (shouldSequential) {
      quickSort(arr, 0, size - 1);
      free(tempArray);
      // printf("Seqarr\n");
      // for (int i = 0; i < size; i++)
      // {
          // printf("%d ", arr[i]);
      // }
      // printf("\n");

      return;
    }

    
    // quickSortJarnGas(tempArray, middle-1);
    // quickSortJarnGas(tempArray + middle, size - middle + 1);
    // printf("will sort left: %d, left start at%d\n", middle, *tempArray);
    // printf("will sort right: %d, right start at: %d\n", size - middle,*(tempArray + middle));
    // printf("will sort left of array\n");
    // for (int i = 0; i < middle; i++)
    // {
        // printf("%d ", tempArray[i]);
    // }
    // printf("\n");
    // printf("will sort right of array\n");
    // for (int i = 0; i < size - middle; i++)
    // {
        // printf("%d ", tempArray[middle + i]);
    // }
    // printf("\n");
    // printf("left sort\n");
    quickSortJarnGas(tempArray, middle);
    // printf("right sort\n");
    quickSortJarnGas(tempArray + middle, size - middle );
    

    // merge
    // printf("merge left\n");
    memcpy(arr, tempArray, middle * sizeof(int));
    // printf("merge right\n");
    
    memcpy(arr + middle, tempArray + middle, (size - middle) * sizeof(int));

    // for (int i = 0; i < size - middle; i++)
    // {
    //     arr[middle + i] = tempArray[middle + i];
    // }

    free(tempArray);
    // printf("arr\n");
    for (int i = 0; i < size; i++)
    {
        // printf("%d ", arr[i]);
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

    // #pragma omp parallel
    //     {
    // #pragma omp single
    //         {
    //             quickSort(data, 0, count - 1);
    //         }
    //     }

    // quickSortJarnGas(data, count);
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