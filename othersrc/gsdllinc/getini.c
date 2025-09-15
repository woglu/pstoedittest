/* Copyright (C) 1998, Russell Lang.  All rights reserved.
  
	Find the path to the gsview32.ini. Special version for pstoedit.
	Merged w/ finder for gvpm.ini (OS/2).

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

#ifndef __OS2__

#ifndef _WIN32
#include <dirent.h>
#endif
#include <windows.h>
#include <iostream>
using namespace std;
#include <assert.h>

static void getini(int verbose,ostream & errstream, char* szIniFile,const char *INIFILEname,DWORD sizeofIniFile)
{

#if 0
	/* get path to EXE */
	GetModuleFileName(phInstance, szExePath, sizeof(szExePath));
	if ((p = strrchr(szExePath,'\\')) != (char *)NULL)
	    p++;
	else
	    p = szExePath;
	*p = '\0';
#endif
	assert(szIniFile);

	/* get path to INI file */
	szIniFile[0] = '\0';
	/* strcpy(szIniFile, szExePath); */
#ifdef _WIN32
#if 0
	const DWORD version = GetVersion();
	bool is_win4 = false;
	if (LOBYTE(LOWORD(version)) >= 4) is_win4 = true;
#else
	const bool is_win4 = true;
#endif
	/* allow for user profiles */
	if (is_win4) {
	    LONG rc;
	    HKEY hkey;
	    DWORD keytype;
	    DWORD cbData;
	    DWORD fa;
	    /* Find the user profile directory */
		rc = RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\ProfileReconciliation"), 0, KEY_READ, &hkey);
	    if (rc == ERROR_SUCCESS) {
			cbData = sizeofIniFile - strlen(INIFILEname);
//			cbData = sizeof(szIniFile)-sizeof(INIFILEname);
//			cout << "cbdata" << (int) cbData << endl;
			keytype =  REG_SZ;
			rc = RegQueryValueEx(hkey, TEXT("ProfileDirectory"), 0, &keytype, (LPBYTE)szIniFile, &cbData);
			(void)RegCloseKey(hkey);
	    }
	    if (rc == ERROR_SUCCESS) {
			if (verbose) { 
				errstream << "Found ProfileDirectory ! " << endl;
			}
#ifdef OS_WIN32_WCE
			fa = GetFileAttributes(LPSTRtoLPWSTR(szIniFile).c_str());
#else
			fa = GetFileAttributes(szIniFile);
#endif

			if ((fa != 0xffffffff) && (fa & FILE_ATTRIBUTE_DIRECTORY))
			  strcat_s(szIniFile,sizeofIniFile, "\\");
			else
				szIniFile[0] = '\0';
			}
	    else {
		    /* If we didn't succeed, use the Windows directory */
		    szIniFile[0] = '\0';
	    }
	}
	if (szIniFile[0] == '\0') {
	    DWORD fa;
	    /* If we didn't succeed, try %USERPROFILE% */
	    char *p = getenv("USERPROFILE");
	    if (p && *p) {
			strcpy_s(szIniFile,sizeofIniFile, p);
#ifdef __BORLANDC__
			OemToCharBuff(szIniFile, szIniFile, lstrlen(szIniFile));
#endif
			p = szIniFile + strlen(szIniFile) - 1;
			if ((*p == '\\') || (*p == '/'))
			   *p = '\0';
		/* check if USERPROFILE contains a directory name */
#ifdef OS_WIN32_WCE
			fa = GetFileAttributes(LPSTRtoLPWSTR(szIniFile).c_str());
#else
			fa = GetFileAttributes(szIniFile);
#endif
			if ((fa != 0xffffffff) && (fa & FILE_ATTRIBUTE_DIRECTORY))
				strcat_s(szIniFile,sizeofIniFile, "\\");
			else
				szIniFile[0] = '\0';
	    }
	}
#else
	{
	    char *p = getenv("USERPROFILE");

	    if (p && *p) {
			strcpy(szIniFile, p); 
#ifdef __BORLANDC__
			OemToAnsiBuff(szIniFile, szIniFile, lstrlen(szIniFile));
#endif
			p = szIniFile + strlen(szIniFile) - 1;
			if ((*p == '\\') || (*p == '/'))
			   *p = '\0';
		/* check if USERPROFILE contains a directory name */

			DIR *d;
			d = opendir(szIniFile);
			if (d) {
			    closedir(d);
			    strcat(szIniFile, "\\");
			}
	    }
	}
#endif
	strcat_s(szIniFile,sizeofIniFile, INIFILEname);
}

#else /*OS/2*/

static void getini(int verbose,ostream & errstream, char* szIniFile,
		   const char *INIFILEname, unsigned int sizeofIniFile)
  /* get path to INI FILE */
{
  char *tail, *env;
  FILESTATUS3  fsts3ConfigInfo = {{0}};
  ULONG        ulBufSize     = sizeof(FILESTATUS3);
  ULONG         inSysDir       = 0;
  APIRET        rc;
      
  /* first, look in SYSTEM_INI dir */
  if ((env = getenv("SYSTEM_INI")) != (char *)NULL) { 
    strcpy(szIniFile, env);
    inSysDir=1;
    if ((tail = strrchr(szIniFile,'\\')) != (PCHAR)NULL) {
      tail++;
      *tail = '\0';
    }
  }
  if (inSysDir) {
    strcat(szIniFile, INIFILEname);
    rc = DosQueryPathInfo((PSZ)szIniFile, FIL_STANDARD, &fsts3ConfigInfo, ulBufSize);
    if (rc != 0)  inSysDir=0;
  }

  /* second, look in the dir of the executable */
  if (!inSysDir)
    {
      PTIB pptib;
      PPIB pppib;
      char szExePath[CCHMAXPATH];
      if ( DosGetInfoBlocks(&pptib, &pppib) == 0 ) 
	/* get path to EXE */
	if ( DosQueryModuleName(pppib->pib_hmte, sizeof(szExePath), szExePath) == 0 ) 
	  {
	    if ((tail = strrchr(szExePath,'\\')) != (PCHAR)NULL) {
	      tail++;
	      *tail = '\0';
	    }
	    strcpy(szIniFile, szExePath);
	    strcat(szIniFile, INIFILEname);
	    rc = DosQueryPathInfo((PSZ)szIniFile, FIL_STANDARD, &fsts3ConfigInfo, ulBufSize);
	    if (rc != 0)
	      strcpy(szIniFile, INIFILEname);
	  }
	else
	  strcpy(szIniFile, INIFILEname);
    }
}

#endif
