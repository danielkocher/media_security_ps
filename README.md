# Media Security PS

## Directories
* /documents:
  * contains documents concerning jxrlib (almost exclusively own notes as there exists no proper documentation) and the ITU-T reference implementation T.835.
* /jpegxr_libraries:
  * contains snapshots of both libraries without modifications as zip files.
* /pictures
  * contains the test pictures (original as well as JPEG-XR format)
* /double_compression_detection
 * contains the implementation of the actual double compression detection using the ITU-T JPEG-XR reference implementation by now (may be changed to jxrlib)

## Coding style
* All modifications to the T.835 are documented by a comment of the form 
  ```C
  /**
   * MS_PS:
   * <description>
   */
  ```
