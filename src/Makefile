# Makefile: EMERGE

CC = gcc

CFLAGS = -g -O0 -lm

INC = -I ../inc

OBJS = main.o \
	transforms.o

TARGET = EMERGE

all: $(TARGET)

$(TARGET):$(OBJS)
	$(CC) $(INC) $(CFLAGS) -o $@ $^

.c.o:
	$(CC) $(INC) $(CFLAGS) -c -o $@ $<

clean: 
	rm -rf $(TARGET) $(OBJS)
