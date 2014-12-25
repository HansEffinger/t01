/*
 * Calculate Pi by integration \int_0^1   4./(1.+x*x) 
 * Combine MPI and OMP
 * 
 * Revised:	Hans Effinger, June 2014
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <mpi.h>
#include <omp.h>

#define MASTER 0

#ifndef _OPENMP
int omp_get_thread_num()  {return 0;} 
int omp_get_num_threads() {return 0;} 
#endif


int main(int argc, char* argv[])
{
   long i, tid, num_intervalls, no_threads;
   double xvalue, fvalue, deltax, sum;
   double pi, reference;
   int numtasks, taskid, namelen, rc;
   char host[MPI_MAX_PROCESSOR_NAME];
   
   if( argc != 3 ) 
   {
       printf("usage: %s <number of intervalls> <number of threads>\n", argv[0]); 
       exit(-1);
   }
   num_intervalls = atoi(argv[1]);
   no_threads    = atoi(argv[2]);
   deltax = 1./(double)num_intervalls;
   sum=0.0;

   rc = MPI_Init(&argc,&argv);
   if( rc != MPI_SUCCESS ) {printf("MPI error MPI_Init \n"); exit(-1);}
   rc = MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
   if( rc != MPI_SUCCESS ) {printf("MPI error MPI_Comm_size \n"); exit(-1);}
   rc = MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
   if( rc != MPI_SUCCESS ) {printf("MPI error MPI_Comm_rank \n"); exit(-1);}
   rc = MPI_Get_processor_name(host, &namelen);
   if( rc != MPI_SUCCESS ) {printf("MPI error MPI_Get_processor_name \n"); exit(-1);}

   /* each host */

   #pragma omp parallel num_threads(no_threads)
   {
      if( omp_get_thread_num() == 0 ) 
         printf("MPI task %d on Host %s Number of threads %2d\n",taskid, host, omp_get_num_threads());

      #pragma omp for private(xvalue, fvalue) reduction(+:sum) 
      for (i=taskid; i<num_intervalls; i += numtasks )
      {
         xvalue = ((double)i + 0.5)*deltax;
         fvalue = 4.0/(1.+ xvalue*xvalue);
         sum += fvalue;
      }
   }


   /*reduction over MPI tasks */
   rc = MPI_Reduce(&sum, &pi, 1, MPI_DOUBLE, MPI_SUM,
                   MASTER, MPI_COMM_WORLD);
   if( rc != MPI_SUCCESS ) {printf("MPI error MPI_Reduce \n"); exit(-1);}

   if( taskid == MASTER )
   {   
      pi = deltax * pi;
      reference = 4.* atan((double)1.0);
      printf("The value of PI is %15.12f error: %15.12f \n",pi, reference-pi);
   }

   rc = MPI_Barrier(MPI_COMM_WORLD);
   if( rc != MPI_SUCCESS ) {printf("MPI error MPI_Barrier \n"); exit(-1);}

   exit(0);

}
