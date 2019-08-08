#ifndef __DICOM_RENDER_H__
#define __DICOM_RENDER_H__

#include "dicom_reader.h"
#include "app.h"

class Dicom_render : public wxGLCanvas
{
  int x = 0, y = 0, z = 0;
  float angle_xz = 0.0f, angle_yz = 0.0f, angle_xy = 0.0f;

  wxGLContext* gl_context;
  Dicom_reader* dicom_reader;
  GLuint tex;

public:
  Dicom_render(wxFrame* parent, int* args, Dicom_reader* dicom_reader);
  virtual ~Dicom_render();

  int& get_x();
  int& get_y();
  int& get_z();
  float& get_angle_xy();
  float& get_angle_xz();
  float& get_angle_yz();

  void init();

  void gen_tex();
  void draw_tex();

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
