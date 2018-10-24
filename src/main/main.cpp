#include "config.h"
#include "precomp.h"

#include "include/cef_app.h"
#include "ui/cefbase.h"
#include "ui/frame.h"
#include "utils/cef.h"
#include "utils/modulepath.h"
#include "utils/option.h"
#include "utils/widexception.h"

#include <boost/scope_exit.hpp>

class MyApp : public wxApp, public CefApp, public CefBrowserProcessHandler {
  typedef wxApp inherit;

protected:
  boost::shared_ptr<wide::Frame> m_mainFrame;

  bool StartUpCEF(int &code, const std::string &path, CefMainArgs &args);
  static void ShutdownCEF(void);

public:
  MyApp() : m_mainFrame(NULL) {}
  ~MyApp() {}
  int OnExit() {
    m_mainFrame.reset();
    wide::exception::removeTranslator();
    ShutdownCEF();
    return inherit::OnExit();
  }

  // CefApp methods:
  virtual CefRefPtr<CefBrowserProcessHandler>
  GetBrowserProcessHandler() OVERRIDE {
    return this;
  }

  bool OnInit();

  // CefBrowserProcessHandler methods:
  virtual void OnContextInitialized() OVERRIDE;

protected:
#ifndef OS_WIN
  void onIdle(wxIdleEvent &event) {
    // std::cout << "event idle" << std::endl;
    CefDoMessageLoopWork();
    event.RequestMore(true);
  }
#endif

private:
  void createFrame(void);
  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(MyApp)
};

DECLARE_APP(MyApp)
IMPLEMENT_APP(MyApp)

bool MyApp::OnInit() {

  if (!wxApp::OnInit()) {
    return false;
  }
  //设置异常翻译。
  wide::exception::installTranslator();

  // initionlize config.
  ///@FIXME:
  ///根据valgrind报告，如果使用wx提供的argv自动转化，其数据在进程销毁时才会清理。因此下文自行处理utf8转化。
  {
    typedef char *str_point;
    str_point *mb_args = new str_point[argc];
    wxMBConvUTF8 wide_wx_utf8_conv;
    for (int i = 0; i < argc; i++) {
      wxString string(argv[i]);
#if defined(OS_WIN)
      if (i == 0) { //检查是否是全路径。如果不是，需要获取全路径。
        const char *path = string.mb_str();
        if (path[1] == ':' && (path[2] == '/' || path[2] == '\\')) {
          mb_args[i] = strdup(string.mb_str());
        } else {
          //不是一个全路径。获取之。
          TCHAR fullpath[1024];
          if (GetModuleFileName(NULL, fullpath, 1024) > 0) {
            wxString temp(fullpath);
            mb_args[i] = strdup(temp.mb_str(wide_wx_utf8_conv));
          } else {
            mb_args[i] = strdup(string.mb_str(wide_wx_utf8_conv));
          }
        }
      } else {
        mb_args[i] = strdup(string.mb_str(wide_wx_utf8_conv));
      }
#else
      mb_args[i] = strdup(string.mb_str());
#endif
    }
    BOOST_SCOPE_EXIT((&mb_args)(&argc)) {
      for (int i = 0; i < argc; i++) {
        free(const_cast<char *>(mb_args[i]));
      }
      delete[] mb_args;
    }
    BOOST_SCOPE_EXIT_END
    // init options from command line.
    if (!wide::Option::instance().initFromArgs(argc, mb_args)) {
      return false;
    }
    std::cout << mb_args[0] << std::endl;
    wide::ModulePath::instance().initialize(mb_args[0]);
    // if not quit,then load config yaml file. not overwrite current options.
    wide::Option::instance().loadConfigurationFile();

    boost::filesystem::path subProc = wide::ModulePath::instance().modulePath();
#if defined(OS_WIN)
    subProc /= "subproc.exe";
    CefMainArgs args(wxGetInstance());
#else
    subProc /= "subproc";
    CefMainArgs args(argc, mb_args);
#endif
    int code = 0;
    std::cout << subProc << std::endl;

    createFrame();
    if (!StartUpCEF(code, subProc.string(), args)) {
      std::cout << "start cef failed!!" << std::endl;
      exit(code);
    }
    std::cout << "start cef success!!" << std::endl;
  }

  Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(MyApp::onIdle));

  std::cout << "try to create frame 12:" << IsMainLoopRunning() << std::endl;

  /*
      CefBrowserSettings browsersettings;
      CefWindowInfo info;
      wxString url("about:blank");
      ClientHandler *m_clientHandler = new ClientHandler();
      CefBrowserHost::CreateBrowser(
          info, static_cast<CefRefPtr<CefClient>>(m_clientHandler),
          url.ToStdString(), browsersettings, NULL);

          //*/

  // wxInitAllImageHandlers();
  // before init openCL,we must init openGL first.

  /*
      std::string source("document.source");
      if (wide::Option::instance().is_existed(source)) {
        wide::DocIO docio;
        std::vector<std::string> srcset =
            wide::Option::instance().get<std::vector<std::string>>(source);
        std::vector<std::string>::iterator it = srcset.begin();
        int idx = 0;
        while (it != srcset.end()) {
          if (docio.loadScene(*it))
            break;
          it++;
          idx++;
        }
        m_mainFrame->setOpenFromCommandLine(idx);
      }
  */
  return true;
}

void MyApp::createFrame(void) {
  std::cout << "try to create frame" << std::endl;
  // Yield();

  m_mainFrame.reset(new wide::Frame(NULL));
  std::cout << "main fraim create " << std::endl;
  SetTopWindow(m_mainFrame.get());
  std::cout << "main fraim shown " << std::endl;
}

void MyApp::OnContextInitialized() {
  // CEF_REQUIRE_UI_THREAD();

  std::cout << "context inited" << std::endl;

  m_mainFrame->onCefInited();
  m_mainFrame->Maximize();
  m_mainFrame->Show();

  // createFrame();
  /*
    CefRefPtr<CefCommandLine> command_line =
        CefCommandLine::GetGlobalCommandLine();

  #if defined(OS_WIN) || defined(OS_LINUX)
    // Create the browser using the Views framework if "--use-views" is
  specified
    // via the command-line. Otherwise, create the browser using the native
    // platform framework. The Views framework is currently only supported on
    // Windows and Linux.
    const bool use_views = command_line->HasSwitch("use-views");
  #else
    const bool use_views = false;
  #endif

    // SimpleHandler implements browser-level callbacks.
    CefRefPtr<SimpleHandler> handler(new SimpleHandler(use_views));

    // Specify CEF browser settings here.
    CefBrowserSettings browser_settings;

    std::string url;

    // Check if a "--url=" value was provided via the command-line. If so, use
    // that instead of the default URL.
    url = command_line->GetSwitchValue("url");
    if (url.empty())
      url = "http://www.google.com";

    if (use_views) {
      // Create the BrowserView.
      CefRefPtr<CefBrowserView> browser_view =
  CefBrowserView::CreateBrowserView( handler, url, browser_settings, NULL,
  NULL);

      // Create the Window. It will show itself after creation.
      CefWindow::CreateTopLevelWindow(new SimpleWindowDelegate(browser_view));
    } else {
      // Information used when creating the native window.
      CefWindowInfo window_info;

  #if defined(OS_WIN)
      // On Windows we need to specify certain flags that will be passed to
      // CreateWindowEx().
      window_info.SetAsPopup(NULL, "cefsimple");
  #endif

      // Create the first browser window.
      CefBrowserHost::CreateBrowser(window_info, handler, url, browser_settings,
                                    NULL);
    }
    //*/
}

bool MyApp::StartUpCEF(int &code, const std::string &path, CefMainArgs &args) {
  CefSettings settings;
  // We use a multithreaded message loop so we don't have to integrate
  // with the wx message loop. on work under windowx platform.
  //#if !defined(CEF_USE_SANDBOX)
  settings.no_sandbox = true;
  //#endif
#if defined(OS_WIN)
  settings.multi_threaded_message_loop = true;
#else
  settings.multi_threaded_message_loop = false;
#endif
  CefString(&settings.browser_subprocess_path) = path;

  CefRefPtr<CefApp> app(dynamic_cast<CefApp *>(this));
  bool bRet = CefInitialize(args, settings, app.get(), NULL);
  // std::cout << "run here:" << bRet << std::endl;
  // CefDoMessageLoopWork();
  // CefRunMessageLoop();
  // std::cout << "run loop not block." << std::endl;
  return bRet;
}

void MyApp::ShutdownCEF(void) { CefShutdown(); }
