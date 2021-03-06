#include "mpi.h"
#include "operacoesSobreMatrizes.h"

void multiplicaMatrizes(float *pMatResul, float *pMat1, float *pMat2, int pLinhaMat1, int pLinhaMat2, int pColunaMat1, int pColunaMat2){	    
	
	for(int i = 0; i < pLinhaMat1; i++){
		for(int j = 0; j < pColunaMat2; j++){
			pMatResul[i*pColunaMat2 + j] = 0;
			for(int k = 0; k < pColunaMat1; k++){
				pMatResul[i*pColunaMat2 + j] = pMatResul[i*pColunaMat2 + j] + pMat1[i*pColunaMat1 + k] * pMat2[k*pColunaMat2 + j];
			}
		}
	}	
}



int main(int argc, char *argv[]) {
	
	int pLinhaMat1, pColunaMat1, pLinhaMat2, pColunaMat2, rank, nTask;
	int info[4];
	float *pMat1, *pMat2, *pMatResul, *pMatFinal, *pMatLocal;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nTask);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
	
    
    if(rank == 0) {
		pMat1 = lerMatriz(argv[1], &pLinhaMat1, &pColunaMat1);
		pMat2 = lerMatriz(argv[2], &pLinhaMat2, &pColunaMat2); 
		
		
		info[0] = pLinhaMat1/nTask;
		info[1] = pColunaMat1;
		info[2] = pLinhaMat2;
		info[3] = pColunaMat2; 
		
		imprimirMatriz(pMat1, pLinhaMat1, pColunaMat1);
		imprimirMatriz(pMat2, pLinhaMat2, pColunaMat2);
		
		pMatFinal = (float*) malloc(pLinhaMat1*pColunaMat2*sizeof(float));
		pMatResul = (float*) malloc(info[0]*info[3]*sizeof(float));   
		pMatLocal = (float*) malloc(info[0]*info[1]*sizeof(float));
		
        MPI_Bcast(info, 4, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(pMat2, info[2]*info[3], MPI_FLOAT, 0, MPI_COMM_WORLD);
		MPI_Scatter(pMat1, info[0]*info[1], MPI_FLOAT, pMatLocal, info[0]*info[1], MPI_FLOAT, 0, MPI_COMM_WORLD);
      
		multiplicaMatrizes(pMatResul, pMatLocal, pMat2, info[0], info[2],  info[1], info[3]);
        
        MPI_Gather(pMatResul, info[0]*info[3], MPI_FLOAT, pMatFinal,info[0]*info[3], MPI_FLOAT, 0, MPI_COMM_WORLD);
        
		//escreverMatriz(argv[3] , pMatFinal, pLinhaMat1, pColunaMat2);
        
        liberarMatriz(pMat1);
        liberarMatriz(pMat2);
        liberarMatriz(pMatResul);
        liberarMatriz(pMatLocal);
        liberarMatriz(pMatFinal);
      
	}else{
        
		MPI_Bcast(info, 4, MPI_INT, 0, MPI_COMM_WORLD);
		//pMat1 = (float*) malloc(info[0]*info[1]*sizeof(float));  
		pMat2 = (float*) malloc(info[2]*info[3]*sizeof(float));
		pMatLocal = (float*) malloc(info[0]*info[1]*sizeof(float));  
 
		MPI_Bcast(pMat2, info[2]*info[3], MPI_FLOAT, 0, MPI_COMM_WORLD);
		MPI_Scatter(pMat1, info[0]*info[1], MPI_FLOAT, pMatLocal, info[0]*info[1], MPI_FLOAT, 0, MPI_COMM_WORLD);
		
		
		imprimirMatriz(pMatLocal, info[0], info[1]);
		imprimirMatriz(pMat2, info[2], info[3]);
		
		
		pMatResul = (float*) malloc(info[0]*info[3]*sizeof(float));
		
		multiplicaMatrizes(pMatResul, pMatLocal, pMat2, info[0], info[2],  info[1], info[3]);
       
        MPI_Gather(pMatResul, info[0]*info[3], MPI_FLOAT, pMatFinal, info[0]*info[3], MPI_FLOAT, 0, MPI_COMM_WORLD);

        liberarMatriz(pMat2);
        liberarMatriz(pMatResul);
        liberarMatriz(pMatLocal);
		 
	}
    
    MPI_Finalize();
	
	return 0;
	
}

