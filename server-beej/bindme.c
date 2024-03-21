#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argn, char* argv[]){

	struct addrinfo hints, *res;
	int sockfd;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ( getaddrinfo(NULL, "9000", &hints, &res) != 0 ){
		fprintf(stderr, "ERROR GETADDRINFO\n");
		return 1;
	}

	char ipstr[INET6_ADDRSTRLEN];
	struct addrinfo *p;

	for(p = res; p != NULL; p = p->ai_next){
		void *addr;
		char *ipver;

		if(p->ai_family == AF_INET){
			struct sockaddr_in *ipv4 = (struct sockaddr_in*)p->ai_addr;
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";

		} else {
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in*)p->ai_addr;
			addr = &(ipv6->sin6_addr);
			ipver = "IPv6";
		}

		inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
		printf("\t%s: %s\n", ipver, ipstr);
	}

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);



	if(sockfd < 0){
		fprintf(stderr, "ERROR with socket\n");
		return 2;
	}

	int yes = 1;

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
	    perror("setsockopt");
	    exit(5);
	}

	int bind_res = 0;
	if ( (bind_res = bind(sockfd, res->ai_addr, res->ai_addrlen)) != 0 ){
		fprintf(stderr, "ERROR binding\n");
		perror("bind()");
		return 3;
	}

	printf("bind is: %d\n", bind_res);
	
}
