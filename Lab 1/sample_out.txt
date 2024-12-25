#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t child1, child2;

    printf("I am parent with PID: %d\n", getpid());

    child1 = fork();
    if (child1 == 0)
    {
        printf("I am child1 with PID: %d\n", getpid());
        execl("/bin/date", "date", NULL);
    }
    else
    {
        wait(NULL);
        child2 = fork();
        if (child2 == 0)
        {
            printf("I am child2 with PID: %d\n", getpid());
            execl("/bin/ls", "ls", NULL);
        }
    }
}