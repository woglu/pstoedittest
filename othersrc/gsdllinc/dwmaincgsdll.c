/* Copyright (C) 1998, Russell Lang.  All rights reserved.
  
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


/* dwmainc.c */
/* Ghostscript DLL loader for Win32 */
/* For console mode application */

/* This is intended as example code, showing how you can
 * use the Ghostscript DLL from your application.
 * If you rename the main() function below to gsdll_main() 
 * and link this file with your program, you can call 
 * Ghostscript using gsdll_main().
 * Note that your application *must* be a console application
 * to use this code, otherwise you will need to remove all
 * references to stdin and stdout from this file.
 *
 * Example compilation:
 *   bcc32 -v -DGS_REVISION=510 -Ik:\bc45\include -Lk:\bc45\lib dwmainc.c
 *
 * This is similar to the OS/2 file dpmainc.c.
 */

/* Russell Lang  1998-01-06 */
#include "cppcomp.h"

#if defined (_WIN32)
#include <windows.h>
// the next define is needed in gsdll.h
#define _Windows

#elif defined (__OS2__)
#define INCL_DOS
#define INCL_WIN
#include <os2.h>
#endif

#include I_stdio
#include I_string_h
#include <fcntl.h>
#include <io.h>

// old DLL (uses aladdin lic) #include "gsdll.hpp"
#include "gsdll.h"

#define MAXSTR 256
#if defined (_WIN32)
const char *szDllName = "gsdll32.dll";
#elif defined (__OS2__)
const char *szDllName = "GSDLL2.DLL";
#endif
const char start_string[] = "systemdict /start get exec\n";
int debug = FALSE;



/* main structure with info about the GS DLL */
typedef struct tagGSDLL {
	BOOL		valid;		/* true if loaded */
#if defined (_WIN32)
	HINSTANCE	hmodule;	/* handle to module */
#elif defined (__OS2__)
        HMODULE hmodule;		/* handle to module */
#endif
	/* pointers to DLL functions */
	PFN_gsdll_revision	revision;
	PFN_gsdll_init		init;
	PFN_gsdll_execute_begin	execute_begin;
	PFN_gsdll_execute_cont	execute_cont;
	PFN_gsdll_execute_end	execute_end;
	PFN_gsdll_exit		exit;
	PFN_gsdll_lock_device	lock_device;
#if defined (_WIN32)
#ifdef OLDGSDLL
	PFN_gsdll_copy_dib	copy_dib;
	PFN_gsdll_copy_palette	copy_palette;
	PFN_gsdll_draw		draw;
#endif
	/* pointer to mswindll device */
	char FAR *device;
#elif defined (__OS2__)
        PFN_gsdll_get_bitmap    get_bitmap;
        /* pointer to os2dll device */
        char *device;
#endif
} GSDLL;
GSDLL gsdll;


typedef int  (gs_write_callback_func) (void * cb_data, const char* text, unsigned long length);

static int
default_gs_addmess(void * /* cb_data */ , const char* text, unsigned long length)
{
    //fputs(text, stdout);
	fwrite(text,1,length,stdout);
	fflush(stdout);
	return length;
}

static gs_write_callback_func* current_write_callback = default_gs_addmess;


void set_gs_write_callback(gs_write_callback_func* new_cb) {
	current_write_callback = new_cb;
}

static char messagebuffer[1000]; // just for local formatting
static void write_message() {
	current_write_callback(0,messagebuffer,strlen(messagebuffer));
}
/* free GS DLL */
/* This should only be called when gsdll_execute has returned. */
/* TRUE means no error. */
BOOL
gs_free_dll(void)
{
BOOL flag;
#if defined (_WIN32)
    if (gsdll.hmodule < (HINSTANCE)HINSTANCE_ERROR) 
	flag = TRUE;
    else
        flag = FreeLibrary(gsdll.hmodule);
    gsdll.hmodule = NULL;
#elif defined (__OS2__)
    if (gsdll.hmodule == (HMODULE) NULL)
	flag=TRUE;
    else
      flag = !DosFreeModule(gsdll.hmodule);
    gsdll.hmodule = (HMODULE) NULL;
#endif
    gsdll.revision = NULL;
    gsdll.init = NULL;
    gsdll.execute_begin = NULL;
    gsdll.execute_cont = NULL;
    gsdll.execute_end = NULL;
    gsdll.exit = NULL;
    gsdll.lock_device = NULL;
#if defined (_WIN32)
#ifdef OLDGSDLL
    gsdll.copy_dib = NULL;
    gsdll.copy_palette = NULL;
    gsdll.draw = NULL;
#endif
#elif defined (__OS2__)
    gsdll.get_bitmap = NULL;
#endif

    gsdll.device = NULL;

    return flag;
}

BOOL
gs_load_dll_cleanup(void)
{
    (void) gs_free_dll();
    return FALSE;
}

/* load GS DLL if not already loaded */
/* return TRUE if OK */
BOOL
gs_load_dll(void)
{
#if defined (_WIN32)
	// long version;
    gsdll.hmodule = LoadLibrary(szDllName);
    if (gsdll.hmodule < (HINSTANCE)HINSTANCE_ERROR)
	return FALSE;

    /* DLL is now loaded */
    /* Get pointers to functions */
    if ( (gsdll.revision = (PFN_gsdll_revision) 
	GetProcAddress(gsdll.hmodule, "gsdll_revision")) == NULL)
	return gs_load_dll_cleanup();
	
#ifdef GS_REVISION
    /* check DLL version  [OPTIONAL] */
    gsdll.revision(NULL, NULL, &version, NULL);
    if (version != (long)GS_REVISION) {
	sprinft(messagebuffer,"Wrong version of DLL found.\n  Found version %ld\n\
  Need version  %ld\n", version, (long)GS_REVISION);
	write_message();
	return gs_load_dll_cleanup();
    }
#endif

    /* continue loading other functions */
    if ( (gsdll.init = (PFN_gsdll_init) 
	GetProcAddress(gsdll.hmodule, "gsdll_init")) == NULL)
	return gs_load_dll_cleanup();
    if ( (gsdll.execute_begin = (PFN_gsdll_execute_begin) 
	GetProcAddress(gsdll.hmodule, "gsdll_execute_begin")) == NULL)
	return gs_load_dll_cleanup();
    if ( (gsdll.execute_cont = (PFN_gsdll_execute_cont) 
	GetProcAddress(gsdll.hmodule, "gsdll_execute_cont")) == NULL)
	return gs_load_dll_cleanup();
    if ( (gsdll.execute_end = (PFN_gsdll_execute_end) 
	GetProcAddress(gsdll.hmodule, "gsdll_execute_end")) == NULL)
	return gs_load_dll_cleanup();
    if ( (gsdll.exit = (PFN_gsdll_exit) 
	GetProcAddress(gsdll.hmodule, "gsdll_exit")) == NULL)
	return gs_load_dll_cleanup();
    if ( (gsdll.lock_device = (PFN_gsdll_lock_device) 
	GetProcAddress(gsdll.hmodule, "gsdll_lock_device")) == NULL)
	return gs_load_dll_cleanup();
#ifdef OLDGSDLL
    if ( (gsdll.copy_dib = (PFN_gsdll_copy_dib) 
	GetProcAddress(gsdll.hmodule, "gsdll_copy_dib")) == NULL)
	return gs_load_dll_cleanup();

    if ( (gsdll.copy_palette = (PFN_gsdll_copy_palette) 
	GetProcAddress(gsdll.hmodule, "gsdll_copy_palette")) == NULL)
	return gs_load_dll_cleanup();
    if ( (gsdll.draw = (PFN_gsdll_draw) 
	GetProcAddress(gsdll.hmodule, "gsdll_draw")) == NULL)
	return gs_load_dll_cleanup();
#endif 
#elif defined (__OS2__)
    char buf[MAXSTR + 40];
    APIRET rc;
    char *p;
    const char *dllname;
    PTIB pptib;
    PPIB pppib;
    char szExePath[MAXSTR];
    char fullname[1024];
    const char *shortname;

    if ((rc = DosGetInfoBlocks(&pptib, &pppib)) != 0) {
      //	fprintf(stdout, "Couldn't get pid, rc = \n", rc);
	return FALSE;
    }
    /* get path to EXE */
    if ((rc = DosQueryModuleName(pppib->pib_hmte, 
				 sizeof(szExePath), szExePath)) != 0) {
      if (debug) {
	sprintf(messagebuffer, "Couldn't get module name, rc = %ld\n", rc);
	write_message();
      }
	return FALSE;
    }
    if ((p = strrchr(szExePath, '\\')) != (char *)NULL) {
	p++;
	*p = '\0';
    }
    dllname = szDllName;
    if (debug) {
	sprintf(messagebuffer, "Trying to load %s\n", dllname);
	write_message();
    }
    memset(buf, 0, sizeof(buf));
    rc = DosLoadModule((PBYTE)buf, sizeof(buf), (PCSZ)dllname, &gsdll.hmodule);
    if (rc) {
	/* failed */
	/* try again, with path of EXE */
	if ((shortname = strrchr((const char *)szDllName, '\\')) == 
	    (const char *)NULL)
	    shortname = szDllName;
	strcpy(fullname, szExePath);
	if ((p = strrchr(fullname, '\\')) != (char *)NULL)
	    p++;
	else
	    p = fullname;
	*p = '\0';
	strcat(fullname, shortname);
	dllname = fullname;
	if (debug) {
	    sprintf(messagebuffer, "Trying to load %s\n", dllname);
	    write_message();
	}
	rc = DosLoadModule((PBYTE)buf, sizeof(buf), (PCSZ)dllname, &gsdll.hmodule);
	if (rc) {
	    /* failed again */
	    /* try once more, this time on system search path */
	    dllname = shortname;
	    if (debug) {
		sprintf(messagebuffer, "Trying to load %s\n", dllname);
		write_message();
	    }
	    rc = DosLoadModule((PBYTE)buf, sizeof(buf), (PCSZ)dllname, &gsdll.hmodule);
	}
    }
    if (rc != 0) 
      return FALSE;

    if (debug) {
      sprintf(messagebuffer,"Loaded Ghostscript DLL\n");
      write_message();
    }

    if ((rc = DosQueryProcAddr(gsdll.hmodule, 0, (PCSZ)"GSDLL_REVISION", 
			       (PFN *) (&gsdll.revision))) != 0) 
      return gs_load_dll_cleanup();
	
#ifdef GS_REVISION
    /* check DLL version */
    gsdll.revision(NULL, NULL, &revision, NULL);
    if (revision != GS_REVISION) {
      sprintf(messagebuffer, "Wrong version of DLL found.\n  Found version %ld\n  Need version  %ld\n", revision, (long)GS_REVISION);
      write_message();
      return gs_load_dll_cleanup();;
    }
#endif

    if ((rc = DosQueryProcAddr(gsdll.hmodule, 0, (PCSZ)"GSDLL_INIT", 
			       (PFN *) (&gsdll.init))) != 0) 
      return gs_load_dll_cleanup();
    if ((rc = DosQueryProcAddr(gsdll.hmodule, 0, (PCSZ)"GSDLL_EXECUTE_BEGIN", 
			       (PFN *) (&gsdll.execute_begin))) != 0) 
      return gs_load_dll_cleanup();
    if ((rc = DosQueryProcAddr(gsdll.hmodule, 0, (PCSZ)"GSDLL_EXECUTE_CONT", 
			       (PFN *) (&gsdll.execute_cont))) != 0) 
      return gs_load_dll_cleanup();
    if ((rc = DosQueryProcAddr(gsdll.hmodule, 0, (PCSZ)"GSDLL_EXECUTE_END", 
			       (PFN *) (&gsdll.execute_end))) != 0) 
      return gs_load_dll_cleanup();
    if ((rc = DosQueryProcAddr(gsdll.hmodule, 0, (PCSZ)"GSDLL_EXIT", 
			       (PFN *) (&gsdll.exit))) != 0) 
      return gs_load_dll_cleanup();
    if ((rc = DosQueryProcAddr(gsdll.hmodule, 0, (PCSZ)"GSDLL_LOCK_DEVICE", 
			       (PFN *) (&gsdll.lock_device))) != 0) 
      return gs_load_dll_cleanup();
    if ((rc = DosQueryProcAddr(gsdll.hmodule, 0, (PCSZ)"GSDLL_GET_BITMAP", 
			       (PFN *) (&gsdll.get_bitmap))) != 0) 
      return gs_load_dll_cleanup();
#endif

    return TRUE;
}


int
read_stdin(char FAR *str, int len)
{
int ch;
int count = 0;
while (count < len) {
	ch = fgetc(stdin);
	if (ch == EOF)
	    return count;
	*str++ = (char) ch;
	count++;
	if (ch == '\n')
	    return count;
}
return count;
}

int 
gsdll_callback(int message, char *str, unsigned long count)
{
    switch (message) {
	case GSDLL_STDIN:
	    return read_stdin(str, count);
	case GSDLL_STDOUT:
	    if (str != (char *)NULL)
//		fwrite(str, 1, count, stdout);
//	    fflush(stdout);
		current_write_callback(0,str,count);
	    return count;
	case GSDLL_DEVICE:
		if (count) {
#if defined (_WIN32)
#define SYSDLL mswindll
#elif defined (__OS2__)
#define SYSDLL os2dll
#endif
	sprintf(messagebuffer,"SYSDLL \
device is not supported by the command line version of Ghostscript.\n\
Select a different device using -sDEVICE= as described in use.txt.\n\
	    Callback: DEVICE %p %s\n", str, count ? "open" : "close");
			write_message();
		}
	    break;
	case GSDLL_SYNC:
	    sprintf(messagebuffer,"Callback: SYNC %p\n", str);
		write_message();
	    break;
	case GSDLL_PAGE:
	    sprintf(messagebuffer,"Callback: PAGE %p\n", str);
		write_message();
	    break;
	case GSDLL_SIZE:
	    sprintf(messagebuffer,"Callback: SIZE %p width=%d height=%d\n", str,
		(int)(count & 0xffff), (int)((count>>16) & 0xffff) );
		write_message();
	    break;
	case GSDLL_POLL:
	    return 0; /* no error */
	default:
	    sprintf(messagebuffer,"Callback: Unknown message=%d\n",message);
		write_message();
	    break;
    }
    return 0;
}

int
main(int argc, char *argv[])
{
	int code;
#if defined(_WIN32)
    (void) setmode(fileno(stdin), O_BINARY);
#endif
    if (!gs_load_dll()) {
		fprintf(stderr, "Can't load %s\n", szDllName);
		return -1;
    }
    code = gsdll.init(gsdll_callback, (HWND)NULL, argc, argv);

    if (!code)
        code = gsdll.execute_begin();
    if (!code) {
		code = gsdll.execute_cont(start_string, strlen(start_string));
		if (!code) {
			gsdll.execute_end();
			gsdll.exit();
		} else
			code = gsdll.exit();
    }
    (void) gs_free_dll();
    if (code == GSDLL_INIT_QUIT) return 0;
    return code;
}
