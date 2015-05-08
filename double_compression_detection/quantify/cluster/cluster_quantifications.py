import sys
import os
import subprocess
import re
import collections
from sets import Set

# get paths to all .dat files to be processed
p = subprocess.Popen("find . -name \"quant.dat\"", stdout=subprocess.PIPE, shell=True)
p.wait()
quantFilepaths = p.stdout.readlines()

# read quantification data for all images in one big dictionary with the following mappings (for a single entry):
# { (<filename>, <compression-factor-1>, <compression-factor-2>, <coefficient-nr>), [ <quantification-result-0>, <quantification-result-1>, ..., <quantification-result-n> ] } } } }
quants = {}
imageNames = Set()
compressionFactors = Set()
for quantFilepath in quantFilepaths:
	# get image filename (the last folder, in essence)
	imageName = quantFilepath.rstrip().split('/')[-2]
	# split filename into parts by separator '-' which denotes the compression factor(s)
	imageNameParts = imageName.split('-')

	# assign parts (compressionFactor2 is zero if single-compressed)
	imageName = imageNameParts[0]
	compressionFactor1 = int(imageNameParts[1])
	compressionFactor2 = 0 if len(imageNameParts) == 2 else int(imageNameParts[2])

	# protocol imageName and compressionFactors
	imageNames.add(imageName)
	compressionFactors.add(compressionFactor1)
	compressionFactors.add(compressionFactor2)

	# read corresponding quant.dat file
	# split lines by ',' (so that we get the 4 results of the 4 methods of each coefficient/line)
	# protocol the list of the 4 values of the coefficient/line into a dictionary
	# the key of the dictionary is a quadruple (<image-name>, <compression-factor-1>, <compression-factor-2>, <coefficient-nr>)
	i = 0
	for line in open(quantFilepath.rstrip()).readlines():
		values = line.rstrip().split(',')
		quants[(imageName, compressionFactor1, compressionFactor2, i)] = values
		i += 1

# determine min. and max. compression factors
minCompressionFactor = maxCompressionFactor = 0
if len(compressionFactors) >= 2:
	compressionFactors = list(compressionFactors)
	maxCompressionFactor = compressionFactors[-1]
	minCompressionFactor = compressionFactors[0]

# debug/informal output
print "Processed images:"
for imageName in imageNames:
	print "\t" + str(imageName)
print "Compression factors: "
for compressionFactor in compressionFactors:
	print "\t" + str(compressionFactor)


# create directory for clustered quantification data
clusterDir = "clustered_quantifications/"
p = subprocess.Popen("mkdir %s" % (clusterDir), shell=True)
p.wait()

# for each image we got
#	- 4 quantification methods
#	- 16 coefficients (1 DC, 15 LP)
#	- n compression factors
#
# while iterating/clustering, some data is gathered to create some statistics about the quantification data and stored into a dictionary
# one entry has the form { <compression-factor-1>, { <compression-factor-2>, { <coefficient-nr>, { <method-nr>, { <statistics-name>, <value> } } } where <statistics-name> denotes a string and <value> denotes an number
# the following statistics are generated:
#	- for each (compression-factor-1, compression-factor-2) combination, constant coefficient-nr and constant method-nr:
#		- min of ...
#		- avg of ...
#		- max of ...
#	...
#	...
#	...
methodCount = 4
coefficientCount = 16
filenames = Set()
idx = 0
for imageName in sorted(imageNames):
	for compressionFactor1 in sorted(compressionFactors):
		for coefficientIndex in xrange(0 , coefficientCount):
			for methodIndex in xrange(0, methodCount):
				if compressionFactor1 <= 0:
					continue

				filename = "%squant_all_c%d_m%d-%d.dat" % (clusterDir, coefficientIndex, methodIndex, compressionFactor1)
				f = open(filename, 'a')
				
				values = [idx]
				if not filename in filenames:
					heads = ["n", "\"SC " + str(compressionFactor1) + "\""]
				for compressionFactor2 in sorted(compressionFactors):
					if compressionFactor2 == compressionFactor1:
						continue

					if (imageName, compressionFactor1, compressionFactor2, coefficientIndex) in quants:
						#print quants[(imageName, compressionFactor1, compressionFactor2, coefficientIndex)][methodIndex]
						values += [quants[(imageName, compressionFactor1, compressionFactor2, coefficientIndex)][methodIndex]]

						if not filename in filenames and compressionFactor2 != 0:
							heads += ["\"DC " + str(compressionFactor1) + "-" + str(compressionFactor2) + "\""]
						#print "values = " + str(values)
				if (not filename in filenames) and heads:
					for head in heads:
						f.write(str(head) + " ")
					f.write("\n")
					headWritten = True
				if values:
					#print values
					for value in values:
						f.write(str(value) + " ")
					f.write("\n")
				f.close()
				filenames.add(filename)
	idx += 1