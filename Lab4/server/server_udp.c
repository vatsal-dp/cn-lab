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
#define BUF_SIZE 4096

int s;
sa_family_t sf;

void *fun(void *adr)
{
  printf("function called");
  char *arr = (char *)adr;
  struct sockaddr_storage *cli_addr;
  inet_pton(sf, arr, &(((struct sockaddr_in *)&cli_addr)->sin_addr));
  socklen_t cli_addr_len = sizeof(cli_addr);
  struct timespec del = {0, 5000000};
  int val;
  FILE *fp = fopen("bigvid.mp4", "rb");
  if (fp == NULL)
  {
    printf("File not found!");
  }
  else
  {
    char buffer[BUF_SIZE];

    while ((val = (fread(buffer, 1, BUF_SIZE, fp))) > 0)
    {
      if (sendto(s, buffer, BUF_SIZE, 0, (struct sockaddr *)&cli_addr, cli_addr_len) == -1)
      {
        bzero(buffer, sizeof(buffer));
        // memset(buffer, 0, sizeof(buffer));
        break;
      }

      // memset(buffer, 0, sizeof(buffer));
      bzero(buffer, sizeof(buffer));
      nanosleep(&del, &del);
    }

    strcpy(buffer, "BYE");
    sendto(s, buffer, sizeof("BYE"), 0, (struct sockaddr *)&cli_addr, cli_addr_len);
    // memset(buffer, 0, sizeof(buffer));
    bzero(buffer, sizeof(buffer));
  }
  fclose(fp);
}

int main(int argc, char *argv[])
{

  struct sockaddr_in sin;
  struct sockaddr_storage client_addr;

  char clientIP[INET_ADDRSTRLEN]; /* For IPv4 addresses */
  socklen_t client_addr_len;
  char buf[BUF_SIZE];

  int len;

  char *host;
  struct hostent *hp;

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

  client_addr_len = sizeof(client_addr);

  /* Receive messages from clients*/
  while (len = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&client_addr, &client_addr_len))
  {
    sf = client_addr.ss_family;
    inet_ntop(sf, &(((struct sockaddr_in *)&client_addr)->sin_addr), clientIP, INET_ADDRSTRLEN);
    // const char * inet_ntop(int, const void *, char *, socklen_t)
    printf("Server got message from %s: %s [%d bytes]\n", clientIP, buf, len);

    buf[len] = '\0';
    printf("Client : %s\n", buf);

    /* Send to client */
    if (strcmp(buf, "GET\n") == 0)
    {
      printf("Creating thread\n");
      pthread_t thr;
      char *cip = (char *)&clientIP;
      pthread_create(&thr, NULL, &fun, (void *)cip);
      pthread_join(thr, NULL);
    }

    // bzero(buf, sizeof(buf));
    memset(buf, 0, sizeof(buf));
  }

  /* Send BYE to signal termination */

  // strcpy(buf, "BYE");

  // sendto(s, buf, sizeof(buf), 0, (struct sockaddr*)&client_addr, client_addr_len);
}