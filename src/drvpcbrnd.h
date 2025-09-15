#ifndef __drvPCBRND_H
#define __drvPCBRND_H

/* 
   drvPCBRND.h : This file is part of pstoedit
   simple backend for pcb-rnd format

   Contributed / Copyright 2018 by: Erich Heinzle a1039181_ATAT_gmail.com
   pcb-rnd exporter based on original gEDA PCB exporter code by.
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

class drvPCBRND : public drvbase {
public:
	derivedConstructor(drvPCBRND);
	~drvPCBRND(); // Destructor

	class DriverOptions : public ProgramOptions {
	public:
		OptionT < double, DoubleValueExtractor > grid;
		OptionT < double, DoubleValueExtractor > snapdist;
		OptionT < double, DoubleValueExtractor > tshiftx;
		OptionT < double, DoubleValueExtractor > tshifty;
		OptionT < bool, BoolTrueExtractor > mm;
		OptionT < bool, BoolTrueExtractor > forcepoly;

		DriverOptions():
		grid(true,"-grid","double number",0,"attempt to snap relevant output to grid (mils) and put failed objects to a different layer",0,0),
		snapdist(true,"-snapdist","double number",0,"grid snap distance ratio (0 < snapdist <= 0.5, default 0.1)",0,0.1),
		tshiftx(true,"-tshiftx","double number",0,"additional x shift measured in target units (mils)",0,0),
		tshifty(true,"-tshifty","double number",0,"additional y shift measured in target units (mils)",0,0),
                mm(true,"-mm","",0,"switch to metric units (mm)",0,false),
		forcepoly(true,"-forcepoly","",0,"force all objects to be interpreted as polygons",0,false)
		{
			ADD(grid);
			ADD(snapdist);
			ADD(tshiftx);
			ADD(tshifty);
			ADD(mm);
			ADD(forcepoly);
		}
	}*options;

#include "drvfuncs.h"
 
private:
    int lineNumber;
    int polygonNumber;
	static bool bOnSegmentAC(const Point & a, const Point & b, const Point & c);
	static int orientation(const Point & a, const Point & b, const Point & c);
	static bool foundIntersection(const Point & a, const Point & b, const Point & c, const Point & d);
	bool isSimplePolygon() const;
	int pcbScale_x(const Point & p) const;
	int pcbScale_y(const Point & p) const;
	static int pcbScale(const double & f);
	void try_grid_snap(int value, bool & success) const;
	static int _grid_snap (int value, double grid) ;
	int grid_snap(int value, bool success) const;
	void gen_preamble();
    void gen_footer();

	C_ostrstream layer_polygons, layer_polygons_nogrid;
    C_ostrstream layer_polygon_outlines, layer_polygon_outlines_nogrid;
	C_ostrstream layer_lines, layer_lines_nogrid;
    C_ostrstream layer_spare;
	double unit, grid;
    const char * unitText;
};
#endif
