CC			= cc
CFLAGS		= -g -Wall -O3

LIB_DIR		= T.835
QUANT_DIR	= quantify

COEFF_FILE	= coeffs.csv
OBJECTS		= main.o histogram.o coefficients_io.o dynamic_string_array.o
PROGRAMS	= main

default: subdefault $(PROGRAMS)

all: subdefault $(PROGRAMS)

debug: subdebug $(PROGRAMS)

main.o: main.c histogram.c coefficients_io.c dynamic_string_array.c
	$(CC) $(CFLAGS) -c main.c histogram.c coefficients_io.c dynamic_string_array.c

main: $(OBJECTS)
	$(CC) -o $@ $^
	
subdefault:
	$(MAKE) -C $(LIB_DIR)
	$(MAKE) -C $(QUANT_DIR)

subdebug:
	$(MAKE) -C $(LIB_DIR) debug

subclean:
	$(MAKE) -C $(LIB_DIR) clean
	$(MAKE) -C $(QUANT_DIR) clean

clean: subclean
	rm -f $(OBJECTS) $(PROGRAMS) $(COEFF_FILE)