//////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2013 by sanpolo CO.LTD                                    //
//                                                                          //
//  This file is part of GMEditor                                           //
//                                                                          //
//  GMEditor is free software; you can redistribute it and/or modify it     //
//  under the terms of the LGPL License.                                    //
//                                                                          //
//  GMEditor is distributed in the hope that it will be useful,but WITHOUT  //
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY      //
//  or FITNESS FOR A PARTICULAR PURPOSE.                                    //
//                                                                          //
//  You should have received a copy of the LGPL License along with this     //
//  program.  If not, see <http://www.render001.com/gmeditor/licenses>.     //
//                                                                          //
//  GMEditor website: http://www.render001.com/gmeditor                     //
//////////////////////////////////////////////////////////////////////////////

/* the orignal code is from 
 * http://www.thunderguy.com/semicolon/2002/08/15/visual-c-exception-handling/3/
 * (Visual C++ exception handling)
 * By Bennett
*/

#ifndef  GME_UTILS_WIN32_WIN32EXCEPTION_H
#define  GME_UTILS_WIN32_WIN32EXCEPTION_H

#include <windows.h>

typedef const void* ExceptionAddress; // OK on Win32 platform

namespace gme
{
    struct exception;
}

class Win32Exception : public std::exception
{
    friend struct gme::exception;
public:
    virtual const char* what()  const throw() { return _event;    };
    ExceptionAddress 	where() const         { return _location; };
    unsigned int		code()  const         { return _code;     };
	EXCEPTION_POINTERS* info()  const         { return _info;     };
	
protected:
	//Constructor only accessible by exception handler
    Win32Exception(EXCEPTION_POINTERS * info);
    static void 		translate(unsigned code, EXCEPTION_POINTERS * info);

private:
    const char * _event;
    ExceptionAddress _location;
    unsigned int _code;

	EXCEPTION_POINTERS * _info;
};

class Win32AccessViolation: public Win32Exception
{
public:
    bool 				isWrite()    const { return _isWrite;    };
    ExceptionAddress	badAddress() const { return _badAddress; };
private:
    Win32AccessViolation(EXCEPTION_POINTERS * info);

    bool _isWrite;
    ExceptionAddress _badAddress;

    friend void Win32Exception::translate(unsigned code, EXCEPTION_POINTERS* info);
};
#endif //GME_UTILS_WIN32_WIN32EXCEPTION_H
