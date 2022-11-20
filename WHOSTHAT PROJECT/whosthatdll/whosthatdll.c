#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "whosthatdll.h"
#define BSIZE 1024


void Generate_Random_Array (int *rn_array, int db_size, int nrounds){

    time_t t;       //variable for time
    int value;
    int i;
    int counter=0;
    int check;

    int *rn_array_2;
    rn_array_2 = (int*) malloc(sizeof(int) * nrounds);


    srand((unsigned) time(&t));

    while (counter < nrounds){

        value = rand()%db_size;
        check = 0;      //if check=0, generated value is not on the random array

        for(i=0; i<nrounds; i++){
            if (rn_array_2[i]==value)
                check = 1;
        }

        if (check==0){
            rn_array_2[counter]=value;
            counter++;
        }
    }

    for(i=0;i<nrounds;i++){
        rn_array[i]=rn_array_2[i];
    }


}




int Get_Best_Score(char* filename) {
  FILE *fh;
  char buffer[BSIZE];
  int current, max;
  int numbers = 0;

  fh = fopen(filename, "r");

  if (fh == NULL)
  {
    printf("Error opening file.\n");
    return 1;
  }

  while (fgets(buffer,BSIZE,fh) != NULL)
  {
    if(buffer[0]!='\n');
    current = atoi(buffer);

    if (numbers == 0) max = current;
    else if (current > max) max = current;

    numbers++;
  }

  fclose(fh);
  printf("è questo il max %d", max);
  return max;

}
