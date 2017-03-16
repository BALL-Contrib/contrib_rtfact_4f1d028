
    #include <stdio.h>
    #include <jpeglib.h>
    int main(int argc, char **argv) { 
      struct jpeg_decompress_struct cinfo;
      jpeg_destroy_decompress(&cinfo);
      return 0;
    }
