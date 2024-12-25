#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SERVER_PORT 5436
#define MAX_LINE 256

int main(int argc, char *argv[])
{
  FILE *fp;
  struct hostent *hp;
  struct sockaddr_in sin;
  char *host;
  char buf[MAX_LINE];
  char sbuf[MAX_LINE];
  char fbuf[MAX_LINE];
  int s;
  int len;
  if (argc == 2)
  {
    host = argv[1];
  }
  else
  {
    fprintf(stderr, "usage: %s host\n", argv[0]);
    exit(1);
  }

  hp = gethostbyname(host);
  if (!hp)
  {
    fprintf(stderr, "%s: unknown host: %s\n", argv[0], host);
    exit(1);
  }
  else
    printf("Client's remote host: %s\n", argv[1]);

  /* build address data structure */
  bzero((char *)&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
  sin.sin_port = htons(SERVER_PORT);

  /* active open */
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("simplex-talk: socket");
    exit(1);
  }
  else
    printf("Client created socket.\n");

  if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
  {
    perror("simplex-talk: connect");
    close(s);
    exit(1);
  }
  else
  {
    printf("Connected\n");
  }

  FILE *d;

  while (1)
  {
    fgets(buf, sizeof(buf), stdin);
    fputs(buf, stdout);
    buf[MAX_LINE - 1] = '\0';

    len = strlen(buf) + 1;
    send(s, buf, len, 0);

    buf[strcspn(buf, "\r\n")] = 0;

    if (strcmp(buf, "Bye") == 0)
    {
      // recv(s, sbuf, sizeof(sbuf), 0);
      // fputs(sbuf, stdout);
      close(s);
      printf("Disconnected\n");
      break;
    }
    recv(s, sbuf, sizeof(sbuf), 0);

    fputs(sbuf, stdout);
    if (strcmp(sbuf, "OK") == 0)
    {
      d = fopen("file.txt", "w");
      int val;
      while (1)
      {
        val = recv(s, fbuf, sizeof(fbuf), 0);
        if (strcmp(fbuf, "Bye") == 0)
        {
          // fputs(fbuf, stdout);
          printf("i am here");
          bzero(fbuf, sizeof(fbuf));
          // fputs("iam here", stdout);
          printf("i am here");
          break;
          // return 0;
        }
        fwrite(fbuf, 1, val, d);
        fputs(fbuf, stdout);
        bzero(fbuf, sizeof(fbuf));
      }
      // buf[strcspn(fbuf, "\0")] = 0;

      fclose(d);
    }
    bzero(fbuf, sizeof(fbuf));
    bzero(sbuf, sizeof(sbuf));
    bzero(buf, sizeof(buf));
  }
  // fclose(d);
}