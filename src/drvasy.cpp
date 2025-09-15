/*
  drvASY.cpp : This file is part of pstoedit
  Backend for Asymptote files
  Contributed by: John Bowman

  Copyright (C) 1993 - 2024 Wolfgang Glunz, wglunz35_AT_geocities.com

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
#ifdef _MSC_VER
// #define _POSIX_				// for MSVC and limits.h
#endif

#include "drvasy.h"

// #include I_fstream
#include I_stdio
#include I_stdlib

#ifndef OS_WIN32_WCE
#include <sys/stat.h>
#endif

#ifndef FLT_MAX
#include <math.h>				// Need definition of FLT_MAX
#include <float.h>				// FLT_MAX on MSVC
#endif

#include <limits.h>				// PATH_MAX for MSVC
#include <ctype.h>				// Need definition of ispunct()
#include <sstream>



#if !(defined(unix) || defined(__unix__) || defined(_unix) || defined(__unix) || defined(__EMX__) || defined (NetBSD)  )
#define LINE_MAX 2048			// for MSVC
#endif
// Constructor -- Initialize variables and take other per-document actions


using std::ostringstream;
using std::string;

drvASY::derivedConstructor(drvASY):
  constructBase,
  // Initialize member variables
  prevFontName(""), prevFontWeight(""), prevR(0.0f), prevG(0.0f), prevB(0.0f),
  // Black
  prevFontAngle(FLT_MAX), prevFontSize(-1.0), prevLineWidth(0.0f),
  prevLineCap(1),		// Round line caps
  prevLineJoin(1),		// Round joins
  prevDashPattern(""),		// Solid lines
  fillmode(false),
  clipmode(false),
  evenoddmode(false),
  firstpage(true),
  imgcount(0),
  level(0)
{
  // Output copyright information
  outf << "// Converted from PostScript(TM) to Asymptote by pstoedit\n"
       << "// Asymptote 1.00 (or later) backend contributed by John Bowman\n"
       << "// pstoedit is Copyright (C) 1993 - 2024 Wolfgang Glunz"
       << " <wglunz35_AT_pstoedit.net>\n\n";
	
  outf << "import pstoedit;" << endl;
}


// Destructor -- Tell Asymptote to end the file
drvASY::~drvASY()
{
	options=nullptr;
}

void drvASY::save()
{
  while(gsavestack.size() && gsavestack.front()) {
    gsavestack.pop_front();
    outf << "gsave();" << endl;
    ++level;
    clipstack.push_back(false);
  }
}
  
void drvASY::restore() {
  while(gsavestack.size() && !gsavestack.front()) {
    gsavestack.pop_front();
    while(clipstack.size() > 0) {
      if(clipstack.back()) {
	    outf << "endclip();" << endl;
	  }
      clipstack.pop_back();
    }
    outf << "grestore();" << endl;
    if(level > 0) --level;
  }
}

// Output a path
void drvASY::print_coords()
{
  bool withinpath = false;	// true=already started plotting points
  unsigned int pointsOnLine = 0;// Number of points on the current output line
  
  save();
  
  if (fillmode || clipmode) {
    bool havecycle=false;
    bool firstpoint=false;
    for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
      const basedrawingelement & elem = pathElement(n);
      switch (elem.getType()) {
      case moveto:
	if(withinpath) {
	  if(firstpoint) {
	    firstpoint=false;
	    // ignore a spurious moveto
	    const unsigned int next=n+1;
	    if(next == numberOfElementsInPath()) break;
	    const Dtype nexttype=pathElement(next).getType();
	    if(nexttype == moveto || nexttype == closepath) break;
	  }
	} else {
	  firstpoint=true;
	  if(clipmode) {
	    outf << "beginclip(";
	    const bool pop=clipstack.size() > 0;
	    if(pop) {
	      clipstack.pop_back();
	      clipstack.push_back(true);
	    }
	  } else
	    outf << "fill(";
	}
      // fall through
      case lineto:
	{
	  const Point & p = elem.getPoint(0);
	  if(elem.getType() == lineto) {
	    if (withinpath) {
	      outf << "--";
	    } else {
	      cerr << "lineto without a moveto; ignoring" << endl;
	      break;
	    }
	  } else {
	    if (withinpath) {
	      outf << "^^";
	      firstpoint=true;
	    }
	  }
	  
	  outf << '(' << (p.x() + x_offset) << ',' << (p.y() + y_offset)
	       << ')';
	  withinpath = true;
	  havecycle=false;
	  pointsOnLine++;
	}
	break;

      case closepath:
	outf << "--cycle";
	havecycle=true;
	break;

      case curveto:
	{
	  if (!withinpath) {
	    cerr << "curveto without a moveto; ignoring" << endl;
	    break;
	  }
	  const Point & p0 = elem.getPoint(0);
	  outf << "..controls (" << (p0.x() + x_offset)
	       << ',' << (p0.y() + y_offset) << ") and (";
	  const Point & pt1 = elem.getPoint(1);
	  outf << (pt1.x() + x_offset) << ',' << (pt1.y() + y_offset) << ")..(";
	  const Point & pt2 = elem.getPoint(2);
	  outf << (pt2.x() + x_offset) << ',' << (pt2.y() + y_offset) << ')';
	  pointsOnLine += 3;
	  withinpath = true;
	  havecycle=false;
	}
	break;

      default:
	errf << "\t\tFatal: unexpected path element in drvasy" << endl;
	abort();
	break;
      }
      if (pointsOnLine >= 3) {
	outf << "\n\t";
	pointsOnLine = 0;
      }
    }
    if (withinpath)	{		// Finish the final path
      if(!havecycle) outf << "--cycle";
      if(evenoddmode) outf << ",currentpen+evenodd";
      outf << ");" << endl;
    }
  } else {					// Stroking, not filling
    for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
      const basedrawingelement & elem = pathElement(n);
      switch (elem.getType()) {
      case moveto:
	if(!withinpath) outf << "draw(";
        // coverity[fallthrough]
        // fall through
      case lineto:
	{
	  const Point & p = elem.getPoint(0);
	  if(elem.getType() == lineto) {
	    if (withinpath) {
	      outf << "--";
	    } else {
	      cerr << "lineto without a moveto; ignoring" << endl;
	      break;
	    }
	  } else {
	    if (withinpath) outf << "^^";
	  }
	  
	  outf << '(' << (p.x() + x_offset) << ',' << (p.y() + y_offset)
	       << ')';
	  withinpath = true;
	  pointsOnLine++;
	}
	break;

      case closepath:
	outf << "--cycle);" << endl;
	withinpath = false;
	pointsOnLine = 0;
	break;

      case curveto:
	{
	  if (!withinpath) {
	    cerr << "curveto without a moveto; ignoring" << endl;
	    break;
	  }
	  const Point & p0 = elem.getPoint(0);
	  outf << "..controls (" << (p0.x() + x_offset)
	       << ',' << (p0.y() + y_offset) << ") and (";
	  const Point & pt1 = elem.getPoint(1);
	  outf << (pt1.x() + x_offset) << ',' << (pt1.y() + y_offset) << ")..(";
	  const Point & pt2 = elem.getPoint(2);
	  outf << (pt2.x() + x_offset) << ',' << (pt2.y() + y_offset) << ')';
	  pointsOnLine += 3;
	  withinpath = true;
	}
	break;

      default:
	errf << "\t\tFatal: unexpected path element in drvasy" << endl;
	abort();
	break;
      }
      if (pointsOnLine >= 3 &&
	  n + 1 < numberOfElementsInPath() && 
	  pathElement(n + 1).getType() != moveto) {
	outf << "\n\t";
	pointsOnLine = 0;
      }
    }
    if (withinpath)	{		// Finish the final path
      outf << ");" << endl;
    }
  }
  restore();
}

// Each page will produce a different figure
void drvASY::open_page()
{
  if(firstpage) firstpage=false;
  else outf << "newpage();" << endl;
}

void drvASY::close_page()
{
}

void drvASY::show_image(const PSImage & imageinfo)
{
  restore();
  
  if (outBaseName == "" ) {
    errf << "images cannot be handled via standard output. Use an output file" << endl;
    return;
  }

  imgcount++;
  
  Point ll,ur;
  imageinfo.getBoundingBox(ll,ur);
  
  ostringstream buf;
  buf << outBaseName << "." << imgcount << ".eps";
  
  outf << "label(graphic(\"" << buf.str() << "\",\"bb="
       << ll.x() << " " << ll.y() << " " << ur.x() << " " << ur.y() << "\"),("
       << ll.x() << "," << ll.y() << "),align);" << endl;
  
  // Try to draw image in a separate layer.
  if(level == 0) 
    outf << "layer();" << endl;
  
  ofstream outi(buf.str().c_str());
  if (!outi) {
    errf << "Could not open file " << buf.str() << " for output";
    exit(1);
  }
  
  imageinfo.writeEPSImage(outi);
  (void)remove(imageinfo.FileName.c_str());

  outi.close();
}

// Output a text string
void drvASY::show_text(const TextInfo & textinfo)
{
  restore();
  
  // Change fonts
  std::string thisFontName(textinfo.currentFontName.c_str());
  const std::string thisFontWeight(textinfo.currentFontWeight.c_str());

  const double ps2tex=72.27/72.0;
    
  if (thisFontName != prevFontName || thisFontWeight != prevFontWeight) {
    if(textinfo.is_non_standard_font) {
      const size_t n = thisFontName.length();
      for(size_t i=0; i < n; i++) thisFontName[i] = (char) tolower(thisFontName[i]);
      outf << "textpen += font(\"" << thisFontName << "\"";
      if(textinfo.currentFontSize > 0) 
	outf << "," << textinfo.currentFontSize*ps2tex;
      outf << ");" << endl;
    } else {
      outf << "textpen += " << thisFontName << "(";
      if(thisFontWeight == string("Bold")) outf << "\"b\"";
      else if(thisFontWeight == string("Condensed")) outf << "\"c\"";
      outf << ");" << endl;
    }
    prevFontName = thisFontName.c_str();
    prevFontWeight = thisFontWeight;
  }
	
  // Change colors
  if (textinfo.currentR != prevR || textinfo.currentG != prevG ||
      textinfo.currentB != prevB) {
    outf << "textpen=0*textpen+rgb("
	 << textinfo.currentR << "," 
	 << textinfo.currentG << ","
	 << textinfo.currentB << ");" << endl;
    prevR = textinfo.currentR;
    prevG = textinfo.currentG;
    prevB = textinfo.currentB;
  }
	
  // Change font size
  if (textinfo.currentFontSize != prevFontSize) {
    if (textinfo.currentFontSize > 0) {
      outf << "textpen += fontsize(" << textinfo.currentFontSize*ps2tex
	   << ");" << endl;
      prevFontSize = textinfo.currentFontSize;
    } else {
      if (Verbose())
	errf << "warning: font size of " << textinfo.
	  currentFontSize << "pt encountered; ignoring" << endl;
    }
  }
  // Change rotation
  if (textinfo.currentFontAngle != prevFontAngle)
    prevFontAngle = textinfo.currentFontAngle;

  // Output the text using macros defined in the constructor
  outf << "label(";
  if(prevFontAngle != 0.0) outf << "rotate(" << prevFontAngle << ")*(";
  bool texify=false;
  bool quote=false;
  const char *c=textinfo.thetext.c_str();
  if(*c) for (; *c; c++) {
    if (*c >= ' ' && *c != '\\' && *c <= '~') {
      if(!texify) {
	if(quote) outf << "\"+";
	else quote=true;
	outf << "texify(\"";
	texify=true;
      }
      if (*c == '"') outf << "\\\"";
      else outf << (*c);
    } else {
      if(texify) {
	outf << "\")+";
	texify=false;
	quote=false;
      }
      if(!quote) {
	outf << "\"";
	quote=true;
      }
      outf << "\\char" << (int) *c;
    }
  } else outf << "\"\"";
  if(quote) outf << "\"";
  if(texify) outf << ")";
  if(prevFontAngle != 0.0) outf << ")";
  outf << ",(" << textinfo.x() << ',' << textinfo.y() << "),align,textpen);" 
       << endl;
}


// Output a path
void drvASY::show_path()
{
  // Change colors
  if (currentR() != prevR || currentG() != prevG || currentB() != prevB) {
    prevR = currentR();
    prevG = currentG();
    prevB = currentB();
    outf << "currentpen=0*currentpen+rgb("
	 << prevR << ","
	 << prevG << ","
	 << prevB << ");" << endl;
  }
  // Change line width
  float linewidth=currentLineWidth();
  // Fix up zero linewidths resulting from a dtransform truncate idtransform
  if(linewidth == 0.0) linewidth=0.5f;
  if(linewidth != prevLineWidth) {
    prevLineWidth = linewidth;
    outf << "currentpen += " << prevLineWidth << "bp;" << endl;
  }
  // Change line cap
  if (currentLineCap() != prevLineCap) {
    prevLineCap = currentLineCap();
    outf << "currentpen += ";
    switch (prevLineCap) {
    case 0:
      outf << "squarecap;" << endl;
      break;
    case 1:
      outf << "roundcap;" << endl;
      break;
    case 2:
      outf << "extendcap;" << endl;
      break;
    default:
      errf << "\t\tFatal: Unknown linecap \"" << prevLineCap << '"' << endl;
      abort();
      break;
    }
  }
  // Change line join
  if (currentLineJoin() != prevLineJoin) {
    prevLineJoin = currentLineJoin();
    outf << "currentpen += ";
    switch (prevLineJoin) {
    case 0:
      outf << "miterjoin;" << endl;
      break;
    case 1:
      outf << "roundjoin;" << endl;
      break;
    case 2:
      outf << "beveljoin;" << endl;
      break;
    default:
      errf << "\t\tFatal: Unknown linejoin \"" << prevLineJoin << '"' << endl;
      abort();
      break;
    }
  }
  // Change dash pattern
  string currentDashPattern = dashPattern();
  if (currentDashPattern != prevDashPattern) {
    prevDashPattern = currentDashPattern;
    size_t p=currentDashPattern.find('[');
    if(p < string::npos) currentDashPattern[p]='\"';
    p=currentDashPattern.find(']');
    if(p < string::npos) {
      currentDashPattern[p]='\"';
      const size_t n=currentDashPattern.length();
      p++;
      if(p < n)
		(void)currentDashPattern.erase(p,n-p);
    }
    outf << "currentpen += linetype(" << currentDashPattern
	 << ",false);" << endl;
  }

  // Determine the fill mode
  evenoddmode=(currentShowType() == drvbase::eofill); 
  fillmode = (evenoddmode || currentShowType() == drvbase::fill);
  if (!fillmode && currentShowType() != drvbase::stroke) {
    errf << "\t\tFatal: unexpected show type " << (int)
      currentShowType() << " in drvasy" << endl;
    abort();
  }
  // Draw the path
  print_coords();
}

void drvASY::ClipPath(cliptype type)
{
  clipmode=true;
  evenoddmode=(type == drvbase::eoclip);
  print_coords();
  clipmode=false;
}

void drvASY::Save()
{
  gsavestack.push_back(true);
}

void drvASY::Restore()
{
  gsavestack.push_back(false);
}
				       
static DriverDescriptionT < drvASY > 
D_asy("asy", "Asymptote Format", "","asy",
      true,	// if backend supports subpaths
      true,	// if backend supports curves, else 0
      false,	// if backend supports elements with fill and edges
      true,	// if backend supports text, else 0
      // DriverDescription::imageformat::png, // support for PNG file images
      DriverDescription::imageformat::memoryeps, // prefer EPS files for images
      DriverDescription::opentype::normalopen, true, // supports multiple pages
      true  /*clipping */ 
      );
