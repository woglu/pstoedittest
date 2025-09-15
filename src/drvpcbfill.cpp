/* 
   drvpcbfill.cpp : This file is part of pstoedit
   simple backend for Pcbfill format.
   Contributed / Copyright 2004 by: Mark Rages 

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

#include "drvpcbfill.h"

#include I_stdio
#include I_string_h
#include I_iostream



drvPCBFILL::derivedConstructor(drvPCBFILL):
//(const char * driveroptions_p,ostream & theoutStream,ostream & theerrStream):
constructBase
{
// driver specific initializations
  outf << "PCB[\"\" 600000 500000]\n\n";
  outf << "Grid[2000.00000000 0 0 0]\n\n";
  outf << "Layer(10 \"silk\")\n(\n";
}

drvPCBFILL::~drvPCBFILL()
{
  outf << ")\n";
  options=nullptr;
}


void drvPCBFILL::close_page()
{
  //outf << "#Seite beendet.\n";
}

void drvPCBFILL::open_page()
{
  //	outf << "#Neue Seite\n";
}
const float SCALE = (100000.0f/72.0f);

void drvPCBFILL::show_path()
{
  //	outf << "\n#Polyline:\n";
  outf << "\tPolygon(0x00000010)\n\t(\n\t\t";

	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		if (pathElement(n).getType() != closepath) {
			const Point & p = pathElement(n).getPoint(0);
				outf << "[" << (int)(p.x()*SCALE) << " " 
                     << (int)(500000-p.y()*SCALE) << "] ";
		}
	}
  outf << "\n\t)\n";
}


static DriverDescriptionT < drvPCBFILL > D_pcbfill("pcbfill", "pcb format with fills", 
												   "See also: \\URL{http://pcb.sourceforge.net}","pcbfill", false,	// if backend supports subpaths
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
 
