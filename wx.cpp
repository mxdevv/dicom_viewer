#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/glcanvas.h>

#include <GL/glu.h>
#include <GL/gl.h>

#include <imebra/imebra.h>

#include <filesystem>
#include <algorithm>
#include <vector>

int cur_image = 0;

float screen_width_k, screen_height_k;

class Dicom_reader {
  int* widths = nullptr, * heights = nullptr;
  GLuint** images = nullptr;
  GLuint* texs;

  void load_image(const imebra::Image* image,
      const imebra::ReadingDataHandlerNumeric* data_handler, int i);
public:
  int images_len = 0;

  void load(const char* path);
  void gen_texs();
  void draw_tex(int i);
};

Dicom_reader dicom_reader;

void Dicom_reader::load(const char* path)
{
  if (std::filesystem::is_regular_file(path)) {
    images_len = 1;
    std::cerr << "Open " << images_len << " file" << std::endl;

    std::unique_ptr<imebra::DataSet>
        imebra_data_set (imebra::CodecFactory::load(path));

    heights = new int[images_len];
    widths = new int[images_len];
    images = new GLuint*[images_len];

    std::unique_ptr<imebra::Image>
        image (imebra_data_set->getImage(0));

    std::string color_space = image->getColorSpace();

    std::cerr << "color_space: " << color_space << std::endl;

    widths[0] = image->getWidth();
    heights[0] = image->getHeight();
    images[0] = new GLuint[widths[0] * heights[0]];

    std::cerr << "width: " << widths[0] << ", height: " << heights[0]
        << std::endl;

    std::unique_ptr<imebra::ReadingDataHandlerNumeric>
        data_handler (image->getReadingDataHandler());

    load_image(image.get(), data_handler.get(), 0);

  } else if (std::filesystem::is_directory(path)) {
    std::cerr << "Open directory" << std::endl;

    std::vector<std::string> paths;

    images_len = 0;
    for(auto& f : std::filesystem::directory_iterator(path)) {
      images_len++;
      paths.push_back(f.path());
    }
    std::cerr << "Open " << images_len << " file" << std::endl;

    std::sort(paths.begin(), paths.end());

    heights = new int[images_len];
    widths = new int[images_len];
    images = new GLuint*[images_len];

    for(int i = 0; i < images_len; i++) {

      std::unique_ptr<imebra::DataSet>
          imebra_data_set (imebra::CodecFactory::load(paths[i]));

      std::unique_ptr<imebra::Image>
          image (imebra_data_set->getImage(0));

      std::string color_space = image->getColorSpace();

      std::cerr << "color_space: " << color_space << std::endl;

      widths[i] = image->getWidth();
      heights[i] = image->getHeight();
      images[i] = new GLuint[widths[i] * heights[i]];

      std::cerr << "width: " << widths[i] << ", height: " << heights[i]
          << std::endl;

      std::unique_ptr<imebra::ReadingDataHandlerNumeric>
          data_handler (image->getReadingDataHandler());

      load_image(image.get(), data_handler.get(), i);
    }
  }
  std::cerr << "-----------------------------" << std::endl;
  std::cerr << "Images size: "
      << images_len * widths[0] * heights[0] * 4.0 / 1024.0 / 1024.0
      << "MB" << std::endl;
}

void Dicom_reader::load_image(const imebra::Image* image,
    const imebra::ReadingDataHandlerNumeric* data_handler, int i)
{
  using t = std::uint32_t;
  t luminance, r, g, b, j = 0;
  if (imebra::ColorTransformsFactory::isMonochrome(image->getColorSpace())) {
    std::cerr << "Image size: " <<
        widths[i] * heights[i] * 4.0 / 1024.0 / 1024.0 << "MB" << std::endl;
    for(t y = 0; y < heights[i]; y++) {
      for(t x = 0; x < widths[i]; x++) {
        luminance = data_handler->getSignedLong(y * widths[0] + x);
        images[i][j++] = luminance * 10'000'000; // 10 млн или ничего не видно
      }
    }
  } else {
    printf("Не судьба...\n");
  }
}

void Dicom_reader::gen_texs()
{
  texs = new GLuint[images_len];

  for(int i = 0; i < images_len; i++) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // что это?

    glGenTextures(1, &texs[i]);
    glBindTexture(GL_TEXTURE_2D, texs[i]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widths[i], heights[i],
                 0, GL_LUMINANCE, GL_UNSIGNED_INT, images[i]);
  }
}

void Dicom_reader::draw_tex(int i)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glBindTexture(GL_TEXTURE_2D, texs[i]);

  glPushMatrix();

  glTranslatef((screen_width_k - 1.0f) / 2.0f, (screen_height_k - 1.0f) / 2.0f,
               0.0f);

  glBegin(GL_QUADS);

  glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
  glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 1.0f, 0.0f);
  glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);
  glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);

  glEnd();

  glPopMatrix();

  glDisable(GL_TEXTURE_2D);
}

class GLPane : public wxGLCanvas
{
public:
  wxGLContext* gl_context;

  GLPane(wxFrame* parent, int* args);
  virtual ~GLPane();

  void resized(wxSizeEvent& evt);

  int getWidth();
  int getHeight();

  void render(wxPaintEvent& evt);

  DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(GLPane, wxGLCanvas)
EVT_PAINT(GLPane::render)
EVT_SIZE(GLPane::resized)
END_EVENT_TABLE()

inline void screen_k_update(GLPane* glPane)
{
  screen_width_k = (glPane->getWidth() > glPane->getHeight())
      ? (float)glPane->getWidth() / glPane->getHeight()
      : 1.0f;
  screen_height_k = (glPane->getHeight() > glPane->getWidth())
      ? (float)glPane->getHeight() / glPane->getWidth()
      : 1.0f;
}

GLPane::GLPane(wxFrame* parent, int* args)
  : wxGLCanvas(parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize,
               wxFULL_REPAINT_ON_RESIZE)
{
	gl_context = new wxGLContext(this);

  // To avoid flashing on MSW
  SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

GLPane::~GLPane()
{
	delete gl_context;
}

void GLPane::resized(wxSizeEvent& evt)
{
  screen_k_update(this);
}

int GLPane::getWidth()
{
  return GetSize().x;
}

int GLPane::getHeight()
{
  return GetSize().y;
}

void GLPane::render(wxPaintEvent& evt)
{
  if(!IsShown()) return;

  wxGLCanvas::SetCurrent(*gl_context);
  /* only to be used in paint events. use wxClientDC to paint outside the
   * paint event */
  wxPaintDC(this);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  dicom_reader.draw_tex(cur_image);

  glFlush();
  SwapBuffers();

  Refresh();
}

class App : public wxApp
{
  virtual bool OnInit();

  wxFrame* frame;
  GLPane* glPane;

public:
  void keyPressed(wxKeyEvent& event);

  DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(App, wxApp)
EVT_KEY_DOWN(App::keyPressed)
END_EVENT_TABLE()

IMPLEMENT_APP(App)

void App::keyPressed(wxKeyEvent& event)
{
  switch(event.GetKeyCode()) {
    case WXK_UP:
      cur_image++;
      break;
    case WXK_DOWN:
      cur_image--;
      break;
  }
}

bool App::OnInit()
{
  frame = new wxFrame((wxFrame *)NULL, -1,  wxT("osdicom"),
                      wxPoint(50,50), wxSize(400,200));

  wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

  int args[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};
  glPane = new GLPane((wxFrame*)frame, args);

  sizer->Add(glPane, 1, wxEXPAND);

  frame->SetSizer(sizer);
  frame->SetAutoLayout(true);

  frame->CreateStatusBar();
  frame->SetStatusText(_T("Hello World"));

  frame->Show();

  /* opengl init */
  glPane->SetCurrent(*glPane->gl_context);

  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  glDisable(GL_POLYGON_SMOOTH);
  glDisable(GL_LINE_SMOOTH);
  glEnable(GL_MULTISAMPLE);
  glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);

  screen_k_update(glPane);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0f, screen_width_k, 0.0f, screen_height_k);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glViewport(0, 0, glPane->getWidth(), glPane->getHeight());

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  /* end opengl init */

  /* init */
  dicom_reader.load("samples/lee/brain_dicom/");
  dicom_reader.gen_texs();
  /* end init */

  return true;
}
