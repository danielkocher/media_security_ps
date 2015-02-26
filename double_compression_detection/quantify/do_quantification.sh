#!/bin/bash

for dir in */ ; do
	./quantify $dir
done