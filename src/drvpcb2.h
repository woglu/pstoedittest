#ifndef __drvPCB2_H
#define __drvPCB2_H

/* 
   drvPCB2.h : This file is part of pstoedit
   Contributed / Copyright 2004 by: Mark Rages 
   Contributed / Copyright 2008 by: Stanislav Brabec sbrabec_AT_suse.cz

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
#include I_strstream

class drvPCB2 : public drvbase {

public:

	derivedConstructor(drvPCB2);
	//(const char * driveroptions_P,ostream & theoutStream,ostream & theerrStream ); // Constructor

	~drvPCB2() override; // Destructor

	class DriverOptions : public ProgramOptions {
	public:
		OptionT < double, DoubleValueExtractor > grid;
		OptionT < double, DoubleValueExtractor > snapdist;
		OptionT < double, DoubleValueExtractor > tshiftx;
		OptionT < double, DoubleValueExtractor > tshifty;
		OptionT < bool, BoolTrueExtractor > mm;
		OptionT < bool, BoolTrueExtractor > stdnames;
		OptionT < bool, BoolTrueExtractor > forcepoly;

		DriverOptions():
		grid(true,"-grid","double number",0,"attempt to snap relevant output to grid (mils) and put failed objects to a different layer",nullptr,0),
		snapdist(true,"-snapdist","double number",0,"grid snap distance ratio (0 < snapdist <= 0.5, default 0.1)",nullptr,0.1),
		tshiftx(true,"-tshiftx","double number",0,"additional x shift measured in target units (mils)",nullptr,0),
		tshifty(true,"-tshifty","double number",0,"additional y shift measured in target units (mils)",nullptr,0),
		mm(true,"-mm","",0,"switch to metric units (mm)",nullptr,false),
		stdnames(true,"-stdnames","",0,"use standard layer names instead of descriptive names",nullptr,false),
		forcepoly(true,"-forcepoly","",0,"force all objects to be interpreted as polygons",nullptr,false)
		{
			ADD(grid);
			ADD(snapdist);
			ADD(tshiftx);
			ADD(tshifty);
			ADD(mm);
			ADD(stdnames);
			ADD(forcepoly);
		}
	}*options;

#include "drvfuncs.h"

private:
	int pcbScale_x(const Point & p) const;
	int pcbScale_y(const Point & p) const;
	static int pcbScale(const double & f) ;
	void try_grid_snap(int value, bool & success) const;
	static int _grid_snap (int value, double grid_p) ;
	int grid_snap(int value, bool success) const;
	void gen_preamble();

	C_ostrstream layer_polygons, layer_polygons_nogrid;
	C_ostrstream layer_pads, layer_pads_nogrid;
	C_ostrstream layer_boundaries_nogrid, layer_boundaries;
	double unit, grid;
};
#endif
