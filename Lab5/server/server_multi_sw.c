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
#define SERVER_PORT2 5433
#define BUF_SIZE 4096

struct pack
{
  int seq;
  int sz;
  char msg[BUF_SIZE];
};

struct ack
{
  int seq;
};

struct timespec reqq, del = {0, 5000000};
/* For IPv4 addresses */
int s, s2;

struct Client
{
  socklen_t fromlen;
  struct sockaddr_in from;
};

void *send_msg(void *adr)
{

  int sq = 0;
  printf("Thread created\n");
  int val;

  struct ack ak;
  bzero(&ak, sizeof(ak));
  struct pack pk;
  bzero(&pk, sizeof(pk));

  struct Client *req = (struct Client *)adr;

  /* Send to client */

  FILE *fp = fopen("video.mp4", "rb");
  if (fp == NULL)
  {
    printf("File not found!\n");
  }
  else
  {
    while ((val = (fread(pk.msg, 1, BUF_SIZE, fp))) > 0)
    {
      // fputs(pk.msg,stdout);
      // if (val < 4096)
      // {
      //   pk.msg[val] = '\0';
      // }

      pk.seq = sq;
      sq ^= 1;
      pk.sz = val;
      if (sendto(s2, &pk, sizeof(pk), 0, (struct sockaddr *)&req->from, req->fromlen) == -1)
      {
        bzero(pk.msg, sizeof(pk.msg));
        break;
      }

      // bzero(pk.msg, sizeof(pk.msg));

      while (recvfrom(s2, &ak, sizeof(ak), 0, (struct sockaddr *)&req->from, &req->fromlen) < 0)
      {

        if (sendto(s2, &pk, sizeof(pk), 0, (struct sockaddr *)&req->from, req->fromlen) == -1)
        {
          fputs("fgh", stdout);
          bzero(pk.msg, sizeof(pk.msg));
          break;
        }
      }
      bzero(pk.msg, sizeof(pk.msg));
      // nanosleep(&del, &del);
    }
    pk.seq ^= 1;
    bzero(pk.msg, sizeof(pk.msg));
    strcpy(pk.msg, "BYE");

    sendto(s2, &pk, sizeof(pk), 0, (struct sockaddr *)&req->from, req->fromlen);

    bzero(pk.msg, sizeof(pk.msg));
    fclose(fp);
  }
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  struct timeval tout = {1, 0};
  struct sockaddr_in sin;
  struct sockaddr_in sin2;
  char *host;
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
  if ((s2 = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
  {
    perror("server: socket2");
    exit(1);
  }

  /* build address data structure and bind to all local addresses*/

  memset((char *)&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;

  memset((char *)&sin2, 0, sizeof(sin2));

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
  /* Else bind to 0.0.0.0 */
  else
  {
    sin.sin_addr.s_addr = INADDR_ANY;
  }

  sin.sin_port = htons(SERVER_PORT);
  sin2 = sin;
  sin2.sin_port = htons(SERVER_PORT2);

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
  if ((bind(s2, (struct sockaddr *)&sin2, sizeof(sin2))) < 0)
  {
    perror("server: bind2");
    exit(1);
  }
  else
  {
    /* Add code to parse IPv6 addresses */
    inet_ntop(AF_INET, &(sin2.sin_addr), clientIP, INET_ADDRSTRLEN);
    printf("Server is listening at address %s:%d\n", clientIP, SERVER_PORT);
  }
  printf("Client needs to send \"GET\" to receive the file %s\n", argv[1]);

  // client_addr_len = sizeof(client_addr);

  /* Receive messages from clients*/
  setsockopt(s2, SOL_SOCKET, SO_RCVTIMEO, (char *)&tout, sizeof(struct timeval));
  pthread_t thr[10];
  int i = 0;
  char buf[BUF_SIZE];
  while (1)
  {
    struct Client *req = (struct Client *)malloc(sizeof(struct Client));
    req->fromlen = sizeof(struct sockaddr_in);

    len = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&req->from, &req->fromlen);

    buf[len] = '\0';
    inet_ntop(client_addr.ss_family, &(((struct sockaddr_in *)&req->from)->sin_addr), clientIP, INET_ADDRSTRLEN);
    printf("Server got message from %s: %s [%d bytes]\n", clientIP, buf, len);
    printf("%s", buf);

    if (strcmp(buf, "GET\n") == 0)
    {

      printf("Creating thread\n");
      pthread_create(&thr[i], NULL, &send_msg, (void *)req);
      // pthread_join(thr[i], NULL);
      i++;
      i %= 10;
    }
    bzero(buf, sizeof(buf));
  }
  memset(buf, 0, sizeof(buf));
}