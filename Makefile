# Makefile for B+tree project.

MAIN=btree

# you need to change the below path

MINIBASE = /Users/jasonbraley/Desktop/Btree

CC=g++

CFLAGS= -DUNIX -Wall -g

SRCS = bt_builder.cpp \
	bt_main.cpp\
	bt_driver.cpp\
	bt_node.cpp\
	bt_index.cpp\
	bt_leaf.cpp\
	bt_scan.cpp
	

all: $(MAIN)
	

OBJS = $(SRCS:.cpp=.o)

$(MAIN):  $(OBJS)
	 $(CC) $(CFLAGS) $(OBJS) -o $(MAIN) 

clean:
	rm -f *.o *~ $(MAIN)