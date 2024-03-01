#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
   // load file and check if it is sorted
    FILE *fp;
    fp = fopen(argv[1], "r");
    int count;
    fscanf(fp, "%d ", &count);
    int *arr = (int *)malloc(count * sizeof(int));
    for (int i = 0; i < count; i++)
    {
        fscanf(fp, "%d ", &arr[i]);
    }
    fclose(fp);
    int sorted = 1;
    for (int i = 0; i < count - 1; i++)
    {
        if (arr[i] > arr[i + 1])
        {
            sorted = 0;
            break;
        }
    }
    if (sorted)
    {
        printf("The array is sorted\n");
    }
    else
    {
        printf("The array is not sorted\n");
    }
    return 0;
}