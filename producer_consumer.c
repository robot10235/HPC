/*
**  PROGRAM: A simple serial producer/consumer program
**
**  One function generates (i.e. produces) an array of random values.  
**  A second functions consumes that array and sums it.
**
**  HISTORY: Written by Tim Mattson, April 2007.
*/
#include <omp.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>

#define N        10000
#define MAX_QUEUE_SIZE 200
/* Some random number constants from numerical recipies */
#define SEED       2531
#define RAND_MULT  1366
#define RAND_ADD   150889
#define RAND_MOD   714025
int randy = SEED;

// queue
int front = 0;
int back = 0;

void Enqueue(double q[], double rand)
{
    q[back % MAX_QUEUE_SIZE]=rand;
    back++;   
}

void Dequeue()
{
    front++;
}

// send msg to q
int Send_msg(double q[])
{
    int flag = 0;
    // do not send msg when q is full
    if(back - front == MAX_QUEUE_SIZE)
        flag = 0;
    // send msg when q is not full
    else{
        flag = 1;
#   pragma omp critical
{    
        randy = (RAND_MULT * randy + RAND_ADD) % RAND_MOD;
//        printf("cri: %f\n", ((double) randy)/((double) RAND_MOD));
        Enqueue(q,((double) randy)/((double) RAND_MOD));  
}    
    }
    return flag;
}

// recv msg from q
double Try_receive(double q[])
{
    double rand = 0; 
    int queue_size;   
    queue_size = back - front;
    if(queue_size <= 0)
        rand = -1;
    else{
        rand = q[front % MAX_QUEUE_SIZE];
        Dequeue();
    }     
    return rand;
}

// if q is empty and the random thread is done, program done
int Done(int done_sending, int thread_count)
{
    int queue_size = back - front;
    if(queue_size == 0 && done_sending == (thread_count / 2))
        return 1;
    else  
        return 0;
}


int main(int argc, char* argv[])
{
    double sum, runtime;
    //double q[MAX_QUEUE_SIZE];
    double *q;
    q = (double *)malloc(MAX_QUEUE_SIZE*sizeof(double));
    int thread_count;
    int done_sending = 0;
    thread_count = strtol(argv[1], NULL, 10);
    sum = 0;
    runtime = omp_get_wtime();
#   pragma omp parallel num_threads(thread_count)
{
    double r;
    int i;
    double my_sum = 0;
    int my_id = omp_get_thread_num();
    if(my_id < thread_count / 2){
        for(i = 0; i < (N * 2 / thread_count); i++){
            // need to resend
            if(!Send_msg(q))
                i--;
            /* for(j = 0;j < back;j++){
                printf("%d ",j);
                printf("%f\n",q[j]);
            }*/ 
        }
    #   pragma omp critical
        done_sending += 1;
    }
    else{
        while(!Done(done_sending, thread_count)){
        #  pragma omp critical
            r = Try_receive(q);
            if(r==-1){
                continue;
            }
            my_sum += r;
                
        }
    #   pragma omp critical
        sum += my_sum;
    }
}
    runtime = omp_get_wtime() - runtime;

    printf(" In %f seconds, The sum is %f \n",runtime,sum);
    free(q);
    return 0;
}
 
