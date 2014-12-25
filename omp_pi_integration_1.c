/*
 * Calculate Pi by integration \int_0^1   4./(1.+x*x) 
 *
 * Revised:	Hans Effinger, June 2014
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>


int main(int argc, char* argv[])
{
   long i, tid, num_intervalls, no_threads;
   double xvalue, fvalue, deltax, sum;
   double pi, reference;
   
   if( argc != 3 ) 
   {
       printf("usage: %s <number of intervalls> <number of threads>\n", argv[0]); 
       exit(-1);
   }
   num_intervalls = atoi(argv[1]);
   no_threads    = atoi(argv[2]);
   sum=0.0;
   deltax = 1./(double)num_intervalls;

   #pragma omp parallel num_threads(no_threads)
   {
      if( omp_get_thread_num() == 0 ) 
         printf("\nNumber of threads %2d\n",omp_get_num_threads());

      #pragma omp for private(xvalue, fvalue) reduction(+:sum) 
      for (i=0; i<num_intervalls; i++)
      {
         xvalue = ((double)i + 0.5)*deltax;
         fvalue = 4.0/(1.+ xvalue*xvalue);
         sum += fvalue;
      }
   }

   pi = deltax * sum;
   reference = 4.* atan((double)1.0);
   printf("The value of PI is %15.12f error: %15.12f \n",pi, reference-pi);

}
