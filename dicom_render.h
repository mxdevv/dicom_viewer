#ifndef __DICOM_RENDER_H__
#define __DICOM_RENDER_H__

#include "dicom_reader.h"
#include "app.h"

class Dicom_render : public wxGLCanvas
{
public: // здесь должно быть private, no 'friend App' не работает о_0
  int cur_image = 0;
  wxGLContext* gl_context;
  Dicom_reader* dicom_reader;

public:
  Dicom_render(wxFrame* parent, int* args, Dicom_reader* dicom_reader);
  virtual ~Dicom_render();

  void init();

  void resized(wxSizeEvent& evt);

  int getWidth();
  int getHeight();

  void render(wxPaintEvent& evt);

  DECLARE_EVENT_TABLE()

  //friend App; // не видит App, почему-то
};

BEGIN_EVENT_TABLE(Dicom_render, wxGLCanvas)
EVT_PAINT(Dicom_render::render)
EVT_SIZE(Dicom_render::resized)
END_EVENT_TABLE()

#endif
