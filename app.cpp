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

Fn_slider::Fn_slider(wxWindow *parent, wxWindowID id, int value, int minValue,
    int maxValue, void (*fn)(int value), const wxPoint& pos, const wxSize& size,
    long style, const wxValidator& validator, const wxString& name)
  : wxSlider(parent, id, value, minValue, maxValue, pos, size, style, validator,
      name), fn(fn)
{
  ;
}

void slider_set_x(int value)
{
  app->dicom_render->get_x() = value;
  app->dicom_render->gen_tex();
}

void slider_set_y(int value)
{
  app->dicom_render->get_y() = value;
  app->dicom_render->gen_tex();
}

void slider_set_z(int value)
{
  app->dicom_render->get_z() = value;
  app->dicom_render->gen_tex();
}

void slider_set_angle_xy(int value)
{
  app->dicom_render->get_angle_xy() = value / 180.0f * M_PI;
  app->dicom_render->gen_tex();
}

void slider_set_angle_xz(int value)
{
  app->dicom_render->get_angle_xz() = value / 180.0f * M_PI;
  app->dicom_render->gen_tex();
}

void slider_set_angle_yz(int value)
{
  app->dicom_render->get_angle_yz() = value / 180.0f * M_PI;
  app->dicom_render->gen_tex();
}

void Fn_slider::scrolling(wxScrollEvent& evt)
{
  fn(GetValue());
}

void App::keyPressed(wxKeyEvent& evt)
{
  static wxChar uc;
  uc = evt.GetUnicodeKey();
  if (uc != WXK_NONE) {
    switch(uc) {
      case 'q': case 'Q':
        dicom_render->z++;
        dicom_render->gen_tex();
        break;
      case 'a': case 'A':
        dicom_render->z--;
        dicom_render->gen_tex();
        break;
      case 'w': case 'W':
        dicom_render->x--;
        dicom_render->gen_tex();
        break;
      case 's': case 'S':
        dicom_render->x++;
        dicom_render->gen_tex();
        break;
      case 'e': case 'E':
        dicom_render->y++;
        dicom_render->gen_tex();
        break;
      case 'd': case 'D':
        dicom_render->y--;
        dicom_render->gen_tex();
        break;
      case 'r': case 'R':
        dicom_render->angle_xz += 0.01f;
        dicom_render->gen_tex();
        break;
      case 'f': case 'F':
        dicom_render->angle_xz -= 0.01f;
        dicom_render->gen_tex();
        break;
      case 't': case 'T':
        dicom_render->angle_yz += 0.01f;
        dicom_render->gen_tex();
        break;
      case 'g': case 'G':
        dicom_render->angle_yz -= 0.01f;
        dicom_render->gen_tex();
        break;
      case 'y': case 'Y':
        dicom_render->angle_xy += 0.01f;
        dicom_render->gen_tex();
        break;
      case 'h': case 'H':
        dicom_render->angle_xy -= 0.01f;
        dicom_render->gen_tex();
        break;
    }
  } else {
    switch(evt.GetKeyCode()) {
      ;
    }
  }
}

bool App::OnInit()
{
  app = this;

  dicom_reader.load("samples/lee/brain_dicom/");

  frame = new wxFrame((wxFrame *)NULL, -1,  wxT("osdicom"),
                      wxDefaultPosition, wxSize(800, 600));

  int args[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};
  dicom_render = new Dicom_render((wxFrame*)frame, args, &dicom_reader);

  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sub_sizer_xyz = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* sub_sizer_angles = new wxBoxSizer(wxHORIZONTAL);

  Fn_slider* slider_x = new Fn_slider(frame, -1, dicom_render->get_x(),
      -dicom_reader.get_width(), dicom_reader.get_width(), slider_set_x,
      wxDefaultPosition, wxDefaultSize, wxSL_VALUE_LABEL);
  Fn_slider* slider_y = new Fn_slider(frame, -1, dicom_render->get_y(),
      -dicom_reader.get_height(), dicom_reader.get_height(), slider_set_y,
      wxDefaultPosition, wxDefaultSize, wxSL_VALUE_LABEL);
  Fn_slider* slider_z = new Fn_slider(frame, -1, dicom_render->get_z(),
      0, dicom_reader.get_length(), slider_set_z, wxDefaultPosition,
      wxDefaultSize, wxSL_VALUE_LABEL);
  Fn_slider* slider_angle_xy = new Fn_slider(frame, -1, 0, -360, 360,
      slider_set_angle_xy, wxDefaultPosition, wxDefaultSize, wxSL_VALUE_LABEL);
  Fn_slider* slider_angle_xz = new Fn_slider(frame, -1, 0, -360, 360,
      slider_set_angle_xz, wxDefaultPosition, wxDefaultSize, wxSL_VALUE_LABEL);
  Fn_slider* slider_angle_yz = new Fn_slider(frame, -1, 0, -360, 360,
      slider_set_angle_yz, wxDefaultPosition, wxDefaultSize, wxSL_VALUE_LABEL);

  wxStaticText* text_x = new wxStaticText(frame, -1, "x", { 0, 0 },
      { -1, -1 } );
  wxStaticText* text_y = new wxStaticText(frame, -1, "y", { 0, 0 },
      { -1, -1 } );
  wxStaticText* text_z = new wxStaticText(frame, -1, "z", { 0, 0 },
      { -1, -1 } );
  wxStaticText* text_angle_xy = new wxStaticText(frame, -1, "xy",
      { 0, 0 }, { -1, -1 } );
  wxStaticText* text_angle_xz = new wxStaticText(frame, -1, "xz",
      { 0, 0 }, { -1, -1 } );
  wxStaticText* text_angle_yz = new wxStaticText(frame, -1, "yz",
      { 0, 0 }, { -1, -1 } );

  sub_sizer_xyz->Add(text_x, 0, wxEXPAND);
  sub_sizer_xyz->Add(slider_x, 1, wxEXPAND);
  sub_sizer_xyz->Add(text_y, 0, wxEXPAND);
  sub_sizer_xyz->Add(slider_y, 1, wxEXPAND);
  sub_sizer_xyz->Add(text_z, 0, wxEXPAND);
  sub_sizer_xyz->Add(slider_z, 1, wxEXPAND);
  sub_sizer_angles->Add(text_angle_xy, 0, wxEXPAND);
  sub_sizer_angles->Add(slider_angle_xy, 1, wxEXPAND);
  sub_sizer_angles->Add(text_angle_xz, 0, wxEXPAND);
  sub_sizer_angles->Add(slider_angle_xz, 1, wxEXPAND);
  sub_sizer_angles->Add(text_angle_yz, 0, wxEXPAND);
  sub_sizer_angles->Add(slider_angle_yz, 1, wxEXPAND);
  sizer->Add(sub_sizer_xyz, 0, wxEXPAND);
  sizer->Add(sub_sizer_angles, 0, wxEXPAND);
  sizer->Add(dicom_render, 1, wxEXPAND);
  frame->SetSizer(sizer);

  frame->SetAutoLayout(true);

  frame->Show();

  dicom_render->init();
  dicom_render->gen_tex();

  return true;
}
