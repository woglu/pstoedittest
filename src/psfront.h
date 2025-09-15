#ifndef __psfront_h
#define __psfront_h
/*
   psfront.h : This file is part of pstoedit
   contains the class responsible for reading the dumped PostScript format
  
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
/*
// ============================================================================
//
// = LIBRARY
//     pstoedit
//
// = FILENAME
//     psfront.h
//
// = RCSID
//     $Id$
*/

#include "drvbase.h"

class PSFrontEnd {
public:
	PSFrontEnd(ostream & outfile_p, 
			ostream & errstream,
			PsToEditOptions & globaloptions_p,
			ProgramOptions * driverOptions_p, // if created from GUI
			const char * infilename_p = nullptr,
			const char * outfilename_p = nullptr, 
			const DriverDescription * driverDesc_p = nullptr, 
			const char * driveroptions_p = nullptr,
			const bool splitpages_p = false, 
			drvbase * backend_p = nullptr);
	~PSFrontEnd();

	void 		run(bool mergelines);	// do the conversion

	unsigned int readBBoxes(BBox * bboxes);

private:

	void            addNumber(float a_number); // add a number to the current path

	float           pop(); // pops and returns last value on stack

	float           popUnScaled(); // pops and returns last value on stack

	void		pstack() const; // for debugging

	int 		yylex();    // read the input and call the backend specific
				    // functions
	static void 	yylexcleanup(); // called from destructor


private:
	ostream &	outFile; 
	ostream &	errf;           // the error stream
	const char * 	infilename;	
	const char * 	outfilename;
	PsToEditOptions & globaloptions;
	ProgramOptions* driverOptions;
	const DriverDescription * driverDesc;
	const char * 	driveroptions;
	const bool	splitpages;
	drvbase *	backend;
	unsigned int    currentPageNumber;
	unsigned int    lineNumber;
	std::vector<float> numbers;
	unsigned int	nextFreeNumber;
	unsigned int	pathnumber; // number of path (for debugging)
	bool 		non_standard_font;
	bool		constraintsChecked;
	Point		currentpoint;
	bool		bblexmode; // indicates whether just the scanning for the Bounding Box is needed
	BBox *		bboxes_ptr;

	// Inhibitors (declared, but not defined)
	PSFrontEnd(const PSFrontEnd&) = delete;
	PSFrontEnd() = delete;
	PSFrontEnd& operator=(const PSFrontEnd&) = delete;
};


#endif
 
 
 
