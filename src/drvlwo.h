#ifndef __drvLWO_h
#define __drvLWO_h

/* 
   drvlwo.h - header for LightWave 3D Object (LWO) polygon driver
             - written by Glenn M. Lewis <glenn@gmlewis.com> - 6/18/96
	       http://www.gmlewis.com/>
	      Based on...

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

class drvLWO : public drvbase {
public:
	derivedConstructor(drvLWO);
	//(const char * driveroptions_P,ostream & theoutStream,ostream & theerrStream ); // Constructor

	~drvLWO() override; // Destructor
	class DriverOptions : public ProgramOptions {
	public:
		DriverOptions() 
		{
		}
	}*options;

#include "drvfuncs.h"

private:
	void print_coords();
  unsigned long total_vertices;
  unsigned long total_polys;
  struct LWO_POLY *polys;

  static void out_ulong(ostream &outs, unsigned long val)
  {
    outs.put((char)((val>>24L)&0x00FFL));
    outs.put((char)((val>>16L)&0x00FFL));
    outs.put((char)((val>> 8L)&0x00FFL));
    outs.put((char)((val     )&0x00FFL));
  }

  static void out_ushort(ostream &outs, unsigned long val)
  {
    outs.put((char)((val>> 8L)&0x00FFL));
    outs.put((char)((val     )&0x00FFL));
  }

  static void out_float(ostream &outs, float val)
  {
    union {
      float f;
      unsigned long u;
    } num;
    num.f = val;
    out_ulong(outs, num.u);
  }


};

#endif
 
