/*
INTEGRANTES:
    - Thomas Lincoln Victor da Silva - 156603
    - Luiz Fernando de Cristo Moloni - 159325
*/

#include <stdio.h>
#include <stdlib.h>
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
        for (int j = 0; j < 50; j++)
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

// Função que retorna a quantidade de vizinhos vivos de cada célula
int getNeighbors(float **grid, int i, int j, int local_rows, int rank)
{
    int count = 0;

    // Verificando se a célula está na borda superior
    if (i == 0)
    {
        // Verificando se a célula está na borda superior esquerda
        if (j == 0)
        {
            if (grid[i][j + 1] > 0.0)
                count++;
            if (grid[i + 1][j + 1] > 0.0)
                count++;
            if (grid[i + 1][j] > 0.0)
                count++;
            if (grid[i + 1][N - 1] > 0.0)
                count++;
            if (grid[i][N - 1] > 0.0)
                count++;
            if (grid[local_rows - 1][N - 1] > 0.0)
                count++;
            if (grid[local_rows - 1][j] > 0.0)
                count++;
            if (grid[local_rows - 1][j + 1] > 0.0)
                count++;
        }
        // Verificando se a célula está na borda superior direita
        else if (j == N - 1)
        {
            if (grid[i][j - 1] > 0.0)
                count++;
            if (grid[i + 1][j - 1] > 0.0)
                count++;
            if (grid[i + 1][j] > 0.0)
                count++;
            if (grid[i + 1][0] > 0.0)
                count++;
            if (grid[i][0] > 0.0)
                count++;
            if (grid[local_rows - 1][0] > 0.0)
                count++;
            if (grid[local_rows - 1][j] > 0.0)
                count++;
            if (grid[local_rows - 1][j - 1] > 0.0)
                count++;
        }
        // Verificando se a célula está na borda superior central
        else
        {
            if (grid[i][j - 1] > 0.0)
                count++;
            if (grid[i + 1][j - 1] > 0.0)
                count++;
            if (grid[i + 1][j] > 0.0)
                count++;
            if (grid[i + 1][j + 1] > 0.0)
                count++;
            if (grid[i][j + 1] > 0.0)
                count++;
            if (grid[local_rows - 1][j + 1] > 0.0)
                count++;
            if (grid[local_rows - 1][j] > 0.0)
                count++;
            if (grid[local_rows - 1][j - 1] > 0.0)
                count++;
        }
    }
    else if (i == local_rows - 1)
    { // Verificando se a célula está na borda inferior
        // Verificando se a célula está na borda inferior esquerda
        if (j == 0)
        {
            if (grid[i - 1][j] > 0.0)
                count++;
            if (grid[i - 1][j + 1] > 0.0)
                count++;
            if (grid[i][j + 1] > 0.0)
                count++;
            if (grid[0][j + 1] > 0.0)
                count++;
            if (grid[0][j] > 0.0)
                count++;
            if (grid[0][N - 1] > 0.0)
                count++;
            if (grid[i][N - 1] > 0.0)
                count++;
            if (grid[i - 1][N - 1] > 0.0)
                count++;
        }
        // Verificando se a célula está na borda inferior direita
        else if (j == N - 1)
        {
            if (grid[i][j - 1] > 0.0)
                count++;
            if (grid[i - 1][j - 1] > 0.0)
                count++;
            if (grid[i - 1][j] > 0.0)
                count++;
            if (grid[i - 1][0] > 0.0)
                count++;
            if (grid[i][0] > 0.0)
                count++;
            if (grid[0][0] > 0.0)
                count++;
            if (grid[0][j] > 0.0)
                count++;
            if (grid[0][j - 1] > 0.0)
                count++;
        }
        // Verificando se a célula está na borda inferior central
        else
        {
            if (grid[i][j - 1] > 0.0)
                count++;
            if (grid[i - 1][j - 1] > 0.0)
                count++;
            if (grid[i - 1][j] > 0.0)
                count++;
            if (grid[i - 1][j + 1] > 0.0)
                count++;
            if (grid[i][j + 1] > 0.0)
                count++;
            if (grid[0][j + 1] > 0.0)
                count++;
            if (grid[0][j] > 0.0)
                count++;
            if (grid[0][j - 1] > 0.0)
                count++;
        }
    }
    else
    {
        // Verificando se a célula está na borda esquerda
        if (j == 0)
        {
            if (grid[i - 1][j] > 0.0)
                count++;
            if (grid[i - 1][j + 1] > 0.0)
                count++;
            if (grid[i][j + 1] > 0.0)
                count++;
            if (grid[i + 1][j + 1] > 0.0)
                count++;
            if (grid[i + 1][j] > 0.0)
                count++;
            if (grid[i + 1][N - 1] > 0.0)
                count++;
            if (grid[i][N - 1] > 0.0)
                count++;
            if (grid[i - 1][N - 1] > 0.0)
                count++;
        }
        // Verificando se a célula está na borda direita
        else if (j == N - 1)
        {
            if (grid[i - 1][j] > 0.0)
                count++;
            if (grid[i - 1][j - 1] > 0.0)
                count++;
            if (grid[i][j - 1] > 0.0)
                count++;
            if (grid[i + 1][j - 1] > 0.0)
                count++;
            if (grid[i + 1][j] > 0.0)
                count++;
            if (grid[i + 1][0] > 0.0)
                count++;
            if (grid[i][0] > 0.0)
                count++;
            if (grid[i - 1][0] > 0.0)
                count++;
        }
        // Verificando se a célula está no centro da matriz
        else
        {
            if (grid[i - 1][j - 1] > 0.0)
                count++;
            if (grid[i - 1][j] > 0.0)
                count++;
            if (grid[i - 1][j + 1] > 0.0)
                count++;
            if (grid[i][j + 1] > 0.0)
                count++;
            if (grid[i + 1][j] > 0.0)
                count++;
            if (grid[i + 1][j + 1] > 0.0)
                count++;
            if (grid[i + 1][j - 1] > 0.0)
                count++;
            if (grid[i][j - 1] > 0.0)
                count++;
        }
    }

    return count;
}

// Função que cria a próxima geração
void CreateNextGen(float **grid, float **newGrid, float* sum, int local_rows, int start_row, int end_row, int rank, int size)
{
    int count = 0;

    // Pegando a quantidade de vizinhos vivos de cada célula na posição i,j
    for (int i = 0; i < local_rows; i++)
    {
        for (int j = 0; j < N; j++)
        {
            count = getNeighbors(grid, i, j, local_rows, rank);

            // Verificando se a célula está viva
            if (grid[i][j] > 0.0)
            {
                // Verificando se a célula tem menos de 2 vizinhos vivos --> Morrem por abandono
                if (count < 2)
                {
                    newGrid[i][j] = 0.0;
                }
                // Verificando se a célula tem 2 ou 3 vizinhos vivos --> Continua Viva
                else if (count == 2 || count == 3)
                {
                    newGrid[i][j] = grid[i][j];
                }
                // Verificando se a célula tem mais de 3 vizinhos vivos --> Morre por superpopulação
                else if (count >= 3)
                {
                    newGrid[i][j] = 0.0;
                }
            }
            else
            {
                // Verificando se a célula tem exatamente 3 vizinhos vivos --> Se torna Viva
                if (count == 3)
                {
                    float value = count / 8.0;
                    if(value > 1.0)
                        value = 1.0;
                    newGrid[i][j] = value;
                }
                else
                {
                    newGrid[i][j] = 0.0;
                }
            }

            // Somando a quantidade de células vivas
            if (newGrid[i][j] > 0.0)
            {
                *sum += 1.0;  
            }  
        }
    }

    //====================================/ Enviando as linhas de fronteira entre os processos /====================================/

    // MPI_Request requests[4];
    // MPI_Status status[4];

    // if (rank > 0)
    // {
    //     MPI_Isend(newGrid[0], N, MPI_FLOAT, rank - 1, TAG_SEND_UPPER, MPI_COMM_WORLD, &requests[0]);
    //     MPI_Irecv(newGrid[1], N, MPI_FLOAT, rank - 1, TAG_SEND_LOWER, MPI_COMM_WORLD, &requests[1]);
    // }

    // if (rank < size - 1)
    // {
    //     MPI_Isend(newGrid[local_rows - 1], N, MPI_FLOAT, rank + 1, TAG_SEND_LOWER, MPI_COMM_WORLD, &requests[2]);
    //     MPI_Irecv(newGrid[local_rows], N, MPI_FLOAT, rank + 1, TAG_SEND_UPPER, MPI_COMM_WORLD, &requests[3]);
    // }

    // MPI_Waitall(4, requests, status);

    // MPI_Sendrecv(newGrid[0], N, MPI_FLOAT, rank - 1, 0, newGrid[local_rows], N, MPI_FLOAT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // MPI_Sendrecv(newGrid[local_rows - 1], N, MPI_FLOAT, rank + 1, 0, newGrid[-1], N, MPI_FLOAT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    //====================================/ FIM /====================================/mp
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

int main(int argc, char **argv)
{
    int rank, size;

    // Inicializando o MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //Determina o número de linhas que cada processo vai receber
    int local_rows = N / size;
    int rest = N % size;

    // Se o número de linhas não for divisível pelo número de processos, o processo 0 recebe as linhas restantes
    if (rank == 0)
        local_rows += rest;

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

    // Alocando os Vetores Temporários
    float *tempUpperGrid = (float *)malloc(N * sizeof(float));
    float *tempLowerGrid = (float *)malloc(N * sizeof(float));

    struct timeval start, end;

    // Gerando a Matriz da Geração Atual
    GenerateGrid(grid, local_rows, rank);

    //====================================/ Enviando as linhas de fronteira entre os processos /====================================/ 
    /////////================NÃO ESTA FUNCIONANDO======================///////////////////
    // // MPI_Request requests[4];
    // // MPI_Status status[4];

    // // if (rank > 0)
    // // {
    // //     MPI_Isend(grid[0], N, MPI_FLOAT, rank - 1, TAG_SEND_UPPER, MPI_COMM_WORLD, &requests[0]); //Envia a linha de cima para o processo anterior
    // //     MPI_Irecv(tempLowerGrid, N, MPI_FLOAT, rank - 1, TAG_SEND_LOWER, MPI_COMM_WORLD, &requests[1]); //Recebe a linha de baixo do processo anterior
    // // }

    // // if (rank < size - 1)
    // // {
    // //     MPI_Isend(grid[local_rows - 1], N, MPI_FLOAT, rank + 1, TAG_SEND_LOWER, MPI_COMM_WORLD, &requests[2]); //Envia a linha de baixo para o próximo processo
    // //     MPI_Irecv(tempUpperGrid, N, MPI_FLOAT, rank + 1, TAG_SEND_UPPER, MPI_COMM_WORLD, &requests[3]); //Recebe a linha de cima do próximo processo
    // // }

    // // MPI_Waitall(4, requests, status);

    // /////////================ESTA FUNCIONANDO======================///////////////////
    // if (rank  > 0) {
    //     if(rank != size - 1) {
    //         MPI_Send(newGrid[local_rows - 1], N, MPI_FLOAT, rank + 1, TAG_SEND_LOWER, MPI_COMM_WORLD); //Envia a linha de baixo para o próximo processo
    //         MPI_Recv(tempUpperGrid, N, MPI_FLOAT, rank + 1, TAG_SEND_UPPER, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //Recebe a linha de cima do próximo processo
    //     } else {
    //         if(rank == size - 1){
    //           MPI_Sendrecv(grid[local_rows - 1], N, MPI_FLOAT, 0, 0, tempUpperGrid, N, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //Envia a linha de baixo para o primeiro processo e recebe a linha de cima do primeiro processo
    //         } else{
    //             MPI_Send(newGrid[0], N, MPI_FLOAT, rank - 1, TAG_SEND_UPPER, MPI_COMM_WORLD); //Envia a linha de cima para o processo anterior
    //             MPI_Recv(tempLowerGrid, N, MPI_FLOAT, rank - 1, TAG_SEND_LOWER, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //Recebe a linha de baixo do processo anterior
    //         }
    //     }    
    // } else {
    //      MPI_Sendrecv(grid[0], N, MPI_FLOAT, size - 1, 0, tempLowerGrid, N, MPI_FLOAT, size - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //Envia a linha de cima para o último processo e recebe a linha de baixo do último processo
    // }

    /////////================ESTA FUNCIONANDO======================///////////////////
    if (rank  != 0) {
        MPI_Send(newGrid[0], N, MPI_FLOAT, rank - 1, TAG_SEND_UPPER, MPI_COMM_WORLD); //Envia a linha de cima para o processo anterior
        MPI_Recv(tempLowerGrid, N, MPI_FLOAT, rank - 1, TAG_SEND_LOWER, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //Recebe a linha de baixo do processo anterior
    } 
    
    if(rank != size - 1) {
        MPI_Send(newGrid[local_rows - 1], N, MPI_FLOAT, rank + 1, TAG_SEND_LOWER, MPI_COMM_WORLD); //Envia a linha de baixo para o próximo processo
        MPI_Recv(tempUpperGrid, N, MPI_FLOAT, rank + 1, TAG_SEND_UPPER, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //Recebe a linha de cima do próximo processo
    }
    // else{
    //     if(rank == 0){
    //         MPI_Sendrecv(grid[0], N, MPI_FLOAT, size - 1, 0, tempLowerGrid, N, MPI_FLOAT, size - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //Envia a linha de cima para o último processo e recebe a linha de baixo do último processo
    //     }
    //     else if(rank == size - 1){
    //         MPI_Sendrecv(grid[local_rows - 1], N, MPI_FLOAT, 0, 0, tempUpperGrid, N, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //Envia a linha de baixo para o primeiro processo e recebe a linha de cima do primeiro processo
    //     }
    // }

    //====================================/ FIM /====================================/

    // Iniciando o cronômetro
    gettimeofday(&start, NULL);

    //====================================/ Criando a Nova Geração /====================================/

    // for (int generation = 0; generation < GEN; generation++)
    // {
    //     float sum = 0.0; // Variável para somar a quantidade de células vivas

    //     CreateNextGen(grid, newGrid, tempLowerGrid, tempUpperGrid, &sum, local_rows, start_row, end_row, rank, size);

    //     // Atualizando a Matriz da Geração Atual com a Nova Geração
    //     float **temp = grid;
    //     grid = newGrid;
    //     newGrid = temp;

    //     float total_sum;
    //     // Soma a quantidade de células vivas de cada processo
    //     MPI_Reduce(&sum, &total_sum, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    //     if(rank == 0){
    //         // Imprimindo a Matriz da Geração Atual   
    //         if(generation <= 5){
    //             printf("\n");
    //             PrintGrid(grid, 10, rank);
    //         }

    //         printf("Geração %d: %.0f células vivas\n", generation + 1, total_sum);
    //     }
    // }

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