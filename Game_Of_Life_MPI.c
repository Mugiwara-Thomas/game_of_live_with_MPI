/*
INTEGRANTES:
    - Thomas Lincoln Victor da Silva - 156603
    - Luiz Fernando de Cristo Moloni - 159325
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <mpi.h>

#define N 2048   // Tamanho do Tabuleiro - Deve ser: 2048
#define GEN 2000 // Número de Gerações - Deve ser: 2000

#define TAG_SEND_UPPER 1
#define TAG_SEND_LOWER 2

// Função para imprimir a Matriz
void PrintGrid(float **grid, int local_n, int rank)
{
    for (int i = 0; i < local_n; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf("%.f|", grid[i][j]);
        }
        printf("\n");
    }
}

// Função que gera uma matriz de tamanho NxN com valor igual a 0 e 1
void GenerateGrid(float **grid, int local_rows, int rank)
{
    for (int i = 0; i < local_rows; i++)
    {
        for (int j = 0; j < N; j++)
        {
            grid[i][j] = 0.0;
        }
    }

    // Posiciona um glider na posição 1,1 que é uma figura que vai se movimento de uma forma específica até sumir
    // Figura de um Glider a partir da posição (1,1)
    if(rank == 0){ // Apenas o processo 0 posiciona o Glider e o R-Pentomino na Matriz
        int line = 1, col = 1;

        grid[line][col + 1] = 1.0;
        grid[line + 2][col] = 1.0;
        grid[line + 1][col + 2] = 1.0;
        grid[line + 2][col + 1] = 1.0;
        grid[line + 2][col + 2] = 1.0;

        // Figura de um R-Pentomino a partir da posição (10,30)
        line = 10, col = 30;

        grid[line][col + 1] = 1.0;
        grid[line][col + 2] = 1.0;
        grid[line + 1][col] = 1.0;
        grid[line + 1][col + 1] = 1.0;
        grid[line + 2][col + 1] = 1.0;
    }
}

// Função que libera a memória alocada para a Matriz
void FreeGrid(float **grid, int local_rows)
{
    for (int i = 0; i < local_rows; i++)
    {
        free(grid[i]);
    }
    free(grid);
} 

int main(int argc, char **argv[])
{
    int rank, size;

    // Inicializando o MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //Determina o número de linhas que cada processo vai receber
    int local_rows = N / size;

    int start_row = rank * local_rows; // Linha inicial do processo
    int end_row = start_row + local_rows; // Linha final do processo

    // Alocando a Matirz da Geração Atual - Grid
    float **grid = (float **)malloc(local_rows * sizeof(float *));
    for (int i = 0; i < local_rows; i++)
    {
        grid[i] = (float *)malloc(N * sizeof(float));
    }

    // Alocando a Matriz da Próxima Geração - NewGrid
    float **newGrid = (float **)malloc(local_rows * sizeof(float *));
    for (int i = 0; i < local_rows; i++)
    {
        newGrid[i] = (float *)malloc(N * sizeof(float));
    }

    struct timeval start, end;

    // Gerando a Matriz da Geração Atual
    GenerateGrid(grid, local_rows, rank);

    // Enviando a Matriz da Geração Atual para os outros processos
    // MPI_Bcast(&grid[0][0], local_rows * N, MPI_FLOAT, 0, MPI_COMM_WORLD);

    //====================================/ Enviando as linhas de fronteira entre os processos /====================================/ 
    
    // Trocando as linhas de fronteira entre os processos
    if (rank  != 0)
    {
        MPI_Send(newGrid[0], N, MPI_FLOAT, rank - 1, 0, MPI_COMM_WORLD);
        MPI_Recv(newGrid[0], N, MPI_FLOAT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    if(rank != size - 1)
    {
        MPI_Send(newGrid[local_rows - 1], N, MPI_FLOAT, rank + 1, 0, MPI_COMM_WORLD);
        MPI_Recv(newGrid[local_rows - 1], N, MPI_FLOAT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    //====================================/ FIM /====================================/

    // Iniciando o cronômetro
    gettimeofday(&start, NULL);

    //====================================/ Criando a Nova Geração /====================================/

    for (int generation = 0; generation < GEN; generation++)
    {
        float sum = 0; // Variável para somar a quantidade de células vivas

        CreateNextGen(grid, newGrid, local_rows, start_row, end_row, rank, size, &sum);

        // Atualizando a Matriz da Geração Atual com a Nova Geração
        float **temp = grid;
        grid = newGrid;
        newGrid = temp;

        float total_sum;
        // Soma a quantidade de células vivas de cada processo
        MPI_Reduce(&sum, &total_sum, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

        // Imprimindo a Matriz da Geração Atual   
        if(generation <= 5){
            printf("\n");
            PrintGrid(grid, local_rows, rank);
        }

        if(rank == 0){
            printf("Geração %d: %.0f células vivas\n", generation + 1, total_sum);

        }
    }

    //====================================/ FIM /====================================/

    // Parar o cronômetro
    gettimeofday(&end, NULL);

    // Calcular o tempo decorrido em segundos
    double elapsed_time = (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec) / 1000000.0);

    if(rank == 0)
        printf("Tempo total para as %d gerações: %.2f segundos\n", GEN, elapsed_time);

    // Liberando a memória alocada para as Matrizes
    FreeGrid(grid, local_rows);
    FreeGrid(newGrid, local_rows);

    MPI_Finalize();

    return 0;
}