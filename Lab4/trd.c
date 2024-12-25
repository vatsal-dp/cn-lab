#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>

// A normal C function that is executed as a thread
// when its name is specified in pthread_create()
void *myThreadFun(void *vargp)
{
    // sleep(1);
    char *ptr = (char *)vargp;
    printf("%s", ptr);
    printf("Printing GeeksQuiz from Thread \n");
    return NULL;
}

int main()
{
    pthread_t thread_id;
    printf("Before Thread\n");
    char *ptr = "S";
    pthread_create(&thread_id, NULL, myThreadFun, (void *)ptr);
    pthread_join(thread_id, NULL);
    printf("After Thread\n");
    exit(0);
}