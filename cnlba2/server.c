#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SERVER_PORT 5436
#define MAX_PENDING 5
#define MAX_LINE 256

struct pack {
  int x;
  int y;
};

int main()
{

  struct sockaddr_in sin;
  char buf[MAX_LINE];
  char fbuf[MAX_LINE];
  socklen_t len;
  int s, new_s;
  char str[INET_ADDRSTRLEN];
  char notfound[] = "File not found\n";
  char ch;
  FILE *fd;
  char *fName;
  struct pack pk;
  memset(&pk, 0, sizeof(pk));
  /* build address data structure */
  bzero((char *)&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = inet_addr("0.0.0.0");
  sin.sin_port = htons(SERVER_PORT);

  /* setup passive open */
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("simplex-talk: socket");
    exit(1);
  }

  inet_ntop(AF_INET, &(sin.sin_addr), str, INET_ADDRSTRLEN);
  printf("Server is using address %s and port %d.\n", str, SERVER_PORT);

  if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0)
  {
    perror("simplex-talk: bind");
    exit(1);
  }
  else
    printf("Server bind done.\n");

  listen(s, MAX_PENDING);

  /* wait for connection, then receive and print text */
  while (1)
  {
    if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0)
    {
      perror("simplex-talk: accept");
      exit(1);
    }
    printf("Listening.\n");
    send(new_s, "Hello\n", 6, 0);
    // recv(new_s, buf, sizeof(buf), 0);
    fputs(buf, stdout);
    //fputs("here\n", stdout);
    bzero(buf, sizeof(buf));

    while (len = recv(new_s, fbuf, sizeof(fbuf), 0))
    {
      if (strcmp(fbuf, "GET\n") == 0) {
        scanf("%d", &pk.x);
        scanf("%d", &pk.y);

        printf("%d\n", pk.x);
        printf("%d\n", pk.y);

        send(new_s, &pk, sizeof(pk), 0);


        int ans;
        memset(&ans, 0, sizeof(ans));

        int tp = 0;

        recv(new_s, &pk, sizeof(pk), 0);
        printf("%d\n", pk.x);

      }
      // fputs(fbuf, stdout);
      // fbuf[strcspn(fbuf, "\r\n")] = 0;

      // int num1, num2;

      bzero(fbuf, sizeof(fbuf));
    }
    close(new_s);
  }
}
