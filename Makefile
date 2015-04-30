SRC_DIR = double_compression_detection
TEST_DIR = test_images

all: clean dcd

dcd:
	$(MAKE) -C $(SRC_DIR)

test_images: all
	$(MAKE) -C $(TEST_DIR)

run:
	$(MAKE) -C $(SRC_DIR) run

clean:
	$(MAKE) -C $(SRC_DIR) clean
	$(MAKE) -C $(TEST_DIR) clean
