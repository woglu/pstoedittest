/*
   drvLATEX2E.cpp : This file is part of pstoedit
   Backend for Latex2E files
   Contributed by: Scott Pakin <scott+ps2ed_AT_pakin.org>

   Copyright (C) 1993 - 2024	Wolfgang Glunz, <wglunz35_AT_pstoedit.net>, 
							Scott Pakin, <scott+ps2ed_AT_pakin.org>

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


#include "drvlatex2e.h"


// Constructor
drvLATEX2E::derivedConstructor(drvLATEX2E):constructBase, buffer(tempFile.asOutput()),
	prevR(0.0f), prevG(0.0f), prevB(0.0f),
   thicklines(false),
   prevfontname(""),
   prevfontsize(0.0f)
{
#if 0
	for (unsigned int i = 0; i < d_argc; i++) {
		assert(d_argv && d_argv[i]);
		if (!strcmp(d_argv[i], "-integers"))
			integersonly = true;
	}
#endif
}


// Destructor
drvLATEX2E::~drvLATEX2E()
{
	options=nullptr;
}

struct Point2e {
	Point2e(const Point& p_, bool integersonly_p) : p(p_),integersonly(integersonly_p) {}
	Point p;
	bool integersonly;
};

// Output a point.
// NOTE: This is a top-level function, not a member of drvLATEX2E.
static ostream & operator << (ostream & os, const Point2e & pt)
{
	if (pt.integersonly) {
		os << '(' << long (pt.p.x() + 0.5) << ',' << long (pt.p.y() + 0.5) << ')';
	} else {
		os << '(' << pt.p.x() << ',' << pt.p.y() << ')';
	}
	return os;
}


// Convert the coordinates on a path to LaTeX2e.
void drvLATEX2E::print_coords()
{
	Point *firstpoint = nullptr;	// Where "closepath" takes us back to
	Point pointlist[3];			// Other points we care about
//old  ostrstream outputline;       // Current line of LaTeX2e to output

	buffer.setf(ios::fixed, ios::floatfield);	// TeX can't deal with scientific notation.
	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		const basedrawingelement & elem = pathElement(n);
		switch (elem.getType()) {
			// Store a new current point.
		case moveto:
			currentpoint = elem.getPoint(0);
			scalepoint(currentpoint);
			updatebbox(currentpoint);
			if (!firstpoint) {
				firstpoint = new Point(currentpoint);
				assert(firstpoint);
			}
			break;

			/*
			 * Draw a line.  If the line is vertical or horizontal (the easy
			 * cases), then use the picture-mode "\line" command.  Otherwise,
			 * don't bother calculating slopes and GCDs and all the other crud
			 * that \line requires.  Instead, take the easy way out and use a
			 * \qbezier command.
			 */
		case lineto:
		case closepath:
			if (elem.getType() == lineto) {
				pointlist[0] = elem.getPoint(0);
				scalepoint(pointlist[0]);
				updatebbox(pointlist[0]);
			} else {
				assert(firstpoint);
				pointlist[0] = *firstpoint;
				delete firstpoint;
				firstpoint = nullptr;
			}
			if (pointlist[0].x() == currentpoint.x()) {	// Vertical line
				if (pointlist[0].y() == currentpoint.y())	// (and not a point)
					break;
				const float p_distance = (float) fabs(pointlist[0].y() - currentpoint.y());
				buffer << "  \\put" << Point2e(currentpoint,options->integersonly) << "{\\line(0," <<
					(pointlist[0].y() > currentpoint.y() ? 1 : -1) << "){";
				if (options->integersonly) {
					buffer << long (p_distance + 0.5) << "}}";
				} else {
					buffer << p_distance << "}}";
				}
			} else if (pointlist[0].y() == currentpoint.y()) {	// Horizontal line
				const float p_distance = (float) fabs(pointlist[0].x() - currentpoint.x());
				buffer << "  \\put" << Point2e(currentpoint,options->integersonly) << "{\\line(" <<
					(pointlist[0].x() > currentpoint.x() ? 1 : -1) << ",0){";
				if (options->integersonly) {
					buffer << long (p_distance + 0.5) << "}}";
				} else {
					buffer << p_distance << "}}";
				}
			} else				// Diagonal line -- use a Bezier
				buffer << "  \\qbezier" << Point2e(currentpoint,options->integersonly) << Point2e(pointlist[0],options->integersonly) << Point2e(pointlist[0],options->integersonly);
			buffer << endl;
			currentpoint = pointlist[0];
			break;

			/*
			 * Draw a Bezier curve.  This is trickier than in most pstoedit
			 * backends because LaTeX2e uses quadratic Beziers, while PostScript
			 * (and hence, pstoedit) use cubic Beziers.  Fortunately, it's
			 * possible to convert from one to the other (more or less), and
			 * that's what we do.
			 */
		case curveto:{
				for (unsigned int cp = 0; cp < 3; cp++) {
					pointlist[cp] = elem.getPoint(cp);
					scalepoint(pointlist[cp]);
					updatebbox(pointlist[cp]);
				}
				const float midx = ((3 * pointlist[0].x() - currentpoint.x()) / 2 +
							  (3 * pointlist[1].x() - pointlist[2].x()) / 2) / 2;
				const float midy =
					((3 * pointlist[0].y() - currentpoint.y()) / 2 +
					 (3 * pointlist[1].y() - pointlist[2].y()) / 2) / 2;
				const Point midpoint(midx, midy);
				buffer << "  \\qbezier" << Point2e(currentpoint,options->integersonly) << Point2e(midpoint,options->integersonly) << Point2e(pointlist[2],options->integersonly) << endl;
				currentpoint = pointlist[2];
			}
			break;

			// We got a bad path element.
		default:
			errf << "\t\tFatal: unexpected case in drvlatex2e " << endl;
			abort();
			break;
		}
	}

	// Finish up the path.
//old  buffer << ends;
//old  outputQ.push (outputline.str());
	if (firstpoint)
		delete firstpoint;
}


// Start of page (really: figure) -- initialize bounding box and "previous"
// versions of things.
void drvLATEX2E::open_page()
{
	currentpoint = Point( 0, 0);
	boundingbox[0] = Point(1e+10, 1e+10);; //lint !e736
	boundingbox[1] = Point(-1e+10, -1e+10); //lint !e736
	prevR = prevG = prevB = 0.0;
	thicklines = false;
	prevfontname = "";
	prevfontsize = -1.0; //lint !e736
} 


// End of page (really: figure) -- output a bounding box and the entire
// contents of the output queue.
void drvLATEX2E::close_page()
{
	// Specify the picture's width and height and, optionally, the origin.
	const Point boxsize(boundingbox[1].x() - boundingbox[0].x(), boundingbox[1].y() - boundingbox[0].y());
	outf << "\\begin{picture}" << Point2e(boxsize,options->integersonly);
	if (boundingbox[0].x() || boundingbox[0].y())
		outf << Point2e(boundingbox[0],options->integersonly);
	outf << endl;

#if 0 // old
	// Output everything in the queue.
	while (!outputQ.empty()) {
		outf << outputQ.front();
		outputQ.pop();
	}
#endif
// now we can copy the buffer the output
	ifstream & inbuffer = tempFile.asInput();
	copy_file(inbuffer, outf);
	(void) tempFile.asOutput();

	// Close the picture environment.
	outf << "\\end{picture}" << endl;

}


/*
 * Output a piece of text.  We rely on pstoedit's font-mapping routine to
 * convert the name directly to parameters to \usefont.
 */
void drvLATEX2E::show_text(const TextInfo & textinfo)
{
//old  ostrstream outputline;       // Current line of LaTeX2e to output

	buffer.setf(ios::fixed, ios::floatfield);	// TeX can't deal with scientific notation.

	// Set the font and font size if (and only if) it's changed.
	const std::string fontname(textinfo.currentFontName.c_str());
	if (fontname != prevfontname) {
	   if (fontname[0] != '{') {
		errf << "Font \"" << fontname
			<<
			"\" is not of the form \"{encoding}{family}{series}{shape}\".\n"
			<<
			"(You may need to use the -fontmap option to point pstoedit to latex2e.fmp.)" << endl;
	   } else {
		buffer << "  \\usefont" << fontname << endl;
	   }
	   prevfontname = fontname;
	}
	const float fontsize = textinfo.currentFontSize * 72.27f / 72.0f;
	if (fontsize != prevfontsize) {
		buffer << "  \\fontsize{";
		if (options->integersonly) {
			const long longsize = long (fontsize + 0.5);
			buffer << longsize << "\\unitlength}{" << longsize;
		} else
			buffer << fontsize << "\\unitlength}{" << fontsize;
		buffer << "\\unitlength}\\selectfont" << endl;
		prevfontsize = fontsize;
	}
	// Set the color if (and only if) it's changed.
	// NOTE: Color requires the "color" package.
	if (textinfo.currentR != prevR || textinfo.currentG != prevG || textinfo.currentB != prevB) {
		prevR = textinfo.currentR;
		prevG = textinfo.currentG;
		prevB = textinfo.currentB;
		buffer << "  \\color[rgb]{" << prevR << ',' << prevG << ',' << prevB << '}' << endl;
	}
	// Scale the starting point and update the bounding box.
	Point textpoint(textinfo.p);
	scalepoint(textpoint);
	updatebbox(textpoint);

	// Output the text string, optionally rotated.
	// NOTE: Rotation requires the "rotation" package.
	buffer << "  \\put" << Point2e(textpoint,options->integersonly) << '{';
	if (textinfo.currentFontAngle) {
		if (options->integersonly) {
			buffer << "\\turnbox{" << long (textinfo.currentFontAngle + 0.5) << "}{";
		} else {
			buffer << "\\turnbox{" << textinfo.currentFontAngle << "}{";
		}
	}
	const char * cp = textinfo.thetext.c_str();
	//buffer << textinfo.thetext.c_str() 

// characters to be handled specially	
//
// % -> \%
// # -> \#
// { -> \{
// } -> \}
// _ -> \_
// & -> \&
// $ -> \$  
// 	Tritt auch manchmal mit veränderter Bedeutung als $$ auf: --> \$\$
// 
// \ -> \textbackslash
// ^ -> \textasciicircum
// ~ -> \textasciitilde
// 
// Zusätzlich machen folgende Zeichen oft Probleme:
// 
// " -> \textquotedblright
// ?`-> \textquestiondown
// !`-> \textexclamdown
// 
	while (cp && *cp) {
		if ((*cp == '%') ||
			(*cp == '#') ||
			(*cp == '{') ||
			(*cp == '}') ||
			(*cp == '$') ||
			(*cp == '_') ||
			(*cp == '&') )
			buffer << '\\' << *cp ; // needs to be escaped
		else if (*cp == '\\') buffer << "\\textbackslash ";
		else if (*cp == '^')  buffer << "\\textasciicircum ";
		else if (*cp == '~')  buffer << "\\textasciitilde ";
		else if (*cp == '"')  buffer << "\\textquotedblright ";
//		else if (*cp == '\\') buffer << "\\textbackslash";
//		else if (*cp == '\\') buffer << "\\textbackslash";
		else buffer << *cp;
		cp++;
	}
	buffer << '}';
	if (textinfo.currentFontAngle)
		buffer << '}';

	// Scale the ending point and update the bounding box.
	currentpoint = textinfo.p_end;
	scalepoint(currentpoint);
	updatebbox(currentpoint);

	// Finish up the text message.
	buffer << endl;				// old << ends;
//old  outputQ.push (outputline.str());
}


/*
 * Convert a path to LaTeX2e picture format.  LaTeX2e's picture mode has no
 * way to fill a region, so we ignore fills.  It can't do line caps, so we
 * ignore those.  Dashes would be a pain to implement (given that we're
 * using \qbezier), so we ignore dash patterns.  Line widths are either
 * "thick" or "thin"; we separate the two at 1pt.  Finally, colors are
 * supported by all the common backends with the color package, so we can
 * deal with that.  */
void drvLATEX2E::show_path()
{
	// Set the line width as best we can.
	if (currentLineWidth() >= 1.0) {
		if (!thicklines) {
			buffer << ("  \\thicklines\n");
			thicklines = true;
		}
	} else if (thicklines) {
		buffer << ("  \\thinlines\n");
		thicklines = false;
	}
	// Set the color if (and only if) it's changed.
	if (currentR() != prevR || currentG() != prevG || currentB() != prevB) {
//old    ostrstream outputline;
		buffer.setf(ios::fixed, ios::floatfield);	// TeX can't deal with scientific notation.
		prevR = currentR();
		prevG = currentG();
		prevB = currentB();
		buffer << "  \\color[rgb]{" << prevR << ',' << prevG << ',' << prevB << '}' << endl;	// old << ends;
//old    buffer <<  (outputline.str());
	}
	// Output the path.
	print_coords();
}


// Draw a rectangle with \framebox.
void drvLATEX2E::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{
//old  ostrstream outputline;
	buffer.setf(ios::fixed, ios::floatfield);	// TeX can't deal with scientific notation.
	Point ll(llx, lly);
	Point ur(urx, ury);

	scalepoint(ll);
	updatebbox(ll);
	scalepoint(ur);
	updatebbox(ur);
	const Point framesize(ur.x() - ll.x(), ur.y() - ll.y());
	buffer << "  \\put" << Point2e(ll,options->integersonly) << "{\\framebox" << Point2e(framesize,options->integersonly) << "{}}" << endl;	// old << ends;
//old  outputQ.push (outputline.str());
}

#if 0
// LaTeX2e's picture mode doesn't support images, so do nothing.
void drvLATEX2E::show_image(const Image &  )
{
}
#endif

// Describe the LaTeX2e backend's capabilities.
static DriverDescriptionT < drvLATEX2E > D_latex2e("latex2e", "\\LaTeX2e picture format", "","tex", true,	// backend supports subpaths
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
												   false,	// backend supports elements which are filled and have edges
												   true,	// backend supports text
												   DriverDescription::imageformat::noimage,	// no support for PNG file images
												   DriverDescription::opentype::normalopen, false,	// if format supports multiple pages in one file
												   false 	// no clipping 
												   
	);

