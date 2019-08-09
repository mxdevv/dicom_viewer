#ifndef __DICOM_RENDER_CPP__
#define __DICOM_RENDER_CPP__

#include "dicom_render.h"

Dicom_render::Dicom_render(wxFrame* parent, int* args,
    Dicom_reader* dicom_reader)
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

Dicom_render::~Dicom_render()
{
  delete gl_context;
}

int& Dicom_render::get_x() { return x; }
int& Dicom_render::get_y() { return y; }
int& Dicom_render::get_z() { return z; }
float& Dicom_render::get_angle_xy() { return angle_xy; }
float& Dicom_render::get_angle_xz() { return angle_xz; }
float& Dicom_render::get_angle_yz() { return angle_yz; }

void Dicom_render::init()
{
  SetCurrent(*gl_context);

  glewInit();

  /*glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
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

void Dicom_render::resized(wxSizeEvent& evt)
{
  glViewport(0, 0, getWidth(), getHeight());
}

int Dicom_render::getWidth()
{
  return GetSize().x;
}

int Dicom_render::getHeight()
{
  return GetSize().y;
}

void Dicom_render::render(wxPaintEvent& evt)
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

void Dicom_render::load_vertex_shader()
{
  shader_v = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(shader_v, 1, &shader_v_text, NULL);
  glCompileShader(shader_v);

  GLint success;
  GLchar log[512];
  glGetShaderiv(shader_v, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader_v, 512, NULL, log);
    std::cerr << "Shader compilation error!\n" << log << std::endl;
  }
}

void Dicom_render::load_fragment_shader()
{
  shader_f = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(shader_f, 1, &shader_f_text, NULL);
  glCompileShader(shader_f);

  GLint success;
  GLchar log[512];
  glGetShaderiv(shader_f, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader_f, 512, NULL, log);
    std::cerr << "Shader compilation error!\n" << log << std::endl;
  }
}

void Dicom_render::link_and_compile_program()
{
  program = glCreateProgram();
  glAttachShader(program, shader_v);
  glAttachShader(program, shader_f);
  glLinkProgram(program);

  GLint success;
  GLchar log[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, NULL, log);
    std::cout << "Linking program fail!\n" << log << std::endl;
  }
  /*glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);*/
}

void Dicom_render::VAO_VBO_init()
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

/*void Dicom_render::gen_tex()
{
  int width = dicom_reader->get_width();
  int height = dicom_reader->get_height();
  int length = dicom_reader->get_length();
  GLuint* image = new GLuint[width * height];

  // очень тяжёлый алгоритм, обязателен к оптимизации
  int li, rx, ry, rz, sx, sy, sz, px, py, pz;
  for(int iy = 0; iy < height; iy++) {
    for(int ix = 0; ix < width; ix++) {
      li = iy * width + ix;

      sx = width / 2;
      sy = height / 2;
      sz = length / 2;

      rx = ix + x - sx;
      ry = iy + y - sy;
      rz = z      - sz;

      // angle_xy
      px = rx; py = ry;
      rx = px * cos(angle_xy) - py * sin(angle_xy);
      ry = px * sin(angle_xy) + py * cos(angle_xy);

      // angle_xz
      px = rx; pz = rz;
      rx = px * cos(angle_xz) + pz * sin(angle_xz);
      rz = -px* sin(angle_xz) + pz * cos(angle_xz);

      // angle_yz
      py = ry; pz = rz;
      ry = py * cos(angle_yz) - pz * sin(angle_yz);
      rz = py * sin(angle_yz) + pz * cos(angle_yz);

      rx += sx;
      ry += sy;
      rz += sz;

      if (rx > width || rx < 0 // x
          || rz >= length || rz < 0 // z
          || ry > height || ry < 0) { // y
        image[li] = 0;
      } else {
        image[li] = dicom_reader->image_3d[rz * width * height + ry * width + rx];
      }
    }
  }

  //glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // что это?

  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
      GL_LUMINANCE, GL_UNSIGNED_INT, image);
}*/

void Dicom_render::gen_tex_3d()
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

void Dicom_render::draw()
{
  static GLuint texMove_location = glGetUniformLocation(program, "texOffset");
  static GLuint texRotate_location = glGetUniformLocation(program, "texRotate");

  glBindTexture(GL_TEXTURE_3D, tex_3d);
  glUseProgram(program);
  glUniform3f(texMove_location, (float)x / width, (float)y / height, 
      (float)z / length);
  glUniform3f(texRotate_location, angle_xy / 180.0f * M_PI,
      angle_yz / 180.0f * M_PI, angle_xz / 180.0f * M_PI);
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

#endif
