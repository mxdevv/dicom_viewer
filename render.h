#ifndef __RENDER_H__
#define __RENDER_H__

#include "dicom_reader.h"
#include "app.h"
#include "shader.cpp"
#include "program.cpp"
#include "texture_render.cpp"

class Render : public wxGLCanvas
{
  int x = 0, y = 0, z = 0;
  int width = 0, height = 0, length = 0;
  float angle_xz = 0.0f, angle_yz = 0.0f, angle_xy = 0.0f;

  wxGLContext* gl_context;
  Dicom_reader* dicom_reader;

  GLuint tex_3d;
  Shader vertex_2d_shader, fragment_2d_shader, vertex_3d_shader,
      fragment_3d_shader;
  Program program_2d, program_3d;
  Texture_render texture_render;

public:
  enum class e_rendering { rendering_2d, rendering_3d };
  e_rendering rendering = e_rendering::rendering_2d;

  Render(wxFrame* parent, int* args, Dicom_reader* dicom_reader);
  virtual ~Render();

  int& get_x();
  int& get_y();
  int& get_z();
  float& get_angle_xy();
  float& get_angle_xz();
  float& get_angle_yz();

  void init();

  void compile_shaders();
  void compile_programs();
  void gen_tex_3d();
  void draw();
  void draw_2d();
  void draw_3d();

  void resized(wxSizeEvent& evt);

  int getWidth();
  int getHeight();

  void render(wxPaintEvent& evt);

  DECLARE_EVENT_TABLE()

  friend class App;
};

BEGIN_EVENT_TABLE(Render, wxGLCanvas)
EVT_PAINT(Render::render)
EVT_SIZE(Render::resized)
END_EVENT_TABLE()

#endif
