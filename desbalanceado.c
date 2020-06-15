#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define ARRAY_SIZE 10000
#define DELTA 2500
void bs(int n, int * vetor)
{
    int c=0, d, troca, trocou =1;

    while (c < (n-1) & trocou )
        {
        trocou = 0;
        for (d = 0 ; d < n - c - 1; d++)
            if (vetor[d] > vetor[d+1])
                {
                troca      = vetor[d];
                vetor[d]   = vetor[d+1];
                vetor[d+1] = troca;
                trocou = 1;
                }
        c++;
        }
}

int *interleaving(int vetor[],int tam)
{
    int *vetor_aux;
    int i1,i2,i_aux;

    vetor_aux = (int *)malloc(sizeof(int)*tam);

    i1 = 0;
    i2 = tam/2;
    for(i_aux = 0; i_aux < tam;i_aux++)
    {
        if(((vetor[i1]<=vetor[i2]) && (i1 < (tam/2))) || (i2 == tam)) 
        {
            vetor_aux[i_aux] = vetor[i1++];
        }
        else
        {
            vetor_aux[i_aux] = vetor[i2++];
        }
        
    }
    return vetor_aux;
}

int main(int argc , char **argv)
{
    int i;
    double t1,t2;
    int proc_n;
    int my_rank;
    MPI_Status status;
    int message[ARRAY_SIZE];
    int left, right;
    int father;

    int *array_aux; 
    int tam_vetor;


    //message = (int *)malloc(ARRAY_SIZE * sizeof(int));

    

    MPI_Init (&argc , & argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); // pega pega o numero do processo atual (rank)
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);  // pega informacao do numero de processos (quantidade total)

    t1 = MPI_Wtime(); //inicio de medicao

    left = 2*my_rank + 1;
    right = 2*my_rank + 2;
    if(my_rank != 0)
    {
        
        MPI_Recv(&message, ARRAY_SIZE, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); //espera mensagens dos escravos
        father = status.MPI_SOURCE;
        MPI_Get_count(&status,MPI_INT,&tam_vetor);
        
    }
    else
    {
       // array = malloc(ARRAY_SIZE * sizeof(int));
        tam_vetor = ARRAY_SIZE;
        for(i=0;i<ARRAY_SIZE;i++)
        {
            message[i] = ARRAY_SIZE - i;
        }
    }

    if(tam_vetor <= DELTA)
    {
        bs(tam_vetor, message);
        
    }
    else
    {
        MPI_Send(&message[0], tam_vetor/2, MPI_INT, left, 0,  MPI_COMM_WORLD);
        //printf("proc %d mandou para %d \n", my_rank, left);
        MPI_Send(&message[tam_vetor/2], tam_vetor/2, MPI_INT, right, 0,  MPI_COMM_WORLD);
     //   printf("proc %d mandou para %d \n", my_rank, right);

        MPI_Recv(&message[0], ARRAY_SIZE, MPI_INT, right, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&message[tam_vetor/2], ARRAY_SIZE, MPI_INT, left, MPI_ANY_TAG, MPI_COMM_WORLD, &status);


        array_aux = interleaving(message,tam_vetor);
/*
        for(i = 0; i < tam_vetor;i++)
        {
            message[i] =  *array_aux++;
        }*/

         
    }
    
    
    if(my_rank != 0)
    {
        MPI_Send(&message[0], tam_vetor, MPI_INT, father, 0,  MPI_COMM_WORLD);
    }
    else
    {
        t2 = MPI_Wtime(); //terminamos a medicao
       // for(i=0;i<ARRAY_SIZE;i++)
         //  printf("[%03d] ", array_aux[i]);
        
       
        printf("\nTempo de execucao: %f\n\n",t2-t1);
        
    }
    
    MPI_Finalize();


    return 0;

}