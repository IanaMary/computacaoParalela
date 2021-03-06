#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

double f(double x) {
   double return_val = 0.0;
   return_val = cos(x * log(1.0/x));
   return return_val;
}

double g(int inicio, int fim, double h, double x) {
	double integral;
	for (int i = inicio; i <= fim; i++) {
         x += h;
         integral += f(x);
    } 
    return integral; 
}

int main(int argc, char *argv[]) {
   // Valor da integral
   double integralTotal, integralProcesso;

   // Limites do intervalo
   double a, b;
   // Número de trapézios
   int nTrapezios;
   // Base do trapézio
   double h, x;
   int i;

   int nTask, rank;

   double info[2];
   int inicio, fim;

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &nTask);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   nTrapezios = atoi(argv[3]);

   inicio = rank * ((nTrapezios)/nTask) + 1;
   fim = (rank + 1) *  ((nTrapezios)/nTask);

   if(rank == (nTask-1)){
      fim = nTrapezios - 1;
   }


   if(rank == 0){
      a = atof(argv[1]);
      b = atof(argv[2]);
  
      h = (b - a) / nTrapezios;
      integralProcesso = (f(a) + f(b))/2.0;

      x = a;

      info[0] = a;
      info[1] = h; 

      MPI_Bcast(info, 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);
      
      integralProcesso = g(inicio, fim, h, x);
      
      MPI_Reduce(&integralProcesso, &integralTotal, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

      integralTotal *= h;
      printf("%d trapézios, estimativa de %.2f a %.2f = %.5f\n", nTrapezios, a, b, integralTotal);


   }
   else{
      MPI_Bcast(info, 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);

      x = info[0] + inicio*info[1];
      integralProcesso = f(x);

      integralProcesso = g(inicio+1, fim, info[1], x);

      MPI_Reduce(&integralProcesso, &integralTotal, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
   }


   MPI_Finalize();
   return 0;
}
