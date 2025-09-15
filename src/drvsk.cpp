/* 
   drvSK.cpp

   */

/*

   Copyright (C) 1999 by Bernhard Herzog

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

#include "cppcomp.h"
#include <math.h>
#include I_stdlib
#include <ctype.h>
#include I_iomanip

#include I_strstream


#include "drvsk.h"
#include "base64writer.h"

#ifndef PI
#define PI 3.14159265358979323846264338327
#endif


drvSK::derivedConstructor(drvSK)
:	constructBase, id(0)
{
	outf << "##Sketch 1 0\n";
	outf << "document()\n";
	outf << "layer('Layer 1',1,1,0,0)\n";
	outf << "guess_cont()\n";
}

drvSK::~drvSK()
{
	options=nullptr;
}

void drvSK::print_coords()
{
	int first_subpath = 1;
	Point start;

	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		const basedrawingelement & elem = pathElement(n);
		switch (elem.getType()) {
		case moveto:
			{
				if (!first_subpath) {
					outf << "bn()\n";
				}
				first_subpath = 0;
				start = elem.getPoint(0);
				outf << "bs(" << start.x() << "," << start.y() << ",0)\n";
			}
			break;

		case lineto:
			{
				const Point & p = elem.getPoint(0);
				outf << "bs(" << p.x() << "," << p.y() << ",0)\n";
			}
			break;

		case closepath:
			{
				outf << "bs(" << start.x() << "," << start.y() << ",0)\n";
				outf << "bC()\n";
			}
			break;

		case curveto:
			{
				const Point & p1 = elem.getPoint(0);
				const Point & p2 = elem.getPoint(1);
				const Point & p3 = elem.getPoint(2);

				outf << "bc(" << p1.x() << "," << p1.y() << ","
					<< p2.x() << "," << p2.y() << "," << p3.x() << "," << p3.y() << ",0)\n";
			}
			break;

		default:
			cerr << "\t\tFatal: unexpected case in drvsk\n";
			abort();
			break;
		}
	}
}


// ignore open_page and close_page since sketch supports only a single
// page
// XXX multiple pages could be faked by putting each page on its own layer.
void drvSK::open_page()
{
}

void drvSK::close_page()
{
}

static void save_solid_fill(ostream & outf, float r, float g, float b)
{
	outf << "fp((" << r << "," << g << "," << b << "))\n";
}

static void
save_line(ostream & outf, float r, float g, float b, float width,
		  int cap, int join, const char *dash_pattern_string)
{
	DashPattern dash_pattern(dash_pattern_string);

	outf << "lp((" << r << "," << g << "," << b << "))\n";
	if (width > 0)
		outf << "lw(" << width << ")\n";
	if (cap != 0)
		outf << "lc(" << cap + 1 << ")\n";
	if (join != 0)
		outf << "lj(" << join << ")\n";
	if (dash_pattern.nrOfEntries > 0) {
		if (width <= 0)
			width = 1.0;

		int num = dash_pattern.nrOfEntries;
		num = num * (num % 2 + 1);
		outf << "ld((" << dash_pattern.numbers[0] / width;
		for (int i = 1; i < num; i++) {
			outf << "," << dash_pattern.numbers[i] / width;
		}
		outf << "))\n";
	}
}

static void save_string(ostream & outf, size_t len, const char *str)
{
	outf << '"';
	while (len-->0) {
		const int c = *str++ & 0xFF;
		if (c >= 0 && c <= 127 && isprint(c)) {
			if (c == '"')
				outf << '\\';
			outf << char (c);
		} else {
			outf << '\\' << std::oct << std::setw(3) << std::setfill('0') << c;
			outf << std::dec << std::setfill(' '); // reset to std
		}
	}
	outf << '"';
}

void drvSK::show_text(const TextInfo & textinfo)
{
	save_solid_fill(outf, fillR(), fillG(), fillB());
	outf << "Fn(\"" << textinfo.currentFontName.c_str() << "\")\n";
	outf << "Fs(" << textinfo.currentFontSize << ")\n";
	outf << "txt(";
	save_string(outf, textinfo.thetext.length(), textinfo.thetext.c_str());
	outf << ",(";
	if (textinfo.currentFontAngle) {
		const double angle = textinfo.currentFontAngle * PI / 180.0;
		const double c = cos(angle);
		const double s = sin(angle);
		outf << c << "," << s << "," << -s << "," << c << ",";
	}
	outf << textinfo.x() << ", " << textinfo.y() << "))\n";
}

void drvSK::show_path()
{
	switch (currentShowType()) {
	case drvbase::stroke:
		save_line(outf, currentR(), currentG(), currentB(),
				  currentLineWidth(), currentLineCap(), currentLineJoin(), dashPattern());
		outf << "fe()\n";
		break;

	case drvbase::fill:
		//cerr << "fill treated as eofill" << endl;
		// fall through into next case...
	case drvbase::eofill:
		save_solid_fill(outf, fillR(), fillG(), fillB());
		//cerr << "currentLineWidth =" << currentLineWidth() << endl;
		if (pathWasMerged()) {
			save_line(outf, edgeR(), edgeG(), edgeB(),
					  currentLineWidth(), currentLineCap(), currentLineJoin(), dashPattern());
			//outf << "#merged\n";
		} else
			outf << "le()\n";
		break;

	default:
		// cannot happen
		cerr << "unexpected ShowType " << (int) currentShowType() << '\n';
		break;
	}

	outf << "b()\n";
	print_coords();

}
#if 0
void drvSK::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{

// according to Vladimir Eltsov this is incomplete:
//
// For a long time pstoedit is shipped with incomplete implementation of rectangles in the 
// removes rectangle support from drvsk (so that the default path 
// representation is used), which I have found to produce much more useful 
// conversion.
//
	outf << "r(" << urx - llx << ",0,0," << ury - lly << "," << llx << "," << lly << ")\n";
}
#endif

bool drvSK::pathsCanBeMerged(const PathInfo & path1, const PathInfo & path2) const
{
	const PathInfo *first;
	const PathInfo *last;

	if (path1.nr < path2.nr) {
		first = &path1;
		last = &path2;
	} else {
		first = &path2;
		last = &path1;
	}

	if ((first->currentShowType == fill || first->currentShowType == eofill)
		&& last->currentShowType == stroke
		&& first->numberOfElementsInPath == last->numberOfElementsInPath) {
		for (unsigned int i = 0; i < last->numberOfElementsInPath; i++) {
			const basedrawingelement *bd1 = first->path[i];
			const basedrawingelement *bd2 = last->path[i];
			if (!(*bd1 == *bd2))
				return false;
		}
//      cerr << "Merge: first " << first->nr
//           << (first->currentShowType == stroke ? " stroke" : " fill")
//           << ", last " << last->nr
//           << (last->currentShowType == stroke ? " stroke" : " fill")
//           << endl;
		return true;
	} else {
		return false;
	}
}





void drvSK::show_image(const PSImage & imageinfo)
{
	if (imageinfo.ncomp > 3) {
		cerr << "image with " << imageinfo.ncomp << " components not supported\n";
		return;
	}

	C_ostrstream ppm;

	switch (imageinfo.type) {
	case ImageType::colorimage:
		if (imageinfo.ncomp != 3 || imageinfo.bits != 8) {
			cerr << "color images must have 8 bits/component " "and 3 components\n";
			cerr << "(image has " << imageinfo.ncomp << " with "
				<< imageinfo.bits << " bits/component)\n";
			return;
		}
		ppm << "P6\n";
		break;

	case ImageType::normalimage:
		if (imageinfo.bits != 8) {
			cerr << "gray images must have 8 bits/component ";
			cerr << "(image has " << imageinfo.bits << " bits/component)\n";
			return;
		}
		ppm << "P5\n";
		break;
	case ImageType::imagemask:
		ppm << "P4\n";
		break;
	default:
		return;
	}

	ppm << imageinfo.width << " " << imageinfo.height << '\n';
	if (imageinfo.type != ImageType::imagemask) {
		// bug according to Jeff Dairiki ppm << imageinfo.bits << '\n';
		ppm << ((1 << imageinfo.bits) -1)  << '\n';
	}

	const int imageid = getid();
	outf << "bm(" << imageid << ")\n";

	{
	Base64Writer base64writer(outf);

#ifdef  USE_NEWSTRSTREAM
	std::string temp = ppm.str();
	// basic_string<char, std::char_traits<char>, std::allocator<char> >	 temp = ppm.str();
	const unsigned char * ppmdata = (const unsigned char*) temp.data();
	(void)base64writer.write_base64( ppmdata, temp.size());
#else
 	(void)base64writer.write_base64( (unsigned  char *) ppm.str(), ppm.pcount());
	ppm.rdbuf()->freeze(0);
#endif

	int bytes_left = imageinfo.nextfreedataitem;
	unsigned char *data = imageinfo.data;
	while (bytes_left) {
		int written = base64writer.write_base64(data, bytes_left);
		data += written;
		bytes_left -= written;
	}

// now done in dtor --	base64writer.close_base64();
	}

	outf << "-\n";

	outf << "im((";
	outf << imageinfo.normalizedImageCurrentMatrix[0] << ",";
	outf << imageinfo.normalizedImageCurrentMatrix[1] << ",";
	outf << -imageinfo.normalizedImageCurrentMatrix[2] << ",";
	outf << -imageinfo.normalizedImageCurrentMatrix[3] << ",";
	outf << (imageinfo.normalizedImageCurrentMatrix[2] * imageinfo.height
			 + imageinfo.normalizedImageCurrentMatrix[4]) << ",";
	outf << (imageinfo.normalizedImageCurrentMatrix[3] * imageinfo.height
			 + imageinfo.normalizedImageCurrentMatrix[5]);
	outf << ")," << imageid << ")\n";

}

static DriverDescriptionT < drvSK > D_sampl("sk", "Sketch format","", "sk", true,	// backend supports subpaths
											true,	// backend supports curves
											true,	// backend supports elements which are filled and stroked
											true,	// backend supports text
											DriverDescription::imageformat::memoryeps,	// no support for PNG file images
											DriverDescription::opentype::normalopen, false,	// if format supports multiple pages in one file
											false  /*clipping */
											);
 
 
