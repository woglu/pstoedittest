#ifndef __drvJAVA2_h
#define __drvJAVA2_h

/*
   drvjava2.h : This file is part of pstoedit
   Class declaration for a Java2 output driver -- test version

   Copyright (C) 1993 - 2024 Wolfgang Glunz, wglunz35_AT_pstoedit.net
   Copyright (C) 2000 TapirSoft Gisbert & Harald Selke GbR, gisbert@tapirsoft.de

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

class drvJAVA2 : public drvbase {

public:

	derivedConstructor(drvJAVA2);
	//(const char * driveroptions_P,ostream & theoutStream,ostream & theerrStream ); // Constructor

	~drvJAVA2() override; // Destructor
	class DriverOptions : public ProgramOptions { 
	public:
		OptionT < RSString, RSStringValueExtractor> jClassName;
		DriverOptions():
			jClassName(true,"java_class_name","string",0,"name of java class to generate",nullptr,(const char *)"PSJava")
		{
			ADD(jClassName);
		}
	}*options;

#include "drvfuncs.h"
	void show_text(const TextInfo & textinfo) override;
	void show_rectangle(const float llx, const float lly, const float urx, const float ury) override;

	void    show_image(const PSImage & imageinfo) override;

private:
	void print_coords();
	void continue_page();

	unsigned int subPageNumber;
	unsigned int numberOfElements;
	unsigned int numberOfImages;
};

#endif
 
 
