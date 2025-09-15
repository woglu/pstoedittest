/*
   drvSWF.cpp : This file is part of pstoedit
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

// libming/swf (not this driver) is not very clean from memory management point of view 
// so exclude this driver from Coverity analysis
#if defined(HAVE_LIBMING) && !defined(__COVERITY__)
#include "drvswf.h"
#include I_fstream
#include I_stdio
#include I_stdlib
#include <string.h>


//static bool trace = false;
//static bool cubic = false;

#include "mingpp.h"

/* Use png library */
#define USE_PNG 1
// #include "ming_config.h"

#if defined(_WIN32) && (_MSC_VER < 1900)
extern "C" {

	double rint(double arg) {
		return ((long) arg);
		// just as a quick workaround
		// libswf needs this - since on Windows, there is no rint in math.h 
}}

#endif

// USE_PNG is defined in ming/config.h
#if defined(HAVE_LIBMINGUTIL) && !( defined(USE_PNG) && USE_PNG )

// only use this if libming doesn't support png2dbl itself (available from version 0.3 on)

#define main MAIN
// strdup is not visible is -ansi is turned on
#define strdup cppstrdup
#include "util/png2dbl.c"
static void png2dbl(const char *pngfile, const char *dblfile)
{
	struct pngdata png;
	png = readPNG(fopen(pngfile, "rb"));
	FILE *f = fopen(dblfile, "wb");
	writeDBL(f, png);
	fclose(f);
}

#endif

const bool withcurves = true;	// curves are still quite broken in SWF


drvSWF::derivedConstructor(drvSWF):
constructBase, imgcount(0), swfscale(1.0f)
{
#if 0
	if (d_argc > 0) {
		for (unsigned int i = 0; i < d_argc; i++) {
			if (strcmp(d_argv[i], "-trace") == 0) {
				trace = true;
			} else if (strcmp(d_argv[i], "-cubic") == 0) {
				cubic = true;
			} else {
				errf << "Unrecognized svg option: " << d_argv[i]
					<< endl;
			}
		}
	}
#endif


	(void) Ming_init();

	Ming_setCubicThreshold(100); // default of 10000 is too coarse

	movie = new SWFMovie();
	movie->setRate(12.0f);
	movie->setDimension(swfscale * 700, swfscale * 1700);	// formerly 20
	movie->setNumberOfFrames(1);

	if (options->trace) {
		printf("%s", "#include <mingpp.h>\n" "void testit(SWFMovie * movie,int pathlimit) {\n");
#if 0
		"(void) Ming_init();\n"
			"movie = new SWFMovie();\n"
			"movie->setRate(12.0f);\n"
			"movie->setDimension(swfscale * 700, swfscale * 1700);\n"
			"movie->setNumberOfFrames(1);\n";
#endif
	}


}

drvSWF::~drvSWF()
{
	const int length = movie->save(outFileName.c_str());
	delete movie;
	if (options->trace)
		printf("}\n");
	printf("// %i bytes written to %s\n", length, outFileName.c_str());
}

// *INDENT-OFF*
#if 0
*INDENT - OFF * This version adds the drawCubic function for drawing
cubic bezier curves, and a number of assorted bug fixes.And probably plenty more bugs.
	$s->drawCurve(bx, by, cx, cy, dx, dy);
	$s->drawCubic(bx, by, cx, cy, dx, dy);
both do the same thing:draw a cubic bezier from the current pen position,
using the three given points as control points.
To control how "tight" the approximation is, use the Ming_setCubicThreshold(num);
function. This gives the amount of error in approximation that each subdivision
of the curve needs in order to stop recursing.The default is 20000 - if your
curves are too sloppy, lower the number.
#endif
// *INDENT-ON*

void drvSWF::print_coords()
{

#if 0
	if (numberOfElementsInPath() > 255) {
		errf << "ignoring path with " << numberOfElementsInPath() << " elements " << endl;
		return;
	}
#endif
 
	if (numberOfElementsInPath() < 2)
		return;


	auto s = new SWFShape;

	if (options->trace) {
		static int pathid = 0;
		printf("if ( %d < pathlimit) {\n" "SWFShape * s = new SWFShape;\n", pathid++);
		//printf("t->addString( \"path#%d\",nullptr);\n", pathid);
		//printf("SWFDisplayItem *  dt = movie->add(t);   dt->move( 200,%d);\n", pathid);
	}


	switch (currentShowType()) {
	case drvbase::stroke:
		break;
	case drvbase::fill:
		{
			SWFFill *swffill = s->addSolidFill((byte) (255 * fillR()), (byte) (255 * fillG()),
											   (byte) (255 * fillB()));
			s->setRightFill(swffill);
			if (options->trace) {
				printf("SWFFill *swffill = s->addSolidFill(%d,%d,%d); \n",
					   (int) (255 * fillR()), (int) (255 * fillG()), (int) (255 * fillB()));
				printf("s->setRightFill(swffill);\n");
			}
			break;
		}
	case drvbase::eofill:
		{
			SWFFill *swffill = s->addSolidFill((byte) (255 * fillR()), (byte) (255 * fillG()),
											   (byte) (255 * fillB()));
			s->setRightFill(swffill);
			if (options->trace) {
				printf("SWFFill *swffill = s->addSolidFill(%d,%d,%d); \n",
					   (int) (255 * fillR()), (int) (255 * fillG()), (int) (255 * fillB()));
				printf("s->setRightFill(swffill);\n");
			}

			break;
		}
	default:
		// cannot happen
		errf << "unexpected ShowType " << (int) currentShowType();
		break;
	}
 // libming supports only byte wise width 
	int linewidth = (int) (swfscale * currentLineWidth());
	if ( linewidth  > 255 ) {
		errf << "Warning: linewidth greater than 255 is not supported by libming" << endl;
		linewidth = 255;
	}
	s->setLine( (byte) linewidth, 
				(byte) (255 * edgeR()),
				(byte) (255 * edgeG()), 
				(byte) (255 * edgeB())
				);

	if (options->trace) {
		printf("s->setLine(%d, %d,%d,%d); \n", (int) (swfscale * currentLineWidth()),
			   (byte) (255 * edgeR()), (byte) (255 * edgeG()),
			   (byte) (255 * edgeB()));
	}

	Point currentpoint(-9999.0f, -9999.0f);
	Point firstpoint(-9999.0f, -9999.0f);
	bool firstfound = false;

	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		const basedrawingelement & elem = pathElement(n);
		switch (elem.getType()) {
		case moveto:{
				const Point & p = elem.getPoint(0);
				if (!firstfound) {
					firstfound = true;
					firstpoint = p;
				}
				s->movePenTo(swfx(p), swfy(p));
				if (options->trace) {
					printf("s->movePenTo(%d,%d);\n", (int) swfx(p), (int) swfy(p));
				}
				currentpoint = p;
			}
			break;
		case lineto:{
				const Point & p = elem.getPoint(0);
				if (!firstfound) {
					firstfound = true;
					firstpoint = p;
				}
				if (p == currentpoint) {
					if (Verbose())
						errf << "ignoring zero length lineto " << endl;
				} else {
					s->drawLineTo(swfx(p), swfy(p));
					if (options->trace) {
						printf("s->drawLineTo(%d,%d);\n", (int) swfx(p), (int) swfy(p));
					}
					currentpoint = p;
				}
			}
			break;
		case closepath:
#if 1
			if (firstfound) {
				if (!(currentpoint == firstpoint)) {
					const coordtype x = swfx(firstpoint);
					const coordtype y = swfy(firstpoint);
					s->drawLineTo(x, y);
					if (options->trace) {
						printf("s->drawLineTo(%d,%d);\n", (int) x, (int) y);
					}
					currentpoint = firstpoint;
				}
			} else {
				errf << "closepath found without first point" << endl;
			}
#endif
			break;
		case curveto:{
				if (withcurves) {
					const Point & b = elem.getPoint(0);
					if (!firstfound) {
						firstfound = true;
						firstpoint = b;
					}
					const Point & c = elem.getPoint(1);
					const Point & d = elem.getPoint(2);
					const coordtype cpx = swfx(currentpoint);
					const coordtype cpy = swfy(currentpoint);
					const coordtype bx = swfx(b);
					const coordtype by = swfy(b);
					const coordtype cx = swfx(c);
					const coordtype cy = swfy(c);
					const coordtype dx = swfx(d);
					const coordtype dy = swfy(d);

					if (currentpoint == b) {
						if (Verbose())
							errf << "converting strange PS-curveto cp = b to simple curveto" <<
								endl;
						s->drawCurveTo(cx, cy, dx, dy);
						if (options->trace) {
							printf("s->drawCurveTo(%d,%d,%d,%d);\n", (int) cx, (int) cy,
								   (int) dx, (int) dy);
						}
						currentpoint = d;
					} else if (c == d) {
						if (Verbose())
							errf << "converting strange PS-curveto c = d to simple curveto" << endl;
						s->drawCurveTo(bx, by, dx, dy);
						if (options->trace) {
							printf("s->drawCurveTo(%d,%d,%d,%d);\n", (int) bx, (int) by,
								   (int) dx, (int) dy);
						}
					} else if ((cpy == by) && (by == cy) && (cy == dy)) {
						if (Verbose())
							errf << "converting curve to line (all same y)" << endl;
						s->drawLineTo(dx, dy);
						if (options->trace) {
							printf("s->drawLineTo(%d,%d);\n", (int) dx, (int) dy);
						}
					} else if ((cpx == bx) && (bx == cx) && (cx == dx)) {
						if (Verbose())
							errf << "converting curve to line (all same x)" << endl;
						s->drawLineTo(dx, dy);
						if (options->trace) {
							printf("s->drawLineTo(%d,%d);\n", (int) dx, (int) dy);
						}
					} else {
						if (options->cubic) {
							if (options->trace) {
								printf("s->drawCubic(%d,%d,%d,%d,%d,%d);\n", (int) bx, (int) by,
									   (int) cx, (int) cy, (int) dx, (int) dy);
								fflush(stdout);
							}
							s->drawCubicTo(bx, by, cx, cy, dx, dy);
						} else {
							const bool approx = true;
							if (approx) {
								const unsigned int fitpoints = 10;
								const Point & cp1 = elem.getPoint(0);
								const Point & cp2 = elem.getPoint(1);
								const Point & ep = elem.getPoint(2);
								for (unsigned int sf = 0; sf < fitpoints; sf++) {
									const float t = 1.0f * sf / (fitpoints - 1);
									const Point & p_at_t =
										PointOnBezier(t, currentpoint, cp1, cp2, ep);
									const coordtype dx1 = swfx(p_at_t);
									const coordtype dy1 = swfy(p_at_t);
									s->drawLineTo(dx1, dy1);
									if (options->trace) {
										printf("s->drawLineTo(%d,%d);\n", (int) dx1, (int) dy1);
									}
								}
							} else {
//
// see http://www.timotheegroleau.com/Flash/articles/cubic_bezier_in_flash.htm for a detailled discussion
//
#if 0
// very basic approx - use intersection point of the cubic bezier tangents as control point
// for the quadratic bezier
								/*

if x1 == x2; xs = x1;
if x3 == x4; xs = x3;

ys - y1   y2 - y1
------ = ------- = m1
xs - x1   x2 - x1

ys - y3   y4 - y3
------ = ------- = m2
xs - x3   x4 - x3


ys = y1 + m1 * (xs - x1) = y3 + m2 * (xs - x3)

xs = (( y1 - m1*x1) - (y3 - m2*x3))/ ( m2 - m1)
ys = y1 + m1 * (xs - x1)  
								*/
								if (bx != cpx) {
									const coordtype m1 = (by - cpy) / (bx - cpx );
									if (dx != cx) {
										const coordtype m2 = (dy -  cy) / (dx -  cx );
										if (m1 != m2) {
											const coordtype xs = (( cpy - m1*cpx) - (cy - m2*cx))/(m2 - m1 );
											const coordtype ys = cpy + m1 * (xs - cpx);
											s->drawCurveTo(xs, ys, dx, dy);
											if (options->trace) {
												printf("s->drawCurveTo(%d,%d,%d,%d);\n", (int) xs, (int) ys,
													(int) dx, (int) dy);
											}
										} else {
											cout << "strange bezier with parallel control lines" << endl;
										}
									} else {
										// dx == cx ; vertical second control line and first is not vertical
										const coordtype xs = dx; // intersection is also on same coordinate
										const coordtype ys = cpy + m1 * (xs - cpx);
										s->drawCurveTo(xs, ys, dx, dy);
										if (options->trace) {
											printf("s->drawCurveTo(%d,%d,%d,%d);\n", (int) xs, (int) ys,
												(int) dx, (int) dy);
										}
									}
								} else {
									// bx != cpx ; first control line is vertical								 
									if (dx != cx) {
										// second is not vertical
										const coordtype xs = bx ;
										const coordtype m2 = (dy -  cy) / (dx -  cx );
										const coordtype ys = cy + m2 * (xs - cx);
										s->drawCurveTo(xs, ys, dx, dy);
										if (options->trace) {
											printf("s->drawCurveTo(%d,%d,%d,%d);\n", (int) xs, (int) ys,
												(int) dx, (int) dy);
										}
									} else {
										cout << "strange bezier with parallel vertical control lines" << endl;
									}
								}
#endif
#if 1
								// simple midpoint approach - use mid between the two ctrl points as new anchor point
								const coordtype ax = (bx + cx )/2.0f;
								const coordtype ay = (by + cy )/2.0f;

									s->drawCurveTo(bx, by, ax, ay);
										if (options->trace) {
											printf("s->drawCurveTo(%d,%d,%d,%d);\n", (int) bx, (int) by,
												(int) ax, (int) ay);
										}
									s->drawCurveTo(cx, cy, dx, dy);
										if (options->trace) {
											printf("s->drawCurveTo(%d,%d,%d,%d);\n", (int) cx, (int) cy,
												(int) dx, (int) dy);
										}

#endif
/*
								s->drawLineTo(bx, by);
								if (options->trace) {
									printf("s->drawLineTo(%d,%d);\n", (int) bx, (int) by);
								}
								s->drawLineTo(cx, cy);
								if (options->trace) {
									printf("s->drawLineTo(%d,%d);\n", (int) cx, (int) cy);
								}
								s->drawLineTo(dx, dy);
								if (options->trace) {
									printf("s->drawLineTo(%d,%d);\n", (int) dx, (int) dy);
								}
*/
							}
						}
					}
					currentpoint = d;
				} else {
					errf << "\t\tFatal: unexpected case in drvSWF " << endl;
					abort();
				}
			}
			break;
		default:
			errf << "\t\tFatal: unexpected case in drvSWF " << endl;
			abort();
			break;
		}

	}

	s->end();
	SWFDisplayItem *d = movie->add(s);
	d->move(0.0f, 0.0f);

	if (options->trace)
		printf("s->end();\n" "SWFDisplayItem * d = movie->add(s);\n" "d->move(0, 0); }\n");

// delete s;
// delete d; // no longer needed in libming version > 0.4.0.beta6

}


void drvSWF::open_page()
{
//  outf << "Opening page: " << currentPageNumber << endl;
}

void drvSWF::close_page()
{
//  outf << "Closing page: " << (currentPageNumber) << endl;
}



void drvSWF::show_text(const TextInfo & textinfo)
{
	RSString fonthome(drvbase::pstoeditDataDir());

	if (drvbase::pstoeditDataDir() != "") {
		fonthome += directoryDelimiter;
		fonthome += "swffonts";
		fonthome += directoryDelimiter;
	}

	RSString fontfilename = fonthome;
	fontfilename += textinfo.currentFontName.c_str();
	fontfilename += ".fdb";

	const char *const fontname = textinfo.currentFontName.c_str();
	if (fileExists(fontfilename.c_str())) {
		if (Verbose()) {
			errf << "loading font from from " << fontfilename.c_str() << endl;
		}
	} else {
		RSString defaultfontname = fonthome;
		defaultfontname += "default.fdb";
		if (fileExists(defaultfontname.c_str())) {
			if (Verbose())
				errf << "no fdb file found for font " << fontname << ". Using " <<
					defaultfontname.c_str() << " instead" << endl;
			fontfilename = defaultfontname;
		} else {
			errf << "no fdb file found for font " << fontname <<
				" and no " << defaultfontname << " either - text ignored." << endl;
			return;
		}
	}

	// if the fontnames ends with .fdb, then the file is read, otherwise browser fonts are used.

	//as long as SWF is not const correct SWFFont *f = new SWFFont(fontfilename.c_str());
	auto f = new SWFFont(const_cast<char *>(fontfilename.c_str()));
	if ((f == nullptr) || (f->font == nullptr)) {
		errf << "Loading font " << fontfilename.c_str() << " failed !" << endl;
		delete f;
		return;
	}

	auto t = new SWFText;
	t->setFont(f);

	// t->moveTo(20 *(textinfo.x + x_offset), 20*(currentDeviceHeight - textinfo.y + y_offset));
	t->setHeight(textinfo.currentFontSize * swfscale);

//  t->setColor(0xff, 0xff, 0);
	t->setColor((unsigned char) (255.0 * textinfo.currentR),
				(unsigned char) (255.0 * textinfo.currentG),
				(unsigned char) (255.0 * textinfo.currentB), 0xff);
	t->addString(textinfo.thetext.c_str(), nullptr);
	// t->setSpacing( 0.5);

/*
  t->setXY( -1, 480);
  t->setHeight( 480);
  t->setSpacing( 1.0);
  t->addString( ( char*) "blargghghgghghgh", nullptr);
*/

	SWFDisplayItem *d = movie->add(t);
#if 0
	const Point p(textinfo.x, textinfo.y);
	d->move(swfx(p), swfy(p));
	d->rotate(textinfo.currentFontAngle);
#else
	const float *CTM = getCurrentFontMatrix();
	const float fontsize = textinfo.currentFontSize;
	const float ma = CTM[0] / fontsize;
	const float mb = -CTM[1] / fontsize;
	const float mc = -CTM[2] / fontsize;
	const float md = CTM[3] / fontsize;

	const Point p(CTM[4], CTM[5]);
	const float mx = swfx(p);
	const float my = swfy(p);

	d->move(0.0f, 0.0f);
	SWFDisplayItem_setMatrix(d->item, ma, mb, mc, md, mx, my);

#endif
	// delete d;  // no longer needed in libming version > 0.4.0.beta6
	// delete f;  // causes memory problems otherwise - at the cost of leaks - Hmmm
	// delete t;


}

void drvSWF::show_path()
{
//static int pathid = 0;

//if ( (pathid >= 0)&&(pathid++ < 5) ) 
	{

		print_coords();
	}

}

#if defined(HAVE_LIBMINGUTIL) || ( defined(USE_PNG) && USE_PNG )
void drvSWF::show_image(const PSImage & imageinfo)
{

	if (outBaseName == "") {
		errf << "images cannot be handled via standard output. Use an output file" << endl;
		return;
	}


	if (imageinfo.isFileImage) {
		// use imageinfo.FileName;
#if 0
		outf << "<image "		// x=\"" << 0 << "\" y=\"" << 0 << "\"" 
			<< " transform=\"matrix("
			<< imageinfo.normalizedImageCurrentMatrix[0] << ' '
			<< /* - */ -imageinfo.normalizedImageCurrentMatrix[1] << ' '
			// doch doch - zumindest bei im.ps 
			// - no longer needed due to normalization in pstoedit.pro
			<< imageinfo.normalizedImageCurrentMatrix[2] << ' '
			<< -imageinfo.normalizedImageCurrentMatrix[3] << ' '
// transfer
			<< imageinfo.normalizedImageCurrentMatrix[4] << ' '
			<< currentDeviceHeight - imageinfo.normalizedImageCurrentMatrix[5]
			<< ")\"" << " width=\"" << imageinfo.
			width << "\"" << " height=\"" << imageinfo.
			height << "\"" << " xlink:href=\"" << imageinfo.FileName << "\"></image>" << endl;

#endif

#if ( defined(USE_PNG) && USE_PNG )
		// from 0.3 on ming may support png directly
		auto bm = new SWFBitmap(imageinfo.FileName.c_str());
#else
		unsigned int len = strlen(imageinfo.FileName.c_str());
		char *outfile = cppstrdup(imageinfo.FileName.c_str());
		outfile[len - 3] = 'd';
		outfile[len - 2] = 'b';
		outfile[len - 1] = 'l';
		png2dbl(imageinfo.FileName.c_str(), outfile);
		SWFBitmap *bm = new SWFBitmap(outfile);
		delete [] outfile;
#endif
		(void) remove(imageinfo.FileName.c_str());

		auto s = new SWFShape;
		SWFFill *swffill = s->addBitmapFill(bm, SWFFILL_TILED_BITMAP);
		s->setRightFill(swffill);

		const float h = (float) bm->getHeight();
		const float w = (float) bm->getWidth();
		s->movePen(0.0f, 0.0f);
		s->drawLine(w, 0.0f);
		s->drawLine(0.0f, h);
		s->drawLine(-w, 0.0f);
		s->drawLine(0.0f, -h);

		/// cout << " h:" << bm->getHeight() << " w:" << bm->getWidth() << endl;
		// SWFDisplayItem *d = movie->add(bm);
		s->end();
		SWFDisplayItem *d = movie->add(s);

		const float *CTM = imageinfo.normalizedImageCurrentMatrix;
		const float ma = CTM[0];
		const float mb = -CTM[1];
		const float mc = CTM[2];
		const float md = -CTM[3];

		const Point p(CTM[4], CTM[5]);
		const float mx = swfx(p);
		const float my = swfy(p);


#if 1
		d->move(0.0f, 0.0f);
		SWFDisplayItem_setMatrix(d->item, ma, mb, mc, md, mx, my);
//          d->item->flags |= ITEM_DISPLACED;
//  d->item->flags |= ITEM_TRANSFORMED;

#else
		const Point p(CTM[4], CTM[5]);
		d->move(swfx(p), swfy(p));
		//  d->rotate(30);
		d->scale(ma, -md);
#endif



	} else {

		errf << "unhandled case for image " << endl;
	}

}
#else

// 
// If neither USE_PNG is defined in ming_config.h nor we have access to util/png2dbl.c we cannot support images
//
void drvSWF::show_image(const PSImage & /* imageinfo */ ) {
	errf << "no support for bitmaps - need to have access to util/png2dbl.c at compile time or at least ming version 0.3 which direct support of png (USE_PNG in ming/config.h)" << endl;
}
#endif

static DriverDescriptionT < drvSWF > D_SWF("swf",	//
										   "SWF driver: ",	//
										   "",
										   "swf",	//
										   false,	// backend supports subpaths
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
										   withcurves,	// backend supports curves
										   true,	// backend supports elements which are filled and have edges
										   true,	// backend supports text
#if defined(HAVE_LIBMINGUTIL) || ( defined(USE_PNG) && USE_PNG )
										   DriverDescription::imageformat::png,	// backend supports Images
#else
   										   DriverDescription::imageformat::noimage,	// no support for Images
#endif
										   DriverDescription::opentype::noopen,	// we open output file ourselves
										   true,	// if format supports multiple pages in one file
										   false	/*clipping */
										   );
#endif
 
