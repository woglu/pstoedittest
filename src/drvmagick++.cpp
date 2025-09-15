/*
   drvMAGICK.cpp : This file is part of pstoedit
   driver for Magick++ API.

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


#include "cppcomp.h"

// needs for linking : CORE_RL_Magick++_.lib CORE_RL_magick_.lib 

#if defined(HAVE_MAGIC)
// Magick++ can only be compiled with STL

#include "drvmagick++.h"

#include I_fstream
#include I_stdio
#include I_stdlib

using namespace Magick;
#ifdef MagickWebSite
// This is GraphicsMagick
// GraphicsMagick has version numbers different from ImageMagick
// #if MagickLibVersion  < 0x201702
typedef DrawableDashArray DrawableDashArrayType;
typedef std::list < Magick::Drawable > drawListType;
#else
// This is for ImageMagick
#if MagickLibVersion  > 0x700
typedef DrawableStrokeDashArray DrawableDashArrayType;
typedef std::vector < Magick::Drawable > drawListType;
#else
typedef DrawableDashArray DrawableDashArrayType;
typedef std::list < Magick::Drawable > drawListType;
#endif
#endif

static const bool withdummycontext = false; 

// #define onedrawlist 1

#ifdef onedrawlist
static std::list < Magick::Drawable > drawList;
#endif

//test typedef std::list<string> MyStringList;

drvMAGICK::derivedConstructor(drvMAGICK):
constructBase,
//imgcount(0),
imageptr(nullptr)
{
// driver specific initializations
// and writing of header to output file
//  outf << "MAGICK header \n";
//  float           scale;
//  float           x_offset;
//  float           y_offset;

//test MyStringList xxx("asdasda");
//test xxx.push_back("asdsda");

	try {
		InitializeMagick(0);
		imageptr = new Image(Geometry(600, 800), Color("white"));
 
		if (withdummycontext)
			imageptr->draw(DrawablePushGraphicContext());
#ifdef onedrawlist
		drawList.push_back(DrawablePushGraphicContext());
		drawList.push_back(DrawableViewbox(0, 0, 640, 800));
#else
		imageptr->draw(DrawableViewbox(0, 0, 640, 800));
#endif

		// Display the result 
		imageptr->display();

//  image.write("result.mvg");
//  image.write("result.jpg");
	}

	catch (Exception & error_) {
		cout << "Caught exception: " << error_.what() << endl;
		ctorOK = false;
		//return 1; 
	}
}

drvMAGICK::~drvMAGICK()
{
	try {
		if (withdummycontext)
			imageptr->draw(DrawablePopGraphicContext());

#ifdef onedrawlist
		drawList.push_back(DrawablePopGraphicContext());
		// Draw everything using completed drawing list 
		imageptr->draw(drawList);
#endif

//      imageptr->write("result.mvg");
#if 0
		// write a jpeg version for debug purposes
		cout << "writing result.jpg" << endl;
		imageptr->write("result.jpg");
#endif
		if (Verbose()) cout << "writing " << outFileName.c_str() << endl;
		imageptr->write(outFileName.c_str());
	}
	catch (const Exception & error_) {
		cout << "Caught exception: " << error_.what() << endl;
	}
	delete imageptr;
	imageptr = nullptr;
}

void drvMAGICK::create_vpath(VPathList &vpath)
{
	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		const basedrawingelement & elem = pathElement(n);
		switch (elem.getType()) {
		case moveto:{
				const ::Point & p = elem.getPoint(0);
				const Magick::Coordinate coord(p.x() + x_offset,
											   currentDeviceHeight - p.y() + y_offset);
				vpath.push_back(PathMovetoAbs(coord));
			}
			break;
		case lineto:{
				const ::Point & p = elem.getPoint(0);
				const Magick::Coordinate coord(p.x() + x_offset,
											   currentDeviceHeight - p.y() + y_offset);
				vpath.push_back(PathLinetoAbs(coord));
			}
			break;
		case closepath:
			vpath.push_back(PathClosePath());
			break;
		case curveto:{
				const ::Point & p0 = elem.getPoint(0);
				const ::Point & p1 = elem.getPoint(1);
				const ::Point & p2 = elem.getPoint(2);
				vpath.
					push_back(PathCurvetoAbs
							  (PathCurvetoArgs
							   (p0.x() + x_offset, currentDeviceHeight - p0.y() + y_offset,
								p1.x() + x_offset, currentDeviceHeight - p1.y() + y_offset,
								p2.x() + x_offset, currentDeviceHeight - p2.y() + y_offset)));
			}
			break;
		default:
			errf << "\t\tFatal: unexpected case in drvMAGICK " << endl;
			abort();
			break;
		}
	}
}


void drvMAGICK::open_page()
{
//  outf << "Opening page: " << currentPageNumber << endl;
}

void drvMAGICK::close_page()
{
//  outf << "Closing page: " << (currentPageNumber) << endl;
}

void drvMAGICK::show_text(const TextInfo & textinfo)
{
	try {
		DrawableList drawList;
		drawList.push_back(DrawablePushGraphicContext());
		drawList.push_back(DrawableFont(textinfo.currentFontName.c_str(), AnyStyle, 400, AnyStretch));
//      drawList.push_back( DrawableText(100,100,"test") );
		drawList.push_back(DrawablePointSize(textinfo.currentFontSize));
		drawList.push_back(DrawableFillColor
					  (ColorRGB(textinfo.currentR, textinfo.currentG, textinfo.currentB)));
		drawList.push_back(DrawableStrokeColor(Color()));	// unset color
#if 1
/*

I assume that the map works the same way as Postscript.  ImageMagick
follows the SVG matrix scheme as described at
http://www.w3.org/TR/SVG/coords.html.  The ordering is

scaleX,rotationX,rotationY,scaleY,translationX,translationY
0      1         2         3      4            5

Locations 0 & 3 are scaling for x,y; 1 & 2 are rotation for x,y; 4 & 5
are translation.
*/

		const float *CTM = getCurrentFontMatrix();
		const float fontsize = textinfo.currentFontSize;
		const double sx_ = CTM[0] / fontsize;
		const double rx_ = -CTM[1] / fontsize;
		const double ry_ = -CTM[2] / fontsize;
		const double sy_ = CTM[3] / fontsize;
		const double tx_ = CTM[4] + x_offset;
		const double ty_ = currentDeviceHeight - CTM[5] + y_offset;
		drawList.push_back(DrawableAffine(sx_, sy_, rx_, ry_, tx_, ty_));
#else
//      drawList.push_back( DrawableAngle(textinfo.currentFontAngle/6.29) );
		drawList.
			push_back(DrawableTranslation
					  (textinfo.x() + x_offset, currentDeviceHeight - textinfo.y() + y_offset));
		drawList.push_back(DrawableRotation(360.0 - textinfo.currentFontAngle));
#endif
		drawList.push_back(DrawableText(0, 0, textinfo.thetext.c_str()));
		drawList.push_back(DrawablePopGraphicContext());
		imageptr->draw(drawList);
	}
	catch (const Exception & error_) {
		cout << "Caught exception: " << error_.what() << endl;
	}
} 

void drvMAGICK::show_path()
{

	static const Color NoColor;	// Bob says that a default color means no-fill/no-stroke

	// Construct drawing list
#ifndef onedrawlist
	drawListType drawList;
#endif

	VPathList vpath;
	create_vpath(vpath);


//  outf << "Path # " << currentNr();
//  if (isPolygon())
//      outf << " (polygon): " << endl;
//  else
//      outf << " (polyline): " << endl;
//  outf << "\tcurrentShowType: ";
#ifndef onedrawlist
	drawList.push_back(DrawablePushGraphicContext());
#endif

	switch (currentShowType()) {
	case drvbase::stroke:
		//outf << "stroked";
		drawList.push_back(DrawableStrokeColor(ColorRGB(edgeR(), edgeG(), edgeB())));
		drawList.push_back(DrawableStrokeWidth(currentLineWidth()));	// Stroke width 
		drawList.push_back(DrawableFillColor(NoColor));
		// image.fillColor("");
		break;
	case drvbase::fill:
		//  outf << "filled";
		drawList.push_back(DrawableStrokeColor(NoColor));
		drawList.push_back(DrawableFillRule(NonZeroRule));
		drawList.push_back(DrawableFillColor(ColorRGB(fillR(), fillG(), fillB())));	// Fill color 
		break;
	case drvbase::eofill:
		//  outf << "eofilled";
		drawList.push_back(DrawableStrokeColor(NoColor));
		drawList.push_back(DrawableFillRule(EvenOddRule));
		drawList.push_back(DrawableFillColor(ColorRGB(fillR(), fillG(), fillB())));	// Fill color 
		break;
	default:
		// cannot happen
		errf << "unexpected ShowType " << (int) currentShowType();
		break;
	}


	{
		// dash handling
		DashPattern dp(dashPattern());
		const float *const d_numbers = dp.numbers;
		// const int nr_of_entries = dp.nrOfEntries;
		double *dasharray = new double[dp.nrOfEntries + 1];
		for (int i = 0; i < dp.nrOfEntries; i++) {
			dasharray[i] = d_numbers[i];
		}
		dasharray[dp.nrOfEntries] = 0;	// mark last element
		drawList.push_back(DrawableDashArrayType(dasharray));	// pulls a copy, so we are the owner of dasharray
//??    DrawableDashOffset
		delete[]dasharray;
	}

	const unsigned int join = currentLineJoin();	// 0 miter; 1 round; 2 bevel
	const LineJoin linejoin =
		(join == 0) ? MiterJoin : (join == 1) ? RoundJoin : (join == 2) ? BevelJoin : UndefinedJoin;
	drawList.push_back(DrawableStrokeLineJoin(linejoin));

	// const float miter = currentMiterLimit();

	const unsigned int cap = currentLineCap();	// 0 butt; 1 round; 2 square
	const LineCap linecap =
		(cap == 0) ? ButtCap : (cap == 1) ? RoundCap : (cap == 2) ? SquareCap : UndefinedCap;
	drawList.push_back(DrawableStrokeLineCap(linecap));

	drawList.push_back(DrawablePath(vpath));

#ifndef onedrawlist
	drawList.push_back(DrawablePopGraphicContext());
	// Draw everything using completed drawing list 
	imageptr->draw(drawList);
#endif

}

#if 0
void drvMAGICK::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{

	outf << "Rectangle ( " << llx << "," << lly << ") (" << urx << "," <<
		ury << ") equivalent to:" << endl;
// just do show_path for a first guess


	show_path();
}
#endif

void drvMAGICK::show_image(const PSImage & imageinfo)
{

	if (imageinfo.isFileImage) {
		try {
			DrawableList drawList;

			const double sx = imageinfo.normalizedImageCurrentMatrix[0];
			const double rx = -imageinfo.normalizedImageCurrentMatrix[1];
			const double ry = imageinfo.normalizedImageCurrentMatrix[2];
			const double sy = -imageinfo.normalizedImageCurrentMatrix[3];

			const double x = 0;
			const double y = 0;
			const double tx = imageinfo.normalizedImageCurrentMatrix[4] + x_offset;
			const double ty =
				currentDeviceHeight - imageinfo.normalizedImageCurrentMatrix[5] + y_offset;


			const double width = imageinfo.width;
			const double height = imageinfo.height;

			cout << " sx " << sx << " sy " << sy << " rx " << rx << " ry " << ry << " tx " << tx <<
				" ty " << ty << " w " << width << " h " << height << endl;

			const std::string filename = imageinfo.FileName.c_str();

			cout << "drawing subimage from " << filename << endl;

			drawList.push_back(DrawablePushGraphicContext());
			drawList.push_back(DrawableAffine(sx, sy, rx, ry, tx, ty));

			Image pngimage(filename);
			//  cout << "rows " << pngimage.rows() << " columns " << pngimage.columns() << endl;
			//  drawList.push_back( DrawableCompositeImage(0,0,width, height, filename) );
			if ((pngimage.rows() > 0) && (pngimage.columns() > 0)) {
				DrawableCompositeImage theimage(x, y, width, height, pngimage);
				theimage.magick("png");
				drawList.push_back(theimage);
			} else {
				errf << "reading image from " << filename << " failed " << endl;
			}
			drawList.push_back(DrawablePopGraphicContext());
			imageptr->draw(drawList);
		}
		catch (const Exception & error_) {
			errf << "Caught exception: " << error_.what() << endl;
		}
	} else {
		errf << "Only PNG file based image are supported" << endl;
	}
}

static DriverDescriptionT < drvMAGICK > D_magick("magick", "MAGICK driver compatible with version " MagickLibVersionText " of " MagickPackageName ".", 
												 "This driver uses the C++ API of ImageMagick or GraphicsMagick to finally produce different output "
												 "formats. The output format is determined automatically by Image-/GraphicsMagick based on the suffix "
												 "of the output filename. So an output file test.png will force the creation of an image in PNG format."
												 " This binary of pstoedit was compiled against version " MagickLibVersionText " of " MagickPackageName ".",
												 "...", true,	// backend supports subpaths
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
												 DriverDescription::imageformat::png,// support for PNG file images
												 DriverDescription::opentype::noopen, false,	// if format supports multiple pages in one file
												 true 	/*clipping */
												 );


#else

// NO SUPPORT FOR MAGIC WITHOUT STL
#pragma NO_SUPPORT_FOR_MAGIC_WITHOUT_STL_AND_IMAGEMAGICK_HEADERS

#endif
 
