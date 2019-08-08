#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/glcanvas.h>

#include <GL/glu.h>
#include <GL/gl.h>

#include <imebra/imebra.h>

#include <filesystem>
#include <algorithm>
#include <vector>

#include "app.h"
#include "dicom_reader.cpp"
#include "dicom_render.cpp"

void App::keyPressed(wxKeyEvent& evt)
{
  switch(evt.GetKeyCode()) {
    case WXK_UP:
      dicom_render->cur_image++;
      break;
    case WXK_DOWN:
      dicom_render->cur_image--;
      break;
  }
}

bool App::OnInit()
{
  frame = new wxFrame((wxFrame *)NULL, -1,  wxT("osdicom"),
                      wxPoint(50,50), wxSize(400,200));

  wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

  int args[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};
  dicom_render = new Dicom_render((wxFrame*)frame, args, &dicom_reader);

  sizer->Add(dicom_render, 1, wxEXPAND);

  frame->SetSizer(sizer);
  frame->SetAutoLayout(true);

  frame->CreateStatusBar();
  frame->SetStatusText(_T("Hello World"));

  frame->Show();

  dicom_render->init();
  dicom_reader.load("samples/lee/brain_dicom/");
  dicom_render->gen_texs();

  return true;
}
