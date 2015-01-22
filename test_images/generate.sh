#!/bin/bash

FOLDER_BASE="extracted/standard_test_images"
FOLDER_ORIGINAL="${FOLDER_BASE}/original/*.tif"
FOLDER_DOUBLE_COMPRESSED="${FOLDER_BASE}/double_compressed"
PATH_JPEGXR="../double_compression_detection/T.835/jpegxr"

for f in $FOLDER_ORIGINAL
do
	for i in $@
	do
		fn=$(basename "$f")
		fn="${fn%.*}"
		fntmpjxr="$fn-$i.jxr"
		fntmptif="$fn-$i.tif"

		mkdir "${FOLDER_DOUBLE_COMPRESSED}/$fn-$i" && \
		${PATH_JPEGXR} -c -o "${FOLDER_DOUBLE_COMPRESSED}/$fn-$i/$fntmpjxr" -q $i "$f" && \
		${PATH_JPEGXR} -o "${FOLDER_DOUBLE_COMPRESSED}/$fn-$i/$fntmptif" "${FOLDER_DOUBLE_COMPRESSED}/$fn-$i/$fntmpjxr"

		echo "Single-compressed file $f using quantisation $i. Result file: $fntmpjxr" && \
		rm -f "${FOLDER_DOUBLE_COMPRESSED}/$fn-$i/$fntmptif" "${FOLDER_DOUBLE_COMPRESSED}$fn-$i/coeffs.csv"

		for j in $@
		do
			if [ $i -ne $j ]
			then
				fnfinaljxr="$fn-$i-$j.jxr"
				
				mkdir "${FOLDER_DOUBLE_COMPRESSED}/$fn-$i-$j" && \
				${PATH_JPEGXR} -o "${FOLDER_DOUBLE_COMPRESSED}/$fn-$i-$j/$fntmptif" "${FOLDER_DOUBLE_COMPRESSED}/$fn-$i/$fntmpjxr" && \
				${PATH_JPEGXR} -c -o "${FOLDER_DOUBLE_COMPRESSED}/$fn-$i-$j/$fnfinaljxr" -q $j "${FOLDER_DOUBLE_COMPRESSED}/$fn-$i-$j/$fntmptif"

				#touch $fntmpjxr $fntmptif $fnfinaljxr
				echo "Double-compressed file $f using quantisation combination ($i, $j). Result file: $fnfinaljxr" && \
				rm -f "${FOLDER_DOUBLE_COMPRESSED}/$fn-$i-$j/$fntmptif" "${FOLDER_DOUBLE_COMPRESSED}$fn-$i-$j/coeffs.csv"
			fi
		done
	done
done