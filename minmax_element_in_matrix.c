#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

void max_mat(int **mat, int thread_count, int m, int n);
void min_mat(int **mat, int thread_count, int m, int n);

int main(int argc, char* argv[])
{
//    srand(time(NULL));
    
    int m, n, i, j;
    int thread_count;

    thread_count = strtol(argv[1], NULL, 10);   
    scanf("%d %d",&m, &n);
    
    int **mat;

    mat = (int **)malloc(sizeof(int*)*m);
    for(int i=0;i<m;i++)
        mat[i] = (int *)malloc(sizeof(int)*n);
    
    for(i=0;i<m;i++)
    {
        for(j=0;j<n;j++)
        {
            mat[i][j] = rand() % 100 + 1;
            printf("%d ",mat[i][j]);
        }
        printf("\n");
    }
    max_mat(mat,thread_count,m,n);
    min_mat(mat,thread_count,m,n);

    for(i=0;i<m;i++)
        free(mat[i]);
    free(mat);
}

void max_mat(int **mat,int thread_count, int m, int n)
{
    int i, j, x, y;
    int max = 0;
#   pragma omp parallel for num_threads(thread_count) 
    for(i=0;i<m;i++)
    {
#   pragma omp parallel for num_threads(thread_count)  
        for(j=0;j<n;j++)
        {

            if(max < mat[i][j])
            {
                x = i;
                y = j;
                max = mat[i][j];
            }
        }
    }
    printf("%d (%d,%d)\n", max, x, y);
}

void min_mat(int **mat,int thread_count, int m, int n)
{
    int i, j, x, y;
    int min = 101;
#   pragma omp parallel for num_threads(thread_count) 
    for(i=0;i<m;i++)
    {
#   pragma omp parallel for num_threads(thread_count)     
        for(j=0;j<n;j++)
        {
            if(min > mat[i][j])
            {
                x = i;
                y = j;
                min = mat[i][j];
            }
        }
    }
    printf("%d (%d,%d)\n", min, x, y);
}

