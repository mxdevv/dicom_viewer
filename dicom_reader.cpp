#ifndef __DICOM_READER_CPP__
#define __DICOM_READER_CPP__

#include "dicom_reader.h"

void Dicom_reader::load(const char* path)
{
  int images_len;

  if (std::filesystem::is_regular_file(path)) {
    images_len = 1;
    std::cerr << "Open " << images_len << " file" << std::endl;

    std::unique_ptr<imebra::DataSet>
        imebra_data_set (imebra::CodecFactory::load(path));

    images.reserve(images_len);

    std::unique_ptr<imebra::Image>
        image (imebra_data_set->getImage(0));

    std::string color_space = image->getColorSpace();

    std::cerr << "color_space: " << color_space << std::endl;

    images.emplace_back(image->getWidth(), image->getHeight());

    std::cerr << "width: " << images[0].width << ", height: "
        << images[0].height << std::endl;

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

    images.reserve(images_len);

    for(int i = 0; i < images_len; i++) {

      std::unique_ptr<imebra::DataSet>
          imebra_data_set (imebra::CodecFactory::load(paths[i]));

      std::unique_ptr<imebra::Image>
          image (imebra_data_set->getImage(0));

      std::string color_space = image->getColorSpace();

      std::cerr << "color_space: " << color_space << std::endl;

      images.emplace_back(image->getWidth(), image->getHeight());

      std::cerr << "width: " << images[i].width << ", height: "
          << images[i].height << std::endl;

      std::unique_ptr<imebra::ReadingDataHandlerNumeric>
          data_handler (image->getReadingDataHandler());

      load_image(image.get(), data_handler.get(), i);
    }
  }
  std::cerr << "-----------------------------" << std::endl;
  std::cerr << "Images size: "
      << images_len * images[0].width * images[0].height * 4.0 / 1024.0 / 1024.0
      << "MB" << std::endl;
}

void Dicom_reader::load_image(const imebra::Image* image,
    const imebra::ReadingDataHandlerNumeric* data_handler, int i)
{
  using t = std::uint32_t;
  t luminance, r, g, b, j = 0;
  if (imebra::ColorTransformsFactory::isMonochrome(image->getColorSpace())) {
    std::cerr << "Image size: " <<
        images[i].width * images[i].height * 4.0 / 1024.0 / 1024.0 << "MB"
        << std::endl;
    for(t y = 0; y < images[i].height; y++) {
      for(t x = 0; x < images[i].width; x++) {
        luminance = data_handler->getSignedLong(y * images[i].width + x);
        /* 10 млн или ничего не видно */
        images[i].data[j++] = luminance * 10'000'000;
      }
    }
  } else {
    printf("Не судьба...\n");
  }
}

#endif
