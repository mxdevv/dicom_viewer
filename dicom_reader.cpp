#ifndef __DICOM_READER_CPP__
#define __DICOM_READER_CPP__

#include "dicom_reader.h"

int Dicom_reader::get_width() { return width; }
int Dicom_reader::get_height() { return height; }
int Dicom_reader::get_length() { return length; }

void Dicom_reader::load(const char* path)
{
  int images_len;

  if (std::filesystem::is_regular_file(path)) {
    images_len = 1;
    std::cerr << "Open " << images_len << " file" << std::endl;

    std::unique_ptr<imebra::DataSet>
        imebra_data_set (imebra::CodecFactory::load(path));

    std::unique_ptr<imebra::Image>
        image (imebra_data_set->getImage(0));

    std::string color_space = image->getColorSpace();

    std::cerr << "color_space: " << color_space << std::endl;

    width = image->getWidth(); height = image->getHeight(); length = images_len;
    image_3d = new GLuint[width * height * images_len];

    std::cerr << "width: " << width << ", height: "
        << height << std::endl;

    std::unique_ptr<imebra::ReadingDataHandlerNumeric>
        data_handler (image->getReadingDataHandler());

    load_image(image.get(), data_handler.get(), 0);

  } else if (std::filesystem::is_directory(path)) {
    std::cerr << "Open directory" << std::endl;

    std::vector<std::string> paths;

    images_len = 0;
    for(auto& f : std::filesystem::directory_iterator(path)) {
      images_len++;
      paths.push_back(f.path());
    }
    std::cerr << "Open " << images_len << " file" << std::endl;

    std::sort(paths.begin(), paths.end());

    std::unique_ptr<imebra::DataSet>
        imebra_data_set (imebra::CodecFactory::load(paths[0]));

    std::unique_ptr<imebra::Image>
        image (imebra_data_set->getImage(0));

    width = image->getWidth(); height = image->getHeight(); length = images_len;
    image_3d = new GLuint[images_len * width * height];

    for(int i = 0; i < images_len; i++) {

      std::unique_ptr<imebra::DataSet>
          imebra_data_set (imebra::CodecFactory::load(paths[i]));

    std::unique_ptr<imebra::Image>
        image (imebra_data_set->getImage(0));

      std::string color_space = image->getColorSpace();

      std::cerr << "color_space: " << color_space << std::endl;

      std::cerr << "width: " << width << ", height: "
          << height << std::endl;

      std::unique_ptr<imebra::ReadingDataHandlerNumeric>
          data_handler (image->getReadingDataHandler());

      load_image(image.get(), data_handler.get(), i);
    }
  }
  std::cerr << "-----------------------------" << std::endl;
  std::cerr << "Images size: "
      << images_len * width * height * 4.0 / 1024.0 / 1024.0
      << "MB" << std::endl;
}

void Dicom_reader::load_image(const imebra::Image* image,
    const imebra::ReadingDataHandlerNumeric* data_handler, int i)
{
  using t = std::uint32_t;
  t luminance, r, g, b, j = 0;
  if (imebra::ColorTransformsFactory::isMonochrome(image->getColorSpace())) {
    std::cerr << "Image size: " <<
        width * height * 4.0 / 1024.0 / 1024.0 << "MB"
        << std::endl;
    for(t y = 0; y < height; y++) {
      for(t x = 0; x < width; x++) {
        luminance = data_handler->getSignedLong(y * width + x);
        image_3d[i * width * height + j++] = luminance * 8'000'000;
      }
    }
  } else {
    printf("Не судьба...\n");
  }
}

#endif
