# Media Security PS

## Directories
* /documents:
  * contains documents concerning jxrlib (almost exclusively own notes as there exists no proper documentation) and the ITU-T reference implementation T.835.
* /double_compression_detection
  * contains the implementation of the actual double compression detection using the ITU-T JPEG-XR reference implementation (subfolder T.835)
* /double_compression_detection/test_images
  * contains the TIF test images which are converted to JXR images
* /jpegxr_libraries:
  * contains snapshots of both libraries without modifications as zip files.
* /papers
  * contains all papers we read concerning existing double compression research (e.g. for JPEG)

## Coding guidelines
* All modifications to the T.835 reference implementation are documented by a comment starting with '*ADDED MS_PS*'

## Requirements
* A unixoide operating system (currently only tested on Linux machines)
* `bison`, `flex`, `gnuplot` and `gnuplot-x11` have to be installed

## Usage (Tested under Linux Mint 17)
* Open a UNIX shell
* Navigate into the root directory of this project
* Issue the following commands in the root directory of this project (in order or just the one you need)
  * `make` - build the T.835 project and the double compression detection project
  * `make images` - generate all the JXR test images (located in the subdirectory `double_compression/test_images/extracted/compressed/`)
  * `make run` - build the whole project and run it on the JXR test images located in the subdirectory `double_compression/test_images/extracted/compressed/` (you should first run `make images`)
  * `make clean` - clean the whole project including all the JXR test images

The savest way to fully build and run the project using the test images is to issue the following command(s):
`make images && make run`
