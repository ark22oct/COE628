
/**
 *
 * COE 628 - Operating Systems
 * Lab 6
 * Abdulrehman Khan
 *
 **/

// if not already compiled run: (wont build in netbeans)
// gcc -pthread -o main main.c
// else if the main executable is already in folder you can just run it
#include <stdlib.h>     /* NULL */
#include  <stdio.h>	/* printf */
#include  <sys/types.h>	/* pid_t */
#include <unistd.h>	/* get_pid */
#include <stdlib.h>     /* exit, EXIT_FAILURE */
#include <sys/wait.h> 	/* wait */
#include <pthread.h>

#define PRODUCER_NO 5	//Number of producers, can change here
#define NUM_PRODUCED 20 //2000 //Number of items to be produced, can change here

void *generator_function(void*);
void *print_function(void*);
long sum; /* Sum of generated values*/
long finished_producers; /* number of the producer that finished producing */

//C: Mutex declaration and initialization
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

//F: Condition variable declaration and initialization
pthread_cond_t conditionalVar = PTHREAD_COND_INITIALIZER; 
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

int main(void) {
    int i=0;
    /* initialize random seed: */
    srand(time(NULL));
    sum = 0;
    finished_producers=0;
    
    //A: Creates five generator thread
    int numGenerator = 5;
    pthread_t generator[numGenerator];
    
    while (i < numGenerator){
        pthread_create(&generator[i], NULL, &generator_function, NULL);
        i++;
     }
    
    //D: Creates print thread
    pthread_t printThread;
    pthread_create(&printThread, NULL, &print_function, NULL);
    
    //B: Makes sure that all generator threads has finished before proceeding
    while (i < numGenerator) {
        pthread_join(generator[i], NULL);

    pthread_mutex_unlock(&mutex1);
    pthread_mutex_destroy(&mutex1);
    i++;
    }

    //E: Makes sure that print thread has finished before proceeding
    pthread_join(printThread, NULL);
    pthread_mutex_destroy(&mutex2);
       
    return (0);

}

void * generator_function(void* junk) {
    pthread_mutex_lock(&mutex1); // lock
    
    long counter = 0;
    long sum_this_generator = 0;
   
    while (counter < NUM_PRODUCED) {

      long tmpNumber = sum;
      long rnd_number = 1;//rand() % 10; 
        // can make = 1, output should be 100 (20 loops*5 threads)
      printf("current sum of the generated number up to now is %ld going to add %ld to it.\n", tmpNumber, rnd_number);
        sum = tmpNumber + rnd_number;
        counter++;
        sum_this_generator += rnd_number;
        usleep(1000);
    }
    
    
    printf("--+---+----+----------+---------+---+--+---+------+----\n");
    printf("The sum of produced items for this number generator at the end is: %ld \n", sum_this_generator);
    printf("--+---+----+----------+---------+---+--+---+------+----\n");
    finished_producers++;
    pthread_mutex_unlock(&mutex1); // unlock
    
    //H: If all generator has finished fire signal for condition variable
    pthread_mutex_lock(&mutex2);
    pthread_cond_signal(&conditionalVar);
    pthread_mutex_unlock(&mutex2);
    
    return (0);
}

void *print_function(void* junk) {
    //G: Wait until all generator has finished
    int i;
    for (i = 0; finished_producers < 5; i++)
        pthread_cond_wait(&conditionalVar, &mutex2);
    
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("The value of counter at the end is: %ld \n", sum);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

    pthread_mutex_unlock(&mutex2);
    
}

