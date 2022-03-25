SHELL := /bin/bash
PROJECT = sia2graph

LOC_INC_DIR = include
LOC_SRC_DIR = src

SOURCES = main.c \
		  $(LOC_SRC_DIR)/* \
		  $(PROJECT).tab.c \
		  lex.zz.c

INCLUDES = $(LOC_INC_DIR)/* \
		   $(PROJECT).tab.h

INCLUDES_DIR = -Iuthash/src \
			   -I/usr/include/igraph \
			   -I$(LOC_INC_DIR) \
			   -I.
LINK_DIR = -L/usr/local/lib
LINK_FILE = -ligraph

CFLAGS = -Wall
DEBUG_FLAGS = -g -O0
BFLAGS = -d -Wall
BDEBUG_FLAGS = --verbose

CC = gcc

all: $(PROJECT)

# compile with dot stuff and debug flags
debug: CFLAGS += $(DEBUG_FLAGS)
debug: BFLAGS += $(BDEBUG_FLAGS)
debug: $(PROJECT)

# compile project
$(PROJECT): $(SOURCES) $(INCLUDES) $(INSTTAB_OBJ)
	$(CC) $(CFLAGS) $(SOURCES) $(INCLUDES_DIR) $(LINK_DIR) $(LINK_FILE) -o $(PROJECT)

# compile lexer (flex)
lex.zz.c: $(PROJECT).lex $(PROJECT).tab.h
	flex $(PROJECT).lex

# compile parser (bison)
$(PROJECT).tab.c $(PROJECT).tab.h: $(PROJECT).y
	bison $(BFLAGS) $(PROJECT).y

.PHONY: clean install valgrind

clean:
	rm -f $(PROJECT).tab.c
	rm -f $(PROJECT).tab.h
	rm -f $(PROJECT).output
	rm -f $(PROJECT)
	rm -f lex.zz.c
	rm -f *.graphml
	rm -f *.gml

install:
	mkdir -p /usr/local/bin
	cp -a $(PROJECT) /usr/local/bin/.

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -v ./$(PROJECT)
