
/*
Alunos:
- Pedro Henrique Carvalho Pereira - 10418861
- Mateus Ribeiro Cerqueira - 10443901

Compilação e execução:
mpicc -o transformacao_mpi transformacao_mpi.c
mpirun -np 5 ./transformacao_mpi

Referências extra aula:
- https://www.geeksforgeeks.org/c/sum-of-an-array-using-mpi/
- https://stackoverflow.com/questions/15658145/how-to-share-work-roughly-evenly-between-processes-in-mpi-despite-the-array-size
- https://stackoverflow.com/questions/17570996/mpi-printing-in-an-order

OBS: diferente dos exemplos anteriores, apenas o rank 0 imprime os vetores.
por isso não precisamos de barrier ou usleep para organizar os prints.
*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


#define DATA_SIZE 100
#define PROCESSOS 5



// transforma cada valor no seu quadrado
void transformaQuad(int *vetor, int tamanho) {
    for (int i = 0; i < tamanho; i++)
        vetor[i] = vetor[i] * vetor[i];
}



int main(int argc, char *argv[]) {

    //init
    int rank;
    int tam;



    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &tam);



    // precisa executar com exatamente 5 processos
    if (tam != PROCESSOS) {
        if (rank == 0) {
            printf("[Erro] Execute com exatamente %d processos.\n", PROCESSOS);
        }

        MPI_Finalize();
        return 1;
    }

    int pedaco = DATA_SIZE / PROCESSOS;

    int *original = NULL;
    int *transformado = NULL;
    int local[DATA_SIZE / PROCESSOS];



    // 1 rank 0 cria o vetor original de 1 a 100
    if (rank == 0) {
        original = malloc(DATA_SIZE * sizeof(int));
        transformado = malloc(DATA_SIZE * sizeof(int));

        if (original == NULL || transformado == NULL) {
            printf("[Erro] Falha ao alocar os vetores.\n");
            free(original);
            free(transformado);
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }

        for (int i = 0; i < DATA_SIZE; i++)
            original[i] = i + 1;

        printf("[Processo 0] Vetor original: [");

        for (int i = 0; i < DATA_SIZE; i++) {
            printf("%d", original[i]);

            if (i < DATA_SIZE - 1)
                printf(", ");
        }

        printf("]\n");
    }



    // 2 divide 20 elementos para cada processo
    MPI_Scatter(original, pedaco, MPI_INT, local, pedaco, MPI_INT, 0, MPI_COMM_WORLD);



    // 3 cada processo transforma seu pedaco localmente
    transformaQuad(local, pedaco);



    // 4 reune todos os pedacos transformados no rank 0
    MPI_Gather(local, pedaco, MPI_INT, transformado, pedaco, MPI_INT, 0, MPI_COMM_WORLD);



    // 5 rank 0 imprime o vetor final transformado
    if (rank == 0) {
        printf("[Processo 0] Vetor transformado: [");

        for (int i = 0; i < DATA_SIZE; i++) {
            printf("%d", transformado[i]);

            if (i < DATA_SIZE - 1)
                printf(", ");
        }

        printf("]\n");

        free(original);
        free(transformado);
    }



    MPI_Finalize();
    return 0;
}