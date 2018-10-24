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
#include "utils/sysinfo.h"

#if WIN32
#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
#else
#include <string.h>
#endif

namespace wide {

int SysInfo::GetSystemIdleMicroSecond(void) {
  int ret = -1;
#if WIN32
  LASTINPUTINFO last_input;
  // BOOL screensaver_active;
  if( GetLastInputInfo(&last_input) /*&& SystemParametersInfo(SPI_GETSCREENSAVERACTIVE, 0, &screensaver_active, 0)*/ )
	{
    ret = last_input.dwTime;
  }
#else
#endif
  return ret;
}

#ifdef WIN32
std::string SysWideToNativeMB(const std::wstring &wide) {
  unsigned int code_page = CP_ACP;
  int wide_length = static_cast<int>(wide.length());
  if (wide_length == 0)
    return std::string();

  // Compute the length of the buffer we'll need.
  int charcount = WideCharToMultiByte(code_page, 0, wide.data(), wide_length,
                                      NULL, 0, NULL, NULL);
  if (charcount == 0)
    return std::string();

  std::string mb;
  mb.resize(charcount);
  WideCharToMultiByte(code_page, 0, wide.data(), wide_length, &mb[0], charcount,
                      NULL, NULL);

  return mb;
}

std::wstring SysNativeMBToWide(const std::string &mb) {
  unsigned int code_page = CP_ACP;
  if (mb.empty())
    return std::wstring();

  int mb_length = static_cast<int>(mb.length());
  // Compute the length of the buffer.
  int charcount =
      MultiByteToWideChar(code_page, 0, mb.data(), mb_length, NULL, 0);
  if (charcount == 0)
    return std::wstring();

  std::wstring wide;
  wide.resize(charcount);
  MultiByteToWideChar(code_page, 0, mb.data(), mb_length, &wide[0], charcount);

  return wide;
}

#else

std::string SysWideToNativeMB(const std::wstring &wide) {
  mbstate_t ps;
  // Calculate the number of multi-byte characters.  We walk through the string
  // without writing the output, counting the number of multi-byte characters.
  size_t num_out_chars = 0;
  memset(&ps, 0, sizeof(ps));
  for (size_t i = 0; i < wide.size(); ++i) {
    const wchar_t src = wide[i];
    // Use a temp buffer since calling wcrtomb with an output of NULL does not
    // calculate the output length.
    char buf[16];
    // Skip NULLs to avoid wcrtomb's special handling of them.
    size_t res = src ? wcrtomb(buf, src, &ps) : 0;
    switch (res) {
    // Handle any errors and return an empty string.
    case static_cast<size_t>(-1):
      return std::string();
      break;
    case 0:
      // We hit an embedded null byte, keep going.
      ++num_out_chars;
      break;
    default:
      num_out_chars += res;
      break;
    }
  }

  if (num_out_chars == 0)
    return std::string();

  std::string out;
  out.resize(num_out_chars);

  // We walk the input string again, with |i| tracking the index of the
  // wide input, and |j| tracking the multi-byte output.
  memset(&ps, 0, sizeof(ps));
  for (size_t i = 0, j = 0; i < wide.size(); ++i) {
    const wchar_t src = wide[i];
    // We don't want wcrtomb to do its funkiness for embedded NULLs.
    size_t res = src ? wcrtomb(&out[j], src, &ps) : 0;
    switch (res) {
    // Handle any errors and return an empty string.
    case static_cast<size_t>(-1):
      return std::string();
      break;
    case 0:
      // We hit an embedded null byte, keep going.
      ++j; // Output is already zeroed.
      break;
    default:
      j += res;
      break;
    }
  }

  return out;
}

std::wstring SysNativeMBToWide(const std::string &native_mb) {
  mbstate_t ps;

  // Calculate the number of wide characters.  We walk through the string
  // without writing the output, counting the number of wide characters.
  size_t num_out_chars = 0;
  memset(&ps, 0, sizeof(ps));
  for (size_t i = 0; i < native_mb.size();) {
    const char *src = native_mb.data() + i;
    size_t res = mbrtowc(NULL, src, native_mb.size() - i, &ps);
    switch (res) {
    // Handle any errors and return an empty string.
    case static_cast<size_t>(-2):
    case static_cast<size_t>(-1):
      return std::wstring();
      break;
    case 0:
      // We hit an embedded null byte, keep going.
      i += 1; // Fall through.
    default:
      i += res;
      ++num_out_chars;
      break;
    }
  }

  if (num_out_chars == 0)
    return std::wstring();

  std::wstring out;
  out.resize(num_out_chars);

  memset(&ps, 0, sizeof(ps)); // Clear the shift state.
  // We walk the input string again, with |i| tracking the index of the
  // multi-byte input, and |j| tracking the wide output.
  for (size_t i = 0, j = 0; i < native_mb.size(); ++j) {
    const char *src = native_mb.data() + i;
    wchar_t *dst = &out[j];
    size_t res = mbrtowc(dst, src, native_mb.size() - i, &ps);
    switch (res) {
    // Handle any errors and return an empty string.
    case static_cast<size_t>(-2):
    case static_cast<size_t>(-1):
      return std::wstring();
      break;
    case 0:
      i += 1; // Skip null byte.
      break;
    default:
      i += res;
      break;
    }
  }

  return out;
}

#endif

} // namespace wide
