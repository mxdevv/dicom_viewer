#ifndef __DICOM_READER_H__
#define __DICOM_READER_H__

#include "dicom_render.h"

class Dicom_reader {
  int images_len = 0;
  int* widths = nullptr, * heights = nullptr;
  GLuint** images = nullptr;

  void load_image(const imebra::Image* image,
      const imebra::ReadingDataHandlerNumeric* data_handler, int i);
public:
  void load(const char* path);

  friend class Dicom_render;
};

#endif
