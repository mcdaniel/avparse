#
# File          : Makefile
# Description   : Build file for avparse (parser library for aviation data)
# Created       : Mon Jan  7 15:20:26 EST 2019
# By            : Patrick Mcdaniel

#
# Environment Setup

# Other parts
INCLUDES=-I.
CC=gcc
CFLAGS=-c $(INCLUDES) -g -Wall 
LINK=gcc
LINKFLAGS=-L/opt/local/lib -lfl
ARCHIVE=ar
ARCHFLAGS=cr
#
# Setup builds

LEXFILE=	avparse.l
LEXCODE=	avparse.yy.c
BISONFILE=	avparse.y
BISONCODE=	avparse.tab.c
BISONDEFS=	avparse.tab.h
LIBOBJS=	$(BISONCODE:.c=.o) \
			$(LEXCODE:.c=.o) \
			avfldparse.o
TARGETS=	avparse

# Suffix rules
.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) -o $@ $<

#
# Project Protections

all : $(TARGETS)

avparse : libavparse.a avparse.o
	$(LINK) $(LINKFLAGS) avparse.o -o $@ -lavparse

libavparse.a : $(LIBOBJS) 
	$(ARCHIVE) $(ARCHFLAGS) $@ $(LIBOBJS) 

$(BISONCODE) : $(BISONFILE)
	bison -d --debug avparse.y

$(LEXCODE) : $(LEXFILE)
	flex -o $(LEXCODE) $(LEXFILE)

clean : 
	rm -f $(TARGETS) $(OBJS) $(LEXCODE) $(BISONCODE) $(BISONDEFS)

install:
	install -C $(TARGETS) $(TARGETDIR)

include $(DEPFILE)

