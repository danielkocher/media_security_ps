#!/bin/bash

# echo on
set -x

find . -name "*.tif" -o -name "*.csv" -o -name "*.dat" -o -name "*.png" -type f | xargs rm -f