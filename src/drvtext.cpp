/* 
   drvTEXT.cpp : This file is part of pstoedit
   Skeleton for the implementation of text based backends
   for example this could be extended towards an HTML backend.

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
#include "drvtext.h"
#include I_fstream
#include I_stdio
#include I_stdlib

// sizes in terms of lines and characters



drvTEXT::derivedConstructor(drvTEXT):
constructBase, charpage(nullptr)
// , dumptextpieces(false), pageheight(200), pagewidth(120)
{
// driver specific initializations
#if 0
	for (unsigned int i = 0; i < d_argc; i++) {
		assert(d_argv && d_argv[i]);
		if (Verbose())
			errf << "% " << d_argv[i] << endl;
		if (strcmp(d_argv[i], "-dump") == 0) {
			dumptextpieces = true;
		} else if (strcmp(d_argv[i], "-height") == 0) {
			pageheight = atoi(d_argv[i + 1]);
			i++;
		} else if (strcmp(d_argv[i], "-width") == 0) {
			pagewidth = atoi(d_argv[i + 1]);
			i++;
		}
	}
#endif

	if (!options->dumptextpieces) {
#if 0
		charpage = new char[options->pageheight * options->pagewidth];
		for (unsigned int ii = 0; ii < options->pageheight; ii++) {
			//charpage[i] = new char[options->pagewidth];
			for (unsigned int j = 0; j < options->pageheight; j++)
				(charpage[ii])[j] = ' ';
		}
#else
		charpage = new char *[options->pageheight];
		for (unsigned int ii = 0; ii < (unsigned int) options->pageheight; ii++) {
			charpage[ii] = new char[options->pagewidth];
			for (unsigned int j = 0; j < (unsigned int) options->pagewidth; j++)
				(charpage[ii])[j] = ' ';
		}
#endif
	}
// and writing of header to output file
//  outf << "Sample header \n";
//  float           scale;
//  float           x_offset;
//  float           y_offset;
}

drvTEXT::~drvTEXT()
{
// driver specific deallocations
// and writing of trailer to output file
	if (options->dumptextpieces) {
		outf << "Sample trailer \n";
	} 
	if (charpage) {
		for (unsigned int i = 0; i < (unsigned int) options->pageheight; i++) {
			delete[]charpage[i];
			charpage[i] = nullptr;
		}
		delete[]charpage;
		charpage = nullptr;
	}
	options=nullptr;
}

void drvTEXT::open_page()
{
	if (options->dumptextpieces)
		outf << "Opening page: " << currentPageNumber << endl;
}

void drvTEXT::close_page()
{
	if (options->dumptextpieces) {
		outf << "Closing page: " << (currentPageNumber) << endl;
		const size_t nroflines = page.size();
		for (unsigned int i = 0; i < nroflines; i++) {
			const Line * const lineptr = page[i];
			const size_t nrofpieces = lineptr->textpieces.size();
			outf << "***********************************************" << endl;
			for (unsigned int j = 0; j < nrofpieces; j++) {
				const TextInfo & textinfo = lineptr->textpieces[j];
				outf << "Text String : " << textinfo.thetext.c_str() << endl;
				outf << '\t' << "X " << textinfo.x() << " Y " << textinfo.y() << endl;
				outf << '\t' << "X_END " << textinfo.x_end() << " Y_END " << textinfo.y_end() << endl;
				outf << '\t' << "currentFontName: " << textinfo.currentFontName.c_str() << endl;
				outf << '\t' << "is_non_standard_font: " << textinfo.is_non_standard_font << endl;
				outf << '\t' << "currentFontFamilyName: " << textinfo.
					currentFontFamilyName.c_str() << endl;
				outf << '\t' << "currentFontFullName: " << textinfo.currentFontFullName.
					c_str() << endl;
				outf << '\t' << "currentFontWeight: " << textinfo.currentFontWeight.c_str() << endl;
				outf << '\t' << "currentFontSize: " << textinfo.currentFontSize << endl;
				outf << '\t' << "currentFontAngle: " << textinfo.currentFontAngle << endl;
				outf << '\t' << "currentR: " << textinfo.currentR << endl;
				outf << '\t' << "currentG: " << textinfo.currentG << endl;
				outf << '\t' << "currentB: " << textinfo.currentB << endl;

			}
		}
		for (unsigned int ii = 0; ii < nroflines; ii++) {
			Line *lineptr = page[ii];
			delete lineptr;
		}
		page.clear();
	} else {
		assert(charpage);
		for (unsigned int i = 0; i < (unsigned int) options->pageheight; i++) {
			for (unsigned int j = 0; j < (unsigned int) options->pagewidth; j++) {
				outf << (charpage[i])[j];
				(charpage[i])[j] = ' ';
			}
			outf << endl;
		}

	}
}

void drvTEXT::show_text(const TextInfo & textinfo)
{
	if (options->dumptextpieces) {
		// check which line this piece of text fits in.
		//
		const size_t nroflines = page.size();
		bool inserted = false;
		for (unsigned int i = 0; i < nroflines; i++) {
			if ((textinfo.y() <= page[i]->y_max)
				&& (textinfo.y() >= page[i]->y_min)) {
				page[i]->textpieces.insert(textinfo);
				inserted = true;
				break;
			}
		}
		if (!inserted) {
			auto newline = new Line;
			//lint -esym(429,newline) // newline is not freed here, but inserted into the list
			page.insert(newline);
			newline->y_max = textinfo.y() + 0.1f * textinfo.currentFontSize;
			newline->y_min = textinfo.y() - 0.1f * textinfo.currentFontSize;
			newline->textpieces.insert(textinfo);
		}
	} else {
		assert(charpage);
		int x = (int) (options->pagewidth * (textinfo.x() / 700.0f));
		int y = (int) (options->pageheight * ((currentDeviceHeight + y_offset - textinfo.y()) / 800.0f));
		if ((x >= 0) && (y >= 0) && (x < options->pagewidth) && (y < options->pageheight)) {
			if (((charpage[y])[x] != ' ')) {
				cerr << "character " << (charpage[y])[x] << " overwritten with " << textinfo.
					thetext.
					c_str()[0] << " at " << x << " " << y <<
					" - Hint increase -width and/or -height" << endl;
			}
			(charpage[y])[x] = textinfo.thetext.c_str()[0];
		} else {
			cerr << "seems to be off-page: " << textinfo.thetext.c_str()[0] << endl;
			cerr << x << " " << y << " " << textinfo.x() << " " << textinfo.y() << endl;
		}
	}
}

void drvTEXT::show_path()
{
}


void drvTEXT::show_image(const PSImage & imageinfo)
{
	unused(&imageinfo);
}

static DriverDescriptionT < drvTEXT > D_text("text", "text in different forms ", "","txt", false,	// if backend supports subpaths, else 0
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
											 false 	/*clipping */
											 );
