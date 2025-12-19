# default to not cross compile unless specified
CROSS_COMPILE ?=

CC := $(CROSS_COMPILE)gcc
TARGET := writer
SOURCE := writer.c

all: default

default:
	$(CC) $(SOURCE) -o $(TARGET)

clean:
	rm -f $(TARGET) *.o
