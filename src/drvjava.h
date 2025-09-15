#ifndef __drvJAVA_h
#define __drvJAVA_h

/* 
   drvsampl.h : This file is part of pstoedit
   Class declaration for a sample output driver with no additional attributes
   and methods (minimal interface)

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

class drvJAVA : public drvbase {

public:

	derivedConstructor(drvJAVA);
	~drvJAVA() override; // Destructor
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

private:
	void print_coords();

};

#endif
 
