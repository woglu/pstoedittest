/*
   drvJAVA2.cpp : This file is part of pstoedit
   backend to generate a Java(TM) 2 applet -- test version

   Copyright (C) 1993 - 2024 Wolfgang Glunz, wglunz35_AT_pstoedit.net
   Copyright (C) 2000 TapirSoft Gisbert & Harald Selke GbR, gisbert_AT_tapirsoft.de

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
#include "drvjava2.h"
#include I_fstream
#include I_stdio
#include I_stdlib
#include <ctype.h>
#include <memory>

// for fabs and sqrt
#include <math.h>

//lint -esym(754,JavaFontDescriptor::javaname) // not used so far
//lint -esym(754,JavaFontDescriptor::javastyle) // not used so far

struct JavaFontDescriptor {
	const char *psname;
 	const char *javaname;
 	const char *javastyle;
};

static const JavaFontDescriptor JavaFonts[] = {	// predefined Fonts
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

const unsigned int numberOfFonts = sizeof(JavaFonts) / (sizeof(JavaFontDescriptor)) - 1;


static const unsigned int limitNumberOfElements = 1000;


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


drvJAVA2::derivedConstructor(drvJAVA2):
constructBase, subPageNumber(0), numberOfElements(0), numberOfImages(0)
{
#if 0
	if (d_argc > 0) {
		assert(d_argv && d_argv[0]);
		jClassName = d_argv[0];
	}
#endif
// driver specific initializations
// and writing of header to output file
	outf << "// Source of " << options->jClassName.value << " produced by pstoedit, driver for Java 2" << endl;
	outf << "import java.awt.Color;" << endl;
	outf << "import java.awt.geom.*;" << endl;
	outf << endl;
	outf << "public class " << options->jClassName.value << " extends PSDrawing {" << endl;
	outf << endl;
	outf << "  PageDescription currentPage = null;" << endl;
	outf << "  PSPathObject    currentPath = null;" << endl;
	outf << endl;
}

drvJAVA2::~drvJAVA2()
{
// driver specific deallocations
// and writing of trailer to output file
	outf << "  public void init() {" << endl;
	for (unsigned int i = 0; i < currentPageNumber; i++) {
		outf << "    setupPage_" << i + 1 << "();" << endl;
	}
	outf << "    super.init();" << endl;
	outf << "  }" << endl;
	outf << "}" << endl;
	options=nullptr;
}


void drvJAVA2::open_page()
{
	outf << "  // Opening page: " << currentPageNumber << endl;
	outf << "  public void setupPage_" << currentPageNumber << "() {" << endl;
	outf << "    currentPage = new PageDescription();" << endl;
	numberOfElements = 0;
	subPageNumber = 0;
}


void drvJAVA2::close_page()
{
	outf << "    // Closing page: " << (currentPageNumber) << endl;
	outf << "    pspages.add(currentPage);" << endl;
	outf << "  }" << endl;
	outf << endl;
}


void drvJAVA2::continue_page()
{
	subPageNumber++;
	outf << "    // Page to be continued: " << currentPageNumber << endl;
	outf << "    continuePage_" << currentPageNumber << "_" << subPageNumber << "();" << endl;
	outf << "  }" << endl;
	outf << endl;
	outf << "  // Continuing page: " << currentPageNumber << endl;
	outf << "  void continuePage_" << currentPageNumber << "_" << subPageNumber << "() {" << endl;
	numberOfElements = 0;
}


void drvJAVA2::print_coords()
{
	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		if (numberOfElements > limitNumberOfElements)
			continue_page();
		const basedrawingelement & elem = pathElement(n);
		switch (elem.getType()) {
		case moveto:{
				const Point & p = elem.getPoint(0);
				outf << "    currentPath.moveTo(" << (p.x() + x_offset) << "f, " <<
					(currentDeviceHeight - p.y() + y_offset) << "f);";
			}
			break;
		case lineto:{
				const Point & p = elem.getPoint(0);
				outf << "    currentPath.lineTo(" << (p.x() + x_offset) << "f, " <<
					(currentDeviceHeight - p.y() + y_offset) << "f);";
			}
			break;
		case closepath:
			outf << "    currentPath.closePath();";
			break;
		case curveto:{
				outf << "    currentPath.curveTo(";
				outf << (elem.getPoint(0).x() +
						 x_offset) << "f, " << (currentDeviceHeight - elem.getPoint(0).y() + y_offset) << "f, ";
				outf << (elem.getPoint(1).x() +
						 x_offset) << "f, " << (currentDeviceHeight - elem.getPoint(1).y() + y_offset) << "f, ";
				outf << (elem.getPoint(2).x() +
						 x_offset) << "f, " << (currentDeviceHeight - elem.getPoint(2).y() + y_offset) << "f);";
			}
			break;
		default:
			errf << "\t\tFatal: unexpected case for elem.getType() in drvjava2" << endl;
			abort();
			break;
		}
		outf << endl;
		numberOfElements++;
	}
}


void drvJAVA2::show_text(const TextInfo & textinfo)
{
	if (numberOfElements > limitNumberOfElements)
		continue_page();
	const unsigned int javaFontNumber = getFontNumber(textinfo.currentFontName.c_str());
	outf << "    currentPage.add(new PSTextObject(new Color(";
	outf << currentR() << "f, " << currentG() << "f, " << currentB() << "f)," << endl;
	outf << "      \"";
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
	outf << "\"," << endl;
	outf << "      " << (textinfo.x() +
						 x_offset) << "f, " << (currentDeviceHeight - textinfo.y() + y_offset) << "f";
#ifdef PASSFONTNAME
	const char *javaFname = JavaFonts[javaFontNumber].javaname;
	const char *javaFstyle = JavaFonts[javaFontNumber].javastyle;
	outf << ", \"" << javaFname << "\", " << javaFstyle;
#else
	outf << ", " << javaFontNumber;
#endif
	const float *CTM = getCurrentFontMatrix();
	if ((fabs(pythagoras(CTM[0], CTM[1] ) - textinfo.currentFontSize) < 1e-5)
		&& (fabs(pythagoras(CTM[2] ,CTM[3] ) - textinfo.currentFontSize) < 1e-5)
		&& (CTM[0] * CTM[3] - CTM[1] * CTM[2] >= 0)) {
		outf << ", " << textinfo.currentFontSize << "f";
		if (textinfo.currentFontAngle) {
			outf << ", " << textinfo.currentFontAngle << "f";
		}
	} else {
		outf << ", new AffineTransform(" << CTM[0] << "f, " << (-CTM[1]) << "f, ";
		outf << (-CTM[2]) << "f, " << CTM[3] << "f, 0f, 0f)";
	}
	outf << "));" << endl;
	numberOfElements++;
}


static void show_dashPattern(ostream & outf, const char *dashPatternString)
{
	const char *p = dashPatternString;
	int state = 0;
	outf << "      new float[] {";
	while ((*p) && (isspace(*p) || (*p == '[')))
		p++;
	while (*p) {
		if (isspace(*p)) {
			if (state == 2)
				state = 1;
		} else {
			if (*p == ']') {
				state = 3;
			} else {
				if (state == 1)
					outf << "f, ";
				if (state == 3)
					outf << "f}, ";
				outf << *p;
				state = 2;
			}
		}
		p++;
	}
	outf << "f";
}


void drvJAVA2::show_path()
{
	outf << "    // Path # " << currentNr() << endl;
	outf << "    currentPath = new PSPathObject(new Color(";
	outf << currentR() << "f, " << currentG() << "f, " << currentB() << "f), ";
	outf << currentLineWidth() << "f";
	if ((currentLineCap() != 0) || (currentLineJoin() != 0)
		|| (currentShowType() != 0) || (currentLineType() != solid)) {
		outf << ", " << currentLineCap() << ", " << currentLineJoin() <<
			", " << currentMiterLimit() << "f, ";
		switch (currentShowType()) {
		case drvbase::stroke:
			outf << "0";
			break;
		case drvbase::fill:
			outf << "1";
			break;
		case drvbase::eofill:
			outf << "2";
			break;
		default:
			errf << "\t\tFatal: unexpected case for currentShowType() in drvjava2" << endl;	// cannot happen
			abort();
		}
		if (currentLineType() != solid) {
			outf << "," << endl;
			show_dashPattern(outf, dashPattern());
		}
	}
	if (isPolygon()) {
		outf << ", true";
	}
	outf << ");" << endl;
	numberOfElements++;
	print_coords();
	outf << "    currentPage.add(currentPath);" << endl;
	numberOfElements++;
}


void drvJAVA2::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{
	if (numberOfElements > limitNumberOfElements)
		continue_page();
	outf << "    // Path # " << currentNr() << endl;
	outf << "    currentPage.add(new PSPathObject(new Color(";
	outf << currentR() << "f, " << currentG() << "f, " << currentB() << "f), ";
	outf << currentLineWidth() << "f";
	if ((currentLineJoin() != 0) || (currentShowType() != 0)) {
		outf << ", " << currentLineJoin();
		switch (currentShowType()) {
		case drvbase::stroke:
			outf << ", 0";
			break;
		case drvbase::fill:
			outf << ", 1";
			break;
		case drvbase::eofill:
			outf << ", 2";
			break;
		default:
			errf << "\t\tFatal: unexpected case for currentShowType() in drvjava2" << endl;	// cannot happen
			abort();
		}
	}
	if (currentLineType() != solid) {
		outf << "," << endl;
		show_dashPattern(outf, dashPattern());
	}
	outf << ", new Rectangle2D.Float(" << (llx + x_offset) << "f, " <<
		(currentDeviceHeight - ury + y_offset) << "f";
	outf << ", " << (urx - llx) << "f, " << (ury - lly) << "f)));" << endl;
	numberOfElements++;
}


void drvJAVA2::show_image(const PSImage & imageinfo)
{
	if (outBaseName == "") {
		errf << "images cannot be handled via standard output. Use an output file" << endl;
		return;
	}
	// write image data to separate file
	const size_t sizefilename = strlen(outBaseName.c_str()) + 21;
	std::unique_ptr<char[]> imgOutFileName ( new char[sizefilename]);
	const size_t sizefullfilename = strlen(outDirName.c_str()) + strlen(outBaseName.c_str()) + 21;
	std::unique_ptr<char[]> imgOutFullFileName ( new char[sizefullfilename]);

	sprintf_s(TARGETWITHLEN(imgOutFileName.get(),sizefilename), "%s_%u.img", outBaseName.c_str(), numberOfImages);
	sprintf_s(TARGETWITHLEN(imgOutFullFileName.get(),sizefullfilename), "%s%s", outDirName.c_str(), imgOutFileName.get());
	outf << "    currentPage.add(new PSImageObject(" << imageinfo.
		width << ", " << imageinfo.height << ", ";
	outf << imageinfo.bits << ", " << imageinfo.ncomp << ", ";
	switch (imageinfo.type) {
	case ImageType::colorimage:
		outf << "0, ";
		break;
	case ImageType::normalimage:
		outf << "1, ";
		break;
	case ImageType::imagemask:
		outf << "2, ";
		break;
	default:
		errf << "\t\tFatal: unexpected case for imageinfo.type in drvjava2" << endl;	// cannot happen
		abort();
	}
	outf << (imageinfo.polarity ? "true" : "false") << "," << endl;
	outf << "      new AffineTransform(";
	outf << (imageinfo.normalizedImageCurrentMatrix[0]) << "f, ";
	outf << (-imageinfo.normalizedImageCurrentMatrix[1]) << "f, ";
	outf << (imageinfo.normalizedImageCurrentMatrix[2]) << "f, ";
	outf << (-imageinfo.normalizedImageCurrentMatrix[3]) << "f, ";
	outf << (imageinfo.normalizedImageCurrentMatrix[4]) << "f, ";
	outf << (currentDeviceHeight - imageinfo.normalizedImageCurrentMatrix[5]) << "f), " << endl;
	outf << "      \"" << imgOutFileName.get() << "\"));" << endl;
	FILE *outFile;
	const auto outfile_err = fopen_s(&outFile, imgOutFileName.get(), "wb");
	if (outfile_err) {
		errf << "ERROR: cannot open image file " << imgOutFileName.get() << endl;
		return;
	}
	if (fwrite(imageinfo.data, 1, imageinfo.nextfreedataitem, outFile) !=
		imageinfo.nextfreedataitem) {
		fclose(outFile);
		errf << "ERROR: cannot write image data to " << imgOutFileName.get() << endl;
		return;
	}
	fclose(outFile);
	numberOfImages++;
	numberOfElements++;
}


static DriverDescriptionT < drvJAVA2 > D_java2("java2", "java 2 source code", "","java2", true,	// if backend supports subpaths, else 0
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
											   true,	// backend supports curves
											   false,	// backend does not support elements with both fill and edges
											   true,	// backend supports text
											   DriverDescription::imageformat::memoryeps,	// no support for PNG file images
											   DriverDescription::opentype::normalopen, true,	// format supports multiple pages in one file
											   false /*clipping */ );
