
// Criteria for writer.sh
// ----------------------
// Accepts the following arguments: the first argument is a full path to a file (including filename) on the filesystem, referred to below as writefile; the second argument is a text string which will be written within this file, referred to below as writestr
//
// Exits with value 1 error and print statements if any of the arguments above were not specified
//
// Creates a new file with name and path writefile with content writestr, overwriting any existing file and creating the path if it doesn’t exist. Exits with value 1 and error print statement if the file could not be created.

#include <stdio.h>
#include <syslog.h>

int main(int argc, char *argv[]) {
	openlog("writer", NULL, LOG_USER);
	// TODO: check to see if the proper number of arguments were provided
	if (argc != 3) {
		syslog(LOG_ERR, "Not enough arguments provided. Expected 3, received %d.\n", argc);
		return -1;
	}
	// Open the file (no need to create if the filepath if it doesn't exist)
	// FILE *fp = fopen(fileName, "w");
	//
	// if (fp == NULL) {
	// 	// TODO: log the error reading the file
	// 	perror("fopen failed");
	// 	return 1;
	// }

	return 0;
} 

