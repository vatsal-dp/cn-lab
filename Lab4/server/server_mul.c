#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define SERVER_PORT 5432
int BUF_SIZE = 4096;

clock_t start, end;
double time_taken = 0;

struct timespec del = {0, 5000000};
/* For IPv4 addresses */
int s;

struct Request
{
  socklen_t fromlen;
  struct sockaddr_in from;
};

void *send_msg(void *adr)
{
  start = clock();
  printf("Thread created\n");
  int val;

  struct Request *request = (struct Request *)adr;
  /* Send to client */

  FILE *fp = fopen("bigvid.mp4", "rb");
  if (fp == NULL)
  {
    printf("File not found!\n");
  }
  else
  {
    char buffer[BUF_SIZE];
    while ((val = (fread(buffer, 1, BUF_SIZE, fp))) > 0)
    {
      if (sendto(s, buffer, BUF_SIZE, 0, (struct sockaddr *)&request->from, request->fromlen) == -1)
      {
        bzero(buffer, sizeof(buffer));
        break;
      }

      bzero(buffer, sizeof(buffer));
      nanosleep(&del, &del);
    }

    strcpy(buffer, "BYE");
    sendto(s, buffer, sizeof("BYE"), 0, (struct sockaddr *)&request->from, request->fromlen);

    bzero(buffer, sizeof(buffer));
    fclose(fp);
  }
  end = clock();

  time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
  printf("%lf", time_taken);
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  struct sockaddr_in sin;
  char *host;
  char *delay;
  char *bufsz;
  struct hostent *hp;
  int len;
  struct sockaddr_storage client_addr;
  socklen_t client_addr_len;
  char clientIP[INET_ADDRSTRLEN];

  if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
  {
    perror("server: socket");
    exit(1);
  }

  /* build address data structure and bind to all local addresses*/
  memset((char *)&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;

  /* If socket IP address specified, bind to it. */
  if (argc == 2)
  {
    host = argv[1];
    hp = gethostbyname(host);
    if (!hp)
    {
      fprintf(stderr, "server: unknown host %s\n", host);
      exit(1);
    }

    memcpy((char *)&sin.sin_addr, hp->h_addr, hp->h_length);
  }
  else if (argc == 3)
  {
    host = argv[1];
    hp = gethostbyname(host);
    if (!hp)
    {
      fprintf(stderr, "server: unknown host %s\n", host);
      exit(1);
    }

    memcpy((char *)&sin.sin_addr, hp->h_addr, hp->h_length);
    delay = argv[2];
    del.tv_nsec = atol(delay);
    // printf("%ld", del.tv_nsec);
  }
  else if (argc == 4)
  {
    host = argv[1];
    hp = gethostbyname(host);
    if (!hp)
    {
      fprintf(stderr, "server: unknown host %s\n", host);
      exit(1);
    }

    memcpy((char *)&sin.sin_addr, hp->h_addr, hp->h_length);
    delay = argv[2];
    del.tv_nsec = atol(delay);
    bufsz = argv[3];
    BUF_SIZE = atoi(bufsz);
    // printf("%ld", del.tv_nsec);
  }
  /* Else bind to 0.0.0.0 */
  else
  {
    sin.sin_addr.s_addr = INADDR_ANY;
  }

  sin.sin_port = htons(SERVER_PORT);

  if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0)
  {
    perror("server: bind");
    exit(1);
  }

  else
  {
    /* Add code to parse IPv6 addresses */
    inet_ntop(AF_INET, &(sin.sin_addr), clientIP, INET_ADDRSTRLEN);
    printf("Server is listening at address %s:%d\n", clientIP, SERVER_PORT);
  }
  printf("Client needs to send \"GET\" to receive the file %s\n", argv[1]);

  /* Receive messages from clients*/
  pthread_t thr[5];
  int i = 0;
  char buf[BUF_SIZE];
  while (1)
  {
    struct Request *request = (struct Request *)malloc(sizeof(struct Request));
    request->fromlen = sizeof(struct sockaddr_in);

    len = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&request->from, &request->fromlen);

    buf[len] = '\0';
    printf("%s", &client_addr.ss_family);
    inet_ntop(client_addr.ss_family, &(((struct sockaddr_in *)&client_addr)->sin_addr), clientIP, INET_ADDRSTRLEN);
    printf("Server got message from %s: %s [%d bytes]\n", clientIP, buf, len);
    printf("%s", buf);

    if (strcmp(buf, "GET\n") == 0)
    {

      printf("Creating thread\n");
      pthread_create(&thr[i], NULL, &send_msg, (void *)request);

      i++;
      i %= 5;
    }
    bzero(buf, sizeof(buf));
  }
  memset(buf, 0, sizeof(buf));
}