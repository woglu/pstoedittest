#ifndef __drvplot_h
#define __drvplot_h

/* 
   drvplot.h : This file is part of pstoedit
   Backend for GNU metafile format and all other output formats produced
   by the GNU libplot/libplotter vector graphics library

   For documentation, see drvplot.cpp.

   Written by Robert S. Maier <rsm_AT_math.arizona.edu> and Wolfgang Glunz
   <wglunz35_AT_pstoedit.net>.

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

class drvplot : public drvbase 
{
 public:
  derivedConstructor (drvplot);	// ctor
  ~drvplot() override;			// dtor
	class DriverOptions : public ProgramOptions {
	public:
		OptionT < RSString, RSStringValueExtractor> type;
		DriverOptions():
			ProgramOptions(true), // expect unhandled options
			type(true,"-plotformat","string",0,"plotutil format to generate",nullptr,(const char *)"meta")
	// pagesize(true,"-pagesize","string",0,"page size, e.g. a4",0,)
		{
			ADD(type);
		}

	}*options;
  
#include "drvfuncs.h"

	void show_image (const PSImage& imageinfo) override; 
	void show_text(const TextInfo & textinfo) override;
	void show_rectangle(const float llx, const float lly, const float urx, const float ury) override;
  
private:
	void print_coords();
	class Plotter *plotter;	// either declared in plotter.h, or faked
	bool physical_page;		// can map PS onto a page of specified size?
	int page_type;		// if so, one of PAGE_{ANSI_A,ISO_A4...}

	void set_line_style ();
	void set_filling_and_edging_style ();
   
	NOCOPYANDASSIGN(drvplot)
};

#endif
 
 
