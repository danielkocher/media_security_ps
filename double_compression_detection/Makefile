CC			= cc
CFLAGS		= -Wall -O3

LIB_DIR		= T.835

COEFF_FILE	= coeffs.csv
OBJECTS		= main.o
PROGRAMS	= main

default: subdefault $(PROGRAMS)

all: subdefault $(PROGRAMS)

debug: subdebug $(PROGRAMS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

main: main.o
	$(CC) -o $@ $^
	
subdefault:
	$(MAKE) -C $(LIB_DIR)

subdebug:
	$(MAKE) -C $(LIB_DIR) debug

subclean:
	$(MAKE) -C $(LIB_DIR) clean

clean: subclean
	rm -f $(OBJECTS) $(PROGRAMS) $(COEFF_FILE)