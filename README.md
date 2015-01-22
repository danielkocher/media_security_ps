# Media Security PS

## Directories
* /documents:
  * contains documents concerning jxrlib (almost exclusively own notes as there exists no proper documentation) and the ITU-T reference implementation T.835.
* /double_compression_detection
 * contains the implementation of the actual double compression detection using the ITU-T JPEG-XR reference implementation (subfolder T.835)
* /jpegxr_libraries:
  * contains snapshots of both libraries without modifications as zip files.
* /papers
  * contains all papers we read concerning existing double compression research (e.g. for JPEG)
* /test_images
  * contains the test images (original as well as single- and double-compressed JPEG-XR format)

## Coding guidelines
* All modifications to the T.835 reference implementation are documented by a comment starting with '*ADDED MS_PS*'

## Requirements
* A unixoide operating system (currently only tested on Linux machines)
* `gnuplot` has to be installed

## Usage
* Execute `exec.sh` in the /double_compression_detection folder (e.g. issuing `sh exec.sh`). This may take some time because this command triggers the decoding of all test_images, the extraction of the coefficients of all decoded images and the drawing of the corresponding histogram for each coefficient.
