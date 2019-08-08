#ifndef __DICOM_RENDER_H__
#define __DICOM_RENDER_H__

#include "dicom_reader.h"
#include "app.h"

class Dicom_render : public wxGLCanvas
{
  int cur_image = 0;
  wxGLContext* gl_context;
  Dicom_reader* dicom_reader;
  GLuint* texs;

public:
  Dicom_render(wxFrame* parent, int* args, Dicom_reader* dicom_reader);
  virtual ~Dicom_render();

  void init();

  void gen_texs();
  void draw_tex(int i);

  void resized(wxSizeEvent& evt);

  int getWidth();
  int getHeight();

  void render(wxPaintEvent& evt);

  DECLARE_EVENT_TABLE()

  friend class App;
};

BEGIN_EVENT_TABLE(Dicom_render, wxGLCanvas)
EVT_PAINT(Dicom_render::render)
EVT_SIZE(Dicom_render::resized)
END_EVENT_TABLE()

#endif
