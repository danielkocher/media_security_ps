#!/bin/bash

# echo on
set -x

sh clean.sh
make
./main
cd test_images/extracted/standard_test_images/double_compressed
sh generate_histograms.sh
#cd ../../../..