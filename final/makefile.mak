CC = gcc
CFLAGS = -pthread -lwiringPi

SRCS = main.c adxl345.c checkPW.c
HEADERS = adxl345.h checkPW.h

OBJS = $(SRCS:.c=.o)
TARGET = main

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)
