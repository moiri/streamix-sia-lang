SHELL := /bin/bash
PROJECT = sia2graph

LOC_INC_DIR = include
LOC_SRC_DIR = src

SOURCES = main.c \
		  $(LOC_SRC_DIR)/*

INCLUDES = $(LOC_INC_DIR)/*

INCLUDES_DIR = -Iuthash/src \
			   -I/usr/local/include/igraph \
			   -I$(LOC_INC_DIR) \
			   -I.
LINK_DIR = -L/usr/local/lib
LINK_FILE = -lfl \
			-ligraph

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
$(PROJECT): lex.yy.c $(PROJECT).tab.c $(PROJECT).tab.h $(SOURCES) $(INCLUDES) $(INSTTAB_OBJ)
	$(CC) $(CFLAGS) $(SOURCES) $(PROJECT).tab.c lex.yy.c $(INCLUDES_DIR) $(LINK_DIR) $(LINK_FILE) -o $(PROJECT)

# compile lexer (flex)
lex.yy.c: $(PROJECT).lex $(PROJECT).tab.h
	flex $(PROJECT).lex

# compile parser (bison)
$(PROJECT).tab.c $(PROJECT).tab.h: $(PROJECT).y
	bison $(BFLAGS) $(PROJECT).y

.PHONY: clean install

clean:
	rm -f $(PROJECT).tab.c
	rm -f $(PROJECT).tab.h
	rm -f $(PROJECT).output
	rm -f $(PROJECT)
	rm -f lex.yy.c
	rm -f *.graphml
	rm -f *.gml

install:
	mkdir -p /usr/local/bin
	cp -a $(PROJECT) /usr/local/bin/.
