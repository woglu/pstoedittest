/* 
   drvpcbrnd.cpp : This file is part of pstoedit
   simple backend for pcb-rnd format

   Contributed / Copyright 2018 - 2024 by: Erich Heinzle a1039181_ATAT_gmail.com
   pcb-rnd exporter based on original gEDA PCB exporter code by.
   Contributed / Copyright 2004 by: Mark Rages 
   Contributed / Copyright 2008 by: Stanislav Brabec sbrabec_AT_suse.cz

   Copyright (C) 1993 - 2024 Wolfgang Glunz, wglunz35_AT_pstoedit.net
   (for the skeleton and the rest of pstoedit)

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

#include "drvpcbrnd.h"

#include I_stdio
#include I_string_h
#include I_iostream
#include <cmath>
#include <iomanip>

extern "C" {
#include "libminuid.inc"
}

const double SCALE = (double)100000.0/(double)72.0;
/* millimeter in mils/100. More exactly 3937.007874016, but pcb seems to use: */
/* 3937.00775 */
const double MM100 = (double)100000.0/(double)25.4;

bool drvPCBRND::bOnSegmentAC(const Point & a, const Point & b, const Point & c)
{
	return ((b.x() <= std::max(a.x(), c.x())) && (b.x() >= std::min(a.x(), c.x())) &&
		    (b.y() <= std::max(a.y(), c.y())) && (b.y() >= std::min(a.y(), c.y())));
}

int drvPCBRND::orientation(const Point & a, const Point & b, const Point & c)
{
	float test = (float(b.y() - a.y()) * (c.x() - b.x())) - (float(b.x() - a.x())*(c.y() - b.y()));
	if (test > 0)
		return 1;	/* CW */
	if (test < 0)
		return 2;	/* CCW */
	return 0;		/* colinear */
}

bool drvPCBRND::foundIntersection(const Point & a, const Point & b, const Point & c, const Point & d)
{
	const int o1 = orientation(a,b,c);
	const int o2 = orientation(a,b,d);
	const int o3 = orientation(c,d,a);
	const int o4 = orientation(c,d,b);

	if ((o1 != o2) && (o3 != o4))
		return true;
	if ((o1 == 0) && bOnSegmentAC(a,b,c))
		return true;
	if ((o2 == 0) && bOnSegmentAC(a,d,c))
		return true;
	if ((o3 == 0) && bOnSegmentAC(b,a,d))
		return true;
	if ((o4 == 0) && bOnSegmentAC(b,c,d))
		return true;
	return false;
}

bool drvPCBRND::isSimplePolygon() const
{
	bool simple = true;
	for (unsigned int i=0; i < (numberOfElementsInPath()-2); i++) {
		for (unsigned int j=i+2; j<(numberOfElementsInPath()-2); j++) {
			simple &= !foundIntersection(pathElement(i).getPoint(0), 
										 pathElement(i+1).getPoint(0),
										 pathElement(j).getPoint(0), 
										 pathElement(j+1).getPoint(0));
			if (!simple) return false;
		}
	}
	return simple;
}

int drvPCBRND::pcbScale_x(const Point & p) const
{
	return (int)((double)p.x() * SCALE + (double)options->tshiftx * unit + (double)0.5);
}

int drvPCBRND::pcbScale_y(const Point & p) const 
{
	return (int)((double)currentDeviceHeight * SCALE - ((double)(p.y()) + (double)1.0) * SCALE + (double)options->tshifty * unit + (double)0.5);
}

int drvPCBRND::pcbScale(const double & f)  
{
	return (int)((double)f * SCALE + (double)0.5);
}

int drvPCBRND::_grid_snap (int value, double grid_p) 
{
	return (int)((int)(((double)value + grid_p / 2) / grid_p) * grid_p + (double)0.5);
}

int drvPCBRND::grid_snap(int value, bool success) const 
{
	if ((options->grid != 0.0) && success) {
		/* pixel-exact snap to pcb grid: */
		return _grid_snap (value, grid);
	} else {
		return value;
	}
}

void drvPCBRND::try_grid_snap(int value, bool & success) const
{
	if (options->grid != 0.0) {
		int value_snap;
		value_snap = _grid_snap(value, grid);
		if (abs (value_snap - value) > grid * options->snapdist) {
			success = false;
		}
	}
}

drvPCBRND::derivedConstructor(drvPCBRND):
constructBase
{
        unit = (options->mm ? MM100 : 100.0);
        unitText = (options->mm ? "mm" : "dmil");
		grid = (double)(options->grid) * unit;
        lineNumber = 0;
        polygonNumber = 0;
}

void drvPCBRND::gen_preamble(){
        outf << "li:pcb-rnd-subcircuit-v7 {\n ha:subc.74 {\n  ha:attributes {\n   refdes = U0\n  }\n";
        outf << "  ha:data {\n   li:padstack_prototypes {\n   }\n   li:objects {\n   }\n   li:layers {\n";
}

void drvPCBRND::gen_footer(){/* need to add libminuid code  here */
        minuid_session_t id_sess;
        minuid_bin_t id_bin;
        minuid_str_t id_text;
        minuid_init(&id_sess);
        int salt = lineNumber * polygonNumber;
        minuid_salt(&id_sess, &salt, sizeof(int));
        minuid_gen(&id_sess, id_bin); 
        minuid_bin2str(id_text, id_bin);        
        outf << "   }\n  }\n  uid = " << id_text << "\n  ha:flags {\n  }\n }\n ha:pixmaps {\n }\n}\n";
	std::cout << "Use File->Import->Load subcircuit into paste buffer in pcb-rnd to insert into layout.\nLarge subcircuits may need scaling before placement.\n"; 
}

static void gen_layer(ostream & outf, C_ostrstream & layer, const char * layer_def)
{
        outf << layer_def << "     li:objects {\n";
        outf << layer.str();
        outf << "     }\n     ha:combining {\n     }\n    }\n";
#ifdef USE_NEWSTRSTREAM
		layer.str("");
#endif

}

drvPCBRND::~drvPCBRND()
{
        gen_preamble (); /* create standard pcb-rnd .lht format sub circuit preamble prior to populating layers */
        gen_layer (outf, layer_polygons, "    ha:top-sig {\n     lid=0\n     ha:type {\n      copper = 1\n      top = 1\n     }\n");
	    gen_layer (outf, layer_polygon_outlines, "    ha:bottom-sig {\n     lid = 1\n     ha:type {\n      bottom = 1\n      copper = 1\n     }\n");
	    gen_layer (outf, layer_polygons_nogrid, "    ha:top-gnd {\n     lid=2\n     ha:type {\n      copper = 1\n      top = 1\n     }\n");
	    gen_layer (outf, layer_polygon_outlines_nogrid, "    ha:bottom-gnd {\n     lid = 3\n     ha:type {\n      bottom = 1\n      copper = 1\n     }\n");
        gen_layer (outf, layer_lines_nogrid, "    ha:outline {\n     lid = 4\n     ha:type {\n      boundary = 1\n     }\n");
        gen_layer (outf, layer_spare, "    ha:bottom-silk {\n     lid = 5\n     ha:type {\n      silk = 1\n      bottom = 1\n     }\n");
	    gen_layer (outf, layer_lines, "    ha:top-silk {\n     lid = 6\n     ha:type {\n      silk = 1\n      top = 1\n     }\n");
        gen_layer (outf, layer_spare, "    ha:subc-aux {\n     lid = 7\n     ha:type {\n      top = 1\n      misc = 1\n      virtual = 1\n     }\n");
        gen_footer (); /* finish off pcb-rnd .lht format sub circuit */
	    options=0;
}


void drvPCBRND::close_page()
{
}

void drvPCBRND::open_page()
{
}

void drvPCBRND::show_path()
{
	bool stroked = false;
	ostream *layer, *layer_nogrid;
	if ((options->forcepoly || isPolygon()) && numberOfElementsInPath() > 2) { /* beware the forced two point polygon */
		bool round_success = true;
		switch (currentShowType()) {
		case drvbase::fill:
		case drvbase::eofill:
			unsigned int numberofvalidelements;
			{
				const Point & p0 = pathElement(0).getPoint(0);
				numberofvalidelements = numberOfElementsInPath();
				if (pathElement(numberofvalidelements-1).getType() == closepath ) numberofvalidelements--; /* ignore closepath */
				const Point & pl = pathElement(numberofvalidelements-1).getPoint(0);
				/* Polygons are closed automatically. Skip last element for already closed polygons. */
				if (p0 == pl) numberofvalidelements--;
			}
			/* If snap to grid fails for any of points draw into layer_polygons_nogrid layer */

			{for (unsigned int n = 0; n < numberofvalidelements; n++) {
				try_grid_snap (pcbScale_x(pathElement(n).getPoint(0)), round_success);
				try_grid_snap (pcbScale_y(pathElement(n).getPoint(0)), round_success);
			}}
                        /* Select grid snapped or non grid snapped copper layer for polygons. */
			if (round_success)
				layer = &layer_polygons;
			else
				layer = &layer_polygons_nogrid;
                        /* We now generate the polygon element, but only if it is non self intersecting */
			if (isSimplePolygon()) {
				*layer << "       ha:polygon." << polygonNumber << " {\n        li:geometry {\n          ta:contour {\n";
				{for (unsigned int n = 0; n < numberofvalidelements; n++) {
					const Point & p = pathElement(n).getPoint(0);
					int x = grid_snap (pcbScale_x(p), round_success),
					    y = grid_snap (pcbScale_y(p), round_success);
					*layer << "           { " << x << unitText << "; " << y << unitText << " }\n";
				}}
				*layer << "          }\n        }\n        ha:flags {\n         clearpoly=1\n        }\n        clearance = 40.0mil\n       }\n";
			}
			polygonNumber++;
			break;

		case drvbase::stroke:
			/* Flag lines with polygon inside to outline layer, as they cannot be component lines. */
                        stroked = true;
			break;
		default:; // no expected
		}
	} 

        // we now prepare to draw non polygon lines, plus boundaries of polygons 
        if (!stroked || isPolygon()) {
                layer = &layer_polygon_outlines;
                layer_nogrid = &layer_polygon_outlines_nogrid; // likely copper
        } else {
                layer = &layer_lines;
                layer_nogrid = &layer_lines_nogrid; // likely boundaries
        }
        // we see if points can be snapped to grid
        bool round_success = true;
        for (unsigned int n = 1; n < numberOfElementsInPath(); n++) {
                try_grid_snap (pcbScale_x(pathElement(n).getPoint(0)), round_success);
                try_grid_snap (pcbScale_y(pathElement(n).getPoint(0)), round_success);
        }
        if (!round_success)
                layer = layer_nogrid;
        // we now generate the line elements and put them into the relevant layer
        for (unsigned int n = 1; n < numberOfElementsInPath(); n++) {
                const Point & p1 = pathElement(n-1).getPoint(0);
                const Point & p = pathElement(n).getPoint(0);
                *layer << "       ha:line." << lineNumber << " {\n        "
                       << "x1=" << grid_snap (pcbScale_x(p1), round_success) << unitText << "; " 
                       << "y1=" << grid_snap (pcbScale_y(p1), round_success) << unitText << "; " 
                       << "x2=" << grid_snap (pcbScale_x(p), round_success) << unitText << "; " 
                       << "y2=" << grid_snap (pcbScale_y(p), round_success) << unitText << "\n"
                       << "        thickness=" << grid_snap (pcbScale(currentLineWidth()), round_success) << unitText << "\n"
                       << "        clearance=40.0mil\n"
                       << "        ha:attributes {\n        }\n"
                       << "        ha:flags {\n         clearline=1\n        }\n       }\n";
                lineNumber++;
        }
}


static DriverDescriptionT < drvPCBRND > D_pcbrnd("pcb-rnd", "pcb-rnd format", 
                                                 "See also: \\URL{http://repo.hu/projects/pcb-rnd} and \\URL{http://www.penguin.cz/\\Tilde utx/pstoedit-pcb/}","lht",
                                                 false,	// if backend supports subpathes
                                                 // if subpathes are supported, the backend must deal with
                                                 // sequences of the following form
                                                 // moveto (start of subpath)
                                                 // lineto (a line segment)
                                                 // lineto 
                                                 // moveto (start of a new subpath)
                                                 // lineto (a line segment)
                                                 // lineto 
                                                 //
                                                 // If this argument is set to false each subpath is drawn 
                                                 // individually which might not necessarily represent
                                                 // the original drawing.
                                                 false,	// if backend supports curves
                                                 false,	// if backend supports elements with fill and edges
                                                 false,	// if backend supports text
                                                 DriverDescription::imageformat::noimage,	// no support for PNG file images
                                                 DriverDescription::opentype::normalopen, false,	// if format supports multiple pages in one file
                                                 false); /*clipping */ 
