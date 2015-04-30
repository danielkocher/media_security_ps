CC			= cc
CFLAGS		= -g -Wall -O3

LIB_DIR		= T.835
QUANT_DIR	= quantify
IMAGES_DIR	= test_images
COMPR_DIR	= extracted/compressed
HIST_DIR	= histogram

COEFF_FILE	= coeffs.csv
HIST_GNU	= hist.gnu
HIST_SH		= generate_histograms.sh
QUANT_BIN	= quantify
QUANT_SH	= do_quantification.sh

OBJECTS		= main.o histogram.o coefficients_io.o dynamic_string_array.o
PROGRAMS	= main

default: subdefault $(PROGRAMS)

all: subdefault $(PROGRAMS)

debug: subdebug $(PROGRAMS)

images: all
	$(MAKE) -C $(IMAGES_DIR)

run: all
	./$(PROGRAMS) $(IMAGES_DIR)/$(COMPR_DIR)

	#Copy files to generate histograms and quantifications
	cp $(HIST_DIR)/$(HIST_SH) $(IMAGES_DIR)/$(COMPR_DIR)/$(HIST_SH)
	cp $(HIST_DIR)/$(HIST_GNU) $(IMAGES_DIR)/$(COMPR_DIR)/$(HIST_GNU)
	cp $(QUANT_DIR)/$(QUANT_BIN) $(IMAGES_DIR)/$(COMPR_DIR)/$(QUANT_BIN)
	cp $(QUANT_DIR)/$(QUANT_SH) $(IMAGES_DIR)/$(COMPR_DIR)/$(QUANT_SH)

	cd $(IMAGES_DIR)/$(COMPR_DIR) && sh $(QUANT_SH) # subshell
	cd $(IMAGES_DIR)/$(COMPR_DIR) && sh $(HIST_SH) # subshell

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
	$(MAKE) -C $(IMAGES_DIR) clean

clean: subclean
	rm -f $(OBJECTS) $(PROGRAMS) $(COEFF_FILE)
	# clean all tif, csv, dat, png, gnu, sh and quantify files from the test_images dir
	#find $(IMAGES_DIR)/$(COMPR_DIR) -name "*.tif" -o -name "*.csv" -o -name "*.dat" -o -name "*.png" -o -name "*.gnu" -o -name "*.sh" -o -name "quantify" -type f | xargs rm -f