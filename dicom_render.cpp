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

  dicom_reader->draw_tex(cur_image);

  glFlush();
  SwapBuffers();

  Refresh();
}

#endif
