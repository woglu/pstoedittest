/* 
   drvHPGL.cpp : This file is part of pstoedit
   HPGL / HPGL2 Device driver supporting text commands

   Copyright (C) 1993 - 2001 Peter Katzmann p.katzmann_AT_thiesen.com
   Copyright (C) 2001  Peter Kuhlemann kuhlemannp_AT_genrad.com
   Copyright (C) 2002 - 2003 Peter Kuhlemann peter.kuhlemann_AT_teradyne.com
   Copyright (C) 2000 - 2024 Glunz (fill support,  improved color handling)
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

  
	Aug 21, 2001: Donald Arseneau (asnd_AT_triumf.ca) adds pen changes when color changes.
    Changed sections are delimited by:
    //  Start DA hpgl color addition
    //  End DA hpgl color addition

for more info about HPGL see: 
http://lprng.sourceforge.net/DISTRIB/RESOURCES/HPGL_short_summary.html
http://www.multicam.com/main/mat/hpgl.htm
 
http://www.piclist.com/techref/language/hpgl.htm
http://www.piclist.com/techref/language/hpgl/commands.htm
 

*/

#define USESPRINTF 1
#include "drvhpgl.h"
#include <math.h>
#include I_stdio



//version up to 3.50 static const float HPGLScale = 10.0f;
// see comments below in open-page area.
static const float HPGLScale = 1016.0f/72.0f;



void drvHPGL::rot(double &x, double &y, int angle)
{
	double tmp;
	switch (angle) {
	case 90:
		tmp = x;
		x = -y;
		y = tmp;
		break;
	case 180:
		x = -x;
		y = -y;
		break;
	case 270:
		tmp = x;
		x = y;
		y = -tmp;
		break;
	default:
		break;
	}
}

drvHPGL::derivedConstructor(drvHPGL):
constructBase, 
	//  Start DA hpgl color addition
	prevColor(0),
	maxPen(0),
	currentPen(0),
	penColors(nullptr)

	//  End DA hpgl color addition
{
	// driver specific initializations
	// and writing of header to output file

	if (strcmp(driverdesc.symbolicname, "pcl") == 0) {
		options->hpgl2 = true;
	}
	if (options->rot90) rotation = 90; else 
		if (options->rot180) rotation = 180; else 
			if (options->rot270) rotation = 270; else rotation = 0;

	//	if (0) {
	//		const char esc = (char) 27;
	//		outf << esc << ".(;";
	//	}

	errf << "Info: This HPGL/PCL driver is not very elaborated - consider using -f plot-hpgl or plot-pcl instead."
		<< endl;

	// will be written by open_page() 	outf << "IN;SC;PU;PU;SP1;LT;VS" << (int) HPGLScale << "\n";

	// read pen colors from file.
	if (options->pencolorsfromfile) {
		if (drvbase::pstoeditDataDir() != "") {
			// *p = '\0';
			RSString test(drvbase::pstoeditDataDir());
			test += directoryDelimiter;
			test += "drvhpgl"; // options.drivername.value;
			test += ".pencolors";
			//	errf << "testing for :" << test.c_str() << endl;
			if (fileExists(test.c_str())) {
				if (Verbose()) {
					errf <<
						"loading pen colors from " << test.c_str() << endl;
				}
				const unsigned int numberofpens = readPenColors(errf, test.c_str(),true);
				penColors = new HPGLColor[numberofpens];
				const HPGLColor initvalue = {0.0f,0.0f,0.0f,0};
				for (unsigned int p = 0; p < numberofpens; p++) {
					penColors[p] = initvalue;
				}
				maxPen = numberofpens;
				(void) readPenColors(errf, test.c_str(),false);
				if (Verbose() ){ errf << "read " << numberofpens << " colors from file " << test.c_str() << endl; }
			}
			else {
				errf << "could not read pen colors from file - " << test.c_str() << " does not exist" << endl; 
			}
		} else {
			errf << "could not read pen colors from file - pstoedit Data Directory is unknown" << endl; 
		}
	} else {
		penColors = new HPGLColor[options->maxPenColors + 1 + 1];	// 1 offset - 0 is not used // one more for flint ;-)
		const HPGLColor initvalue = {0.0f,0.0f,0.0f,0};
		for (unsigned int p = 0; p <= (unsigned int) options->maxPenColors + 1; p++) {
			penColors[p] = initvalue;
		}
	}
}

drvHPGL::~drvHPGL()
{
	// driver specific deallocations
	// and writing of trailer to output file
// should be done either here or in close_page      	outf << "PU;PA0,0;SP;EC;PG1;EC1;OE\n";
	// fillinstruction = nullptr;
	delete [] penColors;
	penColors = nullptr;
	options = nullptr;
}


static constexpr unsigned int intColor(float R, float G, float B) {
	return  256 * (unsigned int) (R * 16) +	16 * (unsigned int) (G * 16) + (unsigned int) (B * 16);
}


unsigned int drvHPGL::readPenColors(ostream & errstream, const char *filename, bool justcount)
{
	if (!fileExists(filename)) {
		errstream << "Could not open font map file " << filename << endl;
		return 0;
	}
	ifstream infile(filename);
	int count = 0;
	unsigned int penid;
	char c;
	while (!infile.eof() ) {
		infile >> penid;
		if (!infile) {
			infile.clear();
			infile >> c;
		//	cout << "read char " << c << endl;
			if (c == '#') {
				// skip chars till EOL
				infile.ignore(256,'\n');
			}
		} else {
		//	cout << "read float " << penid << endl;
			float R,G,B;
			// simple parsing: when reading first float, we expect 3 additional ones
			infile >> R >> G >> B ;
			if (!justcount) {
				if (penid < maxPen) {
					penColors[penid].R=R;
					penColors[penid].G=G;
					penColors[penid].B=B;
					penColors[penid].intColor = intColor(R,G,B);
				} else {
					errf << "error in pen color file: Pen ID too high - " << penid << endl;
				}
			}
			count++;
		}
	}
	return count;
}


void drvHPGL::print_coords()
{
	const unsigned int elems = numberOfElementsInPath();
	if (elems) {
		for (unsigned int n = 0; n < elems; n++) {
			const basedrawingelement & elem = pathElement(n);
			switch (elem.getType()) {
			case moveto:
				{
					const Point & p = elem.getPoint(0);
					double x = (p.x() + x_offset) * HPGLScale;
					double y = (p.y() + y_offset) * HPGLScale;
					rot(x, y, rotation);
#if USESPRINTF
					char str[256];
					sprintf_s(TARGETWITHLEN(str,256), "PU%i,%i;", (int) x, (int) y);
					outf << str;
#else
					outf << "PU" << (int) x << "," << (int) y << ";";
#endif
				}
				break;
			case lineto:
				{
					{
						const Point & p = elem.getPoint(0);
						double x = (p.x() + x_offset) * HPGLScale;
						double y = (p.y() + y_offset) * HPGLScale;
						rot(x, y, rotation);
#if USESPRINTF
						char str[256];
						sprintf_s(TARGETWITHLEN(str,256), "PD%i,%i;", (int) x, (int) y);
						outf << str;
#else
						outf << "PD" << (int) x << "," << (int) y << ";";
#endif
					}
					if (isPolygon() && (n == elems)) {
						const basedrawingelement & elemnull = pathElement(0);
						const Point & pnull = elemnull.getPoint(0);
						double x = (pnull.x() + x_offset) * HPGLScale;
						double y = (pnull.y() + y_offset) * HPGLScale;
						rot(x, y, rotation);
#if USESPRINTF
						char str[256];
						sprintf_s(TARGETWITHLEN(str,256), "PD%i,%i;", (int) x, (int) y);
						outf << str;
#else
						outf << "PD" << (int) x << "," << (int) y << ";";
#endif
					}
				}
				break;
			case closepath:
				{
					const Point & p = pathElement(0).getPoint(0);
					double x = (p.x() + x_offset) * HPGLScale;
					double y = (p.y() + y_offset) * HPGLScale;
					rot(x, y, rotation);
#if USESPRINTF
					char str[256];
					sprintf_s(TARGETWITHLEN(str,256), "PD%i,%i;", (int) x, (int) y);
					outf << str;
#else
					outf << "PD" << (int) x << "," << (int) y << ";";
#endif
				}
				break;
			case curveto:
				errf << "\t\tFatal: unexpected case curveto in drvHPGL " << endl;
				abort();
				break;
			default:
				errf << "\t\tFatal: unexpected case default in drvHPGL " << endl;
				abort();
				break;
			}
		}
	}
}

// note: 23.2.2011 - VS doesn't really mean scale but velocity (which I do not know how it impacts scaling)
// HPGL graphic units are 1/1016 of an inch
// PostScript points are 1/72 of an inch - so there is a factor of 14.11111


// from http://h20000.www2.hp.com/bc/docs/support/SupportManual/bpl13211/bpl13211.pdf Figure 17-7:

// PlotterUnits EquivalentValue
// 1 plu =  0.025 mm (~ 0.00098 in.)
// 40 plu =  1 mm
// 1016 plu =  1 in.
// 3.39 plu =  1 dot @ 300 dp

// http://h20000.www2.hp.com/bizsupport/TechSupport/Document.jsp?objectID=bpl04568


static const unsigned char Ec = 0x1b;

void drvHPGL::open_page()
{
	//IN - init
	//SC - scaling off
	//PU - pen up
	//SP1 - select pen 1
	//LT - line type - solid line
	//VS - Speed
//version up to 3.50	outf << "IN;SC;PU;PU;SP1;LT;VS" << (int) HPGLScale << "\n";
	if (options->hpgl2) { outf << Ec << "E" << Ec << "%0B"; }
	outf << "IN;SC;PU;SP1;LT;" <<  "\n";
}

void drvHPGL::close_page()
{
	outf << "PU;SP;EC;PG1;EC1;OE\n";
	if (options->hpgl2) { outf << Ec << "%0A" << Ec <<"E" ; }
}


#if 0
void drvHPGL::endtext()
{
	if (merge) {
		flush_text(savedtextinfo);
		savedtextinfo.thetext = "";
	}
}
#endif


void drvHPGL::SelectPen(float R, float G, float B)
{
	//  Start DA hpgl color addition

	if (options->pencolorsfromfile) {

		const unsigned int reducedColor = intColor(R,G,B);
	// 	errf << " r " << reducedColor << " RGB " << R << G << B << " prev " << prevColor << endl;

		if (prevColor != reducedColor) {
			// If color changed, find best match in available colors

			// use the pen colors as defined in the pen color file
			// so we need to find the best matching color/pen for the current color
			double bestquality = 1e+100;    // Arbitrary large number

			// Linear search for a match
			// 0th element is never used - 0 indicates "new" color
			unsigned int bestIndex = 0;
			for (unsigned int i = 1; i < maxPen; i++) {
				const double quality = (R - penColors[i].R) * (R - penColors[i].R) +
					(G - penColors[i].G) * (G - penColors[i].G) +
					(B - penColors[i].B) * (B - penColors[i].B);
				if (quality < bestquality) {
					bestquality = quality;
					bestIndex = i;
				}
			}

			// errf << "After lookup  r " << " MP " << maxPen << " "<< reducedColor << " RGB " << R << G << B << " prev " << prevColor << " " << bestquality<< endl;
			// Select new pen if best match is a different pen
			prevColor = reducedColor; // to avoid the lookup in probable case that next item is drawn with same color
			if (currentPen != bestIndex) {
				currentPen=bestIndex;
				outf << "PU; \nSP" << currentPen << ";\n";
			}
		}
	} else if (options->maxPenColors > 0) {

		/* 
		*  The object is to generate pen switching commands when the color
		*  changes.  We keep a list of pen colors, which approximate the 
		*  desired rgb colors.  Choose an existing pen number when the 
		*  rgb color approximates that color, and add a new color to the
		*  list when the rgb color is distinctly new.
		*/

		const unsigned int reducedColor = intColor(R,G,B);

		if (prevColor != reducedColor) {
			// If color changed, see if color has been used before
			unsigned int npen = 0;
			if (maxPen > 0) {
				for (unsigned int j = 1; j <= maxPen; j++) {	// 0th element is never used - 0 indicates "new" color
					if (penColors[j].intColor == reducedColor) {
						npen = j;
					}
				}
			}
			// If color is new, add it to list, if room
			if (npen == 0) {
				if (maxPen < (unsigned int)options->maxPenColors) {
					maxPen++;
				}
				npen = maxPen;
				//cout << "npen : " << npen << " maxPenColors" << maxPenColors << endl;
				penColors[npen].intColor = reducedColor;
				penColors[npen].R = R;
				penColors[npen].G = G;
				penColors[npen].B = B;
			}
			// Select new pen
			prevColor = reducedColor;
			outf << "PU; \nSP" << npen << ";\n";
		}
		//  End DA hpgl color addition
	} else {
		// errf << "nothing done for colors - no option selected " << endl;
	}
}

void drvHPGL::show_text(const TextInfo & textinfo)
{

	const double pi = 3.1415926535;
	const double angleofs = rotation * pi / 180;
	const double dix = 100.0 * cos(textinfo.currentFontAngle * pi / 180.0 + angleofs);
	const double diy = 100.0 * sin(textinfo.currentFontAngle * pi / 180.0 + angleofs);

	double x = (textinfo.x() + x_offset) * HPGLScale;
	double y = (textinfo.y() + y_offset) * HPGLScale;
	rot(x, y, rotation);

/*
drvhpgl.cpp:316: warning: ISO C++ does not support the `%lg' printf format
drvhpgl.cpp:316: warning: ISO C++ does not support the `%lg' printf format
drvhpgl.cpp:318: warning: ISO C++ does not support the `%lg' printf format
drvhpgl.cpp:318: warning: ISO C++ does not support the `%lg' printf format
*/
	SelectPen(textinfo.currentR, textinfo.currentG, textinfo.currentB);

#if USESPRINTF
	char str[256];
	sprintf_s(TARGETWITHLEN(str,256), "DI%g,%g;", dix, diy);
	outf << str;
	sprintf_s(TARGETWITHLEN(str,256), "SI%g,%g;", textinfo.currentFontSize / 1000 * HPGLScale, textinfo.currentFontSize / 1000 * HPGLScale);
	outf << str;
	sprintf_s(TARGETWITHLEN(str,256), "PU%i,%i;", (int) x, (int) y);
	outf << str;
#else
	outf << "DI" << dix << "," << diy << ";";
	outf << "SI" << textinfo.currentFontSize / 1000 * HPGLScale << "," << textinfo.currentFontSize / 1000  * HPGLScale << ";";
	outf << "PU" << (int) x << "," << (int) y << ";";
#endif
	outf << "LB" << textinfo.thetext.c_str() << "\003;" << endl;
}


void drvHPGL::show_path()
{
	if (numberOfElementsInPath()) {

		SelectPen(currentR(), currentG(), currentB());
		
		switch (currentShowType()) {
		case drvbase::stroke:
			break;
		case drvbase::eofill:
			// Probably Wrong, but treat eoffil like fill
		case drvbase::fill:
			{
				const Point & p = pathElement(0).getPoint(0);
				double x = (p.x() + x_offset) * HPGLScale;
				double y = (p.y() + y_offset) * HPGLScale;
				rot(x, y, rotation);
#if USESPRINTF
				char str[256];
				sprintf_s(TARGETWITHLEN(str,256), "PU%i,%i;", (int) x, (int) y);
				outf << str;
#else
				outf << "PU" << (int) x << "," << (int) y << ";";
#endif
				outf << options->fillinstruction.value << ";PM0;";
			}
			break;
		default:				// cannot happen
			errf << "unexpected ShowType " << (int) currentShowType();
			break;
		}
		if (!options->penplotter) {
#if USESPRINTF
			char str[256];
//%lg is not standard - but might be needed // sprintf_s(TARGETWITHLEN(str,256), "PW%lg;", currentLineWidth());
			sprintf_s(TARGETWITHLEN(str,256), "PW%g;", currentLineWidth());
			outf << str;
#else
			outf << "PW" << currentLineWidth() << ";";
#endif
		}

		print_coords();

		switch (currentShowType()) {	// To check which endsequenz we need
		case drvbase::stroke:
			break;
		case drvbase::eofill:
			// Probably Wrong, but treat eoffil like fill
		case drvbase::fill:
			outf << "PM2;FP;EP;";	// EP also draws path
			break;
		default:				// cannot happen
			outf << "unexpected ShowType " << (int) currentShowType();
			break;
		}
		outf << endl;
	}
}


static DriverDescriptionT < drvHPGL > D_HPGL("hpgl", "HPGL code", "","hpgl", false,	// backend supports subpaths
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
												 DriverDescription::opentype::normalopen, false,	// backend support multiple pages
												 false /*clipping */ );

static DriverDescriptionT < drvHPGL > D_PCL("pcl", "PCL code", "","pcl", false,	// backend supports subpaths
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
												 DriverDescription::opentype::normalopen, false,	// backend support multiple pages
												 false /*clipping */ );
