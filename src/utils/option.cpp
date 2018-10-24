//////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2013 by sanpolo CO.LTD                                    //
//                                                                          //
//  This file is part of WIDE                                               //
//                                                                          //
//  You should have received a copy of the MIT LIcense along with this     //
//  program.  If not, see <http://www.wware.org/wide/license.html>.         //
//                                                                          //
//  WIDE website: http://www.wware.org/                                     //
//////////////////////////////////////////////////////////////////////////////

#include "config.h"
#include "utils/i18n.h"
#include "utils/modulepath.h"
#include "utils/option.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/foreach.hpp>
#include <boost/locale.hpp>
#include <boost/program_options.hpp>
#include <boost/scope_exit.hpp>
#include <boost/unordered_map.hpp>

#if WIN32
#include <conio.h>
#include <fcntl.h>
#include <fstream>
#include <io.h>
#include <stdio.h>
#include <windows.h>
#endif

static void ensureConsole(void) {
#if WIN32
  static bool inited = false;
  static const WORD MAX_CONSOLE_LINES = 500;
  if (!inited) {
    int hConHandle;
    long lStdHandle;
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    FILE *fp;
    // allocate a console for this app
    AllocConsole();
    // set the screen buffer to be big enough to let us scroll text
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    coninfo.dwSize.Y = MAX_CONSOLE_LINES;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
    // redirect unbuffered STDOUT to the console
    lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "w");
    *stdout = *fp;
    setvbuf(stdout, NULL, _IONBF, 0);
    // redirect unbuffered STDIN to the console
    lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "r");
    *stdin = *fp;
    setvbuf(stdin, NULL, _IONBF, 0);
    // redirect unbuffered STDERR to the console
    lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "w");
    *stderr = *fp;
    setvbuf(stderr, NULL, _IONBF, 0);
    // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
    // point to console as well
    std::ios::sync_with_stdio();
    inited = true;
  }
#endif
}

static void waitPress(void) {
#if WIN32
  std::cout << __("press any key to quit...") << std::endl;
  ::getch();
#endif
}

namespace wide {

static inline void outCopyRights(void) {
  std::cout << __("(C) copyright 2013-2018 SanPolo Co.LTD") << std::endl;
  std::cout << __("This is Property software; see the source for copying "
                  "conditions.")
            << std::endl;
  std::cout << __("warranty; not even for MERCHANTABILITY or FITNESS FOR A "
                  "PARTICULAR PURPOSE.")
            << std::endl;
  std::cout << __("office website: http://www.wware.org/") << std::endl;
  std::cout << __("Written by masol.lee<masol.li@gmail.com>. ") << std::endl;
}

static inline void outUsage(void) {
  std::cout << "Usage: wide [options] [project-directory]" << std::endl;
}

static inline void
initOption_descritpion(boost::program_options::options_description &cmdline,
                       boost::program_options::options_description &generic) {
  cmdline.add_options()("help,h", ("Display this information."))(
      "version,v", ("Display  version informationl."))(
      "config,c", boost::program_options::value<std::string>(),
      ("Specify config file."))("source,s",
                                boost::program_options::value<std::string>(),
                                ("Specify source scene file."))(
      "lang,l", boost::program_options::value<std::string>(),
      ("Set the language."));
}

struct parser_context {
  boost::program_options::options_description &cmdline_options;
  std::vector<std::string> sources;
  Option &option;
  std::vector<std::string> outfile;
  std::vector<std::string> cameras;
  std::string lang;
  parser_context(boost::program_options::options_description &clo, Option &o)
      : cmdline_options(clo), option(o) {}
};

static inline bool loadConfigFile(const std::string &cfgFile,
                                  parser_context &ctx);
static bool parser_Option(boost::program_options::parsed_options &option,
                          parser_context &ctx) {
  bool bNeedQuit = false;
  std::vector<boost::program_options::option>::iterator it =
      option.options.begin();
  std::string window_tag("window.");
  std::string system_tag("system.");

  std::vector<std::string> configFile;
  while (it != option.options.end()) {
    if (it->unregistered) { //发现一个未注册的配置选项。
      // TODO: more flexible mechanism to extend config.
      if (it->string_key.compare(0, window_tag.length(), window_tag) == 0) {
        std::vector<std::string>::iterator value_it = it->value.begin();
        while (value_it != it->value.end()) {
          ctx.option.put(it->string_key, *value_it);
          value_it++;
        }
      } else if (it->string_key.compare(0, system_tag.length(), system_tag) ==
                 0) {
        std::vector<std::string>::iterator value_it = it->value.begin();
        while (value_it != it->value.end()) {
          ctx.option.put(it->string_key.substr(system_tag.length()), *value_it);
          value_it++;
        }
      } else {
        ensureConsole();
        outCopyRights();
        std::cout << __("invalid parameter ") << it->string_key << std::endl;
        outUsage();
        std::cout << ctx.cmdline_options << std::endl;
        bNeedQuit = true;
        waitPress();
        break;
      }
    } else if (it->string_key == "config") {
      if (it->value.size() == 0) {
        ensureConsole();
        outCopyRights();
        std::cout << __("config need a config file as parameter") << std::endl;
        outUsage();
        waitPress();
        bNeedQuit = true;
        break;
      }
      configFile = it->value;
      std::vector<std::string>::iterator cfgit = configFile.begin();
      bool bcfgFileOK = true;
      while (cfgit != configFile.end()) {
        if (!boost::filesystem::exists(*cfgit)) {
          ensureConsole();
          outCopyRights();
          std::cout << __("specified config file ") << *cfgit
                    << __(" doesn't exist") << std::endl;
          bcfgFileOK = false;
          bNeedQuit = true;
          waitPress();
          break;
        }
        cfgit++;
      }
      if (!bcfgFileOK) {
        break;
      }
    } else if (it->string_key == "help") {
      ensureConsole();
      outCopyRights();
      outUsage();
      std::cout << ctx.cmdline_options << std::endl;
      bNeedQuit = true;
      waitPress();
      break;
    } else if (it->string_key == "version") {
      ensureConsole();
      outCopyRights();
      std::cout << __("wide Version ") << WIDE_VERSION_MAJOR << '.'
                << WIDE_VERSION_MINOR << std::endl;
      bNeedQuit = true;
      waitPress();
      break;
    } else if (it->string_key == "source") {
      std::vector<std::string>::iterator value_it = it->value.begin();
      while (value_it != it->value.end()) {
        if (std::find(ctx.sources.begin(), ctx.sources.end(), *value_it) ==
            ctx.sources.end()) {
          ctx.sources.push_back(*value_it);
        }
        value_it++;
      }
    } else if (it->string_key == "output") {
      std::vector<std::string>::iterator value_it = it->value.begin();
      while (value_it != it->value.end()) {
        if (std::find(ctx.outfile.begin(), ctx.outfile.end(), *value_it) ==
            ctx.outfile.end()) {
          ctx.outfile.push_back(*value_it);
        }
        value_it++;
      }
    } else if (it->string_key == "lang") {
      if (it->value.size() > 0) {
        ctx.lang = *(it->value.begin());
      }
    } else if (it->string_key == "pass") {
      if (it->value.size() > 0) {
        ctx.option.put("pass", boost::lexical_cast<long>(*(it->value.begin())));
      }
    } else if (it->string_key == "camera") {
      std::vector<std::string>::iterator value_it = it->value.begin();
      while (value_it != it->value.end()) {
        if (std::find(ctx.cameras.begin(), ctx.cameras.end(), *value_it) ==
            ctx.cameras.end()) {
          ctx.cameras.push_back(*value_it);
        }
        value_it++;
      }
    }
    it++;
  }

  //开始分析和保存配置文件的内容。
  if (!bNeedQuit && configFile.size()) {
    std::vector<std::string>::iterator cfgit = configFile.begin();
    while (cfgit != configFile.end()) {
      if (loadConfigFile(*cfgit, ctx)) {
        bNeedQuit = true;
        break;
      }
      cfgit++;
    }
  }

  return bNeedQuit;
}

static inline bool loadConfigFile(const std::string &cfgFile,
                                  parser_context &ctx) {
  boost::filesystem::ifstream input(cfgFile.c_str(), std::ios_base::in);
  BOOST_SCOPE_EXIT((&input)) { input.close(); }
  BOOST_SCOPE_EXIT_END

  bool bNeedQuit = false;
  if (input.is_open()) {
    std::cout << __("loading config file ") << cfgFile << " ...";
    boost::program_options::parsed_options po =
        boost::program_options::parse_config_file(input, ctx.cmdline_options,
                                                  true);
    bNeedQuit = parser_Option(po, ctx);
    std::cout << __("done!") << std::endl;
  } else {
    ensureConsole();
    outCopyRights();
    std::cout << __("can not open config file ") << cfgFile << std::endl;
    outUsage();
    waitPress();
    bNeedQuit = true;
  }
  return bNeedQuit;
}

//@TODO: implement_it.
void Option::loadConfigurationFile(void){

}


bool Option::initFromArgs(int argc, char **argv) {
  // fix path in non-utf8 platform.
  {
    std::locale global_loc = std::locale();
    std::locale loc(global_loc,
                    new boost::filesystem::detail::utf8_codecvt_facet);
    // std::locale::global(loc);
    // std::locale::global(boost::locale::generator().generate(""));
    boost::filesystem::path::imbue(loc);
  }

  wide::ModulePath::instance().initialize(argv[0]);

  boost::program_options::options_description cmdline;
  boost::program_options::options_description generic;

  initOption_descritpion(cmdline, generic);

  bool bNeedQuit = false;
  boost::program_options::options_description cmdline_options;
  cmdline_options.add(cmdline).add(generic);

  boost::program_options::positional_options_description p;
  p.add("source", -1);
  boost::program_options::parsed_options po =
      boost::program_options::command_line_parser(argc, argv)
          .options(cmdline_options)
          .positional(p)
          .allow_unregistered()
          .run();

  parser_context ctx(cmdline_options, *this);
  bNeedQuit = parser_Option(po, ctx);
  if (!bNeedQuit) { // system runing, set the source and config log system.
    if (ctx.sources.size()) {
      this->put("document.source", ctx.sources);
    }
    if (ctx.outfile.size()) {
      this->put("document.output", ctx.outfile);
    }
    if (ctx.cameras.size()) {
      this->put("document.camera", ctx.cameras);
    }
    //这里检查是否配置了语言选项。
    if (!ctx.lang.empty()) {
      I18n::instance().setLocale(ctx.lang);
    }
  }

  return !bNeedQuit;
}

} // namespace wide
