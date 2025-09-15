/*
   cmdmain.cpp : This file is part of pstoedit
   main program for command line usage

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
#define TESTDLL 0

#include "cppcomp.h"
#include "pstoedit.h"
#ifdef TESTDLL
#include "poptions.h"
#include <assert.h>
#endif

#include I_iostream

#ifdef OS_WIN32_WCE
#include "WinCEAdapter.h"
#endif


int main(int argc, const char *const argv[])
{
#if defined(_WIN32) || defined (__OS2__)
    if (!pstoedit_checkversion(pstoeditdllversion)) {
	    return 1;
    }
    if (!getenv("PSTOEDITUSEGSEXE")) {
        setPstoeditsetDLLUsage(true); // use the gs DLL in stand-alone mode
    }
#else
	ignoreVersionCheck();
#endif
	// on UNIX like systems, we can use cerr as error stream
	// except for some help/diag messages which are preferred on cout 
	useCoutForDiag(true);
#if TESTDLL
    // test code    
    cout << "running DLL test code" << endl;

    ProgramOptions* const poptions = getProgramOptions();
    assert(poptions);
    // poptions->showvalues(cout, true);
#if 0
    const std::vector<OptionBase*>& options = poptions->getOptions();
    int maxheight = 0;
    for (auto iterator : options) {
        const unsigned int curtype = (iterator)->gettypeID();
        cout << curtype << endl;
    }
#endif
    unsigned int nr = poptions->numberOfOptions();
    cout << "nr of options: " << nr << endl;
    for (unsigned int i = 0; i < nr; i++) {
        cout << i << " " << poptions->iThOption(i)->gettypeID() << endl;
    }

    const int result = 0;
#else
	const int result = pstoeditwithghostscript(argc, argv, cerr, nullptr, nullptr);
#endif
#if MSVC_CHECK_LEAKS
    cout << "now checking memory leaks" << endl;
    _CrtDumpMemoryLeaks();
#endif
	return result;
}
