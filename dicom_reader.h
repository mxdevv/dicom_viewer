#ifndef __DICOM_READER_H__
#define __DICOM_READER_H__

#include "render.h"

class Dicom_reader {
  int width = 0, height = 0, length = 0;

  GLuint* image_3d = nullptr;

  void load_image(const imebra::Image* image,
      const imebra::ReadingDataHandlerNumeric* data_handler, int i);
public:
  int get_width();
  int get_height();
  int get_length();

  void load(const char* path);

  friend class Render;
};

#endif
