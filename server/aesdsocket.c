#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/syslog.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <syslog.h>

#define PORT "9000"
#define BACKLOG 10
#define BUFMAXLINE 4096

int main(int argc, char *argv[]) {
	openlog("server.c", LOG_PID, LOG_USER);
	syslog(LOG_INFO, "starting server program");
	struct addrinfo hints, *res, *p;	
	struct sockaddr_in *peer;
	int status, sockfd, clientsockfd;
	char ipstr[INET6_ADDRSTRLEN];
	struct sockaddr_storage client_addr;
	socklen_t addr_size, peer_size;

	// buffer variables
	char buf[BUFMAXLINE];
	size_t buf_size = sizeof buf;
	int bytes_read;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo("127.0.0.1", PORT, &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return -1;
	}

	
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	// bind
	if ((status = bind(sockfd, res->ai_addr, res->ai_addrlen)) != 0) {
		fprintf(stderr, "error binding to socket\n");
		return -1;
	}
	
		
	// Listens for and accepts a connection
	
	// ready to communicate on socket descriptor clientsockfd
	// log message to syslog 
	
	
	
	// TODO: Receives data over the connection and appends to file /var/tmp/aesdsocketdata, creating this file if it doesn't exist.
	for (;;) {
		if ((status = listen(sockfd, BACKLOG)) != 0) {
			fprintf(stderr, "error on listen call");
			return -1;
		}

		addr_size = sizeof client_addr;
		clientsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);
		// Logs message to the syslog "Accepted connection from xxxx" where XXXX is the IP address of the connected client
		
		// get the ip address (family agnostic)
		void *addr;
		if (client_addr.ss_family == AF_INET) {
			// ipv4
			struct sockaddr_in *s = (struct sockaddr_in *) &client_addr;
			addr = &s->sin_addr;
		} else {
			struct sockaddr_in6 *s = (struct sockaddr_in6 *) &client_addr;
			addr = &s->sin6_addr;
		}
		
		syslog(LOG_INFO, "Accepted connection from: %s", inet_ntop(client_addr.ss_family, addr, ipstr, sizeof ipstr));
		// read from connection
		for (;;) {
			bytes_read = recv(clientsockfd, buf, buf_size, 0);
			if (bytes_read == -1) { // error occured in read
				syslog(LOG_ERR, "error recv");
				return -1;
			} 
			if (bytes_read == 0) { // client closed connection
				// break out of the read
				syslog(LOG_INFO, "client closed the connection");
				break;
			} 

			// at this point the buffer is full from the read
			char *new_line_loc = memchr(buf, '\n', bytes_read);
			if (new_line_loc == NULL) {
				// write the whole buffer to the file and keep reading from the socket
				FILE *fp = fopen("/var/tmp/aesdsocketdata", "a+");
				if (fp == NULL) {
					// File failed to open
					syslog(LOG_ERR, "failed to open file /var/tmp/aesdsocketdata");
					return -1;
				}
	
				fputs(buf, fp);

			} else {
				// just write until the new line
				// THE READ NEEDS TO BE TERMINATED HERE
			}
			// NOTE: subtracting two pointers gives an index as long as the two pointers are referencing the same array
			size_t loc = new_line_loc - buf;

			//syslog(LOG_INFO, "new line located at: %d", (int) loc);

		}
				
		

	}



	freeaddrinfo(res);
	closelog();
	return 0;
		
	

	
	
	
	// 	TODO: Use a \n to separate data packets received (each new line found in the stream read should indicated the end of the stream and an append to the data file) hint: recv returns 0 when the client closes the connection, not sure the client will close the connection in this case or if its assumed that the server will need to handle closing the connection
	// 	TODO: assume the data stream does not include null characters (can be processed using string handling functions)
	// 	TODO: assume the length of the packet will be shorter than the available heap size (as long as you handle malloc() associated failures with error messages you may discard associated over-length packets)
	
	// TODO: Returns the full content of /var/tmp/aesdsocketdata to the client as soon as the received data packet completes
	// 	TODO: assume the total size of hte packets sent will be less than the size of hte root filesystem, however you may NOT assume this total size of all packets sent will be less than the size of the available RAM for the process heap
	
	// TODO: Logs message to the syslog "Closed connection from xxxx" where xxxx is the IP address of the connected client
	
	// TODO: restarts accepting connections from new clients forever in a loop until SIGINT or SIGTERM is received
	
	// TODO: Gracefully exits when SIGINT or SIGTERM is received, completing any open connection operations, closing any open sockets, and deleting the file /var/tmp/aesdsocketdata
	// 	TODO: Logs message to the syslog "Caught signal, exiting" when SIGINT or SIGTERM is received
	return 0;
}
