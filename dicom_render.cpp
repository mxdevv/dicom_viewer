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

void Dicom_render::init()
{
  SetCurrent(*gl_context);

  glEnable(GL_BLEND);
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
  glLoadIdentity();
  glViewport(0, 0, getWidth(), getHeight());

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void Dicom_render::resized(wxSizeEvent& evt)
{
  ;
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

  draw_tex(cur_image);

  glFlush();
  SwapBuffers();

  Refresh();
}

void Dicom_render::gen_texs()
{
  texs = new GLuint[dicom_reader->images_len];

  for(int i = 0; i < dicom_reader->images_len; i++) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // что это?

    glGenTextures(1, &texs[i]);
    glBindTexture(GL_TEXTURE_2D, texs[i]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dicom_reader->widths[i],
        dicom_reader->heights[i], 0, GL_LUMINANCE, GL_UNSIGNED_INT,
        dicom_reader->images[i]);
  }
}

void Dicom_render::draw_tex(int i)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glBindTexture(GL_TEXTURE_2D, texs[i]);

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

#endif
