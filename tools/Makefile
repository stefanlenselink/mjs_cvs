CC = gcc
prefix = /usr/local/bin

INCLUDES = -Iinclude
LIBRARY = -L/usr/local/lib
PROFILE = #-pg
LIBS =
ARCHFLAGS = -mcpu=pentium
WARNINGS = -Wall -Wbad-function-cast -Wcast-align
OPTFLAGS = -O2
# Comment this out for debugging
OPTFLAGS += -g3

CFLAGS = $(OPTFLAGS) $(PROFILE) $(WARNINGS) $(ARCHFLAGS) -DVERSION=\"$(VERSION)\"

# Uncomment these if you use FreeBSD and their weird ncurses port
#CFLAGS += -DFREEBSD_NCURSES_PORT

all: mjscount

default: all

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

mjscount: mjscount.c 
	$(CC) $(CFLAGS) $(INCLUDES) mjscount.c -o mjscount

clean:
	rm -f *~ *.o mjscount core *.core ktrace.out gmon.out DEADJOE

install: all
#	install -c -o 0 -g 0 findmp3 /usr/local/bin

mostlyclean:
	rm -f *~ core *.core *.o

# file dependencies
