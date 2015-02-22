#!/bin/bash

# echo on
set -x

#Clean
sh clean.sh

#Build & Prepare
make
cd histogram
cp exec.sh ../test_images/extracted/standard_test_images/double_compressed/generate_histograms.sh
cd ..
cd quantify
make
cp quantify ../test_images/extracted/standard_test_images/double_compressed/quantify
cd ..

#Run
./main test_images
cd test_images/extracted/standard_test_images/double_compressed
#sh generate_histograms.sh
sh do_quantification.sh
#cd ../../../..