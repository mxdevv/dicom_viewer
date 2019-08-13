#ifndef __DICOM_READER_CPP__
#define __DICOM_READER_CPP__

#include <list>

#include "dicom_reader.h"
#include "string_switch.cpp"

int Dicom_reader::get_width() { return width; }
int Dicom_reader::get_height() { return height; }
int Dicom_reader::get_length() { return length; }

void Dicom_reader::load_from_file(const char* path)
{
  std::unique_ptr<imebra::DataSet>
      imebra_data_set (imebra::CodecFactory::load(path));

  std::unique_ptr<imebra::Image>
      imebra_image (imebra_data_set->getImageApplyModalityTransform(0));

  std::string color_space = imebra_image->getColorSpace();

  width = imebra_image->getWidth();
  height = imebra_image->getHeight();
  length = 1;
  image_3d = new GLubyte[width * height * 1 * 4];

  std::unique_ptr<imebra::ReadingDataHandlerNumeric>
      data_handler (imebra_image->getReadingDataHandler());

  load_image(imebra_image.get(), data_handler.get(), imebra_data_set.get(), 0);
}

void Dicom_reader::load_from_directory(const char* path)
{
  std::vector<std::string> paths;

  int images_len = 0;
  for(auto& f : std::filesystem::directory_iterator(path)) {
    images_len++;
    paths.push_back(f.path());
  }
  std::sort(paths.begin(), paths.end());

  std::unique_ptr<imebra::DataSet>
      imebra_data_set (imebra::CodecFactory::load(paths[0]));

  std::unique_ptr<imebra::Image>
      imebra_image (imebra_data_set->getImageApplyModalityTransform(0));

  width = imebra_image->getWidth();
  height = imebra_image->getHeight();
  length = images_len;
  image_3d = new GLubyte[images_len * width * height * 4];

  for(int i = 0; i < images_len; i++) {
    std::unique_ptr<imebra::DataSet>
        imebra_data_set (imebra::CodecFactory::load(paths[i]));

    std::unique_ptr<imebra::Image>
        imebra_image (imebra_data_set->getImageApplyModalityTransform(0));

    std::string color_space = imebra_image->getColorSpace();

    std::unique_ptr<imebra::ReadingDataHandlerNumeric>
        data_handler (imebra_image->getReadingDataHandler());

    load_image(imebra_image.get(), data_handler.get(), imebra_data_set.get(),
        i);
  }
}

void Dicom_reader::load(const char* path)
{
  if (std::filesystem::is_regular_file(path)) {
    load_from_file(path);
  } else if (std::filesystem::is_directory(path)) {
    load_from_directory(path);
  }
  std::cerr << "Loaded " << length << " file. width: " << width << ", height: "
      << height << ", length: " << length << ", size: "
      << length * width * height * 4.0 / 1024.0 / 1024.0 << "MB" << std::endl;
}

void Dicom_reader::load_image(const imebra::Image* image,
    const imebra::ReadingDataHandlerNumeric* data_handler,
    imebra::DataSet* data_set, int i)
{
  /*using t = std::uint32_t;
  t luminance, r, g, b, j = 0;
  sswitch(image->getColorSpace().c_str()) {
    scase("MONOCHROME1")
    scase("MONOCHROME2") {
      for(t y = 0; y < height; y++) {
        for(t x = 0; x < width; x++) {
          luminance = data_handler->getSignedLong(y * width + x);
          image_3d[i * width * height + j++] = luminance * 8'000'000;
        }
      }
    } break;
    scase("RGB")
    scase("YBR_RCT")
    scase("YBR_ICT")
    scase("YBR_FULL") {
      for(t y = 0; y < height; y++) {
        for(t x = 0; x < width; x++) {
          r = data_handler->getSignedLong((y * width + x) * 3);
          g = data_handler->getSignedLong((y * width + x) * 3 + 1);
          b = data_handler->getSignedLong((y * width + x) * 3 + 2);
          //image_3d[i * width * height + j++] = luminance * 8'000'000;
        }
      }
    } break;
    default: {
      throw unsupported_colorspace_exception();
    } break;
  }*/

  imebra::TransformsChain chain;

  if (imebra::ColorTransformsFactory::isMonochrome(image->getColorSpace())) {
    // Allocate a VOILUT transform. If the DataSet does not contain any
    // pre-defined settings then we will find the optimal ones.
    imebra::VOILUT voilutTransform;

    // Retrieve the VOIs (center/width pairs)
    imebra::vois_t vois = data_set->getVOIs();

    // Retrieve the LUTs
    std::list<imebra::LUT*> luts;
    for(size_t scanLUTs(0); ; ++scanLUTs)
    {
      try
      {
        luts.push_back(data_set->getLUT(imebra::TagId(
            imebra::tagId_t::VOILUTSequence_0028_3010), scanLUTs));
      }
      catch(const imebra::MissingDataElementError&)
      {
        break;
      }
    }

    if(!vois.empty())
    {
      voilutTransform.setCenterWidth(vois[0].center, vois[0].width);
    }
    else if(!luts.empty())
    {
      voilutTransform.setLUT(*luts.front());
    }
    else
    {
      voilutTransform.applyOptimalVOI(*image, 0, 0, width, height);
    }

    chain.addTransform(voilutTransform);
  }

  imebra::DrawBitmap draw(chain);

  // Ask for the size of the buffer (in bytes)
  size_t requestedBufferSize = draw.getBitmap(*image,
      imebra::drawBitmapType_t::drawBitmapRGBA, 4, 0, 0);

  // Now we allocate the buffer and then ask DrawBitmap to fill it
  draw.getBitmap(*image, imebra::drawBitmapType_t::drawBitmapRGBA, 4,
      (char*)&image_3d[i * width * height * 4], requestedBufferSize);
}

#endif
