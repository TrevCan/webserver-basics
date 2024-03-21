#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT "9000"
#define BACKLOG 10

#define BUFFER_SIZE 10000

void sigchld_handler(int);

int main(int argn, char* argv[]){

	struct sigaction sa;

	struct addrinfo hints, *res;
	int sockfd, newsockfd;

	struct sockaddr_storage client_addr;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ( getaddrinfo(NULL, PORT, &hints, &res) != 0 ){
		fprintf(stderr, "ERROR GETADDRINFO\n");
		return 1;
	}

	/* prints available addresses
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
	*/

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

	listen(sockfd, BACKLOG);

	socklen_t sin_size = sizeof(client_addr);

	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if(sigaction(SIGCHLD, &sa, NULL) == -1){
		perror("sigaction");
		exit(1);
	}

	while(1){
	//	client_addr;
		newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);

		if(newsockfd == -1){
			//perror("webserver (accept connection)");
			perror("accept");
			continue;
		}

		printf("New connection accepted\n");


		char* buffer = malloc(BUFFER_SIZE);
		memset(buffer, BUFFER_SIZE, '\0');


		if(!fork()){
			close(sockfd);
			int readfd = recv(newsockfd, buffer, BUFFER_SIZE, 0);
			if(readfd == -1){
				perror("webserver (read)");
				continue;
			}

			printf("FROM CLIENT:\n");
			for(int i = 0; i < readfd; i++){
				putchar(buffer[i]);
			}
			printf("END CLIENT\n\n");

			char* response = "HTTP/1.1 200 OK\r\n\r\n<!doctype html>\r\n<html><head><title>aoijfds</title></head><body><h1>aaaa</h1></body></html>\r\n";

			if( send(newsockfd, response, strlen(response), 0) == -1){
				perror("send()");
			}
			close(newsockfd);
			exit(0);
		}
		close(newsockfd);

		//exit(0);
	}


	
}

void sigchld_handler(int s){
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}
