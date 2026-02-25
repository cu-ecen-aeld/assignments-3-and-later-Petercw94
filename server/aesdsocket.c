#include <errno.h>
#include <signal.h>
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
#include <unistd.h>

#define PORT "9000"
#define BACKLOG 10
#define BUFMAXLINE 4096

volatile sig_atomic_t keep_running = 1;

void signal_handler(int signo) {
	keep_running = 0;
}

int main(int argc, char *argv[]) {
	openlog("server.c", LOG_PID, LOG_USER);
	syslog(LOG_INFO, "starting server program");
	
	// register signal handler
	struct sigaction sa;
	// clear out the initialized struct
	memset(&sa, 0, sizeof(struct sigaction));
	sa.sa_handler = signal_handler;
	sa.sa_flags = 0;

	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);



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
	if ((status = listen(sockfd, BACKLOG)) != 0) {
		fprintf(stderr, "error on listen call");
		return -1;
	}

	while (keep_running) {

		addr_size = sizeof client_addr;
		if ((clientsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size)) == -1) {
			if (errno == EINTR) {
				// received signal, time to cleanup
				break;
			} else {
				syslog(LOG_ERR, "error during accept");
				continue;
			}
		} 
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
				close(clientsockfd);
				if (errno == EINTR) {
					break;
				} 
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
	
				status = fwrite(buf, sizeof *buf, bytes_read, fp);
				// TODO: check status for write failure
				fclose(fp);

			} else {
				// just write until the new line
				// THE READ NEEDS TO BE TERMINATED HERE
				// NOTE: subtracting two pointers gives an index as long as the two pointers are referencing the same array
				FILE *fp = fopen("/var/tmp/aesdsocketdata", "a+");
				size_t loc = new_line_loc - buf;
				loc += 1;
				status = fwrite(buf, sizeof *buf, loc, fp);
				// TODO: check status for write failure
				// Close the file then reopen for reading to return the pointer to the start of the file
				fclose(fp);
				fp = fopen("/var/tmp/aesdsocketdata", "r");
				// TODO: handle errors on the write 
				// TODO: handle incomplete writes
				// TODO: write back the full file
				
				// Send the temp file back to the client once all reading is done
				// Can't assume the entire file will fit into memory, so need to buf the read
				// while writing takes place.
				char read_buf[8192];
				for (;;) {
					ssize_t n = fread(read_buf, 1, sizeof read_buf, fp);
					if (n == 0) break; // done reading
					if (n < 0) { /* TODO: handle error here */}

					size_t off = 0;
					while (off < (size_t)n) {
						ssize_t s = send(clientsockfd, read_buf+off, n - off,0);
						if (s < 0){ /* TODO: handle error here */ }
						off += s; 
					}
				}

				fclose(fp);
				break; // assume the packet is done once a new line has been received
			}

		}
				
		syslog(LOG_INFO, "Closed connection from: %s", inet_ntop(client_addr.ss_family, addr, ipstr, sizeof ipstr));
		close(clientsockfd);
		

		// check if an interrupt has been received
		if (keep_running == 0) break;
	}



	syslog(LOG_INFO, "Caught signal, exiting");
	close(sockfd);
	unlink("/var/tmp/aesdsocketdata");
	freeaddrinfo(res);
	closelog();
	return 0;
		
}
