
// Criteria for writer.sh
// ----------------------
// Accepts the following arguments: the first argument is a full path to a file (including filename) on the filesystem, referred to below as writefile; the second argument is a text string which will be written within this file, referred to below as writestr
//
// Exits with value 1 error and print statements if any of the arguments above were not specified
//
// Creates a new file with name and path writefile with content writestr, overwriting any existing file and creating the path if it doesn’t exist. Exits with value 1 and error print statement if the file could not be created.

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <syslog.h>

int main(int argc, char *argv[]) {
	openlog(NULL, LOG_CONS, LOG_USER);
	if (argc != 3) {
		syslog(LOG_ERR, "Not enough arguments provided. Expected 3, received %d.\n", argc);
		closelog();
		return 1;
	}
	// Open the file (no need to create if the filepath if it doesn't exist)
	int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC , S_IRUSR | S_IWUSR);

	if (fd == -1) {
		char *err;
		if (errno) {
			asprintf(&err, "error opening file: %s\n", strerror(errno));
		} else {
			err = "error opening file\n";
		}
		syslog(LOG_ERR, "%s", err);
		closelog();
		return 1;
	}
	
	ssize_t wb; // written bytes
	size_t w_count = strlen(argv[2])+1; // account for the \n character 
	char *buf;
	asprintf(&buf, "%s\n", argv[2]);
	wb = write(fd, buf, w_count);
	if (wb == -1) {
		char *err;
		if (errno) {
			asprintf(&err, "error writing to file: %s\n", strerror(errno));
		} else {
			err = "error writing to file\n";
		}
		syslog(LOG_ERR, "%s", err);
		closelog();
		return 1;
	}
	else if (wb != w_count) {
		// partial write
		syslog(LOG_ERR, "partial write performed");
		closelog();
		return 1;
	}
	syslog(LOG_INFO, "Writing %s to %s", argv[2], argv[1]);
	closelog();
	return 0;
} 

