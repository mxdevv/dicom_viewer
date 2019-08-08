#ifndef __DICOM_READER_H__
#define __DICOM_READER_H__

#include "dicom_render.h"

class Dicom_reader {
  struct Image {
    int width, height;
    GLuint* data = nullptr;

    Image(int width, int height)
      : width(width), height(height)
    {
      data = new GLuint[width * height];
    }

    ~Image()
    {
      delete data;
    }
  };

  std::vector<Image> images;

  void load_image(const imebra::Image* image,
      const imebra::ReadingDataHandlerNumeric* data_handler, int i);
public:
  void load(const char* path);

  friend class Dicom_render;
};

#endif
