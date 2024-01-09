/***************************************************
    To read a jpg image file and download
    it as a texture map for openGL
    Derived from Tom Lane's example.c
    -- Obtain & install jpeg stuff from web
    (jpeglib.h, jerror.h jmore.h, jconfig.h,jpeg.lib)
****************************************************/
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <jerror.h>
#include <jpeglib.h>
#include <stdio.h>

emscripten::val loadJPEG(const char *FileName)
//================================
{
  emscripten::val image_data = emscripten::val::array();

  unsigned long x, y;
  unsigned int texture_id;
  unsigned long data_size; // length of the file
  int channels;            //  3 =>RGB   4 =>RGBA
  unsigned int type;
  unsigned char *rowptr[1];           // pointer to an array
  unsigned char *jdata;               // data for the image
  struct jpeg_decompress_struct info; // for our jpeg info
  struct jpeg_error_mgr err;          // the error handler

  FILE *file = fopen(FileName, "rb"); // open the file

  info.err = jpeg_std_error(&err);
  jpeg_create_decompress(&info); // fills info structure

  // if the jpeg file doesn't load
  if (!file) {
    fprintf(stderr, "Error reading JPEG file %s!", FileName);
    return emscripten::val::null();
  }

  jpeg_stdio_src(&info, file);
  jpeg_read_header(&info, TRUE); // read jpeg file header

  jpeg_start_decompress(&info); // decompress the file

  // set width and height
  x = info.output_width;
  y = info.output_height;
  channels = info.num_components;

  printf("channels = %d\n", channels);

  data_size = x * y * channels;

  printf("data size is: %lu\n", data_size);

  //--------------------------------------------
  // read scanlines one at a time & put bytes
  //    in jdata[] array. Assumes an RGB image
  //--------------------------------------------
  jdata = (unsigned char *)malloc(data_size);
  while (info.output_scanline < info.output_height) // loop
  {
    // Enable jpeg_read_scanlines() to fill our jdata array
    rowptr[0] = (unsigned char *)jdata + // secret to method
                3 * info.output_width * info.output_scanline;

    jpeg_read_scanlines(&info, rowptr, 1);
  }
  //---------------------------------------------------

  jpeg_finish_decompress(&info); // finish decompressing

  jpeg_destroy_decompress(&info);

  fclose(file); // close the file

  for (auto i = 0; i < data_size; i++) {
    image_data.set(i, jdata[i]);
  }

  free(jdata);

  return image_data;
}

emscripten::val readJPEG(std::string fileName) {
  return loadJPEG(fileName.c_str());
}

EMSCRIPTEN_BINDINGS(read_jpeg) { emscripten::function("readJPEG", &readJPEG); }