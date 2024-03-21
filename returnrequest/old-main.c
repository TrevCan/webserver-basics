
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 6969
#define BUFFER_SIZE 20000

char boilerplatehttp[] = ""
"HTTP/1.0 200 OK\r\n"
"Server: miwebserver!\r\n"
"Content-type: text/html\r\n\r\n"
;

char html_start [] = ""
"<html>\r\n" ;

char html_end [] = ""
"</html>\r\n" ;

char html_code_start [] =	""
"\t<code>\r\n" ;

char html_code_end [] = ""
"\t</code>\r\n" ;

int main () {

	printf("Hello, world!\n");

	char buffer[BUFFER_SIZE];

	memset( buffer, '\0', BUFFER_SIZE);

	char responseBuffer[BUFFER_SIZE];

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if ( sockfd == -1 ) {
		perror("Error: in webserver; creating socket.");	
		return -1;
	}

	u_int yes = 1;

	if ( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes) ) == -1 ){
		perror("webserver setsockopt error");
		return -1;
	}

	printf("TCP socket created successfully!\n");

	struct sockaddr_in host_addr;
	int host_addrlen = sizeof(host_addr);

	host_addr.sin_family = AF_INET;
	// use IP v4

	host_addr.sin_port = htons(PORT);
	// convert integer to right

	host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	// use IP v4

	if ( bind(sockfd, (struct sockaddr *)&host_addr, host_addrlen) == -1 ){
		perror("webserver bind error");
		return 1;
	}

	printf("successfully bound to address \n");

	if ( listen ( sockfd, SOMAXCONN ) == -1 ) {
		perror("webserver. listen.");
		return 1;
	}

	printf("now listening for connections...\n");

	while(1){

		int newsockfd = accept(sockfd, (struct sockaddr *)&host_addr, (socklen_t *)&host_addrlen);

		if (newsockfd < 0){
			perror("webserver accept socket error");
			continue;
		}

		printf("connection accepted! \n");


		int readfd = read(newsockfd, buffer, BUFFER_SIZE);
		if (readfd == -1){
			perror("webserver. read");
			continue;
		}



		char getReqUrl[1000];
		char getPostURL[1000] = { '/0' };

		memset( getReqUrl, '\0', 1000);
		sscanf( buffer, "GET %s ", getReqUrl);		
		sscanf( buffer, "POST %s ", getPostURL);		

		//printf("HTTP GET is: \n%s\n", getReqUrl);

		if ( strstr(getReqUrl, "/head") != NULL ){
			get_response_GET_HEAD( buffer, responseBuffer);
		}
		else if (strstr( getReqUrl, "/tono") != NULL ){
			get_response_tono( buffer, responseBuffer);
		}
		else if (strstr( getPostURL, "/") != NULL ){
			get_response_POST( buffer, responseBuffer);
		}
		else{
			get_response_GET ( buffer, responseBuffer);
		}

	//	urldecode2(buffer, buffer);
		printf(";BEGIN content\n");

		printf("%s", buffer);

//		for ( int i = 0; i < 20000; i++){
//			if ( buffer[i] == '%' ) {
//				// putchar('%');
//				//printf("percent present!\n");
//				printf("%%");
//			}
//			if ( buffer[i] == '\0' ){
//				//printf("NULLCHAR\n");
//			}
//
//			if ( ! buffer[i] == '\0' )
//				printf(buffer[i]);
//		}
		printf(";END content\n");

		printf("Response buffer is: \n%d bytes long.\n", strlen(responseBuffer) );


		//int response = write(newsockfd, buffer, BUFFER_SIZE);
		int response = write(newsockfd, responseBuffer, strlen(responseBuffer));
		// int response = write(newsockfd, buffer, strlen(boilerplatehttp));

		if (response == -1 ){
			perror("webserver. response");
			continue;
		}

		responseBuffer[0] = '\0';

		close(newsockfd);

		memset( buffer, '\0', BUFFER_SIZE);
		// buffer[0] = '\0';
		// memset is necessary because the response does may not return a null byte,
		// therefore when making the strncat() we might get other information from before
		// if the response is smaller than before

	}


	//	return 0;
}


int get_response_GET_HEAD ( void *reqBuffer, void *resBuffer ) {

	strncat(resBuffer, boilerplatehttp, strlen(boilerplatehttp) );
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

	strncat(resBuffer, boilerplatehttp, strlen(boilerplatehttp) );
	strncat(resBuffer, html_start, strlen(html_start) );

	const char intro_html[] = "Hello, world!";
	strncat(resBuffer, intro_html, strlen(intro_html) );

	strncat(resBuffer, html_end, strlen(html_end) );

	return 0;


}

int get_response_tono ( void *reqBuffer, void *resBuffer ) {

	strncat(resBuffer, boilerplatehttp, strlen(boilerplatehttp) );
	strncat(resBuffer, html_start, strlen(html_start) );
	const char icon_html[] = "<head>\n"
		"<link rel=icon href=https://trevcan.duckdns.org/files/assets/tono.jpeg\n"
		" type=image/jpeg >\n"
		" </head>\n";

	const char img_html[] =	"<img src=https://trevcan.duckdns.org/files/assets/tono.jpeg  > ";
	//
	strncat(resBuffer, icon_html, strlen(icon_html) );
	strncat(resBuffer, img_html, strlen(img_html) );

	const char intro_html[] = "Hello, world!";
	strncat(resBuffer, intro_html, strlen(intro_html) );

	strncat(resBuffer, html_end, strlen(html_end) );

	return 0;


}

int get_response_POST ( void *requestBuffer, void *responseBuffer ){




	return 0;

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
