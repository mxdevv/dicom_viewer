#ifndef __RENDER_H__
#define __RENDER_H__

#include "dicom_reader.h"
#include "app.h"

class Render : public wxGLCanvas
{
  int x = 0, y = 0, z = 0;
  int width = 0, height = 0, length = 0;
  float angle_xz = 0.0f, angle_yz = 0.0f, angle_xy = 0.0f;

  wxGLContext* gl_context;
  Dicom_reader* dicom_reader;
  GLuint tex, tex_3d;

  GLuint program_2d, program_3d;
  GLuint shader_v_2d, shader_f_2d, shader_v_3d, shader_f_3d;
  GLfloat vertices[36] = {
      // Positions         // Colors          // Texture Coords
       1.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f, // Top Right
       1.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f, // Bottom Right
      -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, // Bottom Left
      -1.0f,  1.0f, 0.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f  // Top Left 
  };
  GLuint indices[6] = {  // Note that we start from 0!
    0, 1, 3, // First Triangle
    1, 2, 3  // Second Triangle
  };
  GLuint VBO, VAO, EBO;

  const GLchar* shader_v_2d_text =
      "#version 330 core\n"
      "layout (location = 0) in vec3 position;\n"
      "layout (location = 1) in vec3 color;\n"
      "layout (location = 2) in vec3 texCoord;\n"
      "\n"
      "out vec3 ourColor;\n"
      "out vec3 TexCoord;\n"
      "\n"
      "void main()\n"
      "{\n"
      "  gl_Position = vec4(position, 1.0f);\n"
      "  ourColor = color;\n"
      "  TexCoord = texCoord;\n"
      "}\0";

  const GLchar* shader_f_2d_text =
      "#version 330 core\n"
      "in vec3 ourColor;\n"
      "in vec3 TexCoord;\n"
      "\n"
      "out vec4 color;\n"
      "\n"
      "uniform sampler3D ourTexture;\n"
      "uniform vec3 texOffset;\n"
      "uniform vec3 texRotate;\n"
      "vec3 coord;\n"
      "vec3 tmp;\n"
      "\n"
      "void main()\n"
      "{\n"
      "  coord = TexCoord + texOffset - 0.5f;\n"
      "  tmp = coord;\n"
      "  coord.x = tmp.x * cos(texRotate[0]) - tmp.y * sin(texRotate[0]);\n"
      "  coord.y = tmp.x * sin(texRotate[0]) + tmp.y * cos(texRotate[0]);\n"
      "  tmp = coord;\n"
      "  coord.x = tmp.x * cos(texRotate[2]) + tmp.z * sin(texRotate[2]);\n"
      "  coord.z = tmp.x * -sin(texRotate[2]) + tmp.z * cos(texRotate[2]);\n"
      "  tmp = coord;\n"
      "  coord.y = tmp.y * cos(texRotate[1]) - tmp.z * sin(texRotate[1]);\n"
      "  coord.z = tmp.y * sin(texRotate[1]) + tmp.z * cos(texRotate[1]);\n"
      "  coord += 0.5f;\n"
      "  color = texture(ourTexture, coord);\n"
      "  color.a -= 1.0f - color.r;\n"
      "}\0";

  const GLchar* shader_v_3d_text =
      "#version 330 core\n"
      "layout (location = 0) in vec3 position;\n"
      "layout (location = 1) in vec3 color;\n"
      "layout (location = 2) in vec3 texCoord;\n"
      "\n"
      "out vec3 ourColor;\n"
      "out vec3 TexCoord;\n"
      "\n"
      "void main()\n"
      "{\n"
      "  gl_Position = vec4(position, 1.0f);\n"
      "  ourColor = color;\n"
      "  TexCoord = texCoord;\n"
      "}\0";

  const GLchar* shader_f_3d_text =
      "#version 330 core\n"
      "in vec3 ourColor;\n"
      "in vec3 TexCoord;\n"
      "\n"
      "out vec4 color;\n"
      "\n"
      "uniform sampler3D ourTexture;\n"
      "uniform vec3 texOffset;\n"
      "uniform vec3 texRotate;\n"
      "vec3 coord;\n"
      "vec3 tmp;\n"
      "\n"
      "void main()\n"
      "{\n"
      "  coord = TexCoord + texOffset - 0.5f;\n"
      "  tmp = coord;\n"
      "  coord.x = tmp.x * cos(texRotate[0]) - tmp.y * sin(texRotate[0]);\n"
      "  coord.y = tmp.x * sin(texRotate[0]) + tmp.y * cos(texRotate[0]);\n"
      "  tmp = coord;\n"
      "  coord.x = tmp.x * cos(texRotate[2]) + tmp.z * sin(texRotate[2]);\n"
      "  coord.z = tmp.x * -sin(texRotate[2]) + tmp.z * cos(texRotate[2]);\n"
      "  tmp = coord;\n"
      "  coord.y = tmp.y * cos(texRotate[1]) - tmp.z * sin(texRotate[1]);\n"
      "  coord.z = tmp.y * sin(texRotate[1]) + tmp.z * cos(texRotate[1]);\n"
      "  coord += 0.5f;\n"
      "  color = texture(ourTexture, coord);\n"
      "  color.a -= 1.0f - color.r;\n"
      "}\0";

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

  void compile_shader(GLuint& shader, GLuint SHADER_TYPE,
      const GLchar* shader_text);
  void compile_shaders();
  void compile_program(GLuint& program, const GLuint shader_v,
      const GLuint shader_f);
  void compile_programs();
  void VAO_VBO_init();
  void load_test_texture();
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
