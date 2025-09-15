#ifndef __drvMMA_h
#define __drvMMA_h

/* 
   drvmma.h : This file is part of pstoedit
   Call declaration for Mathematica Graphics driver
   Contributed by: Manfred Thole <manfred@thole.org>
   $Id: drvmma.h,v 1.4 2003/03/15 16:25:47 manfred Exp $
   Based on drvsampl.h

   Copyright (C) 1993 - 2024 Wolfgang Glunz, wglunz35_AT_pstoedit.net
                             Manfred Thole, manfred@thole.org

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
#include <math.h>

class drvMMA : public drvbase {

public:

	derivedConstructor(drvMMA);

	~drvMMA() override; // Destructor
	class DriverOptions : public ProgramOptions { 
	public:
		OptionT < bool, BoolTrueExtractor> eofillFills;
		DriverOptions() :
			eofillFills(true,"-eofillfills","",0,"Filling is used for eofill (default is not to fill)",nullptr,false)
		{
			ADD(eofillFills);
		}
	}*options;

#include "drvfuncs.h"
	void show_text(const TextInfo & textinfo) override;

 private:
	void print_coords();
	void RGBColor(float R, float G, float B);
	void draw_path(bool close, Point firstpoint, bool filled);
	// eofill approximation

	// Graphics attributes
	linetype mmaDashing;
	float mmaThickness;
	float mmaR, mmaG, mmaB;

	// temp file for buffering a subpath
  	TempFile tempFile;
	ofstream &buffer;  
};

#endif
 
