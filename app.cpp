#include <GL/glew.h>

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/glcanvas.h>

#include <imebra/imebra.h>

#include <filesystem>
#include <algorithm>
#include <vector>

#include "app.h"
#include "dicom_reader.cpp"
#include "render.cpp"

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
  app->render->get_x() = value;
}

void slider_set_y(int value)
{
  app->render->get_y() = value;
}

void slider_set_z(int value)
{
  app->render->get_z() = value;
}

void slider_set_angle_xy(int value)
{
  app->render->get_angle_xy() = value;
}

void slider_set_angle_xz(int value)
{
  app->render->get_angle_xz() = value;
}

void slider_set_angle_yz(int value)
{
  app->render->get_angle_yz() = value;
}

void Fn_slider::scrolling(wxScrollEvent& evt)
{
  fn(GetValue());
}

Render_choice::Render_choice(wxWindow *parent, wxWindowID id,
    const wxPoint& pos, const wxSize& size, int n, const wxString choices[],
    long style, const wxValidator& validator, const wxString& name)
  : wxChoice(parent, id, pos, size, n, choices, style, validator, name)
{
  ;
}

void Render_choice::choose(wxCommandEvent& evt)
{
  switch(evt.GetSelection()) {
    case 0: 
      // enum публичный!
      app->render->rendering = Render::e_rendering::rendering_2d;
      break;
    case 1: 
      // enum публичный!
      app->render->rendering = Render::e_rendering::rendering_3d;
      break;
  }
}

void App::keyPressed(wxKeyEvent& evt)
{
  /*static wxChar uc;
  uc = evt.GetUnicodeKey();
  if (uc != WXK_NONE) {
    switch(uc) {
      case 'q': case 'Q':
        render->z++;
        break;
      case 'a': case 'A':
        render->z--;
        break;
      case 'w': case 'W':
        render->x--;
        break;
      case 's': case 'S':
        render->x++;
        break;
      case 'e': case 'E':
        render->y++;
        break;
      case 'd': case 'D':
        render->y--;
        break;
      case 'r': case 'R':
        render->angle_xz += 1.0f;
        break;
      case 'f': case 'F':
        render->angle_xz -= 1.0f;
        break;
      case 't': case 'T':
        render->angle_yz += 1.0f;
        break;
      case 'g': case 'G':
        render->angle_yz -= 1.0f;
        break;
      case 'y': case 'Y':
        render->angle_xy += 1.0f;
        break;
      case 'h': case 'H':
        render->angle_xy -= 1.0f;
        break;
    }
  } else {
    switch(evt.GetKeyCode()) {
      ;
    }
  }*/
}

bool App::OnInit()
{
  app = this;

  dicom_reader.load("samples/lee/brain_dicom/");

  frame = new wxFrame((wxFrame *)NULL, -1,  wxT("osdicom"),
                      wxDefaultPosition, wxSize(800, 600));

  int args[] = {WX_GL_CORE_PROFILE, WX_GL_RGBA, WX_GL_DOUBLEBUFFER,
      WX_GL_DEPTH_SIZE, 16, 0};
  render = new Render((wxFrame*)frame, args, &dicom_reader);

  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sub_sizer_xyz = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* sub_sizer_angles = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* sub_sizer_options = new wxBoxSizer(wxHORIZONTAL);

  Fn_slider* slider_x = new Fn_slider(frame, -1, render->get_x(),
      -dicom_reader.get_width(), dicom_reader.get_width(), slider_set_x,
      wxDefaultPosition, wxDefaultSize, wxSL_VALUE_LABEL);
  Fn_slider* slider_y = new Fn_slider(frame, -1, render->get_y(),
      -dicom_reader.get_height(), dicom_reader.get_height(), slider_set_y,
      wxDefaultPosition, wxDefaultSize, wxSL_VALUE_LABEL);
  Fn_slider* slider_z = new Fn_slider(frame, -1, render->get_z(),
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

  const wxString choices[] = { "2d view", "3d view" };
  Render_choice* choice = new Render_choice(frame, -1, wxDefaultPosition,
      wxDefaultSize, 2, choices);
  choice->SetSelection(0);

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
  sub_sizer_options->Add(choice, 0, wxEXPAND);
  sizer->Add(sub_sizer_options, 0, wxEXPAND);
  sizer->Add(sub_sizer_xyz, 0, wxEXPAND);
  sizer->Add(sub_sizer_angles, 0, wxEXPAND);
  sizer->Add(render, 1, wxEXPAND);
  frame->SetSizer(sizer);

  frame->SetAutoLayout(true);

  frame->Show();

  render->init();
  render->compile_shaders();
  render->compile_programs();
  render->VAO_VBO_init();
  render->gen_tex_3d();

  return true;
}
