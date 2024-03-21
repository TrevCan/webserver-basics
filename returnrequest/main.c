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

#include <ctype.h>

// local project library with datatypes
// libraries dont' workkkk idk why
//#include "http.h"
//#include "html.h"

#define PORT "9000"
#define BACKLOG 10

#define BUFFER_SIZE 20000

void sigchld_handler(int);
void handle_sigint(int);

int get_response_GET_HEAD (void*, void*);
int get_response_GET(void*, void*); 
int get_response_POST(void*, void*);
int get_response_GET_redirect(void*, void*, char*);
int get_response_GET_short(void*, void*, char*);
int get_response_POST_short(void*, void*, char*);
int startsWith(const char*, const char*);
void urldecode2(char*, const char*);

int html_generate_redirect(void *destination_buffer, int delay, char *website_url);


typedef struct node{
	char* id;
	char* data;
	struct node *next;
} node;



void* initialize_map(char *id, char* data);
void* add_to_map(node *root, char *id, char *data);
void print_map(node *root);

char http_boilerplatehttp[] = ""
"HTTP/1.0 200 OK\r\n"
"Server: theserver\r\n"
"Content-type: text/html\r\n"
"Content-Length: "
;

char html_start [] = ""
"<html>\n" ;

char html_end [] = ""
"</html>\n" ;

char html_code_start [] = ""
"\t<code>\n" ;

char html_code_end [] = ""
"\t</code>\n" ;

char html_head_start [] = ""
"<head>\n" ;

char html_head_end [] = ""
"</head>\n" ;


char html_redirect_start [] = ""
"<meta http-equiv=\"refresh\" content=\"";

char html_redirect_2 [] = ""
"; URL=";

char html_redirect_end [] = ""
"\" />";

int sockfd, newsockfd;

int idcounter = 0;

node *map;

int main(int argn, char* argv[]){

	map = add_to_map(map, "a", "abc");
	map = add_to_map(map, "b", "bce");
	print_map(map);

	signal(SIGINT, handle_sigint);

	struct sigaction sa;

	struct addrinfo hints, *res;

	struct sockaddr_storage client_addr;

	char* buffer = malloc(BUFFER_SIZE);
	memset(buffer, '\0', BUFFER_SIZE);

	char response_buffer[BUFFER_SIZE];
	memset(buffer, '\0', BUFFER_SIZE);

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


		memset(buffer, BUFFER_SIZE, '\0');
		response_buffer[0] = '\0';


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

			// manage REQUESTS here

			char getReqUrl[1000] = { '\0' };
			char getPostURL[1000] = { '\0' };

			sscanf( buffer, "GET %s ", getReqUrl);		
			sscanf( buffer, "POST %s ", getPostURL);		

			printf("HTTP GET is: \t%s\n", getReqUrl);
			printf("HTTP POST is: \t%s\n", getPostURL);

			//TODO: add host configuration
			// e.g. for host s.thetrevor.tech only activate short url services
			// and for host thetrevor.tech keep all services

			if ( startsWith(getReqUrl, "/head") == 0 ){
				get_response_GET_HEAD( buffer, response_buffer);
			}
			else if (startsWith(getPostURL, "/head") == 0 ){
				get_response_GET_HEAD( buffer, response_buffer);
			}
			else if(startsWith( getReqUrl, "/redirect") == 0 ){
				get_response_GET_redirect( buffer, response_buffer, getReqUrl);
			}	
			else if(startsWith( getReqUrl, "/short") == 0){
				get_response_GET_short(buffer, response_buffer, getReqUrl);
			}
			else if( startsWith( getPostURL, "/short") == 0){
				get_response_POST_short(buffer, response_buffer, getPostURL);
			}
			else if (startsWith( getPostURL, "/") == 0 ){
				get_response_POST( buffer, response_buffer);
			} 
			else {
				get_response_GET ( buffer, response_buffer);
			}

			int size_body = strlen(response_buffer);

			char *tmp = strdup(response_buffer);
			strcpy(response_buffer, http_boilerplatehttp);
			char content_length[4];
			sprintf(content_length, "%d\r\n\r\n", size_body);
			strcat(response_buffer, content_length);
			strcat(response_buffer, tmp);
			free(tmp);

			

	//strncat(resBuffer, http_boilerplatehttp, strlen(http_boilerplatehttp) );

			//char* response = "HTTP/1.1 200 OK\r\n\r\n<!doctype html>\r\n<html><head><title>aoijfds</title></head><body><h1>aaaa</h1></body></html>\r\n";

			if( send(newsockfd, response_buffer, strlen(response_buffer), 0) == -1){
				perror("send()");
			}
			close(newsockfd);
			exit(0);
		}
		close(newsockfd);

		//exit(0);
	}

}

// when we're told to exit... with SIGINT
void handle_sigint(int signum){
	printf("Exiting\n");
	printf("Closing socket\n");
	close(sockfd);
	exit(0);
	
}

// for forking
void sigchld_handler(int s){
	int saved_idcounter = idcounter;
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
	idcounter = saved_idcounter;
}

int get_response_GET_HEAD ( void *reqBuffer, void *resBuffer ) {

	//strncat(resBuffer, http_boilerplatehttp, strlen(http_boilerplatehttp) );
	strncat(resBuffer, html_start, strlen(html_start) );

	const char intro_html[] = "<p>Hello. You are requesting that I return the headers you send. Here they are:</p>";
	strncat(resBuffer, intro_html, strlen(intro_html) );

	strncat(resBuffer, html_code_start, strlen(html_code_start) );
	strncat(resBuffer, reqBuffer, strlen(reqBuffer) );
	strncat(resBuffer, html_code_end, strlen(html_code_end) );
	strncat(resBuffer, html_end, strlen(html_end) );

	return 0;


}

int get_response_GET ( void *reqBuffer, void *resBuffer ) {

	//strncat(resBuffer, http_boilerplatehttp, strlen(http_boilerplatehttp) );
	strncat(resBuffer, html_start, strlen(html_start) );

	const char intro_html[] = "Hello, world!";
	strncat(resBuffer, intro_html, strlen(intro_html) );

	strncat(resBuffer, html_end, strlen(html_end) );

	return 0;


}

int get_response_POST ( void *reqBuffer, void *resBuffer ){

	//strncat(resBuffer, http_boilerplatehttp, strlen(http_boilerplatehttp) );
	strncat(resBuffer, html_start, strlen(html_start) );

	const char intro_html[] = "Hello, world!";
	strncat(resBuffer, intro_html, strlen(intro_html) );

	strncat(resBuffer, html_end, strlen(html_end) );

	return 0;

}

int get_response_GET_redirect(void *reqBuffer, void *resBuffer, char* getreq_url ){
	printf("REDIRECT...\n");

	html_generate_redirect(resBuffer, 1, getreq_url + strlen("/redirect/"));

	return 0;


}


int startsWith(const char* haystack, const char* needle){
	char* found = strstr(haystack, needle);
	if(found != NULL && found == haystack){
		return 0;
	}
	return 1;
}

void urldecode2(char *dst, const char *src)
{
        char a, b;
        while (*src) {
                if ((*src == '%') &&
                    ((a = src[1]) && (b = src[2])) &&
                    (isxdigit(a) && isxdigit(b))) {
                        if (a >= 'a')
                                a -= 'a'-'A';
                        if (a >= 'A')
                                a -= ('A' - 10);
                        else
                                a -= '0';
                        if (b >= 'a')
                                b -= 'a'-'A';
                        if (b >= 'A')
                                b -= ('A' - 10);
                        else
                                b -= '0';
                        *dst++ = 16*a+b;
                        src+=3;
                } else if (*src == '+') {
                        *dst++ = ' ';
                        src++;
                } else {
                        *dst++ = *src++;
                }
        }
        *dst++ = '\0';
}

// delay_time can be up to 3 digits long.
int html_generate_redirect(void *destination_buffer, int delay_time, char *website_url){

	strncat(destination_buffer, html_start, strlen(html_start));
	strncat(destination_buffer, html_head_start, strlen(html_head_start));
	strncat(destination_buffer, html_redirect_start, strlen(html_redirect_start) );

	char delay[4];
	sprintf(delay, "%d", delay_time);
	strncat(destination_buffer, delay, strlen(delay));
	strncat(destination_buffer, html_redirect_2, strlen(html_redirect_2));
	strncat(destination_buffer, website_url, strlen(website_url));
	strncat(destination_buffer, html_redirect_end, strlen(html_redirect_end));
	strncat(destination_buffer, html_head_end, strlen(html_head_end));
	strncat(destination_buffer, html_end, strlen(html_end));

	//printf("res is:\n%s\n", destination_buffer);

	return 0;
}

int get_response_GET_short(void *requestBuffer, void *responseBuffer, char *path){
	print_map(map);
	return 0;
}

int get_response_POST_short(void *requestBuffer, void *responseBuffer, char *path){
	idcounter = idcounter + 1;
	printf("bababdba\n");

	char *url_value = strcasestr( path, "http://");		
	char *url_value2 = strcasestr( path, "https://");		

	printf("IDD: %d\n", idcounter);

	char s[11];
	sprintf(s, "%d", idcounter);

	if(url_value == NULL && url_value2 != NULL){
		map = add_to_map(map, s, url_value2);	
		idcounter = idcounter + 1;
	} else if (url_value != NULL){
		map = add_to_map(map, s, url_value);
		idcounter = idcounter + 1;
	}

	printf("UUUUUUUUUUUU\n");
	printf("url value is: \t%s\n", url_value);

	print_map(map);

	return 0;
}

void print_map(node *root){
	while(root != NULL){
		printf("%s, %s\n", root->id, root->data);
		root = root->next;
	}

}

void* initialize_map(char *id, char* data){
	node *root = malloc(sizeof(node));
	root->id = strndup(id, 11);
	
	root->data = strndup( data, 51);
	root->next = NULL;

	return root;
}

void* add_to_map(node *root, char *id, char* data){
	if(root == NULL){
		return initialize_map(id, data);
	} else{
		node *new_node = malloc(sizeof(node));
		new_node->id = strndup(id, 11);

		new_node->data = strndup(data, 51);
		new_node->next = NULL;

		root->next = new_node;

		return root;
	}
}
