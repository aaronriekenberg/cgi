CC = cc
CFLAGS = -g -Wall
LDFLAGS =

SYSCTL_HW_SRC = sysctl-hw.c sysctl.c
SYSCTL_HW_OBJS = $(SYSCTL_HW_SRC:.c=.o)

all: sysctl-hw

clean:
	rm -f *.o sysctl-hw

sysctl-hw: $(SYSCTL_HW_OBJS)
	$(CC) -static $(LDFLAGS) $(SYSCTL_HW_OBJS) -o $@

depend:
	$(CC) $(CFLAGS) -MM $(SYSCTL_HW_SRC) > .makeinclude

include .makeinclude
