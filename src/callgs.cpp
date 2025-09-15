/*  
   callgs.cpp : This file is part of pstoedit
   interface to Ghostscript

   Copyright (C) 1993 - 2024 Wolfgang Glunz, wglunz35_AT_pstoedit.net
   
   Proposal for a "cleaned up" version: removed (IMHO) dead/old code,
   e.g., WIN32 is "dll only" now, because gs32 comes w/DLL 

   Extended for OS/2

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

#include I_iostream
#include I_fstream

#include "pstoedit.h"

#include I_stdlib

#include I_string_h


// rcw2:Can't put angle brackets onto commandline for g++ on RiscOS :(
#ifdef riscos
#ifndef DEFAULTGS
#define DEFAULTGS <GhostScript$Dir>.ghost
#endif
#endif


#include "miscutil.h"
//extern char *getRegistryValue(ostream & errstream, const char *typekey, const char *key);



static RSString createCmdLine(int argc, const char *const argv[])
{
  // create a single string from all args
  RSString result("");
  for (unsigned int i = 0; i < (unsigned) argc; i++) {
      result += argv[i];
      result += " ";
  }
  return result;
}


#if defined(_WIN32) || defined (__OS2__)

// using Ghostscript DLL
#define WITHDLLSUPPORT 1

#ifdef WITHDLLSUPPORT

#ifndef USEOLDAPI
  #include "callgsdllviaiapi.cpp"
#else
// version which uses obsolete API and gsdll.h
  #include "dwmaincgsdll.c"
#endif

#endif


#if defined(_WIN32)
// retrieve version to be used from registry
#include "wgsver.c"      // ONLY WINDOWS
#endif

// #undef main

#define WITHGETINI
#ifdef WITHGETINI
#include "getini.c"
#endif



// Interface to Ghostscript EXE - as an alternative to the DLL
// The usage of the DLL makes problem in case of gsview, since 
// there can only be one instance of the DLL and Ghostscript active
// in one process. So - if being called from gsview - we need to start 
// Ghostscript via the EXE.
// Same holds if 64 bit Ghostscript should be called from 32 big pstoedit.
#include <windows.h>
#include I_stdio
static int callgsEXE(int argc, const char * const argv[])
{
  int gsresult = 0;
  STARTUPINFO MyStartupInfo; // process data
  memset(&MyStartupInfo,'\0',sizeof(MyStartupInfo));
  MyStartupInfo.cb = sizeof(STARTUPINFO);
  MyStartupInfo.wShowWindow=SW_SHOWMINNOACTIVE;

  PROCESS_INFORMATION MyProcessInformation;
  // CreateProcess fills in this structure
  DWORD gs_status = 0;

  const RSString& commandline = createCmdLine(argc,argv);
  cerr << "running command line: " << commandline << endl;

  BOOL status = CreateProcess(
              nullptr, // Application Name 
#ifdef OS_WIN32_WCE
              LPSTRtoLPWSTR((LPSTR)commandline.c_str()).c_str(),
#else
        (LPSTR)commandline.c_str(),
#endif
              nullptr, // Process attributes (NULL == Default)
              nullptr, // Thread-Attributes (Default)
              FALSE, // InheritHandles
              CREATE_NEW_PROCESS_GROUP, // CreationFlags
              nullptr, // Environment (NULL: same as calling-process)
              nullptr, // Current Directory (NULL: same as calling process)
              (LPSTARTUPINFO)(&MyStartupInfo), // Windows-state at Startup
           // window of calling process gets minimized + and shown disabled
             (LPPROCESS_INFORMATION)(&MyProcessInformation)
            );

  if (status) gsresult=0;
  else        gsresult=-1; // Failure

  for ( ; ; ) { // while true but without compiler warning
        status=GetExitCodeProcess(MyProcessInformation.hProcess, &gs_status);
    if ( !status ) {  // process-Status could not be determined
      gsresult=-1;
      break;  
    } else if (gs_status==STILL_ACTIVE) { // gs still working
      Sleep(500); // polling interval
    } else break; // process terminated
  }
  if (gsresult != 0) {
    cerr << "Interpreter failure: " << gsresult << endl;
  }
  (void)CloseHandle(MyProcessInformation.hProcess);
  (void)CloseHandle(MyProcessInformation.hThread);
  return gsresult;
}

// define PSTOEDITDEBUG

int callgs(int argc, const char * const argv[]) { 
  const bool verbose = true; // debug only

//#define PSTOEDITDEBUG 1 
#ifdef PSTOEDITDEBUG
  cerr << "Commandline " << endl;
  for (int i = 0; i < argc; i++) {
    cerr << "argv[" << i << "]: " << argv[i] << endl;
  }
#endif

  /* force usage of exe - for test purposes only 
  char * dll = strstr(argv[0],"gsdll32.dll");
  strcpy(dll,"gswin32.exe");  
  */

  int result = 0; // false
  // check the first arg in the command line whether it contains gsdll32.dll
  if ((strstr(argv[0],"gsdll32.dll") != nullptr) || (strstr(argv[0],"gsdll64.dll") != nullptr)) {
#ifdef WITHDLLSUPPORT
     result = callgsDLL(argc, (char **) argv, global_cb_data);
#else
    cerr << "Sorry, but DLL support was not enabled in this version of pstoedit" << endl;
    return 2;
#endif
  } else {
    result = callgsEXE(argc,argv); // callgsDLL(argc, (char **) argv);
  }


  if (result == -1 ) /* could not even load the dll/exe - so try fallback exe */ { 
    const char * * newargv = new const char *[argc];
    for (int i = 0; i< argc; i++) { newargv[i] = argv[i]; }

    char * dlldirname = cppstrdup(argv[0]);
    char * p = strrchr(dlldirname, '\\'); 
    if (p) {
      p++;
      *p = 0; 
    }
    const RSString gsdir(dlldirname);
    delete[] dlldirname;

#ifdef _WIN64
    const RSString defaultexe(gsdir + RSString("gswin64c.exe"));
    const RSString fallbackexe(gsdir + RSString("gswin32c.exe"));

#else
    const RSString defaultexe(gsdir + RSString("gswin32c.exe"));
    const RSString fallbackexe(gsdir + RSString("gswin64c.exe"));
#endif
    const RSString exename( fileExists(defaultexe.c_str()) ? defaultexe : fallbackexe );
    if (verbose) {
      cerr << "loading: " << argv[0] << " failed (possibly due to 32/64 bit mix - reverting to call gs as exe via: " << exename << endl;
    }
    newargv[0] = exename.c_str();
    result = callgsEXE(argc,newargv);
    delete [] newargv; /* just the array - the content is not owned */ 
  }  
  return result;
}


#else
// not a windows system
int callgs(int argc, const char *const argv[])
{
#ifndef USE_FORK
  RSString commandline = createCmdLine(argc, argv);
  commandline += " 1>&2" ; // redirect all stdout of Ghostscript to stderr
               // in order not to interfere with stdout of pstoedit
  const int result = system(commandline.c_str());
  return result;
#else
  //
  // version using fork as proposed by Guan Xin - but this does not handle stdout redirection.
  //

  /* Return values:
     -1:          error spawning gs
     -2:          error waiting gs
     otherwise:   exit status of gs
  */

  const int pid = fork();
  
  switch(pid)
  {
    case -1:
      perror("fork");
      return -1;
    case 0:
      execvp(argv[0], argv);
      perror("execvp");
      return -1;
    default:
    {
      int status;
      
      if(pid == waitpid(pid, &status, 0) && WIFEXITED(status))
      {
        return WEXITSTATUS(status);
      }
      return -2;
    }
  }
#endif

}
#endif

#define str(x) #x
#define xstr(x) str(x)


const char *whichPI(ostream & errstream, int verbose, const char *gsregbase, const char * gsToUse)
{
// determines which PostScript interpreter to use
// !FIXME: change the sequence to 2-1-3 (for Unix); with the upcoming gsview for X11 possibly introduce
//         a lookup of "gsviewX11.ini" (or whatever its name is)
// Windows:
//     check the "gsToUse" parameter whether it represents a filename. If yes - use that 
//     if not check whether it represents a version number and find gs according to the
//     if not find the latest installed version of gs
// OS/2:
  // 1. look in the native profile pstoedit.ini
  // 2. look into gvpm.ini
  // 3. look for the compiled-in DEFAULTGS (which has to hold the DLL)
// else (i.e. UNIX):
  // 1. look in the environment for the value of GS
  // 2. look in the "registry" ($(HOME)/.pstoedit.reg)
  // 3. look for the compiled-in DEFAULTGS (which has to hold the executable)
#ifdef DEFAULTGS
  static const char *const defaultgs = xstr(DEFAULTGS);
#else
  static const char *const defaultgs = "";
#endif
  const char *gstocall = nullptr;

  // for debugging verbose = true; 
  if (verbose)
      errstream << endl << "Looking up where to find the PostScript interpreter." << endl;

  if ((gsToUse != nullptr) && !strequal(gsToUse,"") ) {
      if (fileExists(gsToUse)) {
          if (verbose) {
              errstream << " an explicit path was given - using : " << gsToUse << endl;
          }
          // an explicit path is given as option already - this overrules everthing
          return gsToUse;
      } 
  }
#if defined (_WIN32)
  if ((gsToUse != nullptr) && !strequal(gsToUse, "")) {
      // for windows - assuming the string represents a version (e.g. 10.01.1)
      // So get the DLL / EXE according to the version given.
      if (verbose) {
          errstream << " The Ghostscript to use is not an absolute path - continuing lookup using: " << gsToUse << endl;
      }
      // remove the dots from the version given to get a pure integer and do some sanity checks.
      int gsversion = keyToVersionInt(gsToUse);
      // errstream << "Int representation of version is: " << gsversion << endl;
      static char buf[1000];
      if (gsversion && find_gs(buf, sizeof(buf), gsversion /* min ver*/, gsversion /* max ver */, getPstoeditsetDLLUsage(), gsregbase, verbose)) {
          if (verbose) {
             errstream << "GS " << (getPstoeditsetDLLUsage() ? "DLL" : "EXE") << " for version " << gsToUse << " is " << buf << endl;
          }
          gstocall = buf;
      }
  } 
  if (!gstocall) {
      // nothing found so far -> find the highest version
      static char buf[1000];
      if (find_gs(buf, sizeof(buf), 550 /* min ver*/, INT32_MAX /* max version */, getPstoeditsetDLLUsage(), gsregbase, verbose)) {
          if (verbose) {
              errstream << "Latest GS " << (getPstoeditsetDLLUsage() ? "DLL" : "EXE") << " is " << buf << endl;
          }
          gstocall = buf;
      }
  }
  if (!gstocall) {
      // nothing found - bail out (see at end)
      // possibly gs is not installed ?
  }
#if 0

  // WIN32:
  // 1. look in the registry
  // 2. look into gsview32.ini
  // 2. look in the environment for the value of GS -- NOT TRUE! This is commented out.
  // 4. look for the compiled-in DEFAULTGS (which has to hold the DLL)
  // old - possibly remove later
  // 
  // alternative "pointer" to gs
  RSString gstocallfromregistry = getRegistryValue(errstream, "common", "gstocall");
  if (gstocallfromregistry.length()) {
    if (verbose)
      errstream << "found value in registry" << endl;
    static char buffer[2000];
    buffer[2000-1] = 0; // add EOS
    strncpy_s(buffer,2000-1, gstocallfromregistry.c_str(),2000-1);
    //  delete[]gstocallfromregistry;
    gstocall = buffer;
  } else {
    if (verbose)
      errstream << "didn't find value in registry, trying gsview32.ini" << endl;
    // try gsview32.ini
    static char pathname[1000];  // static, since we return it
    const char inifilename[] = "gsview32.ini";
#ifdef WITHGETINI
    char fullinifilename[1000];
    getini(verbose, errstream, fullinifilename, inifilename, sizeof(fullinifilename));
#else
    const char *fullinifilename = inifilename;
#endif
    if (verbose)
      errstream << "looking in " << fullinifilename << endl;
    const DWORD result = GetPrivateProfileString("Options",
                         "GhostscriptDLL",
                         "",  //default
                         pathname,
                         sizeof(pathname),
                         fullinifilename);
    if (result > 0) {
      if (verbose) {
        errstream << "found value in ";
        if (strcmp(inifilename, fullinifilename) == 0) {
          char sysdir[2000];
          sysdir[0] = '\0';
          const UINT ret = GetWindowsDirectory(sysdir, 2000);
          if (ret)
            errstream << sysdir << '\\';
        }
        errstream << fullinifilename << endl;
      }
      gstocall = pathname;
    } else {
        if (verbose) {
        errstream<< "nothing found in gsview32.ini file - using find_gs to lookup latest version of Ghostscript in registry " << endl;
        (void)dumpgsvers(gsregbase, verbose);
      }
      static char buf[1000];
      if (find_gs(buf, sizeof(buf), 550 /* min ver*/ , getPstoeditsetDLLUsage() , gsregbase, verbose)) { 
        if (verbose) {
            if (getPstoeditsetDLLUsage()) {
                errstream << "Latest GS DLL is " << buf << endl;
            } else {
                errstream << "Latest GS EXE is " << buf << endl;
            }
        }
        gstocall = buf;
      } else {
        if (verbose)
          errstream << "find_gs couldn't find GS in registry" << endl;
        if (strlen(defaultgs) > 0) {
          gstocall = defaultgs;
        } else {
          errstream <<
            "Fatal: don't know which interpreter to call. " <<
            "Either enter gstocall into the registry " <<
            "or compile again with -DDEFAULTGS=..." << endl;
          gstocall = nullptr;
        }
      }
    }
  }
#endif // OLD
#elif defined (__OS2__)
  unused(&gsregbase);
  RSString gstocallfromregistry = getRegistryValue(errstream, "common", "gstocall");
  if (gstocallfromregistry.length() ) {
    if (verbose)
      errstream << "found value in pstoedit.ini" << endl;
    static char buffer[2000];
    buffer[2000-1] = 0; // add EOS
    strncpy(buffer, gstocallfromregistry.c_str(),2000-1);
    gstocall = buffer;
  } else {
    if (verbose)
      errstream << "didn't find value in pstoedit.ini, trying gvpm.ini" << endl;
    // try gvpm.ini
    static char pathname[1000];  // static, since we return it
    const char inifilename[] = "gvpm.ini";
#ifdef WITHGETINI
    char fullinifilename[1000];
    getini(verbose, errstream, fullinifilename, inifilename, sizeof(fullinifilename));
#else
    const char *fullinifilename = inifilename;
#endif
    if (verbose)
      errstream << "looking in " << fullinifilename << endl;
    ifstream regfile(fullinifilename);
    int result = 0;
    if (regfile) {
      char line[1000];
      while (!regfile.eof() && !result) {
        regfile.getline(line, 1000);
        if (strstr(line, "[Options]"))
          do {
            regfile.getline(line, 1000);
            if (strstr(line, "GhostscriptDLL=")) {
              strncpy(pathname, line + strlen("GhostscriptDLL="),1000);
              char *cr = strrchr(pathname, '\r');
              if (cr)
                *cr = 0;
              result = 1;
              break;
            }
          }
          while (!regfile.eof() && !strchr(line, '['));
      }
    }
    if (result > 0) {
      if (verbose)
        errstream << "found value in " << fullinifilename << endl;
      gstocall = pathname;
    } else {

      if (strlen(defaultgs) > 0) {  
        gstocall = defaultgs;
        if (verbose)
          errstream << "nothing found so far, trying default: " << defaultgs << endl;
      } else {
        errstream <<
          "Fatal: don't know which interpreter to call. " <<
          "Either enter gstocall into pstoedit.ini " <<
          " or compile again with -DDEFAULTGS=..." << endl;
        gstocall = 0;
      }
    }
  }
#else              //UNIX
  unused(&gsregbase);
  gstocall = getenv("GS");
  if (gstocall == nullptr) {
    if (verbose)
      errstream << "GS not set, trying registry for common/gstocall" << endl;
    RSString gstocallfromregistry = getRegistryValue(errstream, "common", "gstocall");
    if (gstocallfromregistry.length()) {
      if (verbose)
        errstream << "found value in registry" << endl;
      static char buffer[2000];
      buffer[2000-1] = 0; // add EOS
      strncpy(buffer, gstocallfromregistry.c_str(),2000-1);
      gstocall = buffer;
    } else {
        if (strlen(defaultgs) > 0) {
          gstocall = defaultgs;
          if (verbose)
            errstream << "nothing found so far, trying default: " << defaultgs << endl;
        } else {
          errstream <<
            "Fatal: don't know which interpreter to call. " <<
            "Either setenv GS or compile again with -DDEFAULTGS=..." << endl;
          gstocall = nullptr;
        }
    }
  } else {
    if (verbose)
      errstream << "GS is set to:" << gstocall << endl;
  }
#endif
  if (verbose && gstocall&& !strequal(gstocall,""))
    errstream << "Value found is:" << gstocall << endl;
  return gstocall;
}


#if defined(_WIN32)
static const char *getOSspecificOptions(int verbose, ostream & errstream, char *buffer, unsigned int   buflen   )
{
  const char *PIOptions = nullptr;
  if (verbose)
    errstream << "didn't find value in registry, trying gsview32.ini" << endl;
  // try gsview32.ini
  const char inifilename[] = "gsview32.ini";
#ifdef WITHGETINI
  char fullinifilename[1000];
  getini(verbose, errstream, fullinifilename, inifilename, sizeof(fullinifilename));
#else
  const char *fullinifilename = inifilename;
#endif
  if (verbose)
    errstream << "looking in " << fullinifilename << endl;
  const DWORD result = GetPrivateProfileString("Options",
                       "GhostscriptInclude",
                       "",  //default
                       buffer,
                       buflen,
                       fullinifilename);
  if (result > 0) {      //2.
    if (verbose) {
      errstream << "found value in ";
      if (strcmp(inifilename, fullinifilename) == 0) {
        char sysdir[2000];
        sysdir[0] = '\0';
        const UINT ret = GetWindowsDirectory(sysdir, 2000);
        if (ret)
          errstream << sysdir << '\\';
      }
      errstream << fullinifilename << endl;
    }
    PIOptions = buffer;
  } else {
    // 3.
#if 0
    // no longer needed with newest find_gs. gs stores the related gs_lib in the registry.
    static char buf[500];
    const unsigned int gsver = get_latest_gs_version();
    if (gsver) {
      get_gs_string(gsver, "GS_LIB", buf, sizeof(buf));
      if (verbose) {
        errstream << "found GS_LIB as " << buf << " from latest version of gs" << endl;
      }
      PIOptions = buf;
    }
#endif

  }
  return PIOptions;
}

static const char * const lookupplace = "registry";

#elif defined(__OS2__)
static const char *getOSspecificOptions(int verbose, ostream & errstream, char *buffer, unsigned int buflen)
{
  const char *PIOptions = 0;
  if (verbose)
    errstream << "didn't find value in pstoedit.ini, trying gvpm.ini" << endl;
  // try gvpm.ini
  const char inifilename[] = "gvpm.ini";
  char fullinifilename[1000];
  getini(verbose, errstream, fullinifilename, inifilename, sizeof(fullinifilename));
  if (verbose)
    errstream << "looking in " << fullinifilename << endl;
  ifstream regfile(fullinifilename);
  int result = 0;
  if (regfile) {
    char line[1000];
    while (!regfile.eof() && !result) {
      regfile.getline(line, 1000);
      if (strstr(line, "[Options]"))
        do {
          regfile.getline(line, 1000);
          if (strstr(line, "GhostscriptInclude=")) {
            strncpy(buffer, line + strlen("GhostscriptInclude="),buflen);
            char *cr = strrchr(buffer, '\r');
            if (cr)
              *cr = 0;
            result = 1;
            break;
          }
        }
        while (!regfile.eof() && !strchr(line, '['));
    }
  }
  if (result > 0) {      //2.
    if (verbose) {
      errstream << "found value in " << fullinifilename << endl;
    }
    PIOptions = buffer;
  }
  return PIOptions;

}
static const char * const lookupplace = "pstoedit.ini";
#else
static const char *getOSspecificOptions(int verbose, ostream & errstream, char *buffer, unsigned int buflen)
{
  unused(&verbose); unused(&errstream); unused(buffer); unused(&buflen);
  return nullptr;
}
static const char * const lookupplace = "registry";
#endif


const char *defaultPIoptions(ostream & errstream, int verbose)
{
  // returns default options to be passed to the Postscript Interpreter
  // WIN32:
  // 1. look in the registry
  // 2. look into gsview32.ini
  // 3. look for latest version of Ghostscript in registry
  // 4. look in the environment for the value of GS_LIB
  // 5. look for the compiled-in GS_LIB
  // OS/2:
  // 1. look in the native profile pstoedit.ini
  // 2. look into gvpm.ini
  // 3. look in the environment for the value of GS_LIB
  // 4. look for the compiled-in GS_LIB
  // else (i.e. UNIX):
  // 1. look in the "registry" ($(HOME)/.pstoedit.reg)
  // 2. look in the environment for the value of GS_LIB
  // 3. look for the compiled-in GS_LIB

#ifdef GS_LIB
  static const char *const defaultPIOptions = xstr(GS_LIB);
#else
  static const char *const defaultPIOptions = "";
#endif
  static char buffer[2000];
  const char *PIOptions;
  if (verbose)
    errstream << endl << "Looking up specific options for the PostScript interpreter." << endl << "First trying " << lookupplace << " for common/GS_LIB" << endl;

  // try first registry/ini value, then GS_LIB and at last the default
  RSString PIOptionsfromregistry = getRegistryValue(errstream, "common", "GS_LIB");
  if (PIOptionsfromregistry.length()) {  // 1.
    if (verbose)
      errstream << "found value in " << lookupplace << endl;
    strncpy_s(buffer,sizeof(buffer), PIOptionsfromregistry.c_str(),sizeof(buffer));
    // delete[]PIOptionsfromregistry;
    PIOptions = buffer;
  } else {          //2.-4.

    PIOptions = getOSspecificOptions(verbose, errstream, buffer,sizeof(buffer) );

    if (PIOptions == nullptr) {  //3.
      if (verbose)
        errstream << "still not found an entry - now trying GS_LIB env var." << endl;
      PIOptions = getenv("GS_LIB");
      if (PIOptions == nullptr) {  //4.
        if (verbose) errstream << "GS_LIB not set" << endl;
        if (strlen(defaultPIOptions) > 0) {
          PIOptions = defaultPIOptions;
          if (verbose) errstream << "nothing found so far, trying default: " << defaultPIOptions<< endl;
        } else {
          PIOptions = nullptr;
        }
      } else { 
        if (verbose)
          errstream << "GS_LIB is set to:" << PIOptions << endl;
      }
    }
  }

  if (PIOptions && (PIOptions[0] != '-') && (PIOptions[1] != 'I')) {
    static char returnbuffer[2000];
    returnbuffer[2000-1]=0;
    strncpy_s(returnbuffer,sizeof(returnbuffer)-1, "-I",sizeof(returnbuffer)-1);
    strcat_s(returnbuffer,sizeof(returnbuffer)-1, PIOptions);
    PIOptions = returnbuffer;
  }
  if (verbose )
    errstream << "Value returned:" << (PIOptions ? PIOptions : "") << endl << endl;
  return PIOptions;
}

const char *whichPINoVerbose(ostream & errstream, const char *gsregbase, const char * gsToUse)
{
  return whichPI(errstream, 0, gsregbase, gsToUse);
}
