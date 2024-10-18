/**
 * Name: Abdullah Elsayed Ahmed
 * Description: Matrix multiplication with threads.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

/* Defined datatype */
typedef struct 
{
    int rows;
    int cols;
    int **data;
}
Matrix;

typedef struct 
{
    int *arr1;
    Matrix *matrix2;
    int col;
    int n;
    int *block;
}
MultiArrayArgs;

typedef struct 
{
    int num1;
    int num2;
    int *block;
}
MultiNumbersArgs;


/* Functions deceleration */
Matrix *matrix_init(int rows, int cols);
void matrix_destroy(Matrix *matrix);
void matrix_print(Matrix *matrix);

Matrix *multi_approach1(Matrix *matrix1, Matrix *matrix2);
Matrix *multi_approach2(Matrix *matrix1, Matrix *matrix2);

/* Main code */
int main(void)
{
    Matrix *matrix1 = NULL;
    Matrix *matrix2 = NULL;

    // Get file name from the user
    char filename[100];
    fgets(filename, 100, stdin);
    filename[strlen(filename)-1] = '\0';

    // Reading input from file
    FILE *inputFile = fopen(filename, "r");
    if (inputFile == NULL)
    {
        perror("Can't find the file!\n");
        return 1;
    }
    
    int rows, cols;

    // Get first matrix
    fscanf(inputFile, "%d%d", &rows, &cols);

    matrix1 = matrix_init(rows, cols);
    
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            fscanf(inputFile, "%d", &matrix1->data[i][j]);
        }
    }

    // Get second matrix
    fscanf(inputFile, "%d%d", &rows, &cols);

    matrix2 = matrix_init(rows, cols);
    
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            fscanf(inputFile, "%d", &matrix2->data[i][j]);
        }
    }

    fclose(inputFile);


    struct timeval start_time;
    struct timeval end_time;
    Matrix *res;
    double elapsed_time;

    gettimeofday(&start_time, NULL);

    res = multi_approach1(matrix1, matrix2);

    gettimeofday(&end_time, NULL);

    // Calculate the elapsed time in seconds
    elapsed_time = (end_time.tv_sec - start_time.tv_sec) +
                          (end_time.tv_usec - start_time.tv_usec) / 1000000.0;

    if (res == NULL)
    {
        printf("Multiplication of Matrix is not Possible !!\n");
        return 1;
    }

    matrix_print(res);

    printf("END1 %f \n", elapsed_time);

    gettimeofday(&start_time, NULL);

    res = multi_approach2(matrix1, matrix2);

    gettimeofday(&end_time, NULL);

    // Calculate the elapsed time in seconds
    elapsed_time = (end_time.tv_sec - start_time.tv_sec) +
                          (end_time.tv_usec - start_time.tv_usec) / 1000000.0;

    matrix_print(res);

    printf("END2 %f \n", elapsed_time);


    // Free all allocated memory
    matrix_destroy(matrix1);
    matrix_destroy(matrix2);
    matrix_destroy(res);

    return 0;
}

/* Functions defintion */
Matrix *matrix_init(int rows, int cols)
{
    Matrix *matrix = (Matrix *)malloc(sizeof(Matrix));
    
    if (matrix == NULL)
    {
        return NULL;
    }

    matrix->rows = rows;
    matrix->cols = cols;

    int **data = (int **)malloc(sizeof(int **) * rows);
    for (int i = 0; i < rows; i++)
    {
        data[i] = (int *)malloc(sizeof(int) * cols);
    }

    matrix->data = data;
    
    return matrix;
}

void matrix_destroy(Matrix *matrix)
{
    if (matrix != NULL)
    {
        free(matrix->data);
        free(matrix);
    }
}

void matrix_print(Matrix *matrix)
{
    for (size_t i = 0; i < matrix->rows; i++)
    {
        for (size_t j = 0; j < matrix->cols; j++)
        {
            printf("%d ", matrix->data[i][j]);
        }
        printf("\n");
    }
}

void *multi_arrays(void *args)
{
    MultiArrayArgs *multiArrayArgs = (MultiArrayArgs *)args;
    int *arr1 = multiArrayArgs->arr1;
    Matrix *matrix2 = multiArrayArgs->matrix2;
    int col = multiArrayArgs->col;
    int n = multiArrayArgs->n;
    int *block = multiArrayArgs->block;

    (*block) = 0;
    for (int i = 0; i < n; i++)
    {
        (*block) += arr1[i] * matrix2->data[i][col];          
    }
}

void *multi_numbers(void *args)
{
    MultiNumbersArgs *multiNumberArgs = (MultiNumbersArgs *)args;
    int num1 = multiNumberArgs->num1;
    int num2 = multiNumberArgs->num2;
    int *block = multiNumberArgs->block;
    (*block) += num1 * num2;
}

Matrix *multi_approach1(Matrix *matrix1, Matrix *matrix2)
{
    if (matrix1->cols != matrix2->rows)
    {
        return NULL;
    }
    
    pthread_t thread[matrix1->rows][matrix2->cols];

    Matrix *res = matrix_init(matrix1->rows, matrix2->cols);

    for (int row = 0; row < matrix1->rows; row++)
    {
        for (int col = 0; col < matrix2->cols; col++)
        {    
            int *block = &(res->data[row][col]);
            (*block) = 0;
            for (int z = 0; z < matrix1->cols; z++)
            {
                MultiNumbersArgs *multiNumberArgs = (MultiNumbersArgs *)malloc(sizeof(MultiNumbersArgs));
                multiNumberArgs->num1 = matrix1->data[row][z];
                multiNumberArgs->num2 = matrix2->data[z][col];
                multiNumberArgs->block = block;
                pthread_create(&thread[row][col], NULL, multi_numbers, multiNumberArgs);
            }
        }
    }

    for (size_t i = 0; i < matrix1->rows; i++)
    {
        for (size_t j = 0; j < matrix2->cols; j++)
        {
            pthread_join(thread[i][j], NULL);
        }
        
    }
    
    return res;
}

Matrix *multi_approach2(Matrix *matrix1, Matrix *matrix2)
{
    if (matrix1->cols != matrix2->rows)
    {
        return NULL;
    }
    
    pthread_t thread[matrix1->rows];
    Matrix *res = matrix_init(matrix1->rows, matrix2->cols);

    for (int row = 0; row < matrix1->rows; row++)
    {
        for (int col = 0; col < matrix2->cols; col++)
        {    
            MultiArrayArgs *multiArrayArgs = (MultiArrayArgs *)malloc(sizeof(MultiArrayArgs));
            multiArrayArgs->arr1 = matrix1->data[row];
            multiArrayArgs->matrix2 = matrix2;
            multiArrayArgs->col = col;
            multiArrayArgs->n = matrix2->rows;
            multiArrayArgs->block = &(res->data[row][col]);

            pthread_create(&thread[row], NULL, multi_arrays, multiArrayArgs);
        }
    }

    for (size_t i = 0; i < matrix1->rows; i++)
    {
        pthread_join(thread[i], NULL);
    }
    
    return res;
}