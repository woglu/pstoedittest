/* 
   drvrib.cpp - Driver to output RenderMan RIB polygons
             - written by Glenn M. Lewis <glenn_AT_gmlewis.com> - 6/18/96
	       http://www.gmlewis.com/>
		Based on...

   drvSAMPL.cpp : This file is part of pstoedit
   Skeleton for the implementation of new backends

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
#include "drvrib.h"
#include I_fstream
#include I_stdio
#include I_stdlib


drvRIB::derivedConstructor(drvRIB):
//(const char * driveroptions_p,ostream & theoutStream,ostream & theerrStream): // Constructor
constructBase
{
	// driver specific initializations
	// and writing of header to output file
	outf << "##RenderMan RIB-Structure 1.0" << endl;
	outf << "version 3.03" << endl;
	outf << "AttributeBegin" << endl;
}
 
drvRIB::~drvRIB()
{
	// driver specific deallocations
	// and writing of trailer to output file
	outf << "AttributeEnd" << endl;
	options=nullptr;
}

void drvRIB::print_coords()
{
	outf << "PointsGeneralPolygons[1]" << endl;
	outf << "[" << numberOfElementsInPath() << "]" << endl << "[";
	for (unsigned int i = 0; i < numberOfElementsInPath(); i++) {
		outf << i << " ";
	}
	outf << "]" << endl << "\"P\" [";
	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		const basedrawingelement & elem = pathElement(n);
		switch (elem.getType()) {
		case moveto:{
				const Point & p = elem.getPoint(0);
				// outf << "\t\tmoveto ";
				outf << p.x() + x_offset << " " << p.y() + y_offset << " 0 ";
			}
			break;
		case lineto:{
				const Point & p = elem.getPoint(0);
				// outf << "\t\tlineto ";
				outf << p.x() + x_offset << " " << p.y() + y_offset << " 0 ";
			}
			break;
		case closepath:		// Not supported
			// outf << "\t\tclosepath ";
			break;
		case curveto:{			// Not supported
			}
			break;
		default:
			errf << "\t\tFatal: unexpected case in drvrib " << endl;
			abort();
			break;
		}
		outf << endl;
	}
	outf << "]" << endl;
}


void drvRIB::open_page()
{
	//  outf << "Opening page: " << currentPageNumber << endl;
}

void drvRIB::close_page()
{
	//  outf << "Closing page: " << (currentPageNumber) << endl;
}

void drvRIB::show_path()
{
	outf << "Color " << currentR() << " " << currentG() << " " << currentB() << endl;
	print_coords();
}


static DriverDescriptionT < drvRIB > D_rib("rib", "RenderMan Interface Bytestream", "","rib", false,	// if backend supports subpaths, else 0
										   // if subpaths are supported, the backend must deal with
										   // sequences of the following form
										   // moveto (start of subpath)
										   // lineto (a line segment)
										   // lineto 
										   // moveto (start of a new subpath)
										   // lineto (a line segment)
										   // lineto 
										   //
										   // If this argument is set to 0 each subpath is drawn 
										   // individually which might not necessarily represent
										   // the original drawing.
										   false,	// if backend supports curves, else 0
										   false,	// if backend supports elements with fill and edges
										   false,	// if backend supports text, else 0
										   DriverDescription::imageformat::noimage,	// no support for PNG file images
										   DriverDescription::opentype::normalopen, 
										   false,	// if format supports multiple pages in one file
										   false  /*clipping */
										   );
