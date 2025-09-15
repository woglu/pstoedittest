/*
   miscutil.cpp : This file is part of pstoedit
   misc utility functions

   Copyright (C) 1998 - 2024  Wolfgang Glunz, wglunz35_AT_pstoedit.net

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
#include "miscutil.h"
#include <memory>

#ifdef OS_WIN32_WCE
#include "WinCEAdapter.h"
#endif

#include I_stdio

// #ifdef _MSC_VER
// for getcwd ( at least for Visual C++)

#if defined(unix) || defined(__unix__) || defined(_unix) || defined(__unix) || defined(__EMX__) || defined (NetBSD) || defined(__APPLE__) || defined(_AIX)
#include <unistd.h>

// for umask
#include <sys/types.h>
#include <sys/stat.h>

#ifdef __hpux
// HP-UX does not define getcwd in unistd.h
extern "C"  char *getcwd(char *, size_t);
#endif
#elif defined(OS_WIN32_WCE)
#include <windows.h>
#elif defined(_WIN32)
#include <windows.h>
#include <direct.h>
#elif defined (__OS2__)
#define INCL_DOS
#define INCL_WINWINDOWMGR
#define INCL_WINSHELLDATA		/* Window Shell functions       */
#define INCL_WINERRORS
#include <os2.h>
#else
// not Unix / winwdows like
#if HAVE_DIRENT_H
#include <dirent.h>
#elif HAVE_SYS_NDIR_H
#include <sys/ndir.h>
#elif HAVE_SYS_DIR_H
#include <sys/dir.h>
#elif HAVE_NDIR_H
#include <ndir.h>
#endif
#endif

#ifdef HAVE_MKSTEMP
// for using mkstemp we need errno
#include <errno.h>
#endif

#include I_stdlib
#include I_string_h

#if defined(unix) || defined(__unix__) || defined(_unix) || defined(__unix) || defined (NetBSD) 
//take this out (we may have backslashes w/ EMX(OS/2)):  || defined(__EMX__)
void convertBackSlashes(char *) { }
// nothing to do on systems with unix style file names ( / for directories)

#else
void convertBackSlashes(char *fileName)
{
 	char *c ;
	while ((c = strchr(fileName, '\\')) != nullptr)
		*c = '/';
}
#endif

#ifndef HAVE_MKSTEMP
//
// if we do not have mkstemp we need to emulate it using old tempnam
// for some systems we even have to provide an own tempnam
//
#if defined(__STRICT_ANSI__) 
// normally this is in stdio.h except if __STRICT_ANSI__ is defined (GNU specific)
// but it is in again - at least under Linux with g++3.0
#if !( defined (__GNUG__)  && (__GNUC__>=3) )
	extern "C" char *tempnam(const char *, const char *pfx);
#endif
#endif

#if defined(riscos) 
// rcw2: tempnam doesn't seem to be defined in UnixLib 3.7b for RiscOS
static char *tempnam(const char *, const char *pfx)
{
	char tmp[1024];

	strcpy(tmp, "<Wimp$ScrapDir>.");
	strcat(tmp, pfx);
	return strdup(tmp);
}
#endif

#ifdef __WATCOMC__
// tempnam doesn't seem to be defined in Watcoms clibs
static char *tempnam(const char *, const char *pfx)
{
	const char *path;
	char tmp[1024];

	// simply take c: as temp dir, if environment not defined
	if ((path = getenv("TEMP")) == 0L && (path = getenv("TMP")) == 0L) {
		strcpy(tmp, "C:\\");
	} else {
		// take temp dir from environment
		strcpy(tmp, path);
		strcat(tmp, "\\");
	}

	strcat(tmp, pfx);

	// remove all backslashes
	convertBackSlashes(tmp);

	return strdup(tmp);
}
#endif

// #define BUGGYTEMPNAM

#if (defined(__mips) && defined(__sgi))
#define strdup cppstrdup
#endif

#if (defined(DJGPP) && defined(BUGGYTEMPNAM)) || (defined(__mips) && defined(__sgi))
// tempnam under DJGPP behaves different than on all other systems
// and Irix doesn't seem to have tempnam
char *tempnam(const char *, const char *pfx)
{
	return strdup(tmpnam(0));
}
#endif

// endif for not having mkstemp
#else
 // we have MKSTEMP so we need this function later
 static bool dirAccessible (const char * const dirname) {
	struct stat s;
	const int sret = stat(dirname, &s);
	return ((sret != -1) && (S_ISDIR(s.st_mode)));
 }
 static const char * testvar(const char * var) {
	const char * res = getenv(var);
	if (res && dirAccessible(res)) return res;
	else return nullptr;
 }

#endif

RSString full_qualified_tempnam(const char *pref)
{
#ifdef HAVE_MKSTEMP
	const char *path = nullptr;
	char * filename = nullptr;
	const char XXXXXX[] = "XXXXXX" ; // needed for mkstemp template
	(void) ((path = testvar("TEMP"))   == nullptr && 
		(path = testvar("TMP"))    == nullptr &&
		(path = testvar("TMPDIR")) == nullptr &&
		(path = dirAccessible("/tmp" )  ? "/tmp" : "." )  // last resort current dir
		);
	const unsigned int needed = strlen(path) + 1 + strlen(pref) + 1 + strlen(XXXXXX) + 2;
	filename = (char*) malloc(needed); // new char [ needed ];
	assert(filename);
	filename[0] = '\0';
// one getenvs returned something
	strncpy(filename, path, needed);
	strcat_s(filename, needed, "/");
	strcat_s(filename, needed, pref);
	strcat_s(filename, needed, XXXXXX);
	// cout << "using " << filename << " as template for mkstemp" << endl;
	const mode_t current_umask = umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH); // recommended sec practice
	const int fd = mkstemp(filename);
	umask(current_umask); // set back to original state
	// cout << "returned " << filename << " and " << fd << endl;
	if (fd == -1) {
		cerr << "error in mkstemp for " << filename << " " << errno << endl;
		exit(1);
	}

#else 
	char *filename = TEMPNAM(nullptr, pref);
	// W95: Fkt. tempnam() erzeugt Filename+Pfad
	// W3.1: es wird nur der Name zurueckgegeben
#endif

// rcw2: work round weird RiscOS naming conventions
#ifdef riscos
	const RSString result(filename);
	free(filename);
	return result;
#else
	convertBackSlashes(filename);
	RSString result("");
	if ((strchr(filename, '\\') == nullptr) && (strchr(filename, '/') == nullptr)) {	// keine Pfadangaben..
		char cwd[400];
		if (GETCWD(cwd, 400)) {
		    result += cwd;
			result += "/";
		}
	} 
	result += filename;
	free(filename);
	return result;
#endif
}

unsigned short hextoint(const char hc)
{
	if ((hc >= '0') && (hc <= '9')) { // numeric
		return (hc - '0');
	} else if ((hc >= 'A') && (hc <= 'F')) { // upper case hex
		return (hc - 'A' + 10);
	} else if ((hc >= 'a') && (hc <= 'f')) {// lower case hex
		return (hc - 'a' + 10);
	} 
	assert(false && "hc is not a hex character");
	return 0;
#if 0
	char h = hexchar;
	if (h >= 'a' && h <='f') h += 'A' - 'a'; // normalize lowercase to uppercase
	const unsigned short r = ( h <= '9' ) ? (h - '0') : (h + 10 - 'A' ) ; //lint !e732
	return r;
#endif
}
#if defined(_WIN32)
const bool regdebug = false;

RSString tryregistry(HKEY hKey, LPCSTR subkeyn, LPCSTR key)
{
	HKEY subkey;
	static RSString emptyString("");

	const long ret = RegOpenKeyEx(hKey,	// HKEY_LOCAL_MACHINE, //HKEY hKey,
#ifdef OS_WIN32_WCE
								  LPSTRtoLPWSTR(subkeyn).c_str(),	// LPCSTR lpSubKey,
#else
								  subkeyn,	// LPCSTR lpSubKey,
#endif
								  0L,	// DWORD ulOptions,
								  KEY_READ,	// REGSAM samDesired,
								  &subkey	//PHKEY phkResult
								  );

	if (ret != ERROR_SUCCESS) {
		if (regdebug) cerr << "RegOpenKeyEx :" <<subkeyn << ":" << key << ": failed with error code " << ret << endl;
		return emptyString;
	} else {
		if (regdebug) cerr << "RegOpenKeyEx :" <<subkeyn << ":" << key << ": succeeded with return code " << ret << endl;
		const int maxvaluelength = 1000;
		BYTE value[maxvaluelength];
		DWORD bufsize = maxvaluelength;
		DWORD valuetype;

		const long retv = RegQueryValueEx(subkey,	// HKEY_LOCAL_MACHINE, //HKEY hKey,
#ifdef OS_WIN32_WCE
										  LPSTRtoLPWSTR(key).c_str(),	// "SOFTWARE\\wglunz\\pstoedit\\plugindir", //LPCSTR lpValueName,
#else
										  key,	// "SOFTWARE\\wglunz\\pstoedit\\plugindir", //LPCSTR lpValueName,
#endif
										  nullptr,	// LPDWORD lpReserved,
										  &valuetype,	// LPDWORD lpType,
										  value,	// LPBYTE lpData,
										  &bufsize	// LPDWORD lpcbData
			);

		(void) RegCloseKey(subkey);
		if (retv != ERROR_SUCCESS) {
			if (regdebug)  cerr << "RegQueryValueEx :" <<subkeyn << ":" << key << ": failed with error code " << retv << endl;
			return emptyString;
		} else {
			if (regdebug) cerr << "result :" << subkeyn << ":" << key << ": is " << (char*) value << endl;
			return RSString((char*)value);
		}
	}
}

#endif

#ifdef __OS2__
#include "getini.c"
#endif


//lint -save -esym(715,errstream) -esym(1764,errstream)
RSString getRegistryValue(ostream & errstream, const char *typekey, const char *key)
{
	
#if defined(_WIN32)
	unused(&errstream);

//  CString subkey = CString("SOFTWARE\\wglunz\\") + CString(product);
	RSString subkeyn("SOFTWARE\\wglunz\\"); // char subkeyn[1000];
	// subkeyn[0] = '\0';
	// strcat_s(subkeyn,1000, "SOFTWARE\\wglunz\\");
	subkeyn += typekey; // strcat_s(subkeyn,1000, typekey);
	RSString result(tryregistry(HKEY_CURRENT_USER, subkeyn.c_str(), key));
	if (!result.length()) {
		if (regdebug) errstream << "searching in HKEY_CURRENT_USER failed - trying HKEY_LOCAL_MACHINE " << endl;
		result = tryregistry(HKEY_LOCAL_MACHINE, subkeyn.c_str(), key);
	}
	return result;
#elif defined (__OS2__)
	//query a "real" OS/2 profile pstoedit.ini
	HINI hini;
	HAB hab;
	char pszFileName[1000];
	const char inifilename[] = "pstoedit.ini";
	APIRET rc;
	char buffer[1000];

	hab = WinInitialize(0);

	getini(0, errstream, pszFileName, inifilename, (int) sizeof(pszFileName));
	hini = PrfOpenProfile(hab, pszFileName);
	rc = PrfQueryProfileString(hini, typekey, key, nullptr, (PVOID) buffer, (LONG) sizeof(buffer));
	PrfCloseProfile(hini);
	WinTerminate(hab);

	if (rc) {
		// char *r = cppstrdup(buffer);
		return buffer;
	} else
		return RSString();
#else
//
// UNIX version
// Just ask the environment
//
	unused(&errstream);
#if 0
//getenv version
	char envname[1000];
	envname[0] = '\0';
	strcat(envname, typekey);
	strcat(envname, "_");
	strcat(envname, key);
//  cout << "checking " << envname << endl;
	char *envvalue = getenv(envname);
	if (envvalue != 0) {
		char *r = cppstrdup(envvalue);
//      cout << "found " << r << endl;
		return r;
	} else
		return 0;
#else

	char pathbuffer[255];
	// First look in HOME for .pstoedit.reg. If not found there, then look in PATH anywhere
	unsigned int searchresult = searchinpath(getenv("HOME"), ".pstoedit.reg", pathbuffer,
									   sizeof(pathbuffer));
	if (!searchresult)
		searchresult = searchinpath(getenv("PATH"), ".pstoedit.reg", pathbuffer, sizeof(pathbuffer));
	if (!searchresult)
		return RSString();

#if 0
	const char *homedir = getenv("HOME");
	if (!homedir)
		return 0;
	const int len = strlen(homedir) + 20;
	char *obuf = new char[len];
	strcpy(obuf, homedir);
	strcat(obuf, "/.pstoedit.reg");
	ifstream regfile(obuf);
//  cout << "opened " << obuf << endl;
	delete[]obuf;
#endif

	ifstream regfile(pathbuffer);
	if (!regfile)
		return RSString();

	const size_t len = 1000;
	char envname[len];
	envname[0] = '\0';
	strcat_s(envname, len, typekey);
	strcat_s(envname, len, "/");
	strcat_s(envname, len, key);
//  cout << "checking " << envname << endl;
	char line[1000];
	while (!regfile.eof()) {
		regfile.getline(line, 1000);
//      cout << line << endl;
		if (strstr(line, envname)) {
			char *r = cppstrdup(line + strlen(envname) + 1);
			// coverity[uninit_use_in_call]
			char *cr = strrchr(r, '\r');
			if (cr)
				*cr = 0;
//          cout << "found " << r << endl;
			const RSString result(r);
			delete [] r;
			return result;
		}
	}
	return RSString();
#endif
#endif
}
//lint -restore

void copy_file(istream & infile, ostream & outfile)
{
#if 1
	if ( infile.peek() != EOF ) {
		// Calling the copy operator in case the input file is empty results in the error state
		// of the target stream to be set to fail which then blocks any further IO on the output
		// This is not want we want, hence we have to check in advance whether there is something
		// available in the input buffer.
		// Alternatively I tried if (infile.rdbuf())->in_avail() ) 
		// but - in_avail seems to be compiler dependent. E.g. GNU seems to return
		// the number of available chars even without prior peek. MSVC seems to return 0
		// up to the first peek (or other operation).
		// 
#if defined(__SUNPRO_CC) && (__SUNPRO_CC < 0x500)
		// for SUN CC 4.2 rdbuf is non const - i guess this is an error since all other compilers have it const
		outfile << ((istream&)infile).rdbuf();
#else
		outfile << infile.rdbuf();
#endif
	}
#else
// long version. should do the same as above
	unsigned char c;
	while (infile.get(c)) {
		outfile.put(c);
	}
#endif
}

ostream & operator << (ostream & out, const Argv & a)
{
	for (unsigned int i = 0; i < (unsigned int) a.argc; i++) {
		out << a.argv[i];
		if (i < (unsigned int) ( a.argc -1 )) out << ' ';// out << endl;
	}
	return out;
}


TempFile::TempFile()
{
	tempFileName = cppstrdup(full_qualified_tempnam("pstmp").c_str());
//	tempFileName = TEMPNAM(0, "pstmp");
	// cout << "constructed " << tempFileName << endl; 
}

TempFile::~TempFile()
{
	// cout << "detructed " << tempFileName << endl; 
	close();
	(void) remove(tempFileName);
	delete [] tempFileName;
}

ofstream & TempFile::asOutput()
{
	close();
	outFileStream.open(tempFileName);
	if (outFileStream.fail())
		cerr << "opening " << tempFileName << " failed " << endl;
	return outFileStream; //lint !e1536 //exposing low access member
}

ifstream & TempFile::asInput()
{
	close();
	inFileStream.open(tempFileName);
	if (outFileStream.fail()) {
		cerr << "opening " << tempFileName << " failed " << endl;
	}
	return inFileStream; //lint !e1536 //exposing low access member
}

void TempFile::close()
{
//#ifdef HAVE_IS_OPEN
//#define IS_OPEN(file) file.is_open()
//#else
//#define IS_OPEN(file) file
//#endif
// commented, since it also works portably if we call clear()

//  if (IS_OPEN(inFileStream))  {
	inFileStream.close();
//      if (inFileStream.fail()) cerr << "closing inFileStream failed " << endl;
//  }
	inFileStream.clear();
//  if (IS_OPEN(outFileStream))  {
	outFileStream.close();
//      if (outFileStream.fail()) cerr << "closing outFileStream  failed " << endl;
//  }
	outFileStream.clear();
}


#if 0
void freeconst(const void *ptr)
{
	free((char *) ptr);			// just to have the warning about casting away constness
	// once (here)
}
#endif

size_t searchinpath(const char *EnvPath, const char *name,
						   char *returnbuffer, unsigned long buflen)
{
//      const char * EnvPath = getenv("PATH");
	if (!EnvPath)
		return 0;
#if defined(unix) || defined(__unix__) || defined(_unix) || defined(__unix) || defined(__EMX__) || defined (NetBSD) 
	const char separator = ':';
#else
	const char separator = ';';
#endif
	char *path = cppstrdup(EnvPath, 2);
	// append a separator at the end to make the loop below easier
	const size_t pathlen = strlen(path);
	path[pathlen] = separator;
	path[pathlen + 1] = '\0';	// remember, we reserved one char more
	char *colon = path;
	char *lastbegin = path;

//      cout << "looking for " << name << " in " << EnvPath << " sep is " << separator << endl;
	while (*colon) {
		while (*colon && (*colon != separator))
			colon++;
//          cout << "lb " << lastbegin << " *colon" << *colon << endl;
		if (*colon) {
			*colon = '\0';
			RSString test(lastbegin);
			test += "/";
			test += name;
			//cout << "checking " << test.c_str() << endl;
			if (fileExists(test.c_str())) {
				strcpy_s(returnbuffer,buflen, test.c_str());
				delete[]path;
				//cout << " FOUND !! " << endl;
				return strlen(returnbuffer);
			}
			colon++;
			lastbegin = colon;
		}
	}
	delete[]path;
	unused(&buflen);
	return 0;
}

unsigned long P_GetPathToMyself(const char *name, char *returnbuffer, unsigned long buflen)
{
#if defined (OS_WIN32_WCE)
	wchar_t wszReturnBuffer[MAX_PATH] = L"";
	unsigned long ulReturn =  GetModuleFileName(GetModuleHandle(LPSTRtoLPWSTR(name).c_str()), wszReturnBuffer, MAX_PATH);
	wszReturnBuffer[MAX_PATH-1] = L'\0';
	BSS_UTI_WCharToAscii(wszReturnBuffer, returnbuffer, buflen);
	return ulReturn;
#elif defined(_WIN32)
	return GetModuleFileName(GetModuleHandle(name), returnbuffer, buflen);
#elif defined (__OS2__)
	PTIB pptib;
	PPIB pppib;
	APIRET rc;
	if ((rc = DosGetInfoBlocks(&pptib, &pppib)) == 0)
		if ((rc = DosQueryModuleName(pppib->pib_hmte, buflen, returnbuffer)) == 0)
			return strlen(returnbuffer);
		else
			return 0;
	else
		return 0;
#else
	if ( (*name == '/')  || (*name == '.') ) {			// starts with / or .
		strcpy_s(returnbuffer, buflen, name);
		return strlen(returnbuffer);
	} else {
		return searchinpath(getenv("PATH"), name, returnbuffer, buflen);

	}
	return 0;					// not found
#endif
}

void errorMessage(const char *errortext)
{
#if 0 // defined(_WIN32)
	MessageBox(nullptr, errortext, "pstoedit", MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
#else
	cerr << errortext << endl;
#endif
}


DLLEXPORT RSString getOutputFileNameFromPageNumber(const char * const outputFileTemplate, const RSString & pagenumberformatOption, unsigned int pagenumber)
{
	const char PAGENUMBER_String[] = "%PAGENUMBER%";
	const char * pagestringptr_1 = strstr(outputFileTemplate,PAGENUMBER_String);
	const char * pagestringptr_2 = strstr(outputFileTemplate,"%d");
	if ((pagestringptr_1 == nullptr) && (pagestringptr_2 == nullptr)) {
		return RSString(outputFileTemplate);
	} else  {
		const size_t size = strlen(outputFileTemplate) + 30;
		std::unique_ptr<char[]> newname ( new char[ size ]);

		RSString formatting("%");
		formatting += pagenumberformatOption;
		formatting += RSString("d");
		// in simplest case it is %d
		// cout << "formatting " << formatting << endl;

		char pagenumberstring[30] ;

		// coverity[tainted_string_warning]
		sprintf_s(TARGETWITHLEN(pagenumberstring,sizeof(pagenumberstring)),formatting.c_str(),pagenumber);

		// cout << "pagenumberstring : " << pagenumberstring << endl;

		if (pagestringptr_1) // preference to %PAGENUMBER%
		{
			strncpy_s(newname.get(),size,outputFileTemplate,pagestringptr_1-outputFileTemplate); // copy up to %PAGENUMBER%
			// now copy page number
			strcat_s(newname.get(),size,pagenumberstring);
			// now copy trailer
			strcat_s(newname.get(),size,pagestringptr_1+strlen(PAGENUMBER_String));

		} else {
			strncpy_s(newname.get(),size,outputFileTemplate,pagestringptr_2-outputFileTemplate); // copy up to %PAGENUMBER%
			// now copy page number
			strcat_s(newname.get(),size,pagenumberstring);
			// now copy trailer
			strcat_s(newname.get(),size,pagestringptr_2+strlen("%d"));
		}
		//cout << "newname " << newname << endl;
		const RSString result (newname.get());
		return result;
	}
}

#ifndef DONOTIMPLEMENTRSSTRING
// a very very simple resizing string
RSString::RSString(const char *arg) : content(0), allocatedLength(0), stringlength(0)
{
	if (arg) {
		this->assign(arg,strlen(arg));
	}
	// cerr << "{ constructed" << (void*) this << endl;
}

RSString::RSString(const char arg) : content(0), allocatedLength(0), stringlength(0)
{
	char tmp[2];
	tmp[0] = arg; tmp[1] = '\0';
	this->assign(tmp,1);
}

RSString::RSString(const char * arg  , const size_t len) :content(0), allocatedLength(0), stringlength(len)
{
	if (arg) {
		this->assign(arg,stringlength);
	}
}


RSString::RSString(const RSString & s):content(0), allocatedLength(0), stringlength(0)
{
	assert(this != &s);
	this->assign(s.c_str(),s.stringlength);
}

char *RSString::newContent(size_t size)
{
	return new char[size];
}

void RSString::clearContent()
{
	delete[]content;
	content = 0;
	stringlength = 0;
}

RSString::~RSString()
{
	// cerr << (void*) this << "deleted }" << endl;
	clearContent();				//lint !e1506 // call to virtual function in dtor
	allocatedLength = 0;
}

bool RSString::contains(const RSString & s) const
{
	return strstr(c_str(),s.c_str()) != 0;
}

RSString & RSString::operator += (const char* rs)
{
	assert(rs != 0);
	assert(content != 0);
	const size_t rslength = strlen(rs);
	const size_t newlen = stringlength + rslength  + 1;
	char *newstring = newContent(newlen);
//cout << ":" << content << ":" << stringlength << endl;
//cout << ":" << rs.content << ":" << rs.stringlength << endl;
	for (unsigned int i = 0 ; i < stringlength; i++)
	{
		newstring[i] = content[i];
	}
//	strncpy(newstring, content, stringlength);
	for (unsigned int j = 0 ; j < rslength ; j++)
	{
		newstring[stringlength+j] = rs[j];
	}
//	strncat(newstring, rs.content, rs.stringlength);
	newstring[newlen-1] = '\0';
	clearContent();
	content = newstring;
	allocatedLength = newlen;
	stringlength= newlen-1;
	return *this;
}

RSString & RSString::operator += (const RSString & rs)
{
	return (*this) += rs.content;
}


void RSString::assign(const char *src)
{
	assign(src,strlen(src));

}
//      const char *c_str() const { return content; }
void RSString::assign(const char *src, const size_t len )
{
//          cerr << "copy " << src << " to " << (void *) this << endl;
	if (src == 0) {
		// nothing to copy - just clear
		clearContent();
	} else if (content && ((len + 1) <= allocatedLength)) {
		// we have enough space
		for (unsigned int i = 0 ; i < len+1; i++)
		{
			content[i] = src[i];
		}
		//::strncpy(content, src, len+1);
		
	} else {
		// resize
//              cerr << "content before delete is " << (void *) content << endl;
		clearContent();
		allocatedLength = len + 1;
		content = newContent(allocatedLength);
//              cerr << "content after new is " << (void *) content << endl;
		for (unsigned int i = 0 ; i < len+1; i++)
		{
			content[i] = src[i]; 
		}
		//::strncpy(content, src, len+1);
	}
	stringlength = len;
}
#endif

bool fileExists(const char *filename)
{
	std::ifstream test(filename);
	return test.is_open();
}


//FontMapper::~FontMapper() { 
	// cerr << "destroying FontMapper" << endl;

//}

static void skipws(char *&lineptr)
{
	while ( (*lineptr != '\0') && ((*lineptr == ' ') || (*lineptr == '\t')))
		lineptr++;
	return;
}

static char *readword(char *&lineptr)
{
	char *result = nullptr;
	if (*lineptr == '"') {
		result = strtok(lineptr, "\"");
	} else {
		result = strtok(lineptr, "\t ");
	}
	if (result) {
		lineptr = result + strlen(result) + 1;
	}
	return result;
}

void FontMapper::readMappingTable(ostream & errstream, const char *filename)
{
	if (!fileExists(filename)) {
		errstream << "Could not open font map file " << filename << endl;
		return;
	}
	ifstream inFile(filename);
	const int linesize = 255;
	char line[linesize];
	char save[linesize];
	unsigned int linenr = 0;
	while (!inFile.getline(line, linesize).eof()) {
		linenr++;
		strcpy_s(save,linesize, line);
 
		// Notes regarding ANSI C++ version (from KB)
		// istream::get( char* pch, int nCount, char delim ) is different in three ways: 
		// When nothing is read, failbit is set.
		// An eos is always stored after characters extracted (this happens regardless of the outcome).
		// A value of -1 for nCount is an error.

		// If the line contains just a \n then the failbit
		// is set in the ANSI version
		if (inFile.gcount() == 0) {
			inFile.clear();
			continue;
		}

		if (line[0] == '%')
			continue;
		char *lineptr = line;
		//skip initial spaces
		skipws(lineptr);
//		unsigned int linelength = strlen(line);
		if ((*lineptr) == '\0' ) {
			// empty line - ignore
		//	errstream << "empty line in fontmap - ignored " << endl;
			continue;
		}
		const char * const original = readword(lineptr);
		skipws(lineptr);
		const char * const replacement = readword(lineptr);
		if (original && replacement) {
			// errstream << "\"" << original << "\" \"" << replacement <<"\""<< endl;
		        if (replacement[0] == '/') {
				// Map to an existing entry.
			  	const RSString * prevEntry = getValue(replacement+1);
				if (prevEntry == nullptr)
					errstream << "undefined font " <<
					  replacement+1 <<
					  " found in line (" << linenr <<
					  ") of fontmap: " << save << endl;
				else
					insert(original, *prevEntry);
			}
			else
				insert(original, replacement);
		} else {
			errstream << "unexpected line (" << linenr <<
				") found in fontmap: " << save << endl;
		}
	}
}

const char *FontMapper::mapFont(const RSString & fontname) const
{
#if 0
	FontMapping *curEntry = firstEntry;
	// cerr << "Trying to remap font" << fontname << endl;
	while (curEntry != 0) {
		// cerr << "comparing with" << curEntry->original << endl;
		if (curEntry->original == RSString(fontname)) {
			// cerr << "mapped to " << curEntry->replacement.c_str() << endl;
			return curEntry->replacement.c_str();
		}
		curEntry = curEntry->nextEntry;
	}
	return 0;
#endif
	const RSString *r = getValue(fontname);
	if (r) {
		return r->c_str();
	} else {
		for (unsigned int i=0; i<fontname.length(); i++) {
			// patch from Scott Pakin
			// I've noticed that Microsoft's PostScript printer driver, pscript5.dll, 
			// produces EPS files with subsetted fonts and names the subsetted fonts 
			// using a random (?) prefix.  For example, in the attached EPS file, the 
			// subset of Officina Serif Bold is not called OfficinaSerif-Bold but rather 
			// HJCBAA+OfficinaSerif-Bold.  When pstoedit tries to map that to a TeX name 
			// (e.g., for the mpost driver) it fails because mpost.fmp knows only 
			// OfficinaSerif-Bold.
			//
			// The attached patch adds a fallback case to FontMapper::mapFont().  
			// If the specified font can't be mapped and it contains a "+" character, 
			// the method tries a second time starting from after the "+".
	        if (fontname[i] == '+') {
		        const RSString altfontname(fontname.c_str() + i + 1);
				r = getValue(altfontname);
				if (r)
			        return r->c_str();
				else 
					return nullptr;
			}
		}
		return nullptr;
    }
}

unsigned int Argv::parseFromString(const char * const argstring) {
	unsigned int nrOfNewArgs = 0;
	const char * cp = argstring;
	while (cp && *cp) { // for all args
		while (*cp == ' ') cp++; // skip leading space
		if (!*cp) break;
		RSString result("");		
		if (*cp == '"')	{ // handle string arg - read everything until closing "
			cp++; // skip leading "
			while (*cp && (*cp != '"')) {
				result += *cp; 
				cp++;
			}
			if (*cp) cp++; // skip trailing "
		} else {
			while (*cp && (*cp != ' ')) {
				result += *cp; 
				cp++;
			}
			assert(result.length() > 0);
		}
		addarg(result);
		nrOfNewArgs++;
	}
	return nrOfNewArgs;
}
