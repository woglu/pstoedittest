#ifndef __drvPCB1_h
#define __drvPCB1_h

/* 
   drvPCB1.cpp : Hans-Jürgen Jahn    (Version 1.0  10.08.02)
   implements output device for the conversion of a PCB layout
   in PostScript to engraving data.
   this code is derived from drvSAMPL.cpp, see text below

   drvsampl.h : This file is part of pstoedit
   Class declaration for a sample output driver with no additional attributes
   and methods (minimal interface)

   Copyright (C) 1993,1994,1995,1996,1997,1998 Wolfgang Glunz, wglunz35_AT_geocities.com

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

#include "drvbase.h"

class drvPCB1 : public drvbase {

public:

	derivedConstructor(drvPCB1);
	
	~drvPCB1() override; // Destructor
	class DriverOptions : public ProgramOptions { } *options;

#include "drvfuncs.h"

	void print_coords();
//	void show_rectangle(const float llx, const float lly, const float urx, const float ury);
	void show_text(const TextInfo & textinfo) override;

private:
	ofstream pcberrf;
	bool drill_data, drill_fixed;
	float drill_diameter;
	
	bool lineOut();
	bool filledRectangleOut();
	bool filledCircleOut();
	

};

#endif
 
