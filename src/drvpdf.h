#ifndef __drvPDF_H
#define __drvPDF_H

/* 
   drvPDF.h : This file is part of pstoedit
   Backend for PDF format 

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
#include "drvbase.h"

const  unsigned int maxobjects = 1000;

class drvPDF : public drvbase {

public:

	derivedConstructor(drvPDF);
	~drvPDF() override; // Destructor
	class DriverOptions : public ProgramOptions {
	public:
		DriverOptions() 
		{
		}
	}*options;
	const char * const *    knownFontNames() const override ;
private:
	unsigned int newobject();
	void endobject();
	void starttext();
	void endtext();
	void adjustbbox(float x, float y);
	std::streampos startPosition[maxobjects];
	unsigned int currentobject;
	unsigned int pagenr;
	bool 	inTextMode;
	const char * encodingName;
	TempFile      tempFile;
	ofstream     &buffer;
	int	bb_llx,bb_lly,bb_urx,bb_ury;

#include "drvfuncs.h"
private:

	void print_coords();
	void show_text(const TextInfo & textinfo) override;

};
#endif

 
