/*
   dynload.h : This file is part of pstoedit
   declarations for dynamic loading of drivers

   Copyright (C) 1993 - 2024 Wolfgang Glunz, wglunz35_AT_pstoedit.net

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
#include "cppcomp.h"
#include <assert.h>
#include I_iostream

// we need __linux__ instead of just linux since the latter is not defined when -ansi is used.

#if defined(__sparc) || defined(__linux) || defined(__linux__) || defined(__CYGWIN__) || defined(__FreeBSD__) || defined(_WIN32) || defined(__OS2__) || defined(_AIX) || (defined (HAVE_DLFCN_H) && (HAVE_DLFCN_H==1 ) )
#define HAVE_SHAREDLIBS
#endif

#ifdef HAVE_SHAREDLIBS

#ifndef LEANDYNLOAD
#include "drvbase.h"
#endif

#include "dynload.h"

#include I_stdio
#include I_stdlib
#include I_string_h

// for DriverDescription types.

#if defined(__hpux) && defined (__GNUG__)
typedef void (*initfunctype) ();
#endif

#if defined(__linux) || defined(__linux__) || defined(__CYGWIN__) || defined(__FreeBSD__) || defined(__hpux) || defined(_AIX) || (defined (HAVE_DLFCN_H) && (HAVE_DLFCN_H==1 ) )
#include <dlfcn.h>
#elif defined(__sparc)
#if defined(__SVR4)
#include <dlfcn.h>
#else
				// prototypes under SunOS 4.1.x are not compatible with C++
extern "C" {
	void *dlopen(const char *pathname, int mode);
	 DynLoader::fptr dlsym(void *handle, const char *name);
	char *dlerror(void);
	int dlclose(void *handle);
}
#define RTLD_LAZY       1
#endif
#elif defined(__OS2__)
#include <sys/types.h>
#include <dlfcn.h>

#elif defined(_WIN32)

#include <windows.h>
#define WINLOADLIB LoadLibrary
#define WINFREELIB FreeLibrary
	//  static const char * const libsuffix = ".dll";

static char *dlerror()
{
	LPVOID lpMsgBuf;
  //  LPVOID lpDisplayBuf;
    const DWORD dw = GetLastError(); 

    (void)FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, nullptr );
	return (char*)lpMsgBuf; 
}



#else
#error "system unsupported so far"
#endif

DynLoader::DynLoader(const char *libname_P, ostream & errstream_p, int verbose_p):libname(nullptr), handle(nullptr),
errstream(errstream_p),verbose(verbose_p)
{
	if (libname_P) {
		const size_t size = strlen(libname_P) + 1; 
		libname = new char[size];
		strcpy_s(libname, size, libname_P);
		if (verbose) {
			errstream << "creating Dynloader for " << libname << endl;
		}
		open(libname);
	}
}

void DynLoader::open(const char *libname_P)
{
	if (handle) {
		errstream << "error: DynLoader has already opened a library" << endl;
		exit(1);
	}
	const size_t size = strlen(libname_P) + 1; 
	auto fulllibname = new char[size];
	strcpy_s(fulllibname, size, libname_P);
	// not needed strcat(fulllibname_P,libsuffix);

#if defined(__linux) || defined(__linux__) || defined(__CYGWIN__) || defined(__FreeBSD__) || defined(__hpux) || defined(__sparc) || defined(__OS2__) || defined(_AIX) || (defined (HAVE_DLFCN_H) && (HAVE_DLFCN_H==1 ) )
	int loadmode = RTLD_LAZY;	// RTLD_NOW
	handle = dlopen(fulllibname, loadmode);
#elif OS_WIN32_WCE
		if (sizeof(void*) != sizeof(HMODULE)) {
		errstream << "type of handle in dynload.cpp seems to be wrong" << endl;
		return;
	}
	handle = WINLOADLIB(LPSTRtoLPWSTR(fulllibname).c_str());
#elif defined(_WIN32)
	if constexpr(sizeof(void*) != sizeof(HMODULE)) {
		errstream << "type of handle in dynload.cpp seems to be wrong" << endl;
		return;
	}
	handle = WINLOADLIB(fulllibname);
#else
#error "system unsupported so far"
#endif
	if (handle == nullptr) {
		const char * const dlerrormessage = dlerror();
		const char * const dle = dlerrormessage ? dlerrormessage : "NULL";
		errstream << "Problem during opening '" << fulllibname << "' : " << dle 
			<< endl;
		delete[]fulllibname;
		return;
	}
	{
		if (verbose)
			errstream << "loading dynamic library " << fulllibname << " completed successfully" << endl;
#if defined(__hpux) && defined (__GNUG__)
		// in HPUX g++ this is called "_GLOBAL__DI" - on other systems it may be something like "_init"
		// under HP-UX automatic initialization of dlopened shared libs doesn't work - at least not when compiled with g++
		initfunctype fptr = (initfunctype) dlsym(handle, "_GLOBAL__DI");
		if (fptr) {
			if (verbose)
				errstream << "Found an init function for the library, so execute it " << endl;
			fptr();
		}
#endif
	}
	delete[]fulllibname;
}

void DynLoader::close()
{
	if (handle) {
		if (libname && verbose)
				errstream << "closing dynamic library " << libname << endl;
#if defined(__linux) || defined(__linux__) || defined(__CYGWIN__) || defined(__FreeBSD__) || defined(__hpux) || defined(__sparc) || defined(__OS2__) || defined(_AIX) || (defined (HAVE_DLFCN_H) && (HAVE_DLFCN_H==1 ) )


#if (defined(__linux) || defined(__linux__) || defined(__GNU__)) && !(defined(__COVERITY__) || defined(LINT))
		// normally we should call dlclose here. But there is a very strange problem in Linux
		// whenever a plugin.so indirectly loads libpthread (e.g. libdrvmagick++ does it because libMagick++ does it
		// the call to dlclose crashes with a seg fault 
		// Also gdb cannot debug anymore if libpthread is not loaded already by main program.
		// another workaround would be to link -lpthread also to pstoedit main program but that is also not nice
		// so simpler solution is to avoid the dlclose under Linux
		if (libname && verbose)
				errstream << "not really closing dynamic library because of pthread problem under Linux - contact author for details or check dynload.cpp from pstoedit source code " << libname << endl;
		// dlclose(handle);
#else
		dlclose(handle);
#endif

#elif defined(_WIN32)
	 	(void) WINFREELIB((HINSTANCE) handle);
#else
#error "system unsupported so far"
#endif
		handle = nullptr;
	}
}

DynLoader::~DynLoader()
{
	close();
	if (libname && verbose)
		errstream << "destroying Dynloader for " << libname << endl;
	delete[]libname;
	libname=nullptr;
	handle=nullptr;
}

int DynLoader::knownSymbol(const char *name) const
{
	return (getSymbol(name, 0) != nullptr);
}

// a bit of hack since C++ does not support the case of "normal" ptrs to function ptrs.
typedef void (*funcptr)();
union ptrunion {
	void *			u_ptr;
	DynLoader::fptr u_fptr;
};


DynLoader::fptr DynLoader::ptr_to_fptr(void * p) {
	ptrunion u;
	//static_assert( sizeof(u.u_ptr) == sizeof(u.u_fptr) );
	assert( sizeof(u.u_ptr) == sizeof(u.u_fptr) );
	u.u_ptr = p;
	return u.u_fptr;
}

DynLoader::fptr DynLoader::getSymbol(const char *name, int check) const
{
	//
	// see http://www.trilithium.com/johan/2004/12/problem-with-dlsym/ for a nice discussion
	// about the cast problem
	//
#if defined(__linux) || defined(__linux__) || defined(__CYGWIN__) || defined(__FreeBSD__) || defined(__hpux) || defined(__sparc) || defined(__OS2__) || defined(_AIX) || (defined (HAVE_DLFCN_H) && (HAVE_DLFCN_H==1 ) )
	DynLoader::fptr rfptr = ptr_to_fptr(dlsym(handle, name));	//lint !e611 //: Suspicious cast
#elif defined(OS_WIN32_WCE)
	DynLoader::fptr rfptr = ptr_to_fptr(GetProcAddress((HINSTANCE) handle, LPSTRtoLPWSTR(name).c_str()));	//lint !e611 //: Suspicious cast
#elif defined(_WIN32)
#ifdef _WIN64
	DynLoader::fptr rfptr = /* ptr_to_fptr */(GetProcAddress((HINSTANCE) handle, name));	//lint !e611 //: Suspicious cast
#else
	DynLoader::fptr rfptr = ptr_to_fptr(GetProcAddress((HINSTANCE)handle, name));
#endif

#else
#error "system unsupported so far"
#endif
	if ((rfptr == nullptr) && check) {
		const char * const dlerrormessage = dlerror();
		const char * const dle = dlerrormessage ? dlerrormessage : "NULL";
		errstream << "error during getSymbol for " << name << ":" << dle 
			<< endl;
	}
	return rfptr;
}


#ifndef LEANDYNLOAD
// LEANDYNLOAD means just the DynLoader class

// now the stuff handling search for plugins and loading

//lint -sem(PluginVector::add, custodial(1))

// OK, I could take a vector<DynLoader *> but that is not supported with all compilers yet.
class PluginVector {
public:
	enum { maxPlugins = 100 };
	PluginVector():curindex(0u) {
		for (unsigned int i = 0; i < maxPlugins; i++)
			pluginPtr[i] = nullptr;
	} 
	~PluginVector() {
		clear();
	}
	void add(DynLoader * newelem) {
//		cout << "adding plugin (o) " << endl;
//		cerr << "adding plugin (e) " << endl;
		pluginPtr[curindex] = newelem;
		curindex++;
	}
	void clear() {
		for (unsigned int i = 0; i < maxPlugins; i++) {
			delete pluginPtr[i];
			pluginPtr[i] = nullptr;
		}
	}
	DynLoader *pluginPtr[maxPlugins];
	unsigned int curindex;

	NOCOPYANDASSIGN(PluginVector)
};

static PluginVector LoadedPlugins;

extern "C" DLLEXPORT
void unloadpstoeditplugins() {
	LoadedPlugins.clear();
}

static bool loadaPlugin(const char *filename, ostream & errstream, bool verbose)
{
 	if (!filename) return false;
	if ( verbose )
		errstream << "loading plugin: " << filename << endl;

	DriverDescription::currentfilename = filename;
	auto dynloader = new DynLoader(filename, errstream, verbose);
	if (!dynloader->valid()) {
		delete dynloader;
		errstream << "Problem during opening of pstoedit driver plugin: " << filename <<
			". This is no problem as long the driver in this library is not needed. Possibly you need to install further libraries and/or extend the LD_LIBRARY_PATH (*nix) or PATH (Windows) environment variables."
			<< endl;
		return false;
	} else {
#if defined (__APPLE__)
// on MacOS it seems necessary to at least do a dlsym to an existing symbol 
// to get the global ctors being called
		void (*fp) () = ((void (*)()) dynloader->getSymbol("initlibrary"));
		if (fp) {
			if (verbose) {
				errstream << "getSymbol(\"initlibrary\") succeeded." << endl;
			}
		} else {
			errstream << "getSymbol(\"initlibrary\") failed." << endl;
		}
#endif
	}
	LoadedPlugins.add(dynloader);

// this seems to be obsolete. but not with .dll's
//          globalRp->explainformats(errstream);
	if (dynloader->knownSymbol("getglobalRp")) {
		getglobalRpFuncPtr dyngetglobalRpFunc =
			(getglobalRpFuncPtr) dynloader->getSymbol("getglobalRp");
		if (dyngetglobalRpFunc == nullptr) {
			errstream << "could not find getglobalRp " << endl;
			return false;
		}
		const DescriptionRegister * const dynglobalRp = dyngetglobalRpFunc();
		if (dynglobalRp == nullptr) {
			errstream << " didn't find any registered Drivers " << endl;
			return false;
		} else if (dynglobalRp != getglobalRp()) {
//                      globalRp->explainformats(errstream);
			getglobalRp()->mergeRegister(errstream, *dynglobalRp, filename);
		}
	} else {
		// didn't fint getglobalRp
		// so assume that the plug-in does not have
		// it's own copy of globalRp but uses the
		// one from the caller
	}
//          globalRp->explainformats(errstream);
  return true;
}

// for directory search
#if (defined (HAVE_DLFCN_H) && (HAVE_DLFCN_H==1 ) )
#include <dirent.h>
#define DIR_VERSION 1
#elif (defined (HAVE_SYS_NDIR_H) && (HAVE_SYS_NDIR_H==1 ) )
#include <sys/ndir.h>
#define DIR_VERSION 2
#elif (defined (HAVE_SYS_DIR_H) && (HAVE_SYS_DIR_H==1 ) )
#include <sys/dir.h>
#define DIR_VERSION 3
#elif (defined (HAVE_NDIR_H) && (HAVE_NDIR_H==1 ) )
#include <ndir.h>
#define DIR_VERSION 4
#else
//
// last chance
#if defined(__linux) || defined(__linux__) || defined(__CYGWIN__) || defined(__FreeBSD__) || defined(__hpux) || defined(__sparc) || defined(__OS2__) || defined(_AIX)
#include <direct.h>
#define DIR_VERSION 5
#else
// if not even direct.h is available we leave DIR_VERSION unset and later skip to Windows
#endif

#endif

#ifdef DIR_VERSION
// this is for all *nix like systems
bool loadPlugInDrivers(const char *pluginDir, ostream & errstream, bool verbose)
{
	if (pluginDir) {
		DIR *dirp;
		struct dirent *direntp;
#if defined(__OS2__) || defined(__CYGWIN__)
		const char *const suffix = ".dll";
#elif defined (__APPLE__)
		const char *const suffix = ".so";	// ".dylib";
		// Note: the suffix depends on the way the plugin libraries are built via libtool.
		// If the -module flag is set, then the output is a .so library,
		// if no -module is given, then the output is a .dylib library.
		// We use the .so since we use the -module option, since that is needed for other
		// systems such as cygwin.
#elif defined (__hpux)
		const char *const suffix = ".sl";
#else
		const char *const suffix = ".so";
#endif
		if (verbose && pluginDir)
			errstream << "loading plugins from " << pluginDir << " using suffix: " << suffix <<
				endl;

		dirp = opendir(pluginDir);
		if (!dirp) {
			if (verbose && pluginDir)
				errstream << "opendir failed on: " << pluginDir << endl;
			return false;
		}
        bool plugin_loaded = false;
		while ((direntp = readdir(dirp)) != nullptr) {
//      cout <<  direntp->d_name << endl;
			unsigned int flen = strlen(direntp->d_name);
			char *expectedpositionofsuffix = direntp->d_name + flen - strlen(suffix);
//              if ( local filename starts with drv or plugins and ends with .so)
#if defined (__CYGWIN__)
			const char *const libprefix = "cygp2edrv";
#else
			const char *const libprefix = "libp2edrv";
#endif
			if (((strstr(direntp->d_name, libprefix) == direntp->d_name) ||
				 (strstr(direntp->d_name, "plugin") == direntp->d_name)
				) && (strstr(expectedpositionofsuffix, suffix) == expectedpositionofsuffix)) {
				const size_t newlen = strlen(pluginDir) + flen + 2;
				char *fullname = new char[newlen];
				strcpy_s(fullname, newlen, pluginDir);
				strcat_s(fullname, newlen, "/");
				strcat_s(fullname, newlen, direntp->d_name);
//          cout <<  direntp->d_name  << " " << fullname << endl;
				plugin_loaded |= loadaPlugin(fullname, errstream, verbose);
				delete[]fullname;
			}

		}						// while
		closedir(dirp);
		return plugin_loaded;
	} else {
		errstream << "Could not load plugins - parameter pluginDir is null " << endl;
		return false;
	}
}
#elif defined(_WIN32)


bool loadPlugInDrivers(const char *pluginDir, ostream & errstream, bool verbose)
{
	if (pluginDir != nullptr) {
		char szExePath[1000];
		szExePath[0] = '\0';
		(void) P_GetPathToMyself("pstoedit", szExePath, sizeof(szExePath));

		WIN32_FIND_DATA finddata;

		const char pattern[] = "/*.dll";
		const size_t pluginDirsize = strlen(pluginDir);
		const size_t size = pluginDirsize + strlen(pattern) + 1;
		char *searchpattern = new char[size];
		strcpy_s(searchpattern, size, pluginDir);
		strcat_s(searchpattern, size, pattern);
#ifdef OS_WIN32_WCE
		HANDLE findHandle = FindFirstFile(LPSTRtoLPWSTR(searchpattern).c_str(), &finddata);
#else
		HANDLE findHandle = FindFirstFile(searchpattern, &finddata);
#endif
		bool plugin_loaded = false;
		if (findHandle == INVALID_HANDLE_VALUE) {
			if (verbose)
				errstream << "Could not open plug-in directory (" << pluginDir
					<< " or didn't find any plugin there" << endl;
		} else {
			BOOL more = true;	
			while (more) {
				// check for suffix being really .dll because FindFirstFile also matches
				// files such as e.g. .dllx
#ifdef OS_WIN32_WCE
				const size_t len = strlen(LPWSTRtoLPSTR(finddata.cFileName).c_str());
#else
				const size_t len = strlen(finddata.cFileName);
#endif
				// -4 means go back the length of ".dll"
#ifdef OS_WIN32_WCE
				if (STRICMP(LPWSTRtoLPSTR(&finddata.cFileName[len - 4]).c_str(), ".dll") == 0) {
#else
				if (STRICMP(&finddata.cFileName[len - 4], ".dll") == 0) {
#endif
					// cout << &finddata.cFileName[len -4 ] << endl;
					const size_t size_2 = pluginDirsize + len + 3;
					char *fullname = new char[size_2];
					strcpy_s(fullname, size_2, pluginDir);
					strcat_s(fullname, size_2, "\\");
#ifdef OS_WIN32_WCE
					strcat_s(fullname, size_2, LPWSTRtoLPSTR(finddata.cFileName).c_str());
#else
					strcat_s(fullname, size_2, finddata.cFileName);
#endif

//              errstream << "szExePath " << szExePath << endl;

					if ((STRICMP(fullname, szExePath) != 0)
#ifdef OS_WIN32_WCE
						&& (STRICMP(LPWSTRtoLPSTR(finddata.cFileName).c_str(), "pstoedit.dll") != 0)) {
#else
						&& (STRICMP(finddata.cFileName, "pstoedit.dll") != 0)) {
#endif
						// avoid loading dll itself again
						//                 errstream << "loading " << fullname << endl;
						plugin_loaded |= loadaPlugin(fullname, errstream, verbose);
					} else {
						//                 errstream << "ignoring myself " << finddata.cFileName << endl;
					}
					delete[]fullname;
				}
				more = FindNextFile(findHandle, &finddata);
			}
			(void) FindClose(findHandle);
		}
		delete[]searchpattern;
		return plugin_loaded;
	} else {
		errstream << "Could not load plugins - parameter pluginDir is null " << endl;
		return false;
	}
}
#else
#error "system unsupported so far (directory reading)"
#endif

#endif
// LEAN


#else
// no shared libs
#include <iosfwd>
using namespace std;

bool loadPlugInDrivers(const char *pluginDir, ostream & errstream, bool verbose)
{
	if (verbose) {
		errstream <<
			"dummy version of loadPlugInDrivers called since you system doesn't seem to support loading libraries at runtime - If you think that this is not correct, contact the author of this program (wglunz35_AT_pstoedit.net) "
			<< endl;
	}
	return false;
}								// just a dummy Version
#endif
