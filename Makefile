CC = cc
CFLAGS = -g -Wall
LDFLAGS =

SYSCTL_CGI_SRC = sysctl-cgi.c sysctl.c
SYSCTL_CGI_OBJS = $(SYSCTL_CGI_SRC:.c=.o)

all: sysctl-cgi

clean:
	rm -f *.o sysctl-cgi

sysctl-cgi: $(SYSCTL_CGI_OBJS)
	$(CC) -static $(LDFLAGS) $(SYSCTL_CGI_OBJS) -o $@

depend:
	$(CC) $(CFLAGS) -MM $(SYSCTL_CGI_SRC) > .makeinclude

include .makeinclude
