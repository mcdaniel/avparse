#
# File          : Makefile
# Description   : Build file for avparse (parser library for aviation data)
# Created       : Mon Jan  7 15:20:26 EST 2019
# By            : Patrick Mcdaniel

#
# Environment Setup

# Other parts
CC=gcc
CFLAGS=-c $(INCLUDES) -g -Wall 
LINK=gcc
LINKFLAGS=-L/opt/local/lib

#
# Setup builds

LEXFILE=	avparse.l
LEXCODE=	avparse.yy.c
OBJS=		avparse.yy.o
TARGETS=	avparse

#
# Project Protections

all : $(TARGETS)

avparse: $(OBJS)
	$(LINK) $(LINKFLAGS) -o $@ $(OBJS) -lfl

$(LEXCODE) : $(LEXFILE)
	flex -o $(LEXCODE) $(LEXFILE)


clean : 
	rm -f $(TARGETS) $(OBJS) $(LEXCODE)

install:
	install -C $(TARGETS) $(TARGETDIR)

include $(DEPFILE)

