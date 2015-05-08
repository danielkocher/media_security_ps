CC			= cc
CFLAGS		= -g -Wall -O3

LIB_DIR		= T.835
IMAGES_DIR	= test_images
COMPR_DIR	= extracted/compressed
COEFF_FILE	= coeffs.csv

HIST_DIR	= histogram
HIST_GNU	= hist.gnu
HIST_SH		= generate_histograms.sh

QUANT_DIR	= quantify
QUANT_BIN	= quantify
QUANT_SH	= do_quantification.sh

CLUSTER_DIR = $(QUANT_DIR)/cluster
CLUSTER_GNU	= plot_clustered_quant.gnu
CLUSTER_PY	= cluster_quantifications.py
CLUSTER_SH	= plot_clustered_quant.sh

OBJECTS		= main.o histogram.o coefficients_io.o dynamic_string_array.o
PROGRAMS	= main

.PHONY: histograms quantifications clustering

default: subdefault $(PROGRAMS) histograms quantifications clustering

all: default

debug: subdebug $(PROGRAMS)

images: all
	$(MAKE) -C $(IMAGES_DIR)

run: all
	./$(PROGRAMS) $(IMAGES_DIR)/$(COMPR_DIR)

histograms:
	#Copy files to generate histograms and quantifications
	cp $(HIST_DIR)/$(HIST_SH) $(IMAGES_DIR)/$(COMPR_DIR)/$(HIST_SH)
	cp $(HIST_DIR)/$(HIST_GNU) $(IMAGES_DIR)/$(COMPR_DIR)/$(HIST_GNU)
	
	# this is executed in a subshell, thus the 'cd'
	cd $(IMAGES_DIR)/$(COMPR_DIR) && sh $(HIST_SH)

quantifications:
	#Copy files to generate histograms and quantifications
	cp $(QUANT_DIR)/$(QUANT_BIN) $(IMAGES_DIR)/$(COMPR_DIR)/$(QUANT_BIN)
	cp $(QUANT_DIR)/$(QUANT_SH) $(IMAGES_DIR)/$(COMPR_DIR)/$(QUANT_SH)
	
	# this is executed in a subshell, thus the 'cd'
	cd $(IMAGES_DIR)/$(COMPR_DIR) && sh $(QUANT_SH)

clustering:
	#Copy files to generate histograms and quantifications
	cp $(CLUSTER_DIR)/$(CLUSTER_GNU) $(IMAGES_DIR)/$(COMPR_DIR)/$(CLUSTER_GNU)
	cp $(CLUSTER_DIR)/$(CLUSTER_SH) $(IMAGES_DIR)/$(COMPR_DIR)/$(CLUSTER_SH)
	cp $(CLUSTER_DIR)/$(CLUSTER_PY) $(IMAGES_DIR)/$(COMPR_DIR)/$(CLUSTER_PY)

	# this is executed in a subshell, thus the 'cd'
	cd $(IMAGES_DIR)/$(COMPR_DIR) && python $(CLUSTER_PY) && sh $(CLUSTER_SH)

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