/*
 * Calculate Pi by integration \int_0^1   4./(1.+x*x) 
 * Combine MPI and OMP
 * 
 * Revised:	Hans Effinger, Nov 2014
 * 		Hans Effinger, Dez 2014
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>


#define KILO   1024
#define MEGA   (1024*1024)



int main(int argc, char* argv[])
{
   long i, bytes, loops, counter=0;
   clock_t start_t, end_t;
   double total_t;
   char *buffer, *buffer1;
   double datarate;   

   if( argc != 3 ) 
   {
       printf("usage: %s <data size  (bytes)> <number of loops>\n", argv[0]); 
       exit(-1);
   }
   bytes = atoi(argv[1])*KILO;
   loops = atoi(argv[2]);

   printf("--------- loops: %ld, bytes: %ld \n", loops, bytes);


   /* allocat memory on stack */
   buffer = malloc(bytes);
   if( buffer == NULL ) {printf("error malloc buffer \n"); exit(-1);}
   buffer1 = malloc(bytes);
   if( buffer == NULL ) {printf("error malloc buffer \n"); exit(-1);}

   while(1){ 
      start_t = clock();
      counter = 0;
      for(i=0;i<loops;i++)
      {
         memset(buffer, i%256,1);
         memcpy(buffer1,buffer,bytes);
         memcpy(buffer,buffer1,bytes);
         counter++;
      }
      end_t = clock();   
      total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
      datarate = bytes*loops/(2.e0 * MEGA * total_t);
      printf("loops: %ld, time(s): %f, datarate(MB/s): %f \n", counter, (float)total_t, datarate);
   }
   exit(0);

}
