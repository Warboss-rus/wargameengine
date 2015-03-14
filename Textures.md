# Supported texture formats #

  * **BMP**. Only 24 and 32 bit RGB\RGBA uncompressed images are supported.
  * **TGA**. Only 24 and 32 bit RGB\RGBA uncompressed or RLE compressed images are supported.
  * **DDS**
  * **JPEG**. Only baseline (no JPEG progressive) are supported.
  * **PNG**. 8-bit-per-channel only.
  * **GIF**
  * **HDR**
  * **PIC** (Softimage PIC)

# Fastest image loading #

For fastest image loading use DDS as it it compressed in a fromat supported by graphics card and already contains mipmaps. BMP and TGA loading are also fast because of their simplicity. Loading other formats may take a bit longer.