#ifndef __APP_H__
#define __APP_H__

#include <wx/wx.h>
#include "dicom_render.h"
#include "dicom_reader.h"

class App : public wxApp
{
  virtual bool OnInit();

  wxFrame* frame;
  Dicom_reader dicom_reader;
  Dicom_render* dicom_render;

public:
  void keyPressed(wxKeyEvent& event);

  DECLARE_EVENT_TABLE();

  friend Dicom_render;
};

BEGIN_EVENT_TABLE(App, wxApp)
EVT_KEY_DOWN(App::keyPressed)
END_EVENT_TABLE()

IMPLEMENT_APP(App)

#endif
