#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <signal.h>

#define PORT 6000

void handle_sigint(int);

int main(int argv, char** argn){

	signal(SIGINT, handle_sigint);

	// print arguments
	for(int i = 0; i < argv; i++){
		printf("%s\n", argn[i]);

	}

	//create a TCP socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1){
		perror("webserver (socket creation)");
		return -1;
	}

	u_int yes = 1;

	if( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes) ) == -1 ){
		perror("webserver (set socket options)" );
		return -2;
	}

	struct sockaddr_in host_addr;
	int host_addrlen = sizeof(host_addr);

	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(PORT);
	host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if( bind(sockfd, (struct sockaddr *)&host_addr, host_addrlen) != 0 ){
		perror("webserver (bind socket to address)");
		return -3;
	}

	while(1){
		int newsockfd = accept(sockfd, (struct sockaddr *)&host_addr, (socklen_t *)&host_addrlen);

		if(newsockfd < 0){
			perror("webserver (accept connection)");
			continue;
		}

		printf("New connection accepted\n");

		char* buffer = malloc(1000);
		memset(buffer, 1000, '\0');

		int readfd = read(newsockfd, buffer, sizeof(buffer));
		if(readfd == -1){
			perror("webserver (read)");
			continue;
		}

		printf("FROM CLIENT:\n");
		for(int i = 0; i < readfd; i++){
			putchar(buffer[i]);
		}
		printf("END CLIENT\n");
		close(newsockfd);
		exit(0);
	}



	return 0;

}

void handle_sigint(int signum){
	printf("Exiting...a\n");
	exit(0);
	
}
