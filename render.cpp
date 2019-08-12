#ifndef __RENDER_CPP__
#define __RENDER_CPP__

#include "render.h"
#include "shader_sources.h"

Render::Render(wxFrame* parent, int* args, Dicom_reader* dicom_reader)
  : wxGLCanvas(parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize,
               wxFULL_REPAINT_ON_RESIZE), dicom_reader(dicom_reader)
{
  gl_context = new wxGLContext(this);

  width = dicom_reader->get_width();
  height = dicom_reader->get_height();
  length = dicom_reader->get_length();

  // To avoid flashing on MSW
  SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

Render::~Render()
{
  delete gl_context;
}

int& Render::get_x() { return x; }
int& Render::get_y() { return y; }
int& Render::get_z() { return z; }
float& Render::get_angle_xy() { return angle_xy; }
float& Render::get_angle_xz() { return angle_xz; }
float& Render::get_angle_yz() { return angle_yz; }

void Render::init()
{
  SetCurrent(*gl_context);

  glewInit();

  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /*glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  glDisable(GL_POLYGON_SMOOTH);
  glDisable(GL_LINE_SMOOTH);
  glEnable(GL_MULTISAMPLE);
  glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0f, 1.0f, 0.0f, 1.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();*/
  glViewport(0, 0, getWidth(), getHeight());

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void Render::resized(wxSizeEvent& evt)
{
  glViewport(0, 0, getWidth(), getHeight());
}

int Render::getWidth()
{
  return GetSize().x;
}

int Render::getHeight()
{
  return GetSize().y;
}

void Render::render(wxPaintEvent& evt)
{
  if(!IsShown()) return;

  wxGLCanvas::SetCurrent(*gl_context);
  /* only to be used in paint events. use wxClientDC to paint outside the
   * paint event */
  wxPaintDC(this);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  draw();

  glFlush();
  SwapBuffers();

  Refresh();
}

void Render::compile_shaders()
{
  vertex_2d_shader.create(shader_sources::vertex_2d, Shader::e_type::vertex);
  fragment_2d_shader.create(shader_sources::fragment_2d,
      Shader::e_type::fragment);
  vertex_3d_shader.create(shader_sources::vertex_3d, Shader::e_type::vertex);
  fragment_3d_shader.create(shader_sources::fragment_3d,
      Shader::e_type::fragment);
}

void Render::compile_programs()
{
  prg_2d.create(&vertex_2d_shader, nullptr, &fragment_2d_shader);
  prg_3d.create(&vertex_3d_shader, nullptr, &fragment_3d_shader);
}

void Render::VAO_VBO_init()
{
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
      GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat),
      (GLvoid*)0);
  glEnableVertexAttribArray(0);
  // Color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat),
      (GLvoid*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);
  // TexCoord attribute
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat),
      (GLvoid*)(6 * sizeof(GLfloat)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0); // Unbind VAO
}

void Render::gen_tex_3d()
{
  glGenTextures(1, &tex_3d);
  glBindTexture(GL_TEXTURE_3D, tex_3d);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
  float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, dicom_reader->get_width(),
      dicom_reader->get_height(), dicom_reader->get_length(), 0, GL_LUMINANCE,
      GL_UNSIGNED_INT, dicom_reader->image_3d);
}

void Render::draw()
{
  switch(rendering) {
    case e_rendering::rendering_2d:
      draw_2d();
      break;
    case e_rendering::rendering_3d:
      draw_3d();
      break;
  }
}

void Render::draw_2d()
{
  static GLuint texMove_location = glGetUniformLocation(prg_2d.get(),
      "texOffset");
  static GLuint texRotate_location = glGetUniformLocation(prg_2d.get(),
      "texRotate");

  glBindTexture(GL_TEXTURE_3D, tex_3d);
  prg_2d.run();
  glUniform3f(texMove_location, (float)x / width, (float)y / height, 
      (float)z / length);
  glUniform3f(texRotate_location, angle_xy / 180.0f * M_PI,
      angle_yz / 180.0f * M_PI, angle_xz / 180.0f * M_PI);
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void Render::draw_3d()
{
  static GLuint texMove_location = glGetUniformLocation(prg_3d.get(),
      "texOffset");
  static GLuint texRotate_location = glGetUniformLocation(prg_3d.get(),
      "texRotate");
  static GLuint quality_location = glGetUniformLocation(prg_3d.get(),
      "quality");

  glBindTexture(GL_TEXTURE_3D, tex_3d);
  prg_3d.run();
  glBindVertexArray(VAO);
  glUniform1f(quality_location, 1.0f / length);
  glUniform3f(texMove_location, (float)x / width, (float)y / height, 
      (float)z / length);
  glUniform3f(texRotate_location, angle_xy / 180.0f * M_PI,
      angle_yz / 180.0f * M_PI, angle_xz / 180.0f * M_PI);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

#endif
