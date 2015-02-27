#!/bin/bash

# echo on
set -x

#Clean
sh clean.sh

#Build
make

#Run decoding
./main test_images

#Copy files to generate histograms and quantifications
cp ./histogram/exec.sh ./test_images/extracted/standard_test_images/double_compressed/generate_histograms.sh
cp ./histogram/hist.gnu ./test_images/extracted/standard_test_images/double_compressed/hist.gnu
cp ./quantify/quantify ./test_images/extracted/standard_test_images/double_compressed/quantify
cp ./quantify/do_quantification.sh ./test_images/extracted/standard_test_images/double_compressed/do_quantification.sh

#Run histogram generation and quantification
cd test_images/extracted/standard_test_images/double_compressed
#sh generate_histograms.sh
sh do_quantification.sh

#cd ../../../..