DCD_DIR = double_compression_detection

all: clean dcd

dcd:
	$(MAKE) -C $(DCD_DIR)

images:
	$(MAKE) -C $(DCD_DIR) images

run:
	$(MAKE) -C $(DCD_DIR) run

clean:
	$(MAKE) -C $(DCD_DIR) clean
