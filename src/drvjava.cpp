/* 
   drvJAVA.cpp : This file is part of pstoedit
   backend to generate a Java(TM) applet

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
#include "drvjava.h"
#include I_fstream
#include I_stdio
#include I_stdlib

//lint -esym(754,JavaFontDesc::javaname) // not used so far
//lint -esym(754,JavaFontDesc::javastyle) // not used so far
struct JavaFontDesc {
	const char *psname;
 	const char *javaname;
 	const char *javastyle;
};

static const JavaFontDesc JavaFonts[] = {	// predefined Fonts 
	{"Courier", "Courier", "Font.PLAIN"},
	{"Courier-Bold", "Courier", "Font.BOLD"},
	{"Courier-Oblique", "Courier", "Font.ITALIC"},
	{"Courier-BoldOblique", "Courier", "Font.BOLD + Font.ITALIC"},
	{"Helvetica", "Helvetica", "Font.PLAIN"},
	{"Helvetica-Bold", "Helvetica", "Font.BOLD"},
	{"Helvetica-Oblique", "Helvetica", "Font.ITALIC"},
	{"Helvetica-BoldOblique", "Helvetica", "Font.BOLD + Font.ITALIC"},
	{"Times-Roman", "TimesRoman", "Font.PLAIN"},
	{"Times-Bold", "TimesRoman", "Font.BOLD"},
	{"Times-Italic", "TimesRoman", "Font.ITALIC"},
	{"Times-BoldItalic", "TimesRoman", "Font.BOLD + Font.ITALIC"},
	{"ZapfDingbats", "ZapfDingbats", "Font.PLAIN"},
	{nullptr, nullptr, nullptr}
};

const unsigned int numberOfFonts = sizeof(JavaFonts) / (sizeof(JavaFontDesc)) - 1;

static int getFontNumber(const char *const fontname)
{
	const size_t fntlength = strlen(fontname);
	for (unsigned int i = 0; i < numberOfFonts; i++) {
		const size_t JavaFntLengh = strlen(JavaFonts[i].psname);
		if (fntlength == JavaFntLengh) {
			if (strncmp(fontname, JavaFonts[i].psname, fntlength) == 0) {
				return i;
			}
		}
	}
	return 0;					// return Courier if not found
}


drvJAVA::derivedConstructor(drvJAVA):
constructBase
{
#if 0
	if (d_argc > 0) {
		assert(d_argv && d_argv[0]);
		jClassName = d_argv[0];
	}
#endif
// driver specific initializations
// and writing of header to output file
	outf << "import java.applet.*;" << endl;
	outf << "import java.awt.*;" << endl;
	outf << "public class " << options->jClassName.value << " extends PsPages" << endl;
	outf << "{" << endl;
//  outf << "    public void init()" << endl;
//  outf << "    {" << endl;
//  outf << "        setBackground( Color.white );" << endl;
//  outf << "    }" << endl;
}

drvJAVA::~drvJAVA()
{
// driver specific deallocations
// and writing of trailer to output file
	outf << "    public " << options->jClassName.value << "(){" << endl;
	outf << "	pages = new PageDescription[" << currentPageNumber << "];" << endl;
	for (unsigned int i = 0; i < currentPageNumber; i++) {
		outf << "	setupPage_" << i + 1 << "();" << endl;
	}
	outf << "    }" << endl;
	outf << "    public int numberOfPages()" << endl;
	outf << "    {" << endl;
	outf << "    	return " << currentPageNumber << ';' << endl;
	outf << "    }" << endl;
	outf << "}" << endl;
	options=nullptr;
}

void drvJAVA::print_coords()
{
	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		const basedrawingelement & elem = pathElement(n);
		switch (elem.getType()) {
		case moveto:{
				const Point & p = elem.getPoint(0);
				outf << "\tp.addPoint(";
				outf << (int) (p.x() + x_offset) << ","
					<< (int) (currentDeviceHeight - p.y() + y_offset) << ");";
			}
			break;
		case lineto:{
				const Point & p = elem.getPoint(0);
				outf << "\tp.addPoint(";
				outf << (int) (p.x() + x_offset) << ","
					<< (int) (currentDeviceHeight - p.y() + y_offset) << ");";
			}
			break;
		case closepath:
			// outf << "\t\tclosepath ";
			break;
		case curveto:{
				errf << "\t\tFatal: unexpected case in drvjava " << endl;
				abort();
			}
			break;
		default:
			errf << "\t\tFatal: unexpected case in drvjava " << endl;
			abort();
			break;
		}
		outf << endl;
	}
}


void drvJAVA::open_page()
{
	outf << "//Opening page: " << currentPageNumber << endl;
	outf << "    public void setupPage_" << currentPageNumber << "()" << endl;
	outf << "    {" << endl;
	outf << "    PageDescription currentpage = new PageDescription();" << endl;
	outf << "    PSPolygonObject p = null;" << endl;
	outf << "    PSLinesObject   l = null;" << endl;
}

void drvJAVA::close_page()
{
	outf << "//Closing page: " << (currentPageNumber) << endl;
	outf << "    pages[" << currentPageNumber - 1 << "] = currentpage;" << endl;
	outf << "    }" << endl;
}

void drvJAVA::show_text(const TextInfo & textinfo)
{
	const unsigned int javaFontNumber = getFontNumber(textinfo.currentFontName.c_str());
	outf << "\tcurrentpage.theObjects.addElement( new PSTextObject(" << endl;
	outf << "\t\t" << currentR() << "F," << currentG() << "F," << currentB() << "F," << endl;
	outf << "\t\t\"";
	// << textinfo.thetext 
	for (const char *p = textinfo.thetext.c_str(); (*p) != 0; p++) {
		if ((*p) == '"') {
			outf << '\\' << *p;
		} else if ((*p) == '\\') {
			outf << '\\' << *p;
		} else if ((*p) == (char) 13) {	// ^M
			outf << ' ';
		} else {
			outf << *p;
		}
	}
	outf << "\"," << (int) (textinfo.x() +
							x_offset) << "," << (int) (currentDeviceHeight
													   - textinfo.y() + y_offset) << ',' << endl;
#ifdef PASSFONTNAME
	const char *javaFname = JavaFonts[javaFontNumber].javaname;
	const char *javaFstyle = JavaFonts[javaFontNumber].javastyle;
	outf << "\t\t\"" << javaFname << "\"," << javaFstyle
#else
	outf << "\t\t" << javaFontNumber;
#endif
	outf << ',' << (int) (textinfo.currentFontSize + 0.5) << " )\n\t);" << endl;
}

void drvJAVA::show_path()
{
	outf << "\t// Path # " << currentNr() << endl;

// if fill then use a polygon
// else use line-segments.
	switch (currentShowType()) {
	case drvbase::stroke:{
			outf << "\tl = new PSLinesObject(" << endl;
			outf << "\t\t" << currentR() << "F," << currentG() << "F," <<
				currentB() << "F);" << endl;
			for (unsigned int t = 0; t < numberOfElementsInPath(); t++) {
				const Point & p = pathElement(t).getPoint(0);
				outf << "\tl.addPoint(";
				outf << (int) (p.x() + x_offset) << ","
					<< (int) (currentDeviceHeight - p.y() + y_offset) << ");\n ";
			}
			outf << "\tcurrentpage.theObjects.addElement(l);" << endl;
		}
		break;
	case drvbase::fill:
	case drvbase::eofill:{
			outf << "\tp = new PSPolygonObject(";
			outf << currentR() << "F," << currentG() << "F," << currentB()
				<< "F);" << endl;
			print_coords();
			if (!isPolygon()) {
				// make closed polygon anyway
				const basedrawingelement & elem = pathElement(0);
				const Point & p = elem.getPoint(0);
				outf << "\tp.addPoint(";
				outf << (int) (p.x() + x_offset) << ","
					<< (int) (currentDeviceHeight - p.y() + y_offset) << ");\n ";
			}
			outf << "\tcurrentpage.theObjects.addElement(p);" << endl;
		}
		break;
	default:
		// cannot happen
		outf << "unexpected ShowType " << (int) currentShowType();
		break;
	}
	// outf << "\tcurrentLineWidth: " <<  currentLineWidth() << endl;
}


static DriverDescriptionT < drvJAVA > D_java("java1", "java 1 applet source code", "","java", false,	// if backend supports subpaths, else 0
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
											 true,	// if backend supports text, else 0
											 DriverDescription::imageformat::noimage,	// no support for PNG file images
											 DriverDescription::opentype::normalopen, true,	// if format supports multiple pages in one file
											 false /*clipping */ );
