#!/bin/sh
# finder script to return the number of files and number of matching lines
# first argument: path to directory on the filesystem
# second argument: text string to search the first files for
# 
# exits with return value 1 error and print statements if any parameters are not specified (param count = 2)
# exits with return value 1 error and print statements if filesdir does not represent a directory on the filesystem
# prints: The number of files are X and the number of matching lines are Y
#	where: 
#		- X is the number of files in the directory and all subdirectories
#		- Y is the number of matching lines found in respective files, where a matching
#			line refers to a line which contains **searchstr*

# Confirm the number of parametes is as expected
if [ $# != 2 ]
then 
	echo not enough parameters provided. Expected 2, received $#
	exit 1
fi

# confirm filesdir is an actual directory on filesystem
if [ ! -e $1 ]
then 
	echo provided filepath \($1\) does not exist on system
	exit 1
fi

# count the number of files in the directory provided
FILE_COUNT=$(find $1 -type f | wc -l)
LINE_COUNT=$(grep -r $2 $1 | wc -l)
# TODO: find Y
echo The number of files are $FILE_COUNT and the number of matching lines are $LINE_COUNT
