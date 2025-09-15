/*
  drvnoi.cpp
  
  noixml driver implementation of pstoedit driver methods
  
  This driver uses functions from Nemetschek Object Interface to
  create an XML file, readable by Nemetschek Allplan software.
  
  It uses a dynamically loaded proxy dll, pstoed_noi.dll, to call 
  the NOI methods that draw the objects to a dummy Allplan database, 
  and to store them to an xml file.
    
  There in the proxy dll, the bezier curves are represented as polylines,
  or, if they are recognized as circle arcs - as circle arcs.
   
  See "NOIXML driver for PSTOEDIT.txt" for full details.
      
  Programmer: Valentin Naumov, Nemetschek OOD, Sofia, vnaoumov_AT_nemetschek.bg
  Contacts: Stefan Konz, Nemetschek Technology GmbH, Munich, skonz_AT_nemetschek.de

  Copyright (C) 2005 : Valentin Naumov

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

#include "drvnoi.h"
#include <memory>

// function prototypes

typedef bool (* LPFN_BOOL_VOID)();
typedef bool (* LPFN_BOOL_PSTR)(const char *);
typedef bool (* LPFN_BOOL_PSTR_INT)(const char *, int);
typedef bool (* LPFN_BOOL_4DBL)(double, double, double, double);
typedef bool (* LPFN_BOOL_8DBL)(double, double, double, double, double, double, 
  double, double);
#ifndef _WINDEF_
// this already defined in windef.h under Windows
	typedef unsigned char BYTE;
#endif
typedef bool (* LPFN_VOID_3BYTE)(BYTE, BYTE, BYTE);
typedef bool (* LPFN_BOOL_DBL_UNS)(double [][2], unsigned);
typedef bool (* LPFN_BOOL_PSTR_5DBL)(const char *, double, double, double, double, 
  double);
typedef bool (* LPFN_BOOL_2PSTR_2DBL)(const char *, const char *, double, double);
typedef bool (* LPFN_BOOL_4DBL_PSTR)(double, double, double, double, const char *);
typedef bool (* LPFN_BOOL_FLT_UNS_PSTR)(float, unsigned, const char *);

static LPFN_BOOL_PSTR		NoiWriteXML;
static LPFN_BOOL_PSTR_INT	NoiSetOptions;
static LPFN_VOID_3BYTE		NoiSetCurrentColor;
static LPFN_VOID_3BYTE		NoiSetFillColor;
static LPFN_BOOL_4DBL		NoiDrawLine;
static LPFN_BOOL_8DBL		NoiDrawCurve;
static LPFN_BOOL_DBL_UNS	NoiDrawFill;
static LPFN_BOOL_DBL_UNS	NoiDrawPolyline;
static LPFN_BOOL_PSTR_5DBL	NoiDrawText;
static LPFN_BOOL_2PSTR_2DBL	NoiSetFont;
static LPFN_BOOL_4DBL_PSTR	NoiDrawImage;
static LPFN_BOOL_VOID		NoiEndPolyline;
static LPFN_BOOL_FLT_UNS_PSTR NoiSetLineParams;

// This array stores pointers to functions from dynamically loaded proxy dll
typedef DynLoader::fptr noifptr;
static noifptr*DllFunc[] = {(noifptr*)&NoiWriteXML, (noifptr*)&NoiSetCurrentColor, 
  (noifptr*)&NoiSetFillColor, (noifptr*)&NoiDrawLine, (noifptr*)&NoiDrawCurve, 
  (noifptr*)&NoiDrawFill, (noifptr*)&NoiDrawText, (noifptr*)&NoiSetFont, 
  (noifptr*)&NoiDrawImage, (noifptr*)&NoiEndPolyline, (noifptr*)&NoiSetLineParams,
  (noifptr*)&NoiSetOptions, (noifptr*)&NoiDrawPolyline};
  
// Function names
static const char *DllFuncName[] = {"NoiWriteXML", "NoiSetCurrentColor", 
  "NoiSetFillColor", "NoiDrawLine", "NoiDrawCurve", 
  "NoiDrawFill", "NoiDrawText", "NoiSetFont", 
  "NoiDrawImage", "NoiEndPolyline", "NoiSetLineParams",
  "NoiSetOptions", "NoiDrawPolyline"};
  
static unsigned short DLLFUNCNUM = (sizeof(DllFunc)/sizeof(noifptr));

// driver constructor
// looks for resource file and bezier split level parameters
drvNOI::derivedConstructor(drvNOI): constructBase //, imgcount(0)
  {  
  if (!outFileName.length())
	{
    errf << endl << "Please provide output file name" << endl << endl;
    exit(0);
    }

  setdefaultFontName(DEFAULT_FONT_NAME);
  LoadNOIProxy();

  if (!NoiSetOptions) {
	ctorOK= false; 
	return;
  }
  
#if 0
  BezierSplitLevel = DEFAULT_BEZIER_SPLIT_LEVEL;
  ResourceFile = NULL;
  
  for (unsigned int i = 0; i < d_argc; i++) 
	{
	if (strcmp(d_argv[i], RESOURCE_FILE_OPTION) == 0) 
  	  ResourceFile = d_argv[i + 1];

	if (strcmp(d_argv[i], BEZIER_SPLIT_LEVEL_OPTION) == 0) 
  	  BezierSplitLevel = atoi(d_argv[i + 1]);
	}	  
#endif

  (void)NoiSetOptions(options->ResourceFile.value.c_str(), options->BezierSplitLevel);
  }

// destructor - write the xml file and unload the dll
drvNOI::~drvNOI()
  {
  if (hProxyDLL.valid() && outFileName.length())
	(void)NoiWriteXML(outFileName.c_str());

  hProxyDLL.close();
  options=nullptr;
  }

// load the proxy dll and get all the function pointers
// abort the execution, if a function is not found
void drvNOI::LoadNOIProxy()
  {
  hProxyDLL.open(NOI_XML_Proxy_DLL);
  if (hProxyDLL.valid())
	{  
	for (unsigned int i = 0; i < DLLFUNCNUM; i++)
	  {
	  *DllFunc[i] = hProxyDLL.getSymbol(DllFuncName[i]);	
	  if (!*DllFunc[i])
		{
        errf << endl << DllFuncName[i] << " function not found in " << 
		  NOI_XML_Proxy_DLL << ".dll" << endl; 
		abort();
		}
	  }
	}
  }

// add point coordinates to a double array; increment the point number
static void AddPoint(double points[][2], const Point& p, unsigned &npoints)
  {
  points[npoints][0] = p.x();
  points[npoints][1] = p.y();
  npoints++;
  }

//	draw_polyline:
//	  PDF polylines may contain sequence of lines and bezier curves
//	  Allplan polylines are composed of straight lines only
//	  
//	  Accumulate all the points from sequential straight line segments
//	  If a curve starts - first draw the polyline, then draw the bezier curve
void drvNOI::draw_polyline()
  {
  Point p0, pc;
  const Point pf(x_offset, y_offset);
  std::unique_ptr<double[][2]> points ( new double[numberOfElementsInPath()][2]);
  unsigned npoints = 0;

  for (unsigned int n = 0; n < numberOfElementsInPath(); n++) 
	{
	const basedrawingelement &elem = pathElement(n);

	switch (elem.getType()) 
	  {
	  case moveto:
	    (void)NoiDrawPolyline(points.get(), npoints);	
		npoints = 0;	  
		  
		p0 = elem.getPoint(0) + pf;
		pc = p0;
		AddPoint(points.get(), pc, npoints);
		break;

	  case lineto:
		{
		const Point p = elem.getPoint(0) + pf;
		pc = p;
		AddPoint(points.get(), pc, npoints);
		}
		break;

	  case closepath:
	    AddPoint(points.get(), p0, npoints);
  	    (void)NoiDrawPolyline(points.get(), npoints);
		npoints = 0;	  
	    AddPoint(points.get(), p0, npoints);
		break;

	  case curveto:
		{
  	    (void)NoiDrawPolyline(points.get(), npoints);
		npoints = 0;
		
		Point p[3];
		p[0] = elem.getPoint(0) + pf;
		p[1] = elem.getPoint(1) + pf;
		p[2] = elem.getPoint(2) + pf;
		(void)NoiDrawCurve(pc.x(), pc.y(), 
							p[0].x(), p[0].y(), 
							p[1].x(), p[1].y(), 
							p[2].x(), p[2].y());
		pc = p[2];
		
		AddPoint(points.get(), pc, npoints);
		}
		break;
	  default:; // no expected
	  }
	}
	
  (void)NoiDrawPolyline(points.get(), npoints);
  (void)NoiEndPolyline();
  }

//	draw_polygon - very similar to draw_polyline, but represents the filled objects
//  with straight-line borders as Allplan Filling objects
void drvNOI::draw_polygon()
  {
  std::unique_ptr<double[][2]> points (new double[numberOfElementsInPath()][2]);
  unsigned npoints = 0;
  Point p0, pc;
  bool isFillObject = (currentShowType() == fill);
  const Point pf(x_offset, y_offset);
	
  for (unsigned n = 0; n < numberOfElementsInPath(); n++) 
	{
	const basedrawingelement &elem = pathElement(n);

	switch (elem.getType()) 
	  {
	  case moveto:
	    (void)NoiDrawPolyline(points.get(), npoints);	
		npoints = 0;	  
	  
  		p0 = elem.getPoint(0) + pf;
  		pc = p0;
		AddPoint(points.get(), pc, npoints);
		break;

	  case lineto:
		{
		const Point p = elem.getPoint(0) + pf;
		pc = p;
		AddPoint(points.get(), pc, npoints);
		}
		break;

	  case closepath:
	    AddPoint(points.get(), p0, npoints);
	    if (!isFillObject)
	      {
  		  (void)NoiDrawPolyline(points.get(), npoints);
  		  npoints = 0;
	      AddPoint(points.get(), p0, npoints);
	      }
	      
		pc = p0;
		break;

	  case curveto:
		{
		isFillObject = false;
		(void)NoiDrawPolyline(points.get(), npoints);
		npoints = 0;  

		Point p[3];
		p[0] = elem.getPoint(0) + pf;
		p[1] = elem.getPoint(1) + pf;
		p[2] = elem.getPoint(2) + pf;
		(void)NoiDrawCurve(pc.x(), pc.y(), 
							p[0].x(), p[0].y(), 
							p[1].x(), p[1].y(), 
							p[2].x(), p[2].y());
		pc = p[2];
		
		AddPoint(points.get(), pc, npoints);
		break;
		}
	  default:; // not expected
	  }
	}

  isFillObject = isFillObject && (pc == p0);
  if (isFillObject)
	(void)NoiDrawFill(points.get(), npoints);
  else
	(void)NoiDrawPolyline(points.get(), npoints);

  (void)NoiEndPolyline();
  }
  
void drvNOI::show_path()
  {
  (void)NoiSetLineParams(currentLineWidth(), currentLineCap(), dashPattern());
  (void)NoiSetCurrentColor((BYTE)(255 * currentR()), (BYTE)(255 * currentG()), 
    (BYTE)(255 * currentB()));
  (void)NoiSetFillColor((BYTE)(255 * fillR()), (BYTE)(255 * fillG()), (BYTE)(255 * fillB()));
    
  if (isPolygon())
    draw_polygon();
  else
    draw_polyline();

 // switch (currentShowType()) 
	//{
	//case drvbase::stroke:
	//case drvbase::fill:
	//case drvbase::eofill:
	//}
 // edgeR() 
 // edgeG() 
 // edgeB() 
  }


// rectangles are drawn as 4 lines
void drvNOI::show_rectangle(const float llx, const float lly, const float urx, 
  const float ury)
  {
  (void)NoiSetLineParams(currentLineWidth(), currentLineCap(), dashPattern());
  (void)NoiSetCurrentColor((BYTE)(255 * currentR()), (BYTE)(255 * currentG()), 
    (BYTE)(255 * currentB()));
  (void)NoiSetFillColor((BYTE)(255 * fillR()), (BYTE)(255 * fillG()), (BYTE)(255 * fillB()));
  
  (void)NoiDrawLine(llx + x_offset, lly + y_offset, urx + x_offset, lly + y_offset);
  (void)NoiDrawLine(urx + x_offset, lly + y_offset, urx + x_offset, ury + y_offset);
  (void)NoiDrawLine(urx + x_offset, ury + y_offset, llx + x_offset, ury + y_offset);
  (void)NoiDrawLine(llx + x_offset, ury + y_offset, llx + x_offset, lly + y_offset);
  }

// text objects
void drvNOI::show_text(const TextInfo &textinfo)
  {
  const Point pf(x_offset, y_offset);

  (void)NoiSetCurrentColor((BYTE)(255 * textinfo.currentR), (BYTE)(255 * textinfo.currentG), 
    (BYTE)(255 * textinfo.currentB));

  (void)NoiSetFont(textinfo.currentFontName.c_str(), textinfo.currentFontFullName.c_str(), 
    atof(textinfo.currentFontWeight.c_str()), textinfo.currentFontSize);
  
  (void)NoiDrawText(textinfo.thetext.c_str(), textinfo.x() + pf.x(), textinfo.y() + pf.y(), 
	textinfo.x_end() - textinfo.x() + pf.x(), textinfo.y_end() - textinfo.y() + pf.y(), 
	textinfo.currentFontAngle);
  }

// image objects
void drvNOI::show_image(const PSImage &imageinfo)
  {
  assert(imageinfo.isFileImage);
  Point p1, p2;
  imageinfo.getBoundingBox(p1, p2);
  const Point pf(x_offset, y_offset);
  p1 += pf;
  p2 += pf;
  (void)NoiDrawImage(p1.x(), p1.y(), p2.x(), p2.y(), imageinfo.FileName.c_str());
  }

// paging support - each new page is placed on the right
void drvNOI::open_page()
  {  
  if (currentPageNumber > 1)
	x_offset += currentDeviceWidth;
  }

void drvNOI::close_page()
  {  
  }

// driver description, no clipping support
static DriverDescriptionT <drvNOI> D_noi(DRIVER_NAME, DRIVER_DESCR,
  DRIVER_LONG_DESCR, FILE_SUFFIX, true, true, true, true, 
  DriverDescription::imageformat::png, DriverDescription::opentype::normalopen, true, false);

