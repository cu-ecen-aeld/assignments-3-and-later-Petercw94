CROSS_COMPILE ?=

CC := $(CROSS_COMPILE)gcc
TARGET := aesdsocket
SOURCE := aesdsocket.c


all: default

default:
	$(CC) $(SOURCE) -o $(TARGET)

clean:
	rm -f $(TARGET) *.o

