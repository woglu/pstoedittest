/* 
   drvgschem.cpp : This file is part of pstoedit
   simple backend for Gschem format.
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

#include "drvgschem.h"

#include I_stdio
#include I_string_h
#include I_iostream



drvGSCHEM::derivedConstructor(drvGSCHEM):
//(const char * driveroptions_p,ostream & theoutStream,ostream & theerrStream):
constructBase
{
// driver specific initializations
  outf << "v 20040111 1\n";
}

drvGSCHEM::~drvGSCHEM()
{
	options=nullptr;
}


void drvGSCHEM::close_page()
{
  //outf << "#Seite beendet.\n";
}

void drvGSCHEM::open_page()
{
  //	outf << "#Neue Seite\n";
}
const float SCALE = (1000.0f/72.0f);

void drvGSCHEM::show_path()
{
  //	outf << "\n#Polyline:\n";


	for (unsigned int n = 1; n < numberOfElementsInPath(); n++) {
	  const Point & p1 = pathElement(n-1).getPoint(0);
		const Point & p = pathElement(n).getPoint(0);
		outf << "L ";
		outf << (int)(p1.x()*SCALE) << " " 
                     << (int)(p1.y()*SCALE) << " " 
                     << (int)(p.x()*SCALE) << " " 
                     << (int)(p.y()*SCALE) << " 3 0 0 0 -1 -1\n";
	}
}


static DriverDescriptionT < drvGSCHEM > D_gschem("gschem", "gschem format", 
												 "See also:  \\URL{http://wiki.geda-project.org/geda:gaf}","gschem", false,	// if backend supports subpaths
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
 
