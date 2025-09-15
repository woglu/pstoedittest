/* 
   drvpcb2.cpp : This file is part of pstoedit
   simple backend for Pcb format.
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

#include "drvpcb2.h"

#include I_stdio
#include I_string_h
#include I_iostream
#include <cmath>
#include <iomanip>

const double SCALE = (double)100000.0/(double)72.0;
/* millimeter in mils/100. More exactly 3937.007874016, but pcb seems to use: */
/* 3937.00775 */
const double MM100 = (double)100000.0/(double)25.4;

#define sqr(a) ((a)*(a))

int drvPCB2::pcbScale_x(const Point & p) const
{
	return (int)((double)p.x() * SCALE + (double)options->tshiftx * unit + (double)0.5);
}

int drvPCB2::pcbScale_y(const Point & p) const 
{
	// return (int)((double)500000.0 - (double)p.y_ * SCALE + (double)options->tshifty * unit + (double)0.5);
	// patched 7/2011 XXX p.y_ appears to be off by one, why???
	return (int)((double)currentDeviceHeight * SCALE - ((double)(p.y()) + (double)1.0) * SCALE + (double)options->tshifty * unit + (double)0.5);
}

int drvPCB2::pcbScale(const double & f)  
{
	return (int)((double)f * SCALE + (double)0.5);
}

int drvPCB2::_grid_snap (int value, double grid_p) 
{
	return (int)((int)(((double)value + grid_p / 2) / grid_p) * grid_p + (double)0.5);
}

int drvPCB2::grid_snap(int value, bool success) const 
{
	if ((options->grid != 0.0) && success) {
		/* pixel-exact snap to pcb grid: */
		return _grid_snap (value, grid);
	} else {
		return value;
	}
}

void drvPCB2::try_grid_snap(int value, bool & success) const
{
	if (options->grid != 0.0) {
		int value_snap;
		value_snap = _grid_snap(value, grid);
		if (abs (value_snap - value) > grid * options->snapdist) {
			success = false;
		}
	}
}

drvPCB2::derivedConstructor(drvPCB2):
constructBase
{
	unit = (options->mm ? MM100 : 100.0);
	grid = (double)(options->grid) * unit;
}

void drvPCB2::gen_preamble(){
	const int width = pcbScale(currentDeviceWidth);
	const int height = pcbScale(currentDeviceHeight);
	outf << "PCB[\"\" " << width << " " << height << "]\n\n";
//	outf << "PCB[\"\" 600000 500000]\n\n";
	if (options->grid != 0.0) {
		outf << "Grid[";
		outf << std::fixed << std::setprecision(6) << grid;
		outf << " 0 0 1]\n\n";
	} else {
		outf << "Grid[1000.000000 0 0 0]\n\n";
	}
}

static void gen_layer(ostream & outf, C_ostrstream & layer, const char * layer_def, const bool & force)
{
	if (layer.tellp() || force) {
		outf << "Layer(" << layer_def << "\")\n(\n"
		     << layer.str()
		     << ")\n";
#ifdef USE_NEWSTRSTREAM
		layer.str("");
#endif
	}
}

drvPCB2::~drvPCB2()
{
	gen_preamble ();
	if (options->stdnames)
	{
		gen_layer (outf, layer_polygons, "1 \"component", false);
		gen_layer (outf, layer_pads, "2 \"solder", false);
		gen_layer (outf, layer_polygons_nogrid, "3 \"GND", false);
		gen_layer (outf, layer_pads_nogrid, "5 \"signal1", false);
		gen_layer (outf, layer_boundaries_nogrid, "9 \"silk", false);
		gen_layer (outf, layer_boundaries, "10 \"silk", true);
	} else {
		gen_layer (outf, layer_polygons, "1 \"poly", false);
		gen_layer (outf, layer_polygons_nogrid, "2 \"poly.nogrid", false);
		gen_layer (outf, layer_pads, "3 \"pads", false);
		gen_layer (outf, layer_pads_nogrid, "4 \"pads.nogrid", false);
		gen_layer (outf, layer_boundaries, "5 \"bound", false);
		gen_layer (outf, layer_boundaries_nogrid, "6 \"bound.nogrid", false);
		/* generate empty silk, otherwise bounds are interpreted as silk */
		outf << "Layer(10 \"silk\")\n(\n"
		     << ")\n";
	}
	options=nullptr;
}


void drvPCB2::close_page()
{
}

void drvPCB2::open_page()
{
}

void drvPCB2::show_path()
{
	bool round_success;
	ostream *layer, *layer_nogrid;
	if (options->forcepoly || isPolygon()) {
		switch (currentShowType()) {
		case drvbase::fill:
		case drvbase::eofill:
			unsigned int numberofvalidelements;

			/* FIXME: How polygon with <3 vertices happen? */
			if (numberOfElementsInPath() < 3)
				return;

			{
				const Point & p0 = pathElement(0).getPoint(0);
				numberofvalidelements = numberOfElementsInPath();
				if (pathElement(numberofvalidelements-1).getType() == closepath ) numberofvalidelements--; // closepath can be ignored
				const Point & pl = pathElement(numberofvalidelements-1).getPoint(0);
				/* Polygons are closed automatically. Skip last element for already closed polygons. */
				if (p0 == pl) numberofvalidelements--;
			}
			/* If snap to grid fails for any of points draw into layer_polygons_nogrid layer */
			round_success = true;
			{for (unsigned int n = 0; n < numberofvalidelements; n++) {
				try_grid_snap (pcbScale_x(pathElement(n).getPoint(0)), round_success);
				try_grid_snap (pcbScale_y(pathElement(n).getPoint(0)), round_success);
			}}
			if (round_success)
				layer = &layer_polygons;
			else
				layer = &layer_polygons_nogrid;
			/* Put polygons in grid into component layer. They may be PCB copper areas. */
			*layer << "\tPolygon(\"clearpoly\")\n"
			       << "\t(\n";
			{for (unsigned int n = 0; n < numberofvalidelements; n++) {
				const Point & p = pathElement(n).getPoint(0);
				const int x = grid_snap (pcbScale_x(p), round_success);
				const int y = grid_snap (pcbScale_y(p), round_success);
				*layer << "\t\t[" << x << " " << y << "]\n";
			}}
			*layer << "\t)\n";

			/* Rectangles can be also soldering pads. Create pads of proper size as duplicates.
			   User has to remove the unwanted instance. (Well, any tetragon is converted to pad)? */
			if (numberofvalidelements == 4) {
				Point try1_p1, try1_p2, try2_p1, try2_p2, pad_p1, pad_p2, pad_p1_corr, pad_p2_corr;
				double lensq, widsq, try1_lensq, try2_lensq; 
				/* Try1: line in direcion of vertices 0->1 */
				try1_p1 = Point( (pathElement(1).getPoint(0).x() + pathElement(2).getPoint(0).x()) / 2,			
								(pathElement(1).getPoint(0).y() + pathElement(2).getPoint(0).y()) / 2);
				try1_p2 = Point( (pathElement(3).getPoint(0).x() + pathElement(0).getPoint(0).x()) / 2,
								(pathElement(3).getPoint(0).y() + pathElement(0).getPoint(0).y()) / 2);
				try1_lensq = sqr(try1_p1.x() - try1_p2.x()) + sqr(try1_p1.y() - try1_p2.y());
				/* Try1: line in direcion of vertices 1->2 */
				try2_p1 = Point((pathElement(0).getPoint(0).x() + pathElement(1).getPoint(0).x()) / 2,
								(pathElement(0).getPoint(0).y() + pathElement(1).getPoint(0).y()) / 2);
				try2_p2 = Point((pathElement(2).getPoint(0).x() + pathElement(3).getPoint(0).x()) / 2,
								 (pathElement(2).getPoint(0).y() + pathElement(3).getPoint(0).y()) / 2);
				try2_lensq = sqr(try2_p1.x() - try2_p2.x()) + sqr(try2_p1.y() - try2_p2.y());
				/* Use the longer line from these two */
				/* FIXME: what to do for square */
				if (try1_lensq > try2_lensq) {
					pad_p1 = try1_p1;
					pad_p2 = try1_p2;
					lensq = try1_lensq;
					widsq = try2_lensq;
				} else {
					pad_p1 = try2_p1;
					pad_p2 = try2_p2;
					lensq = try2_lensq;
					widsq = try1_lensq;
				}
				/* pcb adds line width to lenght. Subtract it. */
				const double lwidth = sqrt(widsq);
				const double lratio = sqrt(widsq/lensq/4);
				pad_p1_corr = Point((float)(pad_p1.x() - lratio*(pad_p1.x()-pad_p2.x())),				
				 (float)(pad_p1.y() - lratio*(pad_p1.y()-pad_p2.y())));

				pad_p2_corr = Point((float)(pad_p2.x() + lratio*(pad_p1.x()-pad_p2.x())),
				(float)(pad_p2.y() + lratio*(pad_p1.y()-pad_p2.y())));
				round_success = true;
				try_grid_snap (pcbScale_x(pad_p1_corr), round_success);
				try_grid_snap (pcbScale_y(pad_p1_corr), round_success);
				try_grid_snap (pcbScale_x(pad_p2_corr), round_success);
				try_grid_snap (pcbScale_y(pad_p2_corr), round_success);
				try_grid_snap (pcbScale(lwidth), round_success);
				if (round_success)
					layer = &layer_pads;
				else
					layer = &layer_pads_nogrid;
				*layer << "\tLine["
				       << grid_snap (pcbScale_x(pad_p1_corr), round_success) << " "
				       << grid_snap (pcbScale_y(pad_p1_corr), round_success) << " "
				       << grid_snap (pcbScale_x(pad_p2_corr), round_success) << " "
				       << grid_snap (pcbScale_y(pad_p2_corr), round_success) << " "
				       << grid_snap (pcbScale(lwidth), round_success) << " 200 \"clearline\"]\n";
			}
			break;

		case drvbase::stroke:
			/* Put lines with polygon inside to silk layer, as they cannot be component lines. */
			layer = &layer_boundaries;
			layer_nogrid = &layer_boundaries_nogrid;
			goto polyline;
			break;
		default:; // no expected
		}
	} else {
		/* Put lines that are not associated with polygon into component layer. They are probably PCB traces. */
		layer = &layer_polygons;
		layer_nogrid = &layer_polygons_nogrid;
	polyline:
		round_success = true;
		{for (unsigned int n = 1; n < numberOfElementsInPath(); n++) {
			try_grid_snap (pcbScale_x(pathElement(n).getPoint(0)), round_success);
			try_grid_snap (pcbScale_y(pathElement(n).getPoint(0)), round_success);
		}}
		if (!round_success)
			layer = layer_nogrid;
		{for (unsigned int n = 1; n < numberOfElementsInPath(); n++) {
			const Point & p1 = pathElement(n-1).getPoint(0);
			const Point & p = pathElement(n).getPoint(0);
			*layer << "\tLine["
			       << grid_snap (pcbScale_x(p1), round_success) << " " 
			       << grid_snap (pcbScale_y(p1), round_success) << " " 
			       << grid_snap (pcbScale_x(p), round_success) << " " 
			       << grid_snap (pcbScale_y(p), round_success) << " "
			       << grid_snap (pcbScale(currentLineWidth()), round_success)
			       << " 2000 0x00000020]\n";
		}}
	}
}


static DriverDescriptionT < drvPCB2 > D_pcb("pcb", "pcb format", 
											"See also: \\URL{http://pcb.sourceforge.net} and \\URL{http://www.penguin.cz/\\Tilde utx/pstoedit-pcb/}","pcb", false,	// if backend supports subpaths
												   // if subpaths are supported, the backend must deal with
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
												   false /*clipping */ );
 
