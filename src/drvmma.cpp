/*
   drvMMA.cpp : This file is part of pstoedit
   Backend for Mathematica Graphics
   Contributed by: Manfred Thole <manfred_AT_thole.org>
   $Id: drvmma.cpp,v 1.5 2003/03/15 16:25:46 manfred Exp $
   Based on drvSAMPL.cpp

   Copyright (C) 1993 - 2024 Wolfgang Glunz, wglunz35_AT_pstoedit.net,
                             Manfred Thole, manfred_AT_thole.org

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


/*
    Not implemented:
     - eofill - driver option '-eofillfills' (fill/don't fill)
     - images

    Usage within Mathematica:
       gr = Get["myfile.m"]
       Show /@ gr

    Use the following example rule for font scaling:
       grScaled = gr /. Rule[FontSize, a_] :> Rule[FontSize, a/2.]
 */

#include "drvmma.h"
#include I_fstream
#include I_stdio
#include I_stdlib




drvMMA::derivedConstructor(drvMMA):
constructBase,
mmaDashing(solid), mmaThickness(0.0), mmaR(0.0), mmaG(0.0), mmaB(0.0), buffer(tempFile.asOutput())
{
//	bool show_usage_line = false;
	// Driver options
#if 0
	for (unsigned int i = 0; i < d_argc; i++) {
		assert(d_argv && d_argv[i]);	//lint !e796 !e1776
		if (Verbose())
			outf << "% " << d_argv[i] << endl;
		if (strcmp(d_argv[i], "-eofillfills") == 0) {
			eofillFills = true;
		} else if (strcmp(d_argv[i], "-help") == 0) {
			errf << "-help         # Show this message" << endl;
			errf << "-eofillfills  # Filling is used for eofill (default is not to fill)" << endl;
			show_usage_line = true;
		} else {
			errf << "Unknown mma driver option: " << d_argv[i] << endl;
			show_usage_line = true;
		}
	}


	if (show_usage_line) {
		errf << "Usage -f 'mma: [-help] [-eofillfills]'" << endl;
	}
#endif
	// MMA has a different scientific notation
	(void) buffer.setf(ios::fixed, ios::floatfield);
	(void) outf.setf(ios::fixed, ios::floatfield);
	outf << "{\n";
}

// Destructor
//   We are closing List here
drvMMA::~drvMMA()
{
	outf << "Sequence[]\n}\n";	// Dummy!
	options=nullptr;
}

// Output a point.
// NOTE: This is a top-level function, not a member of drvMMA.
// Attention: there is also a version with a const ref pt in
// drvbase.h
static ostream & operator << (ostream & os, Point & pt)
{
	os << "{" << pt.x() << ", " << pt.y() << "}";
	return os;
}

// Draw a (closed) path
void drvMMA::draw_path(bool close, Point firstpoint, bool filled)
{
	ifstream & inbuffer = tempFile.asInput();
	if (filled) {
		RGBColor(fillR(), fillG(), fillB());
		outf << "Polygon[{";
		copy_file(inbuffer, outf);
		(void) inbuffer.seekg(0);
		outf << "}],\n";
	}
	RGBColor(currentR(), currentG(), currentB());
	outf << "Line[{";
	copy_file(inbuffer, outf);
	if (close) {
		outf << ", " << firstpoint;
	}
	outf << "}],\n";
	// tempFile.asOutput();
}


void drvMMA::print_coords()
{
	Point firstpoint;		// Where "closepath" takes us back to
	Point tmppoint;			// Temporary Point
	bool datapres = false;		// Is data in the buffer?
	bool filled = false;		// Must we fill it?

	switch (currentShowType()) {
	case drvbase::stroke:
		filled = false;
		break;
	case drvbase::fill:
		filled = true;
		break;
	case drvbase::eofill:		// It is nearly impossible to do eofill in MMA
		filled = options->eofillFills.value;
		break;
	default:
		break;
	}
	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		const basedrawingelement & elem = pathElement(n);
		switch (elem.getType()) {
		case moveto:
			if (datapres) {
				draw_path(false, firstpoint, filled);
			}
			firstpoint = elem.getPoint(0);
			(void) tempFile.asOutput();	// Reset tmpfile
			datapres = false;
			buffer << firstpoint;
			break;
		case lineto:
			datapres = true;
			tmppoint = elem.getPoint(0);
			buffer << ", " << tmppoint;
			break;
		case closepath:
			if (datapres) {
				draw_path(true, firstpoint, filled);
				datapres = false;
			}
			break;
			// We got a bad path element.
		case curveto:
		default:
			errf << "\t\tFatal: unexpected case in drvmma " << endl;
			abort();
			break;
		}
	}
	// Paste out data
	if (datapres) {
		draw_path(false, firstpoint, filled);
	}
}


void drvMMA::open_page()
{
	outf << "Graphics[{\n";
	// Initialize with impossible values to force setting these values
	// in show_path, show_text, ...
	mmaDashing = linetype(-1);
	mmaThickness = -1.0; //lint !e736
	mmaR = mmaG = mmaB = -1.0; //lint !e736 
}

void drvMMA::close_page()
{
	outf << "Sequence[]},\n";	// Dummy!
	outf << "AspectRatio -> Automatic, PlotRange -> All],\n";
}

//  Compute direction & offset:
//     angle = Exp[I*\[Pi]*angle/180.];
//     dir = {Re[angle], Im[angle]};
//     offset *= angle;
void drvMMA::show_text(const TextInfo & textinfo)
{
	double dirRe, dirIm;
	double offRe = -1.0;
	double offIm = -0.6;

	dirRe = cos(0.0174533 * textinfo.currentFontAngle);	// Pi/180
	dirIm = sin(0.0174533 * textinfo.currentFontAngle);	// Pi/180
	offRe = offRe * dirRe - offIm * dirIm;
	offIm = offRe * dirIm + offIm * dirRe;
	RGBColor(textinfo.currentR, textinfo.currentG, textinfo.currentB);
	outf << "Text[\"";
	// Escape special characters
	for (const char *c = textinfo.thetext.c_str(); *c; c++) {
		switch (*c) {
		case '"':
		case '\\':
			outf << '\\';
		//lint -fallthrough
		default: ;
		}
		
		outf << *c;
	}
	outf << "\", ";
	outf << "{" << textinfo.x() << ", " << textinfo.y() << "}, ";
	outf << "{" << offRe << ", " << offIm << "}, ";
	outf << "{" << dirRe << ", " << dirIm << "}, \n";
	outf << "TextStyle -> {";
	// FontFamily->"Times", FontSlant->"Italic", FontWeight->"Bold",
	// FontSize->12
	if (!strncmp(textinfo.currentFontName.c_str(), "Times", 5)) {
		outf << "FontFamily -> \"Times\", ";
	} else if (!strncmp(textinfo.currentFontName.c_str(), "Helvetica", 9)) {
		outf << "FontFamily -> \"Helvetica\", ";
	} else if (!strncmp(textinfo.currentFontName.c_str(), "Courier", 7)) {
		outf << "FontFamily -> \"Courier\", ";
	}
	if (strstr(textinfo.currentFontName.c_str(), "Italic")) {
		outf << "FontSlant -> \"Italic\", ";
	} else if (strstr(textinfo.currentFontName.c_str(), "Oblique")) {
		outf << "FontSlant -> \"Oblique\", ";
	}
	if (!strcmp(textinfo.currentFontWeight.c_str(), "Bold")) {
		outf << "FontWeight -> \"Bold\", ";
	}
	outf << "FontSize -> " << textinfo.currentFontSize;
	outf << "}],\n";
}

void drvMMA::show_path()
{
	if (mmaDashing != currentLineType()) {
		mmaDashing = currentLineType();
		switch (mmaDashing) {
		case solid:
			outf << "AbsoluteDashing[{}],\n";
			break;
		case dashed:
			outf << "AbsoluteDashing[{10, 5}],\n";
			break;
		case dashdot:
			outf << "AbsoluteDashing[{10, 5, 1, 5}],\n";
			break;
		case dotted:
			outf << "AbsoluteDashing[{1,5}],\n";
			break;
		case dashdotdot:
			outf << "AbsoluteDashing[{10, 5, 1, 5, 1, 5}],\n";
			break;
		default: ;// nothing to do
		}
	}
	if (mmaThickness != currentLineWidth()) {
		mmaThickness = currentLineWidth();
		outf << "AbsoluteThickness[" << mmaThickness << "],\n";
	}
	print_coords();
}

void drvMMA::RGBColor(float R, float G, float B)
{
	if ((mmaR != R) || (mmaG != G) || (mmaB != B)) {
		mmaR = R;
		mmaG = G;
		mmaB = B;
		outf << "RGBColor[" << R << ", " << G << ", " << B << "],\n";
	}
}

static DriverDescriptionT < drvMMA > D_mma("mma", "Mathematica graphics", "","m", true,	// backend supports subpaths
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
										   true,	// backend supports elements which are filled and have edges
										   true,	// backend supports text
										   DriverDescription::imageformat::noimage,	// no support for PNG file images
										   DriverDescription::opentype::normalopen, true,	// if format supports multiple pages in one file
										   false  /*clipping */ 
										   );

