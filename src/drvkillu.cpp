/* 
   drvkillu.cpp : This file is part of pstoedit
   Implementation of Kontour output driver.

   Copyright (C) 1993 - 2024 Wolfgang Glunz, wglunz35_AT_pstoedit.net
   Copyright (C) 1998,1999 Kai-Uwe Sattler, kus_AT_iti.cs.uni-magdeburg.de

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
#include "drvkillu.h"
#include I_fstream
#include I_stdio
#include I_stdlib
#include <math.h>



drvKontour::derivedConstructor(drvKontour):constructBase
{
// driver specific initializations
// and writing of header to output file

// <?xml version="1.0" encoding="UTF-8"?><!DOCTYPE killustrator >
//<killustrator mime="application/x-killustrator" version="3" editor="KIllustrator" >
//
 

	outf << "<?xml version=\"1.0\"?>\n<!DOCTYPE killustrator >"
		 <<	"<killustrator mime=\"application/x-killustrator\" version=\"3\" editor=\"KIllustrator\"  >\n";

	outf << "<head>\n" <<
		"<layout format=\"a4\" orientation=\"portrait\" width=\"210\"" <<
		"height=\"297\" lmargin=\"0\" tmargin=\"0\" rmargin=\"0\" bmargin=\"0\" />\n"
		<< "</head>\n";
}

drvKontour::~drvKontour()
{
	// driver specific deallocations
	// and writing of trailer to output file
	outf << "</killustrator>\n";
	options=nullptr;
}

void drvKontour::print_coords()
{
	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		const basedrawingelement & elem = pathElement(n);
		switch (elem.getType()) {
		case moveto:{
				const Point & p = elem.getPoint(0);
				outf << "<point x=\"" << p.x() + x_offset
					<< "\" y=\"" << currentDeviceHeight - p.y() + y_offset << "\" />\n";
			}
			break;
		case lineto:{
				const Point & p = elem.getPoint(0);
				outf << "<point x=\"" << p.x() + x_offset
					<< "\" y=\"" << currentDeviceHeight - p.y() + y_offset << "\" />\n";
			}
			break;
		case closepath:
			break;
		case curveto:{
				for (unsigned int cp = 0; cp < 3; cp++) {
					const Point & p = elem.getPoint(cp);
					outf << "<point x=\"" << p.x() + x_offset
						<< "\" y=\"" << currentDeviceHeight - p.y() + y_offset << "\" />\n";
				}
			}
			break;
		default:
			errf << "\t\tFatal: unexpected case in drvKontour " << endl;
			abort();
			break;
		}
	}
}


void drvKontour::open_page()
{
}

void drvKontour::close_page()
{
}

void drvKontour::show_text(const TextInfo & textinfo)
{
	static const float toRad = 3.14159265359f / 180.0f;
	const double angle = (360.0 - textinfo.currentFontAngle) * toRad;
	const double cosa = cos(angle);
	const double sina = sin(angle);
	outf << "<text matrix=\""
		<< cosa << " " << sina << " 0 "
		<< -sina << " " << cosa << " 0 "
		<< textinfo.x() << " " << currentDeviceHeight - textinfo.y() << " 1"
		<< "\""
		<< " strokecolor=\""
		<< cvtColor(textinfo.currentR) << " "
		<< cvtColor(textinfo.currentG) << " "
		<< cvtColor(textinfo.currentB) << "\""
		<< " strokestyle=\"1\"" << " linewidth=\"0\"" << " fillstyle=\"0\"" << " align=\"0\" >\n";
	outf << "<font face=\"";
	if (::strcmp(textinfo.currentFontName.c_str(), "Times-Roman") == 0)
		outf << "times";
	else
		outf << textinfo.currentFontName.c_str();
	outf << "\" point-size=\"" << textinfo.currentFontSize << "\"" << " weight=\"";
	if (strcmp(textinfo.currentFontWeight.c_str(), "Regular") == 0)
		outf << 50;
	else if (strcmp(textinfo.currentFontWeight.c_str(), "Bold") == 0)
		outf << 87;
	else
		outf << 50;
	outf << "\">\n";

	const char *c = textinfo.thetext.c_str();
	while (*c != '\0') {
		if (*c == '<')
			outf << "&lt;";
		else if (*c == '&')
			outf << "&amp;";
		else if (*c == '>')
			outf << "&gt;";
		else
			outf << *c;
		c++;
	}
	outf << "</font></text>\n";
}

void drvKontour::show_path()
{
	if (isPolygon())
		outf << "<polygon ";
	else
		outf << "<polyline ";
	outf << "matrix=\"1 0 0 0 1 0 0 1\" "
		<< "strokecolor=\"" << currentR() << " "
		<< currentB() << " " << currentG() << "\" "
		<< "strokestyle=\"" << 1 << "\" " << "linewidth=\"" << currentLineWidth() << "\" ";
	if (currentShowType() == drvbase::fill) {
		outf << "fillstyle=\"" << 1 << "\" " << "fillcolor=\"" << cvtColor(currentR())
			<< " " << cvtColor(currentG())
			<< " " << cvtColor(currentB()) << "\" ";
	} else if (currentShowType() == drvbase::eofill) { 
	} else {
		outf << "fillstyle=\"" << 0 << "\" ";
	}
	outf << "arrow1=\"0\" " << "arrow2=\"0\">\n";
	print_coords();
	if (isPolygon())
		outf << "</polygon>\n";
	else
		outf << "</polyline>\n";
}

void drvKontour::show_rectangle(const float llx, const float lly,
									 const float urx, const float ury)
{
	outf << "<rectangle "
		<< "matrix=\"1 0 0 0 1 0 0 1\" "
		<< "strokecolor=\"" << cvtColor(currentR()) << " "
		<< cvtColor(currentB()) << " " << cvtColor(currentG()) << "\" "
		<< "strokestyle=\"" << 1 << "\" "
		<< "linewidth=\"" << currentLineWidth() << "\" "
		<< "fillstyle=\"" << 0 << "\" "
		<< "x=\"" << llx << "\" "
		<< "y=\"" << currentDeviceHeight - lly << "\" "
		<< "with=\"" << urx - llx << "\" "
		<< "height=\"" << ury - lly << "\" " << "rounding=\"" << 0 << "\"/>" << endl;
}

int drvKontour::cvtColor(float c)
{
	return int ((c * 255.0) + 0.5);
}

static DriverDescriptionT < drvKontour > D_Kontour("kil", ".kil format for Kontour", "","kil", false,	// backend supports subpaths
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
															 true,	// backend supports text
															 DriverDescription::imageformat::noimage,	// no support for PNG file images
															 DriverDescription::opentype::normalopen, false,	// if format supports multiple pages in one file
															 false
															 /*clipping */
	);
