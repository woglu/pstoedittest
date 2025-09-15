/*
   drvcfdg.cpp : This file is part of pstoedit
   Backend for Context Free Design Grammar files
   Contributed by: Scott Pakin <scott+ps2ed_AT_pakin.org>

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

#include "drvcfdg.h"

#include I_iostream

// Constructor -- Initialize variables and take other per-document actions
drvCFDG::derivedConstructor(drvCFDG): constructBase
{
	// Prevent the use of scientific notation, which CFDG doesn't
	// understand. For now, we hardwire the precision to 6 digits
	// after the decimal point.
	outf.setf(ios::fixed);
	outf.precision(6);

	// Output copyright information
	outf << "// Converted from PostScript(TM) to CFDG by pstoedit\n"
	     << "// CFDG backend contributed by Scott Pakin <scott+ps2ed_AT_pakin.org>\n"
	     << "// pstoedit is Copyright (C) 1993 - 2024 Wolfgang Glunz"
	     << " <wglunz35_AT_pstoedit.net>\n\n";

	// Output the CFDG startshape
	outf << "startshape page1\n";
}


drvCFDG::~drvCFDG()
{
	options=nullptr;
}

// Each page produces a different path
void drvCFDG::open_page()
{
	outf << "\n"
	     << "path page" << currentPageNumber << " {" << endl;
}

void drvCFDG::close_page()
{
	outf << "}" << endl;
}


// Given an RGB color, output it as an HSV color.
void drvCFDG::print_rgb_as_hsv(float red, float green, float blue)
{
	float hue, saturation, value;
	float minchan, maxchan, delta;

	// Determine V (value)
	minchan = red;
	minchan = minchan < green ? minchan : green;
	minchan = minchan < blue ? minchan : blue;
	maxchan = red;
	maxchan = maxchan > green ? maxchan : green;
	maxchan = maxchan > blue ? maxchan : blue;
	value = maxchan;
	delta = maxchan - minchan;
	if (maxchan == 0.0) {
		// Black
		outf << "hue 0 sat 0 b 0";
		return;
	}

	// Determine S (saturation)
	saturation = delta / maxchan;
	if (saturation == 0.0) {
		// Gray
		outf << "hue 0 sat 0 b " << value;
		return;
	}

	// Determine H (hue)
	if (red == maxchan)
		hue = (green - blue) / delta;
	else if (green == maxchan)
		hue = 2.0f + (blue - red) / delta;
	else
		hue = 4.0f + (red - green) / delta;
	hue *= 60.0;
	if (hue < 0.0 )
		hue += 360.0;

	// Output the HSV color
	outf << "hue " << hue << " sat " << saturation << " b " << value;
}


void drvCFDG::print_coords()
{
	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		const basedrawingelement & elem = pathElement(n);
		switch (elem.getType()) {
		case moveto:{
				const Point & p = elem.getPoint(0);
				outf << "  MOVETO ( "
				     << p.x() + x_offset
				     << ", " << p.y() + y_offset
				     << " )";
			}
			break;
		case lineto:{
				const Point & p = elem.getPoint(0);
				outf << "  LINETO ( "
				     << p.x() + x_offset
				     << ", " << p.y() + y_offset
				     << " )";
			}
			break;
		case closepath:
			outf << "  CLOSEPOLY ( )";
			break;
		case curveto:{
				outf << "  CURVETO ( ";
				for (unsigned int i = 0; i < 3; i++) {
					const unsigned int cp = (i + 2) % 3;
					const Point & p = elem.getPoint(cp);
					if (i != 0)
						outf << ", ";
					outf << p.x() + x_offset
					     << ", " << p.y() + y_offset;
				}
				outf << " )";
			}
			break;
		default:
			errf << "\t\tFatal: unexpected case in drvcfdg " << endl;
			abort();
			break;
		}
		outf << endl;
	}
}

void drvCFDG::show_path()
{
	print_coords();
	switch (currentShowType()) {
	case drvbase::stroke:
		outf << "  STROKE ( "
		     << currentLineWidth()
		     << ", ";
		switch (currentLineCap()) {
		case 0:
			outf << "CF::ButtCap";
			break;
		case 1:
			outf << "CF::RoundCap";
			break;
		case 2:
			outf << "CF::SquareCap";
			break;
		default:
			// cannot happen
			errf << "unexpected LineCap " << (int) currentLineCap();
			abort();
			break;
		}
		outf << " ) [ ";
		print_rgb_as_hsv(edgeR(), edgeG(), edgeB());
		outf << " ]";
		break;
	case drvbase::fill:
		outf << "  FILL [ ";
		print_rgb_as_hsv(fillR(), fillG(), fillB());
		outf << " ]";
		break;
	case drvbase::eofill:
		outf << "  FILL ( CF::EvenOdd ) [ ";
		print_rgb_as_hsv(fillR(), fillG(), fillB());
		outf << " ]";
		break;
	default:
		// cannot happen
		errf << "unexpected ShowType " << (int) currentShowType();
		abort();
		break;
	}
	outf << endl;
}




static DriverDescriptionT < drvCFDG > D_cfdg("cfdg", "Context Free Design Grammar", "Context Free Design Grammar, usable by Context Free Art (http://www.contextfreeart.org/)", "cfdg",
					     true,	// backend supports subpaths
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
					     true,	// backend supports curves
					     true,	// backend supports elements which are filled and have edges
					     false,	// backend does not support text
					     DriverDescription::imageformat::noimage,  // no support for PNG file images
					     DriverDescription::opentype::normalopen, true,	// if format supports multiple pages in one file
					     false	// backend does not support clipping
					     );
