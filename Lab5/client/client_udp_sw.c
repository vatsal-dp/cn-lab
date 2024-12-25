#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>

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

int main(int argc, char *argv[])
{
  FILE *fp;

  struct hostent *hp;

  struct sockaddr_in sin;
  struct sockaddr_in sin2;

  char *host;

  char buf[BUF_SIZE];
  char prev[BUF_SIZE];
  int s;

  int len;
  socklen_t slen;
  int n;

  host = argv[1];
  /* translate host name into peer's IP address */

  hp = gethostbyname(host);

  if (!hp)
  {

    fprintf(stderr, "client: unknown host: %s\n", host);

    exit(1);
  }

  else
    printf("Host %s found!\n", argv[1]);

  /* build address data structure */

  memset((char *)&sin, 0, sizeof(sin));
  memset((char *)&sin2, 0, sizeof(sin2));
  sin.sin_family = AF_INET;

  memcpy((char *)&sin.sin_addr, hp->h_addr, hp->h_length);

  sin.sin_port = htons(SERVER_PORT);
  sin2 = sin;
  sin2.sin_port = htons(SERVER_PORT2);

  /* create socket */

  if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
  {

    perror("client: socket");

    exit(1);
  }

  printf("Client will get data from to %s:%d.\n", argv[1], SERVER_PORT);

  printf("To play the music, pipe the downlaod file to a player, e.g., ALSA, SOX, VLC: cat recvd_file.wav | vlc -\n");

  /* send message to server */
  while (1)
  {
    fgets(buf, sizeof(buf), stdin);

    buf[BUF_SIZE - 1] = '\0';

    len = strlen(buf) + 1;

    if (sendto(s, buf, len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {

      perror("Client: sendto()");

      return 0;
    }
    struct ack ak;
    memset(&ak, 0, sizeof(ak));
    struct pack pk;
    memset(&pk, 0, sizeof(pk));
    ak.seq = -1;
    bzero(buf, sizeof(buf));
    FILE *fd = fopen("out.mp4", "wb");
    while ((n = recvfrom(s, &pk, sizeof(pk), 0, (struct sockaddr *)&sin2, &slen)) > 0)
    {
      if (pk.seq == ak.seq)
      {
        ak.seq = pk.seq;
        if (sendto(s, &ak, sizeof(ak), 0, (struct sockaddr *)&sin2, sizeof(sin2)) < 0)
        {
          perror("Client: sendto()");
          return 0;
        }
        continue;
      }
      if (strcmp(pk.msg, "BYE") == 0)
      {
        bzero(pk.msg, sizeof(pk.msg));
        fputs("tata", stdout);
        break;
      }

      // char *idx = strchr(pk.msg, '\0');
      // int ll = sizeof(pk.msg);
      // if (idx != NULL)
      // {
      //   ll = (int)(idx - pk.msg);
      // }

      fwrite(pk.msg, 1, pk.sz, fd);
      // fputs(pk.msg,stdout);

      // printf("%d",pk.seq);
      bzero(pk.msg, sizeof(pk.msg));

      ak.seq = pk.seq;
      if (sendto(s, &ak, sizeof(ak), 0, (struct sockaddr *)&sin2, sizeof(sin2)) < 0)
      {
        perror("Client: sendto()");
        return 0;
      }
    }
    fclose(fd);
    // buffer[n] = '\0';
    // recv(s, buf, sizeof(buf), 0);

    // fputs(buf, stdout);

    close(s);
  }
}
