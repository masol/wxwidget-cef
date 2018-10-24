#include "config.h"
#include "precomp.h"
#include "ui/chromiumcontrol.h"
#include "ui/frame.h"

namespace wide {

BEGIN_EVENT_TABLE(Frame, inherited)
EVT_CLOSE(Frame::onClose)
END_EVENT_TABLE()

Frame::Frame(wxWindow *parent)
    : wxFrame(parent, -1, _("WIDE"), wxDefaultPosition, wxSize(800, 600),
              wxDEFAULT_FRAME_STYLE) {
  // ctor

  std::cout << "enter frame.." << std::endl;
  DECLARE_WXCONVERT;
  // notify wxAUI which frame to use
  m_mgr.SetManagedWindow(this);
  std::cout << "enter frame 2.." << std::endl;

  // create several text controls
  wxTextCtrl *text1 =
      new wxTextCtrl(this, -1, _("Pane 1 - sample text"), wxDefaultPosition,
                     wxSize(200, 150), wxNO_BORDER | wxTE_MULTILINE);

  //*/
  /*
  wxChromiumControl *text1 =
      new wxChromiumControl(this, -1, _("http://baidu.com"),wxDefaultPosition,
                     wxSize(200, 150), wxNO_BORDER | wxTE_MULTILINE);
  //*/
  m_mgr.AddPane(text1, wxLEFT);
  //m_mgr.Update();
}

Frame::~Frame() {
  // dtor
}

void
Frame::onCefInited()
{
  wxChromiumControl *text1 =
      new wxChromiumControl(this, -1, _("http://baidu.com"),wxDefaultPosition,
                     wxSize(200, 150), wxNO_BORDER | wxTE_MULTILINE);
  m_mgr.AddPane(text1, wxCENTER);
  m_mgr.Update();
}


/** @brief 这里是真正的退出实现。所有清理工作在这里执行。
 **/
void Frame::onClose(wxCloseEvent &event) {
  // deinitialize the frame manager
  /*
m_mgr.UnInit();
  Destroy();
wxTheApp->ExitMainLoop();
exit(0);
  event.Skip(false);
  */
  Destroy();
}

} // namespace wide
