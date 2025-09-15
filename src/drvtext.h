#ifndef __drvTEXT_h
#define __drvTEXT_h

/* 
   drvtext.h : This file is part of pstoedit
   Class declaration for a text oriented output driver 

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
#include "ordlist.h"

class drvTEXT : public drvbase {

public:

	derivedConstructor(drvTEXT);
	// (const char * driveroptions_P,ostream & theoutStream,ostream & theerrStream ); // Constructor

	~drvTEXT() override; // Destructor
	class DriverOptions : public ProgramOptions { 
	public:
		OptionT < int, IntValueExtractor >pageheight;
		OptionT < int, IntValueExtractor >pagewidth;
		OptionT < bool, BoolTrueExtractor> dumptextpieces;

		DriverOptions() :
			pageheight(true,"-height", "number",0, "page height in terms of characters",nullptr,200),
			pagewidth(true,"-width", "number",0, "page width in terms of characters",nullptr,150),
			dumptextpieces(true,"-dump", "", 0,"dump text pieces",nullptr,false)
		{
			ADD(pageheight);
			ADD(pagewidth);
			ADD(dumptextpieces);
		}
	}*options;

	class XSorter {
	public:
		static bool compare(const drvbase::TextInfo & t1, const drvbase::TextInfo & t2) {
			return t1.x() > t2.x();
		}
	};
	class Line {
	public:
		typedef ordlist<drvbase::TextInfo,drvbase::TextInfo,XSorter> TextInfoList;
		TextInfoList textpieces;
		float y_max; // the y-interval of text assigned to this line 
		float y_min; // 
	};


	class YSorter {
	public:
		static bool compare(const Line * l1, const Line * l2) {
			return l1->y_max < l2->y_max;
		}
	};

	typedef ordlist<Line *,Line *,YSorter> LinePtrList;

#include "drvfuncs.h"

	void show_text(const TextInfo & textinfo) override;

	void    show_image(const PSImage & imageinfo) override; 

	LinePtrList page; 

	char ** charpage; // for text page output
#if 0
	bool dumptextpieces;
	// sizes in terms of lines and characters
	unsigned int pageheight  ;
	unsigned int pagewidth  ;
#endif

	NOCOPYANDASSIGN(drvTEXT)

};

#endif
 
