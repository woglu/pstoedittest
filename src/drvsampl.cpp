/*
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
#include "drvsampl.h"
#include I_fstream
#include I_stdio
#include I_stdlib



drvSAMPL::derivedConstructor(drvSAMPL):
//(const char * driveroptions_p,ostream & theoutStream,ostream & theerrStream): // Constructor
constructBase //, imgcount(0)
{
// driver specific initializations
// and writing of header to output file
	outf << "Sample header \n";
//  float           scale;
//  float           x_offset;
//  float           y_offset;
}

drvSAMPL::~drvSAMPL()
{
// driver specific deallocations
// and writing of trailer to output file
	outf << "Sample trailer \n";
	options=nullptr;
}

void drvSAMPL::print_coords()
{
	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		const basedrawingelement & elem = pathElement(n);
		switch (elem.getType()) {
		case moveto:{
				const Point & p = elem.getPoint(0);
				outf << "\t\tmoveto ";
				outf << p.x() + x_offset << " " << /*   currentDeviceHeight -  */ p.y() +
					y_offset << " ";
			}
			break;
		case lineto:{
				const Point & p = elem.getPoint(0);
				outf << "\t\tlineto ";
				outf << p.x() + x_offset << " " << /*   currentDeviceHeight -  */ p.y() +
					y_offset << " ";
			}
			break;
		case closepath:
			outf << "\t\tclosepath ";
			break;
		case curveto:{
				outf << "\t\tcurveto ";
				for (unsigned int cp = 0; cp < 3; cp++) {
					const Point & p = elem.getPoint(cp);
					outf << (p.x() + x_offset) << " " << /*   currentDeviceHeight -  */ (p.y() +
																						y_offset) <<
						" ";
				}
			}
			break;
		default:
			errf << "\t\tFatal: unexpected case in drvsample " << endl;
			abort();
			break;
		}
		outf << endl;
	}
}


void drvSAMPL::open_page()
{
	outf << "Opening page: " << currentPageNumber << endl;
}

void drvSAMPL::close_page()
{
	outf << "Closing page: " << (currentPageNumber) << endl;
}

void drvSAMPL::show_text(const TextInfo & textinfo)
{
	outf << "Text String : " << textinfo.thetext.c_str() << endl;
	outf << '\t' << "X " << textinfo.x() << " Y " << textinfo.y() << endl;
	outf << '\t' << "X_END " << textinfo.x_end() << " Y_END " << textinfo.y_end() << endl;
	outf << '\t' << "currentFontName: " << textinfo.currentFontName.c_str() << endl;
	outf << '\t' << "is_non_standard_font: " << textinfo.is_non_standard_font << endl;
	outf << '\t' << "currentFontFamilyName: " << textinfo.currentFontFamilyName.c_str() << endl;
	outf << '\t' << "currentFontFullName: " << textinfo.currentFontFullName.c_str() << endl;
	outf << '\t' << "currentFontWeight: " << textinfo.currentFontWeight.c_str() << endl;
	outf << '\t' << "currentFontSize: " << textinfo.currentFontSize << endl;
	outf << '\t' << "currentFontAngle: " << textinfo.currentFontAngle << endl;
	outf << '\t' << "glyphnames: " << textinfo.glyphnames << endl;
	outf << '\t' << "currentR: " << textinfo.currentR << endl;
	outf << '\t' << "currentG: " << textinfo.currentG << endl;
	outf << '\t' << "currentB: " << textinfo.currentB << endl;
	const float *CTM = getCurrentFontMatrix();
	outf << '\t' << "currentFontMatrix: [";
	for (int i = 0; i < 6; i++) {
		outf << " " << CTM[i];
	}
	outf << ']' << endl;
}

void drvSAMPL::show_path()
{
	outf << "Path # " << currentNr();
	if (isPolygon())
		outf << " (polygon): " << endl;
	else
		outf << " (polyline): " << endl;
	outf << "\tcurrentShowType: ";
	switch (currentShowType()) {
	case drvbase::stroke:
		outf << "stroked";
		break;
	case drvbase::fill:
		outf << "filled";
		break;
	case drvbase::eofill:
		outf << "eofilled";
		break;
	default:
		// cannot happen
		outf << "unexpected ShowType " << (int) currentShowType();
		break;
	}
	outf << endl;
	outf << "\tcurrentLineWidth: " << currentLineWidth() << endl;
	outf << "\tcurrentR: " << currentR() << endl;
	outf << "\tcurrentG: " << currentG() << endl;
	outf << "\tcurrentB: " << currentB() << endl;
	outf << "\tedgeR:    " << edgeR() << endl;
	outf << "\tedgeG:    " << edgeG() << endl;
	outf << "\tedgeB:    " << edgeB() << endl;
	outf << "\tfillR:    " << fillR() << endl;
	outf << "\tfillG:    " << fillG() << endl;
	outf << "\tfillB:    " << fillB() << endl;
	outf << "\tcurrentLineCap: " << currentLineCap() << endl;
	outf << "\tdashPattern: " << dashPattern() << endl;
	outf << "\tPath Elements 0 to " << numberOfElementsInPath() - 1 << endl;
	print_coords();
}

void drvSAMPL::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{
	outf << "Rectangle ( " << llx << "," << lly << ") (" << urx << "," <<
		ury << ") equivalent to:" << endl;
// just do show_path for a first guess
	show_path();
}
void drvSAMPL::show_image(const PSImage & imageinfo)
{
	if (outBaseName == "") {
		errf << "images cannot be handled via standard output. Use an output file" << endl;
		return;
	}
	if (imageinfo.isFileImage) {
		outf << "<image "		 
			<< " transform=\"matrix("
			<< imageinfo.normalizedImageCurrentMatrix[0] << ' '
			<< -imageinfo.normalizedImageCurrentMatrix[1] << ' '
			<< imageinfo.normalizedImageCurrentMatrix[2] << ' '
			<< -imageinfo.normalizedImageCurrentMatrix[3] << ' '
// transfer
			<< imageinfo.normalizedImageCurrentMatrix[4] << ' '
			<< currentDeviceHeight - imageinfo.normalizedImageCurrentMatrix[5]
			<< ")\"" 
			<< " width=\"" << imageinfo.width << "\"" 
			<< " height=\"" << imageinfo.height << "\"" 
			<< " xlink:href=\"" << imageinfo.FileName << "\"></image>" << endl;

	} else {
		assert (0 && "should not happen since drivers supports PNG file images");
#if 0
	char *PNGoutFullFileName = new char[strlen(outDirName) + strlen(outBaseName) + 21];
	sprintf(PNGoutFullFileName, "%s%s%02d.png", outDirName, outBaseName, imgcount++);

	outf << PNGoutFullFileName << endl;
//  cout << outBaseName << endl;
//  cout << outDirName << endl;
	char *title = new char[strlen(inFileName.c_str()) + 100];
	sprintf(title, "raster image number %d from input file %s", imgcount, inFileName.c_str());
	char generator[100];
	sprintf(generator, "pstoedit version: %s", version);
	imageinfo.writePNGImage(PNGoutFullFileName, inFileName.c_str(), title, generator);

	delete[]title;
	delete[]PNGoutFullFileName;


	outf << "Image:\n";
	outf << "\ttype: ";
	switch (imageinfo.type) {
	case colorimage:
		outf << " colorimage\n";
		break;
	case normalimage:
		outf << " normalimage\n";
		break;
	case imagemask:
		outf << " imagemask\n";
		break;
	default:
		break;
	}
	outf << "\theight: " << imageinfo.height << endl;
	outf << "\twidth: " << imageinfo.width << endl;
	outf << "\tbits/component: " << imageinfo.bits << endl;
	outf << "\tnumber of color components: " << imageinfo.ncomp << endl;
	outf << "\timageMatrix: ";
	{
		for (unsigned int i = 0; i < 6; i++) {
			outf << imageinfo.imageMatrix[i] << ' ';
		}
	}
	outf << endl;
	outf << "\tnormalizedImageCurrentMatrix: ";
	{
		for (unsigned int i = 0; i < 6; i++) {
			outf << imageinfo.normalizedImageCurrentMatrix[i] << ' ';
		}
	}
	outf << endl;
	outf << "\tpolarity: " << imageinfo.polarity << endl;
	{
		for (unsigned int i = 0; i < imageinfo.nextfreedataitem; i++) {
			if (!(i % 8))
				outf << "\n\t " << i << ":\t";
			outf << (int) imageinfo.data[i] << ' ';
		}
	}
	outf << endl;

#endif
	}
}

static DriverDescriptionT < drvSAMPL > D_sampl("sample", "sample driver", "This is a long description for the sample driver","sam", true,	// backend supports subpaths
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
											   true,	// backend supports text
											   DriverDescription::imageformat::png,	// support for PNG file images
											   DriverDescription::opentype::normalopen, true,	// if format supports multiple pages in one file
											   true  /*clipping */ 
											   );
