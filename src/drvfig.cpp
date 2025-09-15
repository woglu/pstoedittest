/*
   drvFIG.cpp : This file is part of pstoedit
   Based on the skeleton for the implementation of new backends

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

/*
    FIG 3.1 driver by Ian MacPhedran (Ian_MacPhedran_AT_engr.usask.ca)
    April 1995

    Color support and conversion to use C++ streams done by Wolfgang Glunz

    Object depth support by Gerhard Kircher <kircher_AT_edvz.tuwien.ac.at>
    March 1996

    Curves and image support by Leszek Piotrowicz <zorro_AT_delta.ds2.pg.gda.pl>
    July 1997

    Support for xfig format 3.2 (X-Splines) by Wolfgang Glunz

    Support for subpaths by Burkhard Plaum
    
 Change to level calculation Ian MacPhedran
 November 2000
 Simplify - remove superfluous 3.1 code Ian MacPhedran
 December 2000
 Use quintic interpolated X-Splines Ian MacPhedran
 December 2000
 Fix for font scaling - Thanks to Robert K. Nelson - Ian MacPhedran
 April 2002

*/

#include "drvfig.h"

// for sprintf
#include I_stdio
#include I_string_h
#include I_iostream
#include I_iomanip
#include <memory>


/*
A fig file that contains the option "Inches" in the preamble has the resolution set to 1200 units/inch. 
In fact, the resolution is also given in the fig file, but the 1200 is never changed. 
With the option "Metric", the resolution is understood, by both Xfig and fig2dev, to be 450 /cm, 
although the default resolution of 1200 still remains in the file. 
The [format specification]( http://mcj.sourceforge.net/fig-format.html) gives a hint about this. 
Xfig writes "Metric" and "1200" to the fig file, fig2dev reads it, and scales a 450 units long line by approx. 
5% such that it has, on output, the correct length of 472.44/1200 in.
With the -metric option to the fig driver, pstoedit should do the same.
*/
static float PntFig; 


static const char *colorstring(float r, float g, float b)
{
	static char buffer[15];
	sprintf_s(TARGETWITHLEN(buffer,15), "%s%.2x%.2x%.2x", "#", (unsigned int) (r * 255 + 0.5),
			(unsigned int) (g * 255 + 0.5), (unsigned int) (b * 255 + 0.5));

	return buffer;
}

static const char *fig_default_colors[] = {
	"#000000",
	"#0000ff",
	"#00ff00",
	"#00ffff",
	"#ff0000",
	"#ff00ff",
	"#ffff00",
	"#ffffff",
	"#000090",
	"#0000b0",
	"#0000d0",
	"#87ceff",
	"#009000",
	"#00b000",
	"#00d000",
	"#009090",
	"#00b0b0",
	"#00d0d0",
	"#900000",
	"#b00000",
	"#d00000",
	"#900090",
	"#b000b0",
	"#d000d0",
	"#803000",
	"#a04000",
	"#c06000",
	"#ff8080",
	"#ffa0a0",
	"#ffc0c0",
	"#ffe0e0",
	"#ffd700"
};

const int defaults = sizeof(fig_default_colors) / sizeof(char *);
static ColorTable colorTable(fig_default_colors, defaults, colorstring);

static unsigned int registercolor(float r, float g, float b)
{
	return colorTable.getColorIndex(r, g, b);
}

static void dumpnewcolors(ostream & theoutStream)
{
	unsigned int current = defaults;
	const char *colstring = nullptr;
	while ((colstring = colorTable.getColorString(current)) != nullptr) {
		theoutStream << "0 " << current << " " << colstring << endl;
		current++;
	}
}


drvFIG::derivedConstructor(drvFIG):
constructBase,
buffer(tempFile.asOutput()),
imgcount(1),
//format(32),
//startdepth(999),
//use_correct_font_size(false),
glob_min_x(0), glob_max_x(0), glob_min_y(0), glob_max_y(0),
loc_min_x(0), loc_max_x(0), loc_min_y(0), loc_max_y(0), glo_bbox_flag(0), loc_bbox_flag(0)

{
	const char *const units = (options->metric) ? "Metric" : "Inches";
	PntFig = ((options->metric) 
	            ? (450.0f * 2.54f)
		    : 1200.0f
		 )/ 72.0f;

	// Set the papersize
	const char *const paper_size  = (options->depth_in_inches <= 11.0 ? "Letter" : "A4");

	// set FIG specific values
	currentDeviceHeight = options->depth_in_inches * 1200.0f ;
	// We use objectId as depth value.
	// We need this because editing will reorder objects of equal depth,
	// which has an undesireable effect if objects overlap.
	// Depth must be in the range 0..999 (FIG 3.1).
	// If we have more than 1000 objects this will get negative and
	// xfig will set negative depth values to zero.
	// This feature will thus become useless if we have more
	// than 1000 objects. This is an xfig limitation.
	objectId = options->startdepth + 1;	// +1 because a predecrement is done when used

	x_offset = 0.0;
	y_offset = currentDeviceHeight;
	// output buffer, needed because
	// color entries must be written at
	// top of output file, but are known
	// only after processing the full input

	// print the header part
	outf << "#FIG 3.2\nPortrait\nFlush left\n" << units << "\n" << paper_size <<
		"\n100.00\nSingle\n0\n1200 2\n";
}

drvFIG::~drvFIG()
{
	dumpnewcolors(outf);
	// now we can copy the buffer the output
	ifstream & inbuffer = tempFile.asInput();
	copy_file(inbuffer, outf);
	options=nullptr;
}


unsigned int
 drvFIG::nrOfCurvetos() const
{
	unsigned int nr = 0;
	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		const basedrawingelement & elem = pathElement(n);
		if (elem.getType() == curveto)
			nr++;
	}
	return nr;
}

void drvFIG::prpoint(ostream & os, const Point & p, bool withspaceatend) const
{
	os << (int) (PntFig * p.x() + 0.5f) << " " << (int) (y_offset - (PntFig * p.y()) + 0.5f);
	if (withspaceatend)
		os << " ";
}


 // static int nd_calls=0; Debug

// bounding box
void drvFIG::new_depth()
{
// errf <<  "Called #" << nd_calls++ << "\n"; Debug
	if (glo_bbox_flag == 0) {
		glob_max_y = loc_max_y;
		glob_min_y = loc_min_y;
		glob_max_x = loc_max_x;
		glob_min_x = loc_min_x;
		glo_bbox_flag = 1;
	} else {
		if ((loc_max_y <= glob_min_y) ||
			(loc_min_y >= glob_max_y) || (loc_max_x <= glob_min_x) || (loc_min_x >= glob_max_x)) {
// outside global bounds, increase global box
			if (loc_max_y > glob_max_y)
				glob_max_y = loc_max_y;
			if (loc_min_y < glob_min_y)
				glob_min_y = loc_min_y;
			if (loc_max_x > glob_max_x)
				glob_max_x = loc_max_x;
			if (loc_min_x < glob_min_x)
				glob_min_x = loc_min_x;
		} else {
// inside global bounds, decrease depth and create new bounds
//      errf << " Overlap detected\n" 
//          << " Local " << loc_min_x << ", " << loc_min_y
//          << ", " << loc_max_x << ", " << loc_max_y << "\n"
//          << "Global " << glob_min_x << ", " << glob_min_y 
//          << ", " << glob_max_x << ", " << glob_max_y << "\n"; Debug
			glob_max_y = loc_max_y;
			glob_min_y = loc_min_y;
			glob_max_x = loc_max_x;
			glob_min_x = loc_min_x;
			if (objectId)
				objectId--;		// don't let it get < 0
//      errf << " Depth increased " << objectId << "\n"; Debug
		}
	}
	loc_bbox_flag = 0;
}

void drvFIG::addtobbox(const Point & p)
{
	if (loc_bbox_flag == 0) {
		loc_max_y = p.y();
		loc_min_y = p.y();
		loc_max_x = p.x();
		loc_min_x = p.x();
		loc_bbox_flag = 1;
	} else {
		if (loc_max_y < p.y())
			loc_max_y = p.y();
		if (loc_min_y > p.y())
			loc_min_y = p.y();
		if (loc_max_x < p.x())
			loc_max_x = p.x();
		if (loc_min_x > p.x())
			loc_min_x = p.x();
	}
}


void drvFIG::print_polyline_coords()
{
	int j = 0;
	//  const Point & p;
	const unsigned int last = numberOfElementsInPath() - 1;
	for (unsigned int n = 0; n <= last; n++) {
		const basedrawingelement & elem = pathElement(n);
		if (j == 0) {
			buffer << "\t";
		}
		switch (elem.getType()) {
		case lineto:
		case moveto:
			{
				const Point & p = pathElement(n).getPoint(0);
				prpoint(buffer, p, (n != last));
			}
			break;
		case closepath:
			{
				const Point & p = pathElement(0).getPoint(0);
				prpoint(buffer, p, (n != last));
			}
			break;
		case curveto:
		default:
			errf << "\t\tFatal: unexpected case in drvfig " << endl;
			abort();
			break;

		}

		j++;
		if (j == 5) {
			j = 0;
			buffer << "\n";
		}
	}
	if (j != 0) {
		buffer << "\n";
	}
}

// print edge points
void drvFIG::print_spline_coords1()
{
// IJMP - need curr_point
	Point P1;
	int j = 0;
	const unsigned int last = numberOfElementsInPath() - 1;
	for (unsigned int n = 0; n <= last; n++) {
		if (j == 0) {
			buffer << "\t";
		}
		const basedrawingelement & elem = pathElement(n);
		switch (elem.getType()) {
		case moveto:
		case lineto:
			{
				const Point & p = elem.getPoint(0);
				prpoint(buffer, p, (n != last));
				P1 = p;
			}
			j++;
			if (j == 5) {
				j = 0;
				buffer << "\n";
			}
			break;
		case curveto:
			{
// IJMP - change to quintic spline
				// put all points, middle points will have control value 1
// IJMP = do bezier fit
				
				const Point & P2 = elem.getPoint(0);
				const Point & P3 = elem.getPoint(1);
				const Point & P4 = elem.getPoint(2);

				for (int cp = 1; cp <= 5; cp++) {
					const Point p = PointOnBezier((cp * 0.2f), P1, P2, P3, P4);
					// p.x_ = bezpnt((cp * 0.2f), P1.x_, P2.x_, P3.x_, P4.x_);
					// p.y_ = bezpnt((cp * 0.2f), P1.y_, P2.y_, P3.y_, P4.y_);
					prpoint(buffer, p, !((n == last) && (cp == 5)));
					j++;
					if (j == 5) {
						j = 0;
						buffer << "\n";
					}
//                      if ((j == 0) && (cp != 5) && ( n+1 != (numberOfElementsInPath()))) { buffer << "\t"; }
					if ((j == 0) && (n != (numberOfElementsInPath()))) {
						buffer << "\t";
					}
				}
				P1 = P4;
			}
			break;
		case closepath:
			{
				const Point & p = pathElement(0).getPoint(0);
				P1 = p;
				prpoint(buffer, p, (n != last));
			}
			j++;
			if (j == 5) {
				j = 0;
				buffer << "\n";
			}
			break;
		default:
			errf << "\t\tFatal: unexpected case in drvfig " << endl;
			abort();
			break;
		}
	}
	if (j != 0) {
		buffer << "\n";
	}
	buffer << "\t";
}

// print control points
void drvFIG::print_spline_coords2()
{
	int j = 0;
	Point lastp;
	const int maxj = 8;

	const unsigned int last = numberOfElementsInPath() - 1;
	for (unsigned int n = 0; n <= last; n++) {
		const basedrawingelement & elem = pathElement(n);
		switch (elem.getType()) {
		case moveto:
			{
				buffer << " 0";
				if (n != last)
					buffer << " ";
				j++;
				if (j == maxj) {
					j = 0;
					buffer << "\n";
					if ((n + 1) != numberOfElementsInPath()) {
						buffer << "\t";
					}
				}
				lastp = elem.getPoint(0);
			}
			break;
		case lineto:
			{
				buffer << " 0";
				if (n != last)
					buffer << " ";
				j++;
				if (j == maxj) {
					j = 0;
					buffer << "\n";
					if ((n + 1) != numberOfElementsInPath()) {
						buffer << "\t";
					}
				}
			}
			break;
		case closepath:
			{
				buffer << " 0";
				if (n != last)
					buffer << " ";
				j++;
				if (j == maxj) {
					j = 0;
					buffer << "\n";
					if ((n + 1) != numberOfElementsInPath()) {
						buffer << "\t";
					}
				}
			}
			break;
		case curveto:
			{
// IJMP - change to quintic spline
				// put all points, middle points will have control value -1
				float kp = 0.0;
				for (unsigned int i = 0; i < 5; i++) {
					if (i == 1) {
						kp = -1.0; //lint !e736
					}
					if (i == 4) {
						kp = 0.0;
					}
					buffer << " " << kp;
					if (!((n == last) && (i == 4)))
						buffer << " ";
					j++;
					if (j == maxj) {
						j = 0;
						buffer << "\n";
						if (!((i == 4)
							  && ((n + 1) == numberOfElementsInPath()))) {
							buffer << "\t";
						}
					}
				}
				lastp = elem.getPoint(2);
			}
			break;
		default:
			errf << "\t\tFatal: unexpected case in drvfig " << endl;
			abort();
			break;
		}
	}
	if (j != 0) {
		buffer << endl;
	}
}

void drvFIG::close_page()
{
// Well, since FIG doesn't support multipage output, we'll move down
// 11 inches and start again.
	y_offset += currentDeviceHeight;
	// reset depth counter
	objectId = options->startdepth + 1;	// also changed to 499
}

void drvFIG::open_page()
{
}

struct FontTableType { 
	int index; 
	const char * fontname; 
};
	//
	// FIG 3.1 uses an index for the popular fonts:
	//
	// (we cannot make this array local to drvFIG::show_textstring
	// because some CCs don't support that yet.

static const FontTableType FigPSFonts[] = {
	{0,"Times-Roman"}, 
	{1,"Times-Italic"}, 
	{2,"Times-Bold"}, 
	{3,"Times-BoldItalic"},
	{4,"AvantGarde-Book"}, 
	{5,"AvantGarde-BookOblique"}, 
	{6,"AvantGarde-Demi"},
	{7,"AvantGarde-DemiOblique"}, 
	{8,"Bookman-Light"}, 
	{9,"Bookman-LightItalic"},
	{10,"Bookman-Demi"}, 
	{11,"Bookman-DemiItalic"}, 
	{12,"Courier"}, 
	{13,"Courier-Oblique"},
	{14,"Courier-Bold"}, 
	{15,"Courier-BoldOblique"}, 
	{16,"Helvetica"},
	{17,"Helvetica-Oblique"},
	{18,"Helvetica-Bold"}, 
	{19,"Helvetica-BoldOblique"}, 
	{20,"Helvetica-Narrow"},
	{21,"Helvetica-Narrow-Oblique"}, 
	{22,"Helvetica-Narrow-Bold"},
	{23,"Helvetica-Narrow-BoldOblique"}, 
	{24,"NewCenturySchlbk-Roman"},
	{25,"NewCenturySchlbk-Italic"}, 
	{26,"NewCenturySchlbk-Bold"},
	{27,"NewCenturySchlbk-BoldItalic"}, 
	{28,"Palatino-Roman"},
	{29,"Palatino-Italic"}, 
	{30,"Palatino-Bold"}, 
	{31,"Palatino-BoldItalic"},
	{32,"Symbol"}, 
	{33,"ZapfChancery-MediumItalic"}, 
	{34,"ZapfDingbats"}
};

static const size_t MaxPSFntnum = sizeof(FigPSFonts) / sizeof(FontTableType) - 1;

/*
	For font_flags bit 2 = 0 (LaTeX fonts):

	 0	Default font
	 1	Roman
	 2	Bold
	 3	Italic
	 4	Sans Serif
	 5	Typewriter
*/
	static const FontTableType FigLaTeXFonts[] = {
		{0,"Default font"},
		{0,"Default"},
		{0,"Defaultfont"},
		{1,"Roman"},
		{2,"Bold"},
		{3,"Italic"},
		{4,"Sans Serif"},
		{4,"Sans-Serif"},
		{4,"SansSerif"},
		{4,"Sansserif"},
		{5,"Typewriter"}
	};

static const size_t MaxLaTeXFntnum = sizeof(FigLaTeXFonts) / sizeof(FontTableType) - 1;


static int getfigFontnumber(const char *fname,const FontTableType *FigFonts, unsigned int MaxFntNum)
{
	const size_t fntlength = strlen(fname);
	for ( unsigned int i = 0; i <= MaxFntNum; i++) {
		if (fntlength == strlen(FigFonts[i].fontname)) {
			if (strncmp(fname, FigFonts[i].fontname, fntlength) == 0)
				return FigFonts[i].index;
		}
	}
	return -1;
}



void drvFIG::show_text(const TextInfo & textinfo)
{
	const float toRadians = 3.14159265359f / 180.0f;

/*
	The font_flags field is defined as follows:

	 Bit	Description

	  0	Rigid text (text doesn't scale when scaling compound objects)
	  1	Special text (for LaTeX)
	  2	PostScript font (otherwise LaTeX font is used)
	  3	Hidden text

*/

/*
Notation for font names:

((LaTeX|PostScript|<empty>)(::special)::)FontName

Examples:

LaTeX::Fontname
LaTeX::special::Fontname

FontName
PostScript::special::Fontname
::special::Fontname (same as PostScript::special::Fontname)


*/

	int fontflags = 4; // fontflags (bit vector)
	// 4 is 0100 - PostScript

	int FigFontNum = 0;
	const char * const specialindex = strstr(textinfo.currentFontName.c_str(),"::special::");
	const bool special = (specialindex != nullptr);
	if (!strncmp(textinfo.currentFontName.c_str(),"LaTeX::",7) ) {
		// it is a LaTeX Font
		fontflags = special ? 2 : 0 ; // 0010 or 0000  - LaTeX
		const char* fontname = special ? (specialindex + 11) : ( textinfo.currentFontName.c_str() + 7);
		FigFontNum = getfigFontnumber(fontname,FigLaTeXFonts,MaxLaTeXFntnum);
		// debug cout << "LaTeX::" << (const char *) (special ? "special" : "normal") << "::"<< fontname << " " <<textinfo.currentFontName.c_str() << endl;
		if (FigFontNum == -1) {
			errf << "Warning, unsupported font " << fontname << ", using LaTeX default instead.";
			FigFontNum = 0;
		}
	} else {
		const char * fontname = textinfo.currentFontName.c_str();
		if (!strncmp(textinfo.currentFontName.c_str(),"PostScript::",12) ) {
			fontname+=12; // just skip "PostScript::"
		}
		if (special) { fontname+=11 ; } // just skip "::special::"
		fontflags = special ? 6 : 4; // 0110 or 0100 - PostScript
		// debug cout << "PostScript::" << (const char *) (special ? "special" : "normal") << "::" << fontname<< " " <<textinfo.currentFontName.c_str() << endl;
		FigFontNum = getfigFontnumber(fontname,FigPSFonts,MaxPSFntnum);
		if (FigFontNum == -1) {
			errf << "Warning, unsupported font " << fontname << ", using ";
			FigFontNum = getfigFontnumber(defaultFontName,FigPSFonts,MaxPSFntnum);
			if (FigFontNum != -1) {
				errf << defaultFontName;
			} else {
				if (strstr(fontname, "Bold") == nullptr) {
					if (strstr(fontname, "Italic") == nullptr) {
						errf << "Times-Roman";
						FigFontNum = 0;	// Times-Roman
					} else {
						FigFontNum = 1;
						errf << "Times-Italic";
					}
				} else {
					if (strstr(fontname, "Italic") == nullptr) {
						errf << "Times-Bold";
						FigFontNum = 2;	// Times-Bold
					} else {
						FigFontNum = 3;
						errf << "Times-BoldItalic";
					}
				}
			}
			errf << " instead." << endl;
		}
	}

	const unsigned int color = registercolor(textinfo.currentR, textinfo.currentG,
									textinfo.currentB);
	float localFontSize = textinfo.currentFontSize;
	if (localFontSize <= 0.1) {
		localFontSize = 9;
	}
	if (!options->use_correct_font_size) {
		// formerly xfig used scaled font sizes and not the X11 font sizes
		localFontSize = (((float)localFontSize * 80.0f) / 72.0f) + 0.5f; 
	}

	const float FigHeight = PntFig * localFontSize;
	const float FigLength = FigHeight * strlen(textinfo.thetext.c_str());
	const float PSHeight = localFontSize;
	const float PSLength = PSHeight * strlen(textinfo.thetext.c_str());
// Calculate BBox
	if (textinfo.currentFontAngle == 0) {
		addtobbox(textinfo.p);
		addtobbox(Point((textinfo.x() + PSLength), (textinfo.y() + PSHeight)));
	} else if (textinfo.currentFontAngle == 90) {
		addtobbox(textinfo.p);
		addtobbox(Point((textinfo.x() - PSHeight), (textinfo.y() + PSLength)));
	} else if (textinfo.currentFontAngle == 180) {
		addtobbox(textinfo.p);
		addtobbox(Point((textinfo.x() - PSLength), (textinfo.y() - PSHeight)));
	} else if (textinfo.currentFontAngle == 270) {
		addtobbox(textinfo.p);
		addtobbox(Point((textinfo.x() + PSHeight), (textinfo.y() - PSLength)));
	} else {
// To simplify this, a box of width 2*PSLength centered on textinfo.(x,y) used
		addtobbox(Point((textinfo.x() - PSLength), (textinfo.y() + PSLength)));
		addtobbox(Point((textinfo.x() + PSLength), (textinfo.y() + PSLength)));
		addtobbox(Point((textinfo.x() - PSLength), (textinfo.y() - PSLength)));
		addtobbox(Point((textinfo.x() + PSLength), (textinfo.y() - PSLength)));
	}
	buffer << "# text\n";
	new_depth();

	
	buffer << "4 0 "; // 4 means text, 0 left justified
	buffer << color;
	if (objectId)
		objectId--;				// don't let it get < 0
	buffer << " " << objectId // depth
		<< " -1 " // pen_style - not used
		<< FigFontNum << " "
		<< (int) localFontSize << " "
		<< textinfo.currentFontAngle * toRadians 
		<< " " << fontflags << " "  
		<< FigHeight << " "
		<< FigLength << " "
		<< (int) (PntFig * textinfo.x() + 0.5f) << " "
		<< (int) (y_offset - (PntFig * textinfo.y()) + 0.5f) << " " << textinfo.thetext.c_str() << "\\001\n";
}

void drvFIG::bbox_path()
{
	for (unsigned int i = 0; i < numberOfElementsInPath(); i++) {
		const basedrawingelement & elem = pathElement(i);
		switch (elem.getType()) {
		case curveto:
			{
				addtobbox(elem.getPoint(0));
				addtobbox(elem.getPoint(1));
				addtobbox(elem.getPoint(2));
				break;
			}
		case moveto:
		case lineto:
			{
				addtobbox(elem.getPoint(0));
				break;
			}
		case closepath:
		default:
			{					// will get caught later
				break;
			}
		}
	}
	new_depth();
}

void drvFIG::show_path()
{
	float localLineWidth = currentLineWidth();
	localLineWidth *= 80.0f/72.0f; // xfig scales width differently - added in 3.50 - wogl
	// don't know whether this should be synchronized with -usecorrectfontsize option.



	/*
	3.50:
Originally, the resolution of Fig lines was 1/80 inch, and it was thought that the line width of "1" looked too thick, so it was reduced.
Therefore, the final width of lines in PostScript/EPS is:

Figwidth == 1: 7.5
Figwidth > 1: (Figwidth-1)*15

Regards,
Brian Smith

in reverse:

l < (15+7.5)/2 : f = 1
f = (l /15) + 1

	*/

	const float boundaryforOneCase = (((15.0f + 7.5f)/2.0f) / 15.0f);  // 0.75
	// if calculated linewidth is (without "+1 correction") > 0.75 
	// then apply correction by 1 
	// for the 0.75 case itself it means - map it to 1.75 and from there to 2
	// if it is < 0.75, then leave it so and create in fig a 1 ( (int) 0.75+0.5 )
	if (Verbose() ) {
		cerr << "localLineWidth " << localLineWidth  << " b " << boundaryforOneCase << endl;
	}
	if (localLineWidth > boundaryforOneCase) { 
		localLineWidth += 1.0f; // see above
	} else {
		// line width of 0 remain 0 - everything else is at least 1 
		if ((localLineWidth < 0.0) || ((localLineWidth > 0.0) && (localLineWidth <= 1.0))) {
			localLineWidth = 1.0;
		} 
	}

	unsigned int linestyle = 0;
	switch (currentLineType()) {
	case solid:
		linestyle = 0;
		break;
	case dashed:
		linestyle = 1;
		break;
	case dotted:
		linestyle = 2;
		break;
	case dashdot:
		linestyle = 3;
		break;
	case dashdotdot:
		linestyle = 4;
		break;
	default: ; // not expected
	}

	const unsigned int linecap = currentLineCap();
	const unsigned int linejoin = currentLineJoin();
	// Calculate BBox
	bbox_path();

	const unsigned int curvetos = nrOfCurvetos();
	if (curvetos == 0)			// polyline
	{
		buffer << "# polyline\n";
		buffer << "2 1 " << linestyle << " " << (int) (localLineWidth + 0.5f) << " ";
		const unsigned int color = registercolor(currentR(), currentG(), currentB());
		const int fill_or_nofill = (currentShowType() == drvbase::stroke) ? -1 : 20;
		if (objectId)
			objectId--;			// don't let it get < 0
		buffer << color << " " << color << " " << objectId << " 0 " <<
			fill_or_nofill << " " << "4.0" << 
			" " << linejoin << " " << linecap 
			//" 0 0"
			<< " 0 0 0 ";
		// 4.0 is the gap spec. we could also derive this from the input
		buffer << (int) (numberOfElementsInPath()) << "\n";
		print_polyline_coords();
	} else						// contains at least one curveto 
	{
		buffer << "# spline\n";
		// 3 2 means "open interpolated spline"
		buffer << "3 4 " << linestyle << " " << (int) (localLineWidth + 0.5f) << " ";
		const unsigned int color = registercolor(currentR(), currentG(), currentB());
		const int fill_or_nofill = (currentShowType() == drvbase::stroke) ? -1 : 20;
		if (objectId)
			objectId--;			// don't let it get < 0
		buffer << color << " " << color << " " << objectId << " 0 " <<
			fill_or_nofill << " " << "4.0" << //" 0"
			" " << linecap 
			<< " 0 0 ";
		// 4.0 is the gap spec. we could also derive this from the input

		// IJMP - change to quintic spline - 5 pnts per spline, not 3
		buffer << numberOfElementsInPath() + 5 * curvetos - curvetos << "\n";
		print_spline_coords1();
		print_spline_coords2();

	}
}


void drvFIG::show_image(const PSImage & imageinfo)
{
	if (outBaseName == "") {
		errf << "images cannot be handled via standard output. Use an output file " << endl;
		return;
	}
	if (imageinfo.isFileImage) {
		// use imageinfo.FileName;
		/* 
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
    */

	Point ll, ur;
	imageinfo.getBoundingBox(ll, ur);
// Calculate BBox
	addtobbox(ll);
	addtobbox(ur);
	const Point fig_ur(PntFig * ur.x(), y_offset - PntFig * ll.y());
	const Point fig_ll(PntFig * ll.x(), y_offset - PntFig * ur.y());

	// first output link to an external *.eps file into *.fig file
	buffer << "# image\n";
	new_depth();
	buffer << "2 5 0 1 -1 -1 ";
	if (objectId)
		objectId--;				// don't let it get < 0
	buffer << objectId << " 0 -1 0.000 0 0 -1 0 0 5\n";
	buffer << "\t0 " << imageinfo.FileName << "\n";

	buffer << "\t" << (int) fig_ll.x() << " " << (int) fig_ll.y() << " "
		<< (int) fig_ur.x() << " " << (int) fig_ll.y() << " "
		<< (int) fig_ur.x() << " " << (int) fig_ur.y() << " "
		<< (int) fig_ll.x() << " " << (int) fig_ur.y() << " "
		<< (int) fig_ll.x() << " " << (int) fig_ll.y();
	buffer << "\n";



	} else {
	const size_t filenamelen = strlen(outBaseName.c_str()) + 21;
	std::unique_ptr<char[]> EPSoutFileName (new char[filenamelen]);
	const size_t fullfilenamelen = strlen(outDirName.c_str()) + strlen(outBaseName.c_str()) + 21;
	std::unique_ptr<char[]> EPSoutFullFileName (new char[fullfilenamelen]);

	sprintf_s(TARGETWITHLEN(EPSoutFileName.get(),filenamelen), "%s%02d.eps", outBaseName.c_str(), imgcount++);
	sprintf_s(TARGETWITHLEN(EPSoutFullFileName.get(),fullfilenamelen), "%s%s", outDirName.c_str(), EPSoutFileName.get());
	ofstream outi(EPSoutFullFileName.get());
	if (!outi) {
		errf << "Could not open file " << EPSoutFullFileName.get() << " for output";
		exit(1);
	}
	// remember, we have to flip the image from PostScript coord to fig coords
	Point ll, ur;
	imageinfo.getBoundingBox(ll, ur);
// Calculate BBox
	addtobbox(ll);
	addtobbox(ur);
	const Point fig_ur(PntFig * ur.x(), y_offset - PntFig * ll.y());
	const Point fig_ll(PntFig * ll.x(), y_offset - PntFig * ur.y());

	// first output link to an external *.eps file into *.fig file
	buffer << "# image\n";
	new_depth();
	buffer << "2 5 0 1 -1 -1 ";
	if (objectId)
		objectId--;				// don't let it get < 0
	buffer << objectId << " 0 -1 0.000 0 0 -1 0 0 5\n";
	buffer << "\t0 " << EPSoutFileName.get() << "\n";

	buffer << "\t" << (int) fig_ll.x() << " " << (int) fig_ll.y() << " "
		<< (int) fig_ur.x() << " " << (int) fig_ll.y() << " "
		<< (int) fig_ur.x() << " " << (int) fig_ur.y() << " "
		<< (int) fig_ll.x() << " " << (int) fig_ur.y() << " "
		<< (int) fig_ll.x() << " " << (int) fig_ll.y();
	buffer << "\n";



	imageinfo.writeEPSImage(outi);
	outi.close();

	}
}


static const char * const additionalDoku = 
"The xfig format driver supports special fontnames, which may be produced by using a fontmap file. "
"The following types of names are supported:\\\\"
"\n\\begin{verbatim}\n"
"General notation:\n"
"\"PostScript Font Name\" ((LaTeX|PostScript|empty)(::special)::)XFigFontName\n"
"\n"
"Examples:\n"
"\n"
"Helvetica LaTeX::SansSerif\n"
"Courier LaTeX::special::Typewriter\n"
"GillSans \"AvantGarde Demi\"\n"
"Albertus PostScript::special::\"New Century Schoolbook Italic\"\n"
"Symbol ::special::Symbol (same as PostScript::special::Symbol)\n"
"\\end{verbatim}\n"
"See also the file examplefigmap.fmp in the misc directory of the pstoedit source distribution for an example font map file for xfig. "
"Please note that the fontname has to be among those supported by xfig. "
"See - \\URL{https://mcj.sourceforge.net/fig-format.html} for a list of legal font names";

static DriverDescriptionT < drvFIG > D_fig( "fig", ".fig format for xfig", additionalDoku, "fig", false, true, true, true, DriverDescription::imageformat::memoryeps,	// no support for PNG file images
										   DriverDescription::opentype::normalopen,
										   false, false /*clipping */ );

static DriverDescriptionT < drvFIG > D_xfig("xfig", ".fig format for xfig", "", "fig", false, true, true, true, DriverDescription::imageformat::memoryeps,	// no support for PNG file images
											DriverDescription::opentype::normalopen,
											false, false /*clipping */ );
static DriverDescriptionT < drvFIG > D_tfig("tfig", ".fig format for xfig - test only version", "", "fig", false, true, true, true, 
											DriverDescription::imageformat::png,	
											DriverDescription::opentype::normalopen,
											false, false /*clipping */ );
