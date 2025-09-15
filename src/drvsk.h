#ifndef __drvSK_h
#define __drvSK_h

/* 
   drvsk.h
*/

/*

   Copyright (C) 1999 by Bernhard Herzog

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

class drvSK : public drvbase {

public:

    derivedConstructor(drvSK);

    ~drvSK() override;
	class DriverOptions : public ProgramOptions {
	public:
		DriverOptions() 
		{
		}
	}*options;

#include "drvfuncs.h"
	// void show_rectangle(const float llx, const float lly, const float urx, const float ury);
	void show_text(const TextInfo & textinfo) override;

    void    show_image(const PSImage & imageinfo) override;

 protected:
    bool pathsCanBeMerged(const PathInfo &, const PathInfo &) const override;

 private:
	void print_coords();
    int id;
	int getid() { return id++; }

};

#endif
 
