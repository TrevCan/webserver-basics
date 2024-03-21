/*
 * af
 * int -- socket descriptor
 * struct addrinfo {
    int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
    int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
    int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
    int              ai_protocol;  // use 0 for "any"
    size_t           ai_addrlen;   // size of ai_addr in bytes
    struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
    char            *ai_canonname; // full canonical hostname

    struct addrinfo *ai_next;      // linked list, next node
};
 * struct sockaddr {
    unsigned short    sa_family;    // address family, AF_xxx
    char              sa_data[14];  // 14 bytes of protocol address
}; 
 * 
 * // (IPv4 only--see struct sockaddr_in6 for IPv6)

struct sockaddr_in {
    short int          sin_family;  // Address family, AF_INET
    unsigned short int sin_port;    // Port number
    struct in_addr     sin_addr;    // Internet address
    unsigned char      sin_zero[8]; // Same size as struct sockaddr
};
 * sockaddr_in -- for IPv4
 * sockaddr_in6 -- for IPv6
 * *sockaddr_in and *sockaddr are castable pointers
 *
 *
 *
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main(int argn, char** argv){

	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;

	memset(&hints, 0, sizeof hints); 	// make empty
	hints.ai_family = AF_UNSPEC;		// ipv6 or ipv4
	hints.ai_socktype = SOCK_STREAM;	// tcp only!
	hints.ai_flags = AI_PASSIVE;		// fill my ip
	
	if ( (status = getaddrinfo(NULL, "9000", &hints, &servinfo)) != 0){
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
	}



	// servinfo is now a linked list to 1 or more struct addrinfos

	freeaddrinfo(servinfo);



}
