#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>

#define SERVER_PORT 5432
#define BUF_SIZE 4096

clock_t start, end;
double time_taken;

int main(int argc, char *argv[])
{

    FILE *fp;
    struct hostent *hp;
    struct sockaddr_in sin;

    char *host;
    char buf[BUF_SIZE];
    int s;
    int len;
    socklen_t slen;
    int n;

    if ((argc == 2) || (argc == 3))
    {
        host = argv[1];
    }
    else
    {
        fprintf(stderr, "usage: client serverIP [download_filename(optional)]\n");
        exit(1);
    }

    if (argc == 3)
    {
        fp = fopen(argv[2], "w");

        if (fp == NULL)
        {
            fprintf(stderr, "Error opening output file\n");
            exit(1);
        }
    }

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
    sin.sin_family = AF_INET;
    memcpy((char *)&sin.sin_addr, hp->h_addr, hp->h_length);
    sin.sin_port = htons(SERVER_PORT);

    /* create socket */
    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("client: socket");
        exit(1);
    }

    printf("Client will get data from to %s:%d.\n", argv[1], SERVER_PORT);
    printf("To play the music, pipe the downlaod file to a player, e.g., ALSA, SOX, VLC: cat recvd_file.wav | vlc -\n");

    /* send message to server */
    fgets(buf, sizeof(buf), stdin);
    buf[BUF_SIZE - 1] = '\0';
    len = strlen(buf) + 1;

    if (sendto(s, buf, len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
        perror("Client: sendto()");
        return 0;
    }
    start = clock();
    bzero(buf, sizeof(buf));
    FILE *fp2 = fopen("bigout.mp4", "wb");

    while (1)
    {
        n = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&sin, &slen);

        if (strcmp(buf, "BYE") == 0)
        {
            // memset(buf, 0, sizeof(buf));
            bzero(buf, sizeof(buf));
            break;
        }

        fwrite(buf, 1, n, fp2);
        bzero(buf, sizeof(buf));
        // memset(buf, 0, sizeof(buf));
    }
    fclose(fp2);
    end = clock();

    time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("%lf", time_taken);
    close(s);
}