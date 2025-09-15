/*
   drvMPOST.cpp : This file is part of pstoedit
   Backend for MetaPost files
   Contributed by: Scott Pakin <scott+ps2ed_AT_pakin.org>

   Copyright (C) 1993 - 2024 Wolfgang Glunz, wglunz35_AT_geocities.com

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
#ifdef _MSC_VER
// #define _POSIX_					// for MSVC and limits.h
#endif

#include "drvmpost.h"


// #include I_fstream
#include I_stdio
#include I_stdlib

#include <math.h>				// Need definition of FLT_MAX
#include <float.h>				// FLT_MAX on MSVC

#include <limits.h>				// PATH_MAX for MSVC
#include <ctype.h>				// Need definition of ispunct()


#if !(defined(unix) || defined(__unix__) || defined(_unix) || defined(__unix) || defined(__EMX__) || defined (NetBSD)  )
#define LINE_MAX 2048			// for MSVC
#endif

static const std::string emptystring("");

// Constructor -- Initialize variables and take other per-document actions

drvMPOST::derivedConstructor(drvMPOST):
constructBase,
	// Initialize member variables
	prevFontName(""), prevR(0.0f), prevG(0.0f), prevB(0.0f),	// Black
	prevFontAngle(FLT_MAX), prevFontSize(-1.0), prevLineWidth(0.0f), prevLineCap(1),	// Round line caps
	prevLineJoin(1),			// Round joins
	prevDashPattern(""),		// Solid lines
	fillmode(false)
{
	// Prevent the use of scientific notation, which MetaPost
	// doesn't understand.	For now, we hardwire the precision to
	// 6 digits after the decimal point.
	outf.setf(ios::fixed);
	outf.precision(6);

	// Output copyright information
	outf << "% Converted from PostScript(TM) to MetaPost by pstoedit\n"
		<<
		"% MetaPost backend contributed by Scott Pakin <scott+ps2ed_AT_pakin.org>\n"
		<< "% pstoedit is Copyright (C) 1993 - 2024 Wolfgang Glunz" <<
		" <wglunz35_AT_pstoedit.net>\n\n";

	/*
	 * Output some useful assignments and macro defintions
	 *
	 * Thanks to S.J.Henriksen <eesjh_AT_ee.newcastle.edu.au> for the showtext
	 * macro
	 */
	outf << "% Generate structured PostScript\n"
		<< "prologues := 1;\n"
		<< "\n"
		<<
		"% Display a given string with its *baseline* at a given location\n"
		<< "% and with a given rotation angle\n" <<
		"vardef showtext(expr origin)(expr angle)(expr string) =\n" <<
		"  draw string infont defaultfont scaled defaultscale\n" <<
		"    rotated angle shifted origin;\n" << "enddef;\n" << endl;
}


// Destructor -- Tell MetaPost to end the file
drvMPOST::~drvMPOST()
{
	outf << "end" << endl;
	options=nullptr;
}


// Output a path
void drvMPOST::print_coords()
{
	bool withinpath = false;	// true=already started plotting points
	unsigned int pointsOnLine = 0;	// Number of points on the current output line

	if (fillmode) {
		for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
			const basedrawingelement & elem = pathElement(n);
			switch (elem.getType()) {
			case moveto:
			case lineto:
				{
					const Point & p = elem.getPoint(0);
					if (withinpath)
						outf << "--";
					else
						outf << "fill ";
					outf << '(' << (p.x() + x_offset) << ',' << (p.y() + y_offset)
						<< ')';
					withinpath = true;
					pointsOnLine++;
				}
				break;

			case closepath:
				if (prevDashPattern == emptystring)
					outf << "--cycle;" << endl;
				else
					outf << "--cycle " << prevDashPattern << ';' << endl;
				withinpath = false;
				pointsOnLine = 0;
				break;

			case curveto:
				{
					if (!withinpath) {
						cerr << "curveto without a moveto; ignoring" << endl;
						break;
					}
					const Point & p0 = elem.getPoint(0);
					outf << "..controls (" << (p0.x() + x_offset)
						<< ',' << (p0.y() + y_offset) << ") and (";
					const Point & pt1 = elem.getPoint(1);
					outf << (pt1.x() + x_offset) << ',' << (pt1.y() + y_offset) << ")..(";
					const Point & pt2 = elem.getPoint(2);
					outf << (pt2.x() + x_offset) << ',' << (pt2.y() + y_offset) << ')';
					pointsOnLine += 3;
					withinpath = true;
				}
				break;

			default:
				errf << "\t\tFatal: unexpected path element in drvmpost" << endl;
				abort();
				break;
			}
			if (pointsOnLine >= 3) {
				outf << "\n\t";
				pointsOnLine = 0;
			}
		}
		if (withinpath) {			// Finish the final path
			if (prevDashPattern == emptystring)
				outf << "--cycle;" << endl;
			else
				outf << "--cycle " << prevDashPattern << ';' << endl;
		}
	} else {					// Stroking, not filling
		for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
			const basedrawingelement & elem = pathElement(n);
			switch (elem.getType()) {
			case moveto:
				{
					if (withinpath)
						outf << prevDashPattern << ';' << endl;
					outf << "draw ";
					const Point & p = elem.getPoint(0);
					outf << '(' << (p.x() + x_offset) << ',' << (p.y() + y_offset)
						<< ')';
					withinpath = true;
					pointsOnLine = 1;	// Contrast with the case where fillmode is set
				}
				break;

			case lineto:
				{
					const Point & p = elem.getPoint(0);
					if (withinpath) {
						outf << "--";
					} else {
						cerr << "lineto without a moveto; ignoring" << endl;
						break;
					}
					outf << '(' << (p.x() + x_offset) << ',' << (p.y() + y_offset)
						<< ')';
					withinpath = true;
					pointsOnLine++;
				}
				break;

			case closepath:
				if (prevDashPattern == emptystring ) {
					outf << "--cycle;" << endl;
				} else {
					outf << "--cycle " << prevDashPattern << ';' << endl;
				}
				withinpath = false;
				pointsOnLine = 0;
				break;

			case curveto:
				{
					if (!withinpath) {
						cerr << "curveto without a moveto; ignoring" << endl;
						break;
					}
					const Point & p0 = elem.getPoint(0);
					outf << "..controls (" << (p0.x() + x_offset)
						<< ',' << (p0.y() + y_offset) << ") and (";
					const Point & pt1 = elem.getPoint(1);
					outf << (pt1.x() + x_offset) << ',' << (pt1.y() + y_offset) << ")..(";
					const Point & pt2 = elem.getPoint(2);
					outf << (pt2.x() + x_offset) << ',' << (pt2.y() + y_offset) << ')';
					pointsOnLine += 3;
					withinpath = true;
				}
				break;

			default:
				errf << "\t\tFatal: unexpected path element in drvmpost" << endl;
				abort();
				break;
			}
			if (pointsOnLine >= 3 &&
				n + 1 < numberOfElementsInPath() && pathElement(n + 1).getType() != moveto) {
				outf << "\n\t";
				pointsOnLine = 0;
			}
		}
		if (withinpath)	{		// Finish the final path
			outf << prevDashPattern << ';' << endl;
		}
	}
}


// Each page will produce a different figure
void drvMPOST::open_page()
{
	outf << "beginfig(" << currentPageNumber << ");" << endl;
}

void drvMPOST::close_page()
{
	outf << "endfig;" << endl;
}

// Output a text string
void drvMPOST::show_text(const TextInfo & textinfo)
{
	static bool texshortchar = false;	// 0=ASCII; 1=TeX character set

	// Change fonts
	std::string thisFontName(textinfo.currentFontName.c_str());
	if (thisFontName == emptystring ) {	// If we're this brain-damaged, we must be a TeX font
		thisFontName = textinfo.currentFontFullName.c_str();
		if (!texshortchar) {
			outf << "shortchar := char(24);" << endl;	// Cedilla in TeX land
			texshortchar = true;
		}
		if (Verbose() && thisFontName != prevFontName)
			errf << "nameless font (" << thisFontName << "?) -- "
				<< "assuming TeX character set" << endl;
	} else if (texshortchar) {
		outf << "shortchar := \"_\";" << endl;
		texshortchar = false;
	}
	if (thisFontName != prevFontName) {
		outf << "defaultfont := \"" << thisFontName << "\";" << endl;
		prevFontName = thisFontName.c_str();
	}
	// Change colors
	if (textinfo.currentR != prevR || textinfo.currentG != prevG || textinfo.currentB != prevB) {
		outf << "drawoptions (withcolor ("
			<< textinfo.currentR << ',' << textinfo.
			currentG << ',' << textinfo.currentB << "));" << endl;
		prevR = textinfo.currentR;
		prevG = textinfo.currentG;
		prevB = textinfo.currentB;
	}
	// Change font size
	if (textinfo.currentFontSize != prevFontSize) {
		if (textinfo.currentFontSize > 0) {
			outf << "defaultscale := " << textinfo.currentFontSize
				<< "/fontsize defaultfont;" << endl;
			prevFontSize = textinfo.currentFontSize;
		} else {
			if (Verbose())
				errf << "warning: font size of " << textinfo.
					currentFontSize << "pt encountered; ignoring" << endl;
			outf << "%defaultscale := " << textinfo.
				currentFontSize << "/fontsize defaultfont;" << endl;
		}
	}
	// Change rotation
	if (textinfo.currentFontAngle != prevFontAngle)
		prevFontAngle = textinfo.currentFontAngle;

	// Output the text using macros defined in the constructor
	outf << "showtext ((" << textinfo.x() << ',' << textinfo.y() << "), " << prevFontAngle << ", \"";
	for (const char *c = textinfo.thetext.c_str(); *c; c++)
		if (*c == '"')
			outf << "\" & char(34) & \"";
		else
			outf << (*c);
	outf << "\");" << endl;
}


// Output a path
void drvMPOST::show_path()
{
	// Change colors
	if (currentR() != prevR || currentG() != prevG || currentB() != prevB) {
		prevR = currentR();
		prevG = currentG();
		prevB = currentB();
		outf << "drawoptions (withcolor ("
			<< prevR << ',' << prevG << ',' << prevB << "));" << endl;
	}
	// Change line width
	if (currentLineWidth() != prevLineWidth) {
		prevLineWidth = currentLineWidth();
		outf << "pickup pencircle scaled " << prevLineWidth << "bp;" << endl;
	}
	// Change line cap
	if (currentLineCap() != prevLineCap) {
		prevLineCap = currentLineCap();
		switch (prevLineCap) {
		case 0:
			outf << "linecap := butt;" << endl;
			break;
		case 1:
			outf << "linecap := rounded;" << endl;
			break;
		case 2:
			outf << "linecap := squared;" << endl;
			break;
		default:
			errf << "\t\tFatal: Unknown linecap \"" << prevLineCap << '"' << endl;
			abort();
			break;
		}
	}
	// Change line join
	if (currentLineJoin() != prevLineJoin) {
		prevLineJoin = currentLineJoin();
		switch (prevLineJoin) {
		case 0:
			outf << "linejoin := mitered;" << endl;
			break;
		case 1:
			outf << "linejoin := rounded;" << endl;
			break;
		case 2:
			outf << "linejoin := beveled;" << endl;
			break;
		default:
			errf << "\t\tFatal: Unknown linejoin \"" << prevLineJoin << '"' << endl;
			abort();
			break;
		}
	}
	// Change dash pattern
	const char *currentDashPattern = dashPattern();	// Current dash pattern string
	unsigned long lengthOn, lengthOff;	// Fraction on vs. off
	float dashOffset;			// Offset into dash pattern
	if (sscanf_s(currentDashPattern, "[ ] %f", &dashOffset) == 1)
		prevDashPattern = "";
	else {
		char temptext[100];

		if (sscanf_s(currentDashPattern, "[%lu] %f", &lengthOn, &dashOffset)
			== 2) {
			if (dashOffset)
				sprintf_s(TARGETWITHLEN(temptext,100),
						" dashed evenly scaled %lubp shifted -%fbp", lengthOn, dashOffset);
			else
				sprintf_s(TARGETWITHLEN(temptext,100), " dashed evenly scaled %lubp", lengthOn);
			prevDashPattern = temptext;
		} else
			if (sscanf_s
				(currentDashPattern, "[%lu %lu] %f", &lengthOn, &lengthOff, &dashOffset) == 3) {
			if (dashOffset)
				sprintf_s(TARGETWITHLEN(temptext,100),
						" dashed dashpattern(on %lubp off %lubp) shifted (-%f,0)",
						lengthOn, lengthOff, dashOffset);
			else
				sprintf_s(TARGETWITHLEN(temptext,100), " dashed dashpattern(on %lubp off %lubp)", lengthOn, lengthOff);
			prevDashPattern = temptext;
		} else {
			if (Verbose())
				cerr << "Dash pattern \"" << currentDashPattern <<
					"\" is too complex;\n" << "using a generic pattern instead" << endl;
			prevDashPattern = " dashed evenly";
		}
	}

	// Determine the fill mode
	if (Verbose() && currentShowType() == drvbase::eofill)
		errf << "MetaPost does not support eofill; using fill instead" << endl;
	fillmode = (currentShowType() == drvbase::eofill || currentShowType() == drvbase::fill);
	if (!fillmode && currentShowType() != drvbase::stroke) {
		errf << "\t\tFatal: unexpected show type " << (int)
			currentShowType() << " in drvmpost" << endl;
		abort();
	}
	// Draw the path
	print_coords();
}

static DriverDescriptionT < drvMPOST > D_mpost("mpost", "MetaPost format", "","mp", true,	// if backend supports subpaths, else 0
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
											   true,	// if backend supports curves, else 0
											   false,	// if backend supports elements with fill and edges
											   true,	// if backend supports text, else 0
											   DriverDescription::imageformat::noimage,	// no support for PNG file images
											   DriverDescription::opentype::normalopen, true,	// if format supports multiple pages in one file
											   false  /*clipping */ 
											   );
