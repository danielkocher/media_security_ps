#!/bin/bash

# echo on
set -x

find test_images -name "*.tif" -o -name "*.csv" -o -name "*.dat" -o -name "*.png" -o -name "*.gnu" -o -name "*.sh" -o -name "quantify" -type f | xargs rm -f