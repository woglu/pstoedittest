/*
   drvVTK.cpp : This file is part of pstoedit
   Backend for VTK files, e.g. for ParaView - http://www.paraview.org/

   Copyright (C) 2008 - 2024 Wolfgang Glunz, wglunz35_AT_pstoedit.net

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
#include "drvvtk.h"
#include I_fstream
#include I_stdio
#include I_stdlib

drvVTK::derivedConstructor(drvVTK):
		constructBase, 
		pointsCount(0), 
		lineCount(0), 
		linepoints(0),
		pointStream(pointFile.asOutput()), 
		polyStream(polyFile.asOutput() ) ,
		colorStream(colorFile.asOutput() )
{
	// driver specific initializations
	// and writing of header to output file
	outf << "# vtk DataFile Version 2.0" << endl;
	outf << "created by pstoedit " << endl;
	outf << "ASCII" << endl;
	outf << "DATASET POLYDATA" << endl;
}

drvVTK::~drvVTK()
{
	// now we can copy the points the output
	outf << "POINTS " << pointsCount << " float" << endl;
	ifstream & inbuffer_p = pointFile.asInput();
	copy_file(inbuffer_p, outf);

	outf << "LINES " << lineCount << " " << linepoints + lineCount  << endl;
	// now we can copy the polylines the output
	ifstream & inbuffer_l = polyFile.asInput();
	copy_file(inbuffer_l, outf);
	outf << endl; 

	outf << "CELL_DATA " << lineCount << endl;
	outf << "COLOR_SCALARS colors " << 4 << endl;
	// to see the correct colors, uncheck the "map scalars" in the Display tab of ParaView
	// now we can copy the polylines the output
	ifstream & inbuffer_c = colorFile.asInput();
	copy_file(inbuffer_c, outf);

	options=nullptr;
}

int drvVTK::add_point(const Point & p)
{
	pointStream << p.x() + x_offset << " " <<  p.y() + y_offset << " " << 0.0f << endl;
	pointsCount++;
	return pointsCount;
}
#if 0 
// not used so far
int drvVTK::add_line(int s, int e, float r, float g, float b)
{
	polyStream << "2 " << s-1 << " " << e-1 << endl;
	colorStream << r << " " << g << " " << b  << " 0.5" << endl;
	lineCount++;
	return lineCount;
}
#endif

// Version with multi-segment lines
void drvVTK::print_coords()
{
	int bp = 0;
	colorStream << fillR() << " " << fillG() << " " << fillB() << " 0.5" << endl;

	polyStream << numberOfElementsInPath() << " " ;
	linepoints += numberOfElementsInPath();
	lineCount++;
	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		const basedrawingelement & elem = pathElement(n);
		switch (elem.getType()) {
		case moveto:{
				const Point & p = elem.getPoint(0);
				const int m = add_point(p);
				polyStream << m-1 << " ";
				bp = m; 
			}
			break;
		case lineto:{
				const Point & p = elem.getPoint(0);
				const int l = add_point(p);
				polyStream << l-1 << " ";
			}
			break;
		case closepath:
			polyStream << bp-1 << " ";
			break;
		case curveto:{
			errf << "\t\tFatal: unexpected case in drvVTK - curveto " << endl;
			}
			break;
		default:
			errf << "\t\tFatal: unexpected case in drvVTK : default" << endl;
			abort();
			break;
		}
	}
	polyStream << endl;
}

void drvVTK::open_page()
{
	// outf << "# Opening page: " << currentPageNumber << endl;
}

void drvVTK::close_page()
{
	// outf << "# Closing page: " << (currentPageNumber) << endl;
}

void drvVTK::show_text(const TextInfo & /* textinfo*/ )
{
	errf << "\t\tFatal: unexpected case in drvVTK - show_text " << endl;
	// should not happen - text is not supported
}

void drvVTK::show_path()
{
	print_coords();
}

void drvVTK::show_rectangle(const float , const float , const float , const float )
{
// just do show_path for a first guess
	show_path();
}
void drvVTK::show_image(const PSImage & )
{
	errf << "\t\tFatal: unexpected case in drvVTK - show_image " << endl;
}

static DriverDescriptionT < drvVTK > D_VTK("vtk", "VTK driver", "this is a long description for the VTKe driver","vtk", false,	// backend supports subpaths
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
											   false,	// backend supports curves
											   false,	// backend supports elements which are filled and have edges
											   false,	// backend supports text
											   DriverDescription::imageformat::noimage,	// support for PNG file images
											   DriverDescription::opentype::normalopen, true,	// if format supports multiple pages in one file
											   false  /*clipping */ 
											   );
