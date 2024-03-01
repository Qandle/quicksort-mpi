// generate input file 1,000,000
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    FILE *fp;
    fp = fopen("input10.txt", "w");
    srand(time(0));
    fprintf(fp, "%d\n", 10);
    for (int i = 0; i < 10; i++)
    {
        fprintf(fp, "%d ", rand());
    }
    fclose(fp);
    return 0;
}