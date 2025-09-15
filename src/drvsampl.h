#ifndef __drvSAMPL_h
#define __drvSAMPL_h

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

class drvSAMPL : public drvbase {

public:

	derivedConstructor(drvSAMPL);
	~drvSAMPL() override; // Destructor
	class DriverOptions : public ProgramOptions {
	public:
		OptionT < int, IntValueExtractor >sampleoption;
		DriverOptions(): 
			sampleoption(true,"-sampleoption","integer",0,"just an example",nullptr,99)
		{
			ADD(sampleoption);
		}
	}*options;

#include "drvfuncs.h"
	void show_rectangle(const float llx, const float lly, const float urx, const float ury) override;
	void show_text(const TextInfo & textinfo) override;

public:

	void    show_image(const PSImage & imageinfo) override; 

private:
	void print_coords();
	//int	      imgcount;

};

#endif
 
 
