#ifndef __DICOM_READER_H__
#define __DICOM_READER_H__

#include "render.h"

class Dicom_reader {
  int width = 0, height = 0, length = 0;

  GLubyte* image_3d = nullptr;

  inline void load_from_file(const char* path);
  inline void load_from_directory(const char* path);

  void load_image(const imebra::Image* image,
      const imebra::ReadingDataHandlerNumeric* data_handler,
      imebra::DataSet* data_set, int i);
public:
  int get_width();
  int get_height();
  int get_length();

  void load(const char* path);

  class unsupported_colorspace_exception { };

  friend class Render;
};

#endif
