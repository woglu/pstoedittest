#ifndef cppcomp_h
#define cppcomp_h

//disable warning about copyleft from PVS studio.
//-V::1042

//{
/*
   cppcomp.h : This file is part of pstoedit
   header declaring compiler dependent stuff

   Copyright (C) 1998 - 2024 Wolfgang Glunz, wglunz35_AT_pstoedit.net

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/
#define MSVC_CHECK_LEAKS 0
// for development only
// needs a bit more work to handle also new - see 
// https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2015
#if MSVC_CHECK_LEAKS
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  
#endif

#ifdef HAVE_CONFIG_H
	#include "pstoedit_config.h"
#endif

#ifdef _MSC_VER
	#ifndef DLLEXPORT
	#define DLLEXPORT __declspec( dllexport )
	#endif
#else
	#define DLLEXPORT
#endif

#ifdef _AIX
	#define _unix
#endif

#if defined (_MSC_VER) && (_MSC_VER >= 1100) 
	// MSVC 5 and 6 have ANSI C++ header files, but the compilation
	// is much slower and object files get bigger. 
	// add other compiler that support STL and the ANSI C++ standard here
	// 1100 means : compiler v 11 which is MSVC 5
	// 1200 - VS 6
	// 1300 - VS 7 (2002)
	// 1400 - VS 8 (2005)
	// 1900 - VS 2015

        // Visual Studio 2008 does not have nullptr
        #if (_MSC_VER <= 1400)
          #define nullptr NULL
        #endif

	// NOTE: If your compiler or installation does not come with
	// an installation of the STL, just comment out the next line

	#define HAVE_TYPENAME

	#define USE_NEWSTRSTREAM

	#define HAVE_AUTOPTR

	#if (_MSC_VER >= 1900) 
		#define _ALLOW_RTCc_IN_STL
	#endif

#endif


#if defined (__GNUG__)  && (__GNUC__>=3)

	#define HAVE_TYPENAME
	#define HAVE_AUTOPTR
	#define USE_NEWSTRSTREAM

#endif

#ifdef __SUNPRO_CC
	#if __SUNPRO_CC >= 0x500
		#define HAVE_TYPENAME
		#if __SUNPRO_CC_COMPAT >= 5
// SUN's new compiler seems to work with STL (only)
			#define HAVE_AUTOPTR
			#ifndef INTERNALBOOL
				#define INTERNALBOOL
			#endif
		#else
// we compile with -compat=4 
			#ifdef INTERNALBOOL
				#undef INTERNALBOOL
			#endif
		#endif
	#else
// for the old version (CC 4.x)
	#endif
#endif     

#ifndef HAVE_TYPENAME
	#define typename 
#endif

//
// Note: for MSVC the usage of precompiled headers has to be turned 
// off, otherwise compilation fails within the stdio. (the first time
// it works, but not a second one) (at least with MSVC 5)
//


#define I_fstream		<fstream>
#define I_iomanip		<iomanip>
#define I_ios			<ios>
#define I_iostream		<iostream>

#if (defined (_MSC_VER) && _MSC_VER >= 1100)
// 1100 is version 5.0
#pragma warning(disable: 4786)
#define I_istream       <istream>
#define I_ostream       <ostream>
#define I_streamb       <streambuf>
#else
#define I_istream       <iostream>
#define I_ostream       <iostream>
#define I_streamb       <iostream>
#endif

#ifdef  USE_NEWSTRSTREAM
#define I_strstream 	<sstream>
#define C_istrstream std::stringstream
#define C_ostrstream std::ostringstream
#else
#define I_strstream 	<strstream>
#define C_istrstream istrstream
#define C_ostrstream ostrstream
#endif


#define I_stdio			<cstdio>
#define I_stdlib		<cstdlib>

// #define USESTD using namespace std;
#define USESTD using std::cout; using std::cerr; using std::ios;  using std::ofstream; using std::ifstream; using std::ostream; using std::istream; using std::endl; using std::cin; 
//using std::string;


#define NOCOPYANDASSIGN(classname) \
	private: \
		classname(const classname&) = delete; \
		const classname & operator=(const classname&) = delete;

//lint -esym(665, NOCOPYANDASSIGN)
//classname not in ()


// rcw2: work round case insensitivity in RiscOS
#ifdef riscos
  #define I_string_h "unix:string.h"
#else
  #define I_string_h <string.h>
#endif

// some code seems to rely on _WIN32 instead of WIN32
#if defined (WIN32)
	#ifndef _WIN32 
		#define _WIN32 WIN32
	#endif
#endif


//{
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
// for MS VS 8 (== cl version 14) we can use the new secure *_s string functions
// for other systems we have to "emulate" them
#define TARGETWITHLEN(str,len) str,len

// MSVC moans: 'open': The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: _open. See online help for details.

#define OPEN _open
#define CLOSE _close
#define READ _read
#define STRICMP _stricmp
#define FILENO _fileno
#define SETMODE _setmode
#define TEMPNAM _tempnam
#define GETCWD _getcwd

// some I did not migrate (yet)
#define _CRT_SECURE_NO_WARNINGS 1

#else
//} {

#include I_iostream
#include I_string_h	// for strlen
#include I_stdlib	// for exit

#define OPEN open
#define CLOSE close
#define READ read
#define STRICMP strcasecmp
//#define STRICMP stricmp
#define FILENO fileno
#define SETMODE setmode
#define TEMPNAM tempnam
#define GETCWD getcwd

#include <assert.h>
#include <errno.h>
// Windows promotes fopen_s but g++ does not have it
inline int fopen_s(FILE **fp, const char *filename, const char *mode) {
  assert(fp);
  assert(filename);
  assert(mode);
  *(fp) = fopen(filename, mode);
  if (*fp) {
    return 0;
  } else {
    return errno;
  }
}


USESTD

// approach for emulation:
// * guarantee that the result string ends with \n (array[size-1] = 0)
// * if the above condition cannot be achieved - exit with error message
//
// the implementation is far from optimal from a performance point of view - but for pstoedit this is not critical
//

static inline void strncpy_s(char * de, size_t de_size, const char *  so, size_t count)   {
	const size_t sourcelen = so ? strlen(so) : 0;
	size_t tobecopied = sourcelen < count ? sourcelen : count;
	if ( tobecopied < de_size ) {
		while (so && *so && (tobecopied > 0) ) {
			*de = *so; ++de; ++so; --tobecopied;
		} // does not copy final 0
		*de = 0;
	} else {
		cerr << "buffer overflow in strcpy_s. Input string: '" << (so ? so : "NULL") << "' count: " << count  << " sourcelen " << sourcelen << " buffersize " << de_size << endl;
		exit(1);
	}
}
static inline void strcpy_s(char * de, size_t de_size, const char * so) {
	strncpy_s(de, de_size, so, strlen(so) );
}

static inline void strcat_s(char * de, size_t de_size, const char *  so) {
	// copy string into de + strlen(de)
	const size_t already_in = strlen(de);
	strcpy_s(de + already_in, de_size - already_in, so);
}

// sprintf_s requires a second argument indicating the size of the target string
// because sprintf can have any number of arguments, we cannot handle this in the 
// same manner as the functions above. So we need to hide/unhide this second argument
// for older compilers

#ifdef HAVE_SNPRINTF
	#define sprintf_s snprintf
	#define TARGETWITHLEN(str,len) str,len
#else
	#define sprintf_s sprintf
	#define TARGETWITHLEN(str,len) str
#endif

// sscanf_s requires a size argument for output strings, unless we use "to-string", we can use sscanf
// but this has to be assured in each individual case !!!

// TARGETWITHLEN is not being used in the context of sscanf
#define sscanf_s sscanf

#endif
//}

// used to eliminate compiler warnings about unused parameters
inline void unused(const void * const) { }
//lint -esym(522,unused)
//lint -esym(523,unused)

#endif
//}
