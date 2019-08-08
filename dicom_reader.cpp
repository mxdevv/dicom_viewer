#ifndef __DICOM_READER_CPP__
#define __DICOM_READER_CPP__

#include "dicom_reader.h"

void Dicom_reader::load(const char* path)
{
  if (std::filesystem::is_regular_file(path)) {
    images_len = 1;
    std::cerr << "Open " << images_len << " file" << std::endl;

    std::unique_ptr<imebra::DataSet>
        imebra_data_set (imebra::CodecFactory::load(path));

    heights = new int[images_len];
    widths = new int[images_len];
    images = new GLuint*[images_len];

    std::unique_ptr<imebra::Image>
        image (imebra_data_set->getImage(0));

    std::string color_space = image->getColorSpace();

    std::cerr << "color_space: " << color_space << std::endl;

    widths[0] = image->getWidth();
    heights[0] = image->getHeight();
    images[0] = new GLuint[widths[0] * heights[0]];

    std::cerr << "width: " << widths[0] << ", height: " << heights[0]
        << std::endl;

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

    heights = new int[images_len];
    widths = new int[images_len];
    images = new GLuint*[images_len];

    for(int i = 0; i < images_len; i++) {

      std::unique_ptr<imebra::DataSet>
          imebra_data_set (imebra::CodecFactory::load(paths[i]));

      std::unique_ptr<imebra::Image>
          image (imebra_data_set->getImage(0));

      std::string color_space = image->getColorSpace();

      std::cerr << "color_space: " << color_space << std::endl;

      widths[i] = image->getWidth();
      heights[i] = image->getHeight();
      images[i] = new GLuint[widths[i] * heights[i]];

      std::cerr << "width: " << widths[i] << ", height: " << heights[i]
          << std::endl;

      std::unique_ptr<imebra::ReadingDataHandlerNumeric>
          data_handler (image->getReadingDataHandler());

      load_image(image.get(), data_handler.get(), i);
    }
  }
  std::cerr << "-----------------------------" << std::endl;
  std::cerr << "Images size: "
      << images_len * widths[0] * heights[0] * 4.0 / 1024.0 / 1024.0
      << "MB" << std::endl;
}

void Dicom_reader::load_image(const imebra::Image* image,
    const imebra::ReadingDataHandlerNumeric* data_handler, int i)
{
  using t = std::uint32_t;
  t luminance, r, g, b, j = 0;
  if (imebra::ColorTransformsFactory::isMonochrome(image->getColorSpace())) {
    std::cerr << "Image size: " <<
        widths[i] * heights[i] * 4.0 / 1024.0 / 1024.0 << "MB" << std::endl;
    for(t y = 0; y < heights[i]; y++) {
      for(t x = 0; x < widths[i]; x++) {
        luminance = data_handler->getSignedLong(y * widths[0] + x);
        images[i][j++] = luminance * 10'000'000; // 10 млн или ничего не видно
      }
    }
  } else {
    printf("Не судьба...\n");
  }
}

#endif
