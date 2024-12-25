#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#define SERVER_PORT 5436
#define MAX_LINE 256

struct pack {
	int x;
	int y;
};

int main(int argc, char *argv[]) {
	int s;
	struct hostent *hp;
	struct sockaddr_in sin;
	char *host;

	char recvbuf[256];


	host = argv[1];
	hp =  gethostbyname(host);
	if (!hp) {
		printf("host not found\n");
		exit(1);
	}

	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
	sin.sin_port = htons(SERVER_PORT);


	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		exit(1);
	} else {
		printf("Socket created\n");
	}

	struct pack pk;
	memset(&pk, 0, sizeof(pk));

	if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
		printf("Not connected\n");
		close(s);
		exit(1);
	} else {
		printf("Connected\n");
	}
	int len;
	len = recv(s, recvbuf, sizeof(recvbuf), 0);
	fputs(recvbuf, stdout);

	bzero(recvbuf, sizeof(recvbuf));

	while (fgets(recvbuf, sizeof(recvbuf), stdin)) {
		send(s, recvbuf, sizeof(recvbuf), 0);
		int tp = 0;
		while ((tp = recv(s, &pk, sizeof(pk), 0)) < 0) {
			continue;
		}

		printf("%d\n", pk.x);
		printf("%d\n", pk.y);
		int ans = pk.x + pk.y;
		printf("%d\n", ans);
		pk.x = ans;
		send(s, &pk, sizeof(pk), 0);
	}
	// send(s, "Yo broo", 7 * sizeof(char), 0);
	//sleep(5);
	//send(s, "Yo broo\n", 8 * sizeof(char), 0);




	//recv(s,&num2,sizeof(num2),0);


}