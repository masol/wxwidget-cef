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


#include "config.h"
#include <string>
#include <Windows.h>
#include "Win32Exception.h"


Win32Exception::Win32Exception(EXCEPTION_POINTERS * info) {
	_location = info->ExceptionRecord->ExceptionAddress;
	_code = info->ExceptionRecord->ExceptionCode;
	_info = info;
	switch (_code) {
		case EXCEPTION_ACCESS_VIOLATION:
			_event = "Access violation";
			break;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			_event = "Division by zero";
			break;
		default:
			_event = "Unlisted exception";
	}
}

void Win32Exception::translate(unsigned code, EXCEPTION_POINTERS * info) {
	// Windows guarantees that *(info->ExceptionRecord) is valid
	switch (code) {
		case EXCEPTION_ACCESS_VIOLATION:
			throw Win32AccessViolation(info);
			break;
		default:
			throw Win32Exception(info);
	}
}

Win32AccessViolation::Win32AccessViolation(EXCEPTION_POINTERS * info) : Win32Exception(info) {
	_isWrite = info->ExceptionRecord->ExceptionInformation[0] == 1;
	_badAddress = reinterpret_cast<ExceptionAddress>(info->ExceptionRecord->ExceptionInformation[1]);
}

