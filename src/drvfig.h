#ifndef __drvFIG_h
#define __drvFIG_h

/*
   drvFIG.h : This file is part of pstoedit
   Interface for new driver backends

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

class drvFIG : public drvbase {

public:

	derivedConstructor(drvFIG);
	//(const char * driveroptions_P, ostream & theoutStream, ostream & theerrStream, const char* outPath_P, const char* outName_P); // Constructor

	~drvFIG() override; // Destructor
	class DriverOptions : public ProgramOptions {
	public:
		OptionT < int, IntValueExtractor > startdepth ; 
		OptionT < bool, BoolTrueExtractor > metric ;
		OptionT < bool, BoolTrueExtractor > use_correct_font_size ;
		OptionT < int, IntValueExtractor > depth_in_inches ; 


		DriverOptions():
			startdepth(true,"-startdepth","number",0,"set the initial depth (default 999)",nullptr,999),
			metric(true,"-metric","",0,"switch to centimeter display (default inches)",nullptr,false),
			use_correct_font_size(true,"-usecorrectfontsize","",0,"do not scale fonts for xfig. Use this if you also use this option with xfig",nullptr,false),
			depth_in_inches(true,"-depth","number",0,"set the page depth in inches (default 11)",nullptr,11)
		{
			ADD(startdepth);
			ADD(metric);
			ADD(use_correct_font_size);
			ADD(depth_in_inches);
		}
	}*options;

private:
	int	      objectId;
	TempFile  tempFile;
	ofstream  &buffer;
	int	      imgcount;
	//int	      format; // either 31 or something else
//	int	      startdepth; // Start depth for objectID (range 0 - 999)
//	bool	  use_correct_font_size; // if xfig is used in -use_correct_font_size mode as well

#include "drvfuncs.h"
	void show_text(const TextInfo & textinfo) override;

	unsigned int nrOfCurvetos() const;
	void print_polyline_coords();
//	void print_spline_coords_noxsplines(int linestyle, float localLineWidth);
	void prpoint(ostream & os, const Point& p,bool withspaceatend = true) const;
	void print_spline_coords1();
	void print_spline_coords2();
	void new_depth();
	void addtobbox(const Point& p);
	void bbox_path();

 // Bounding Box
   float glob_min_x , glob_max_x , glob_min_y , glob_max_y ;
   float loc_min_x , loc_max_x , loc_min_y , loc_max_y ;
   int glo_bbox_flag ,loc_bbox_flag ;

public:
   void show_image(const PSImage & imageinfo) override;
};
#endif

 
