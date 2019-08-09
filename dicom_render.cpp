#ifndef __DICOM_RENDER_CPP__
#define __DICOM_RENDER_CPP__

#include "dicom_render.h"

Dicom_render::Dicom_render(wxFrame* parent, int* args,
    Dicom_reader* dicom_reader)
  : wxGLCanvas(parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize,
               wxFULL_REPAINT_ON_RESIZE), dicom_reader(dicom_reader)
{
  gl_context = new wxGLContext(this);

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

  //draw_tex();
  draw_test();

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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
      (GLvoid*)0);
  glEnableVertexAttribArray(0);
  // Color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
      (GLvoid*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);
  // TexCoord attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
      (GLvoid*)(6 * sizeof(GLfloat)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0); // Unbind VAO
}

void Dicom_render::gen_tex()
{
  int width = dicom_reader->get_width();
  int height = dicom_reader->get_height();
  int length = dicom_reader->get_length();
  GLuint* image = new GLuint[width * height];

  /* очень тяжёлый алгоритм, обязателен к оптимизации */
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

      /* angle_xy*/
      px = rx; py = ry;
      rx = px * cos(angle_xy) - py * sin(angle_xy);
      ry = px * sin(angle_xy) + py * cos(angle_xy);

      /* angle_xz */
      px = rx; pz = rz;
      rx = px * cos(angle_xz) + pz * sin(angle_xz);
      rz = -px* sin(angle_xz) + pz * cos(angle_xz);

      /* angle_yz */
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

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // что это?

  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
      GL_LUMINANCE, GL_UNSIGNED_INT, image);
}

void Dicom_render::draw_tex()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glBindTexture(GL_TEXTURE_2D, tex);

  glPushMatrix();

  glBegin(GL_QUADS);

  glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
  glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 1.0f, 0.0f);
  glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);
  glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);

  glEnd();

  glPopMatrix();

  glDisable(GL_TEXTURE_2D);
}

void Dicom_render::draw_test()
{
  glBindTexture(GL_TEXTURE_2D, tex);
  glUseProgram(program);
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

#endif
