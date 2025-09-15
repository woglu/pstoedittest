/* 
   drvTGIF.cpp : This file is part of pstoedit
   Backend for TGIF

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

#include "drvtgif.h"
#include I_fstream
#include I_stdio
#include I_stdlib

// for sin and cos
#include <math.h>
#include "pstoedit_config.h"


static const char *colorstring(float r, float g, float b)
{
	static char buffer[10];
	sprintf_s(TARGETWITHLEN(buffer,10), "%s%.2x%.2x%.2x", "#", (unsigned int) (r * 255),
			(unsigned int) (g * 255), (unsigned int) (b * 255));
	return buffer;
}

const float tgifscale = 128.0f / 72.0f;

drvTGIF::derivedConstructor(drvTGIF):
constructBase, buffer(tempFile.asOutput()), objectId(1)
{
//  magnification(theMagnification)
	// set tgif specific values
//no longer 	scale = tgifscale * scale;	/* tgif internal scale factor */

//no more it will be overwritten by makeedit.l 
//	currentDeviceHeight = 792.0f * tgifscale; 

	x_offset = 0.0f;			/* set to fit to tgif page    */
	y_offset = 89.61f;      	/*          "                 */

	// cannot write any header part, since we need the total number of pages
	// in the header
	if (Verbose())
		errf << "% Driver options:" << endl;
#if 0
	for (unsigned int i = 0; i < d_argc; i++) {
		assert(d_argv && d_argv[i]);
		if (Verbose())
			errf << "% " << d_argv[i] << endl;
		if (strcmp(d_argv[i], "-ta") == 0) {
			textAsAttribute = true;
		}
	}
#endif
}

drvTGIF::~drvTGIF()
{
	outf << "% TGIF" << endl;
	outf << "state(0,33," << 100	// (int) ((scale / tgifscale )* 100) 
		<<
		",0,0,1,16,1,9,1,1,0,0,1,0,1,0,'Courier',0,17,0,0,1,5,0,0,1,1,0,16,1,0,1,"
		<< currentPageNumber << ",1,0,1056,1497,0,0,2880)." << endl;
	outf << "unit(\"1 pixel/pixel\")." << endl;
	outf << "generated_by(\"pstoedit\",0,\"" << drvbase::VersionString() << "\")." << endl;
	// now we can copy the buffer the output
	ifstream & inbuffer = tempFile.asInput();
	copy_file(inbuffer, outf);
	options=nullptr;
}

void drvTGIF::print_coords()
{
	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		const basedrawingelement & elem = pathElement(n);
		switch (elem.getType()) {
		case lineto:
		case moveto:{
				const Point & p = pathElement(n).getPoint(0);
				buffer << p.x()*tgifscale + x_offset;
				buffer << "," << currentDeviceHeight* tgifscale - p.y()*tgifscale + y_offset;
				if (n != numberOfElementsInPath() - 1) {
					// it is not the last point, so add a ,
					buffer << ',';
				}
				if ((!((n + 1) % 8))
					&& ((n + 1) != numberOfElementsInPath())) {
					buffer << "\n\t";
				}
			}
			break;
		case closepath:{
				const Point & p = pathElement(0).getPoint(0);
				buffer << p.x()*tgifscale + x_offset;
				buffer << "," << currentDeviceHeight* tgifscale - p.y()*tgifscale + y_offset;
				if (n != numberOfElementsInPath() - 1) {
					// it is not the last point, so add a ,
					buffer << ',';
				}
				if ((!((n + 1) % 8))
					&& ((n + 1) != numberOfElementsInPath())) {
					buffer << "\n\t";
				}
			}
			break;
		case curveto:
		default:
			errf << "\t\tFatal: unexpected case in drvtgif " << endl;
			abort();
			break;

		}
	}
}
void drvTGIF::close_page()
{
}

void drvTGIF::open_page()
{
	buffer << "page(" << currentPageNumber << ",\"\",1).\n";
}

static const int Fill = 1;
static const int noFill = 0;
void drvTGIF::show_text(const TextInfo & textinfo)
{
	if (options->textAsAttribute) {
//  buffer << "box('magenta',150,50,250,100,0,1,1,22,0,0,0,0,0,'1',[\n"
		buffer << "box('" << colorstring(textinfo.currentR,
										 textinfo.currentG, textinfo.currentB) << "'";
		buffer << "," << (textinfo.x()*tgifscale) + x_offset;	// llx
		buffer << "," << currentDeviceHeight* tgifscale - (textinfo.y_end()*tgifscale) + y_offset - textinfo.currentFontSize*tgifscale;	// lly
		buffer << "," << textinfo.x_end()*tgifscale + x_offset;	//urx
		buffer << "," << currentDeviceHeight* tgifscale - textinfo.y()*tgifscale + y_offset;	//ury
		const int fillpat = noFill;
		buffer << "," << fillpat << "," << 1	// currentLineWidth() 
			<< "," << Fill << "," << objectId++ << ",0,0,0,0,0,'1',[\n" << "attr(\"href=\", \"";
		const char *cp = textinfo.thetext.c_str();
		while (*cp) {
			if ((*cp == '"') || (*cp == '\\')) {
				buffer << '\\';
			}
			buffer << *cp;
			cp++;
		}
		buffer << "\", 1, 1, 0,\n";
	}
	buffer << "text('" << colorstring(textinfo.currentR, textinfo.currentG,
									  textinfo.currentB) << "'";
	buffer << "," << textinfo.x()*tgifscale + x_offset;
#ifdef OLDTGIF
// for version 3
	buffer << "," << currentDeviceHeight* tgifscale - textinfo.y + y_offset - textinfo.currentFontSize;
	// TGIF's origin of text is at the top line, pstoedit's at the bottom
#else
	buffer << "," << currentDeviceHeight* tgifscale - textinfo.y()*tgifscale + y_offset - textinfo.currentFontSize*tgifscale;
#endif
	buffer << ",'" << textinfo.currentFontName.c_str() << "'";
	const bool boldfont = (strstr(textinfo.currentFontName.c_str(), "Bold") != nullptr);
	const bool italicfont = ((strstr(textinfo.currentFontName.c_str(), "Italic") != nullptr)
							 || (strstr(textinfo.currentFontName.c_str(), "Oblique") != nullptr));
	int fonttype = 0;
	if (boldfont) {
		if (italicfont)
			fonttype = 3;
		else
			fonttype = 1;
	} else {
		if (italicfont)
			fonttype = 2;
		else
			fonttype = 0;
	}
	const float fontSize = textinfo.currentFontSize * tgifscale ;
	buffer << "," << fonttype << "," << (int) (fontSize +
											   0.5) << ",1,0,0,1,70," <<
		fontSize << "," << objectId++ << ",0,14,3,0,0,0,0,0,0,0,0,0,0,\"\",0,";

#ifdef OLDTGIF
	const bool rotated = true;	// wogl textinfo.currentFontAngle != 0.0;
	if (rotated) {
		buffer << "1,0,[" << endl;
	} else {
		buffer << "0,0,[" << endl;
	}
	if (rotated) {
#else
	bool transformed = false;

	if (fontSize != 0.0) {
		const float *CTM = getCurrentFontMatrix();

		transformed = !(fabs(CTM[0]*tgifscale - fontSize) < (1.0e-5) &&
			fabs(CTM[1]) < (1.0e-5) &&
			fabs(CTM[2]) < (1.0e-5) && fabs(CTM[3]*tgifscale - fontSize) < (1.0e-5)); 
			// no rotation and no shear
	}
	if (transformed) {
		buffer << "1,0,[" << endl;
	} else {
		buffer << "0,0,[" << endl;
	}
	if (transformed) {
#endif
//#define TGIFDEBUG
#ifdef TGIFDEBUG
		const float toRadians = 3.14159265359f / 180.0f;
		const float angleInRadians = textinfo.currentFontAngle * toRadians;
		const float cosphi = (float) cos(angleInRadians);
		const float sinphi = (float) sin(angleInRadians);
//  const float Sx = textinfo.currentFontSize;
//  const float Sy = textinfo.currentFontSize;
#endif
//  const int len = strlen(textinfo.thetext.c_str());
// if we use the tx of CTM  buffer << '\t'<< 0.0 ;
// if we use the ty of CTM  buffer << "," << 0.0 ;
		buffer << '\t' << textinfo.x()*tgifscale + x_offset;
		buffer << "," << currentDeviceHeight* tgifscale - textinfo.y()*tgifscale + y_offset;

		// the obbox stuff
#ifdef OLDTGIF
		buffer << "," << textinfo.x() + x_offset;
		buffer << "," << currentDeviceHeight* tgifscale - textinfo.y() + y_offset;
		buffer << "," << textinfo.x() + x_offset + len * textinfo.currentFontSize;
		buffer << "," << currentDeviceHeight* tgifscale - textinfo.y() + textinfo.currentFontSize + y_offset;
#else
		// starting with tgif 3.0 pl 7. an all 0 bounding box causes tgif to recalculate it
		buffer << "," << 0;
		buffer << "," << 0;
		buffer << "," << 0;
		buffer << "," << 0;
#endif

		const float *CTM = getCurrentFontMatrix();
#ifdef TGIFDEBUG
		for (int i = 0; i < 6; i++) {
			buffer << " CTM [" << i << "] = " << CTM[i] << endl;
		}
		const float SX = pythagoras(CTM[0] , CTM[1] );
		const float SY = pythagoras(CTM[2] , CTM[3] );
		buffer << " SX " << SX << endl;
		buffer << " SY " << SY << endl;
#endif
		// the CTM stuff 
		buffer << "," << (CTM[0] / textinfo.currentFontSize) * 1000;
		buffer << "," << (-1.0 * CTM[1] / textinfo.currentFontSize) * 1000;
		buffer << "," << (-1.0 * CTM[2] / textinfo.currentFontSize) * 1000;
		buffer << "," << (CTM[3] / textinfo.currentFontSize) * 1000;

#ifdef TGIFDEBUG
		buffer << "," << cosphi * 1000;
		buffer << "," << -sinphi * 1000;
		buffer << "," << sinphi * 1000;
		buffer << "," << cosphi * 1000;
#endif
//  buffer << "," << (CTM[4] +x_offset  ) * 1;
//  buffer << "," << (currentDeviceHeight* tgifscale - CTM[5] +y_offset ) * 1;
		buffer << "," << 0;		// no translate
		buffer << "," << 0;		// no translate

		// the bbox stuff
#ifdef OLDTGIF
		buffer << "," << textinfo.x() + x_offset;
		buffer << "," << currentDeviceHeight* tgifscale - textinfo.y()*tgifscale + y_offset;
		buffer << "," << textinfo.x() + x_offset + len * textinfo.currentFontSize;
		buffer << "," << currentDeviceHeight* tgifscale - textinfo.y() + y_offset +
			textinfo.currentFontSize + y_offset;
#else
		// starting with tgif 3.0 pl 7. an all 0 bounding box causes tgif to recalculate it
		buffer << "," << 0;
		buffer << "," << 0;
		buffer << "," << 0;
		buffer << "," << 0;
#endif

		buffer << "],[" << endl;
	}
	buffer << "\t\"";
	const char *cp = textinfo.thetext.c_str();
	while (*cp) {
		if ((*cp == '"') || (*cp == '\\')) {
			buffer << '\\';
		}
		buffer << *cp;
		cp++;
	}
	if (options->textAsAttribute) {
		buffer << "\"]))\n]).\n";
	} else {
		buffer << "\"]).\n";
	}
}

void drvTGIF::show_path()
{
//     buffer << "# Path " << currentNr() << " " << numberOfElementsInPath() << endl;
	const int fillpat = (currentShowType() == drvbase::stroke) ? noFill : Fill;
	if (isPolygon()) {
		buffer << "polygon('" << colorstring(currentR(), currentG(), currentB())
			<< "'," << numberOfElementsInPath()
			<< ",[" << endl << "\t";
		print_coords();
		buffer << "]," << fillpat << "," << currentLineWidth()* tgifscale
			<< "," << Fill << ",0," << objectId++ << ",0,0,0,0,0,'";
//   << ",0,0,0,0,0,'1"; // bug fix 
/*	I think the 1 at the end has to be deleted.
         << ",0,0,0,0,0,'";

This error resulted for polygons in lines that where too wide.
This was however not visible in tgif, because (i think) tgif
doesn't allow lines to be wider than 7.
It became visible after printing in tgif to postscript or eps.
*/
		buffer << (int) (currentLineWidth()* tgifscale + 0.5);
		buffer << "',\n    \"";
		for (unsigned int i = 0; i < numberOfElementsInPath(); i = i + 4) {
			if ((i > 0) && ((i) % 256) == 0) {
				buffer << "\n     ";
			}
			buffer << '0';
		}
		buffer << "\",[" << endl << "])." << endl;
	} else {
		buffer << "poly('" << colorstring(currentR(), currentG(), currentB())
			<< "'," << numberOfElementsInPath()
			<< ",[" << endl << "\t";
		print_coords();
		buffer << "],0," << currentLineWidth()* tgifscale
			<< "," << Fill << "," << objectId++ << ",0," << fillpat << ",0,0,0,3,0,0,0,'";
		buffer << (int) (currentLineWidth()* tgifscale + 0.5);
		buffer << "','8','3',\n    \"";
		for (unsigned int i = 0; i < numberOfElementsInPath(); i = i + 4) {
			if ((i > 0) && ((i) % 256) == 0) {
				buffer << "\n     ";
			}
			buffer << '0';
		}
		buffer << "\",[" << endl << "])." << endl;
	}
}

void drvTGIF::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{
	buffer << "box('" << colorstring(currentR(), currentG(), currentB()) << "'";
	buffer << "," << llx*tgifscale + x_offset;
	buffer << "," << currentDeviceHeight* tgifscale - lly*tgifscale + y_offset;
	buffer << "," << urx*tgifscale + x_offset;
	buffer << "," << currentDeviceHeight* tgifscale - ury*tgifscale + y_offset;
	const int fillpat = (currentShowType() == drvbase::stroke) ? noFill : Fill;
	buffer << "," << fillpat << "," << currentLineWidth()*tgifscale
		<< "," << Fill << "," << objectId++ << ",0,0,0,0,0,'1',[" << endl << "])." << endl;
}

static DriverDescriptionT < drvTGIF > D_tgif("tgif", "Tgif .obj format", "","obj", false,	// if backend supports subpaths, else 0
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
											 true,	// if backend supports elements with fill and edges
											 true,	// if backend supports text, else 0
											 DriverDescription::imageformat::noimage,	// no support for PNG file images
											 DriverDescription::opentype::normalopen, true,	// if format supports multiple pages in one file
											 false  /*clipping */ 
											 );
