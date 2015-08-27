CC = gcc
LDFLAGS = -lpthread 

CFLAGS = -I. -Wall -std=c99 -g

SRCS = onoff.c
SRCS += vtimer.c
SRCS += test_onoff.c

OBJS = $(patsubst %.c,%.o,$(SRCS)) 

all: $(OBJS)
	$(CC)  $(OBJS) -o test_onoff $(LDFLAGS)

#%.c:%.o
#	$(CC) $(CFLAGS)  $< -o $@

clean:
	rm -rf test_onoff 
	rm -rf *.o 
