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

  GLuint program;
  GLuint shader_v, shader_f;
  GLfloat vertices[32] = {
      // Positions          // Colors           // Texture Coords
       0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Top Right
       0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom Right
      -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
      -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // Top Left 
  };
  GLuint indices[6] = {  // Note that we start from 0!
    0, 1, 3, // First Triangle
    1, 2, 3  // Second Triangle
  };
  GLuint VBO, VAO, EBO;

  const GLchar* shader_v_text =
      "#version 330 core\n"
      "layout (location = 0) in vec3 position;\n"
      "layout (location = 1) in vec3 color;\n"
      "layout (location = 2) in vec2 texCoord;\n"
      "\n"
      "out vec3 ourColor;\n"
      "out vec2 TexCoord;\n"
      "\n"
      "void main()\n"
      "{\n"
      "   gl_Position = vec4(position, 1.0f);\n"
      "   ourColor = color;\n"
      "   TexCoord = texCoord;\n"
      "}\0";

  const GLchar* shader_f_text =
      "#version 330 core\n"
      "in vec3 ourColor;\n"
      "in vec2 TexCoord;\n"
      "\n"
      "out vec4 color;\n"
      "\n"
      "uniform sampler2D ourTexture;\n"
      "\n"
      "void main()\n"
      "{\n"
      "    color = texture(ourTexture, TexCoord);\n"
      "}\0";

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

  void load_vertex_shader();
  void load_fragment_shader();
  void link_and_compile_program();
  void VAO_VBO_init();
  void load_test_texture();
  void gen_tex();
  void draw_tex();
  void draw_test();

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
