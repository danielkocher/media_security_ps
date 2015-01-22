#!/bin/bash

# echo on
set -x

sh clean.sh
make
./main test_images
cd test_images/extracted/standard_test_images/double_compressed
sh generate_histograms.sh
#cd ../../../..