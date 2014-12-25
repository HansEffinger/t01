/*
 * Calculate Pi by integration \int_0^1   4./(1.+x*x) 
 * Combine MPI and OMP
 * 
 * Revised:	Hans Effinger, Nov 2014
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <mpi.h>

#define MASTER 0
#define SLAVE  1

#define KILO   1024
#define MEGA   (1024*1024)



int main(int argc, char* argv[])
{
   long i, bytes, loops, counter=0;
   int numtasks, taskid, namelen, rc;
   char host[MPI_MAX_PROCESSOR_NAME];
   MPI_Status status;
   char *buffer, *buffer1;
   double wtime, ticks, datarate;   

   if( argc != 3 ) 
   {
       printf("usage: %s <data size  (bytes)> <number of loops>\n", argv[0]); 
       exit(-1);
   }
   bytes = atoi(argv[1])*MEGA;
   loops = atoi(argv[2]);

   rc = MPI_Init(&argc,&argv);
   if( rc != MPI_SUCCESS ) {printf("MPI error MPI_Init \n"); exit(-1);}
   rc = MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
   if( rc != MPI_SUCCESS ) {printf("MPI error MPI_Comm_size \n"); exit(-1);}
   rc = MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
   if( rc != MPI_SUCCESS ) {printf("MPI error MPI_Comm_rank \n"); exit(-1);}
   rc = MPI_Get_processor_name(host, &namelen);
   if( rc != MPI_SUCCESS ) {printf("MPI error MPI_Get_processor_name \n"); exit(-1);}

   /* allocat memory on stack */
   buffer = malloc(bytes);
   if( buffer == NULL ) {printf("error malloc buffer \n"); exit(-1);}
   buffer1 = malloc(bytes);
   if( buffer == NULL ) {printf("error malloc buffer \n"); exit(-1);}

   ticks = MPI_Wtick();
      
   if( taskid == MASTER )
   {   
     while(1){ 
     counter = 0;
     wtime = MPI_Wtime();

      for(i=0;i<loops;i++)
      {

         memcpy(buffer1,buffer,bytes);
         memcpy(buffer,buffer1,bytes);

         counter++;
      }
      wtime = MPI_Wtime() - wtime;
      datarate = bytes*loops/(2.e0 * MEGA * wtime);
      printf("MASTER loops: %ld, time(s): %f, datarate(MB/s): %f \n", counter, wtime, datarate);
}
   } else 
   {
   }
   rc = MPI_Finalize();
   if( rc != MPI_SUCCESS ) {printf("MPI error MPI_Finalize \n"); exit(-1);}


   exit(0);

}
