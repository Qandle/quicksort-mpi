// generate input file 1,000,000
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    FILE *fp;
    fp = fopen("input100.txt", "w");
    srand(time(0));
    fprintf(fp, "%d\n", 100);
    for (int i = 0; i < 100; i++)
    {
        fprintf(fp, "%d ", rand());
    }
    fclose(fp);
    return 0;
}