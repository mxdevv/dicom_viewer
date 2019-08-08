#ifndef __APP_H__
#define __APP_H__

#include <wx/wx.h>
#include "dicom_render.h"
#include "dicom_reader.h"

class App : public wxApp
{
  virtual bool OnInit();

  wxFrame* frame;

public:
  Dicom_reader dicom_reader;
  Dicom_render* dicom_render;

  void keyPressed(wxKeyEvent& evt);

  DECLARE_EVENT_TABLE();
};

App* app;

BEGIN_EVENT_TABLE(App, wxApp)
EVT_KEY_DOWN(App::keyPressed)
END_EVENT_TABLE()

IMPLEMENT_APP(App)


class Fn_slider : public wxSlider
{
  void (*fn)(int);

public:
  Fn_slider(wxWindow *parent, wxWindowID id, int value, int minValue,
      int maxValue, void (*fn)(int value), const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize, long style = wxSL_HORIZONTAL,
      const wxValidator& validator = wxDefaultValidator,
      const wxString& name = wxSliderNameStr);

  void scrolling(wxScrollEvent& evt);

  DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(Fn_slider, wxSlider)
EVT_SCROLL(Fn_slider::scrolling)
END_EVENT_TABLE()

#endif
