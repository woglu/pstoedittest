/*
   drvPCB1.cpp : Hans-Jürgen Jahn    (Version 1.0  10.08.02)
   this code is derived from drvSAMPL.cpp, see text below

   Copyright (C) 1993 - 2024 Wolfgang Glunz, wglunz35_AT_pstoedit.net
   (for the skeleton and the rest of pstoedit)

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
#include "drvpcb1.h"
#include I_fstream
#include I_stdio
#include I_stdlib
#include <algorithm>


//output unit is mil
// const double pcb_scale = 1000.0/72.0;

drvPCB1::derivedConstructor(drvPCB1):
//(const char * driveroptions_p,ostream & theoutStream,ostream & theerrStream): // Constructor
	constructBase
{

	pcberrf.open("pcberror.dat");
	if (pcberrf.fail())
	{
	   cout << "could not open pcberror.dat";
	   exit(1);			
	}

// driver specific initializations
// and writing of header to output file
	pcberrf << "Sample header \n";
	// no longer supported - scale ; scale=pcb_scale;
//	float           x_offset;
//	float           y_offset;

	const char* env = getenv("pcbdrv_drill");
	drill_data=false;	
	drill_fixed=true;
	drill_diameter=0.0f;
	if (env!=nullptr)
	{
	   if (0!=strcmp(env,"no"))
	   {
	      drill_data=true;	
		  char* tail;
	      drill_diameter=(float)strtod(env,&tail);
	      if (env!=tail)
	         drill_fixed=true;
	      else
	         drill_fixed=false;
	   }
	}
}

drvPCB1::~drvPCB1() {
// driver specific deallocations
// and writing of trailer to output file
	pcberrf << "Sample trailer \n";
	pcberrf.close();
	options=nullptr;
}

void drvPCB1::print_coords()
{
    for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
	const basedrawingelement & elem = pathElement(n);
	switch (elem.getType()) {
		case moveto: {
			const Point & p = elem.getPoint(0);
			pcberrf << "\t\tmoveto ";
        		pcberrf  << p.x() + x_offset << " "
	     			<<  /*   currentDeviceHeight -  */   p.y() + y_offset << " " ;
			}
			break;
		case lineto: {
			const Point & p = elem.getPoint(0);
			pcberrf << "\t\tlineto ";
        		pcberrf  << p.x() + x_offset << " "
	     			<<  /*   currentDeviceHeight -  */   p.y() + y_offset << " " ;
			}
			break;
		case closepath:
			pcberrf << "\t\tclosepath ";
			break;
		case curveto:{
			pcberrf << "\t\tcurveto " ;
			for (unsigned int cp = 0 ; cp < 3; cp++ ) {
				const Point & p = elem.getPoint(cp);
        			pcberrf  << (p.x() + x_offset) << " "
	     				<<  /*   currentDeviceHeight -  */   (p.y() + y_offset) << " " ;
			}
			}
			break;
		default:
			errf << "\t\tFatal: unexpected case in drvsample " << endl;
			abort();
			break;
	}
	pcberrf << endl;
    }
}


void drvPCB1::open_page()
{
	pcberrf << "Opening page: " << currentPageNumber << endl;
}

void drvPCB1::close_page()
{
	pcberrf << "Closing page: " << (currentPageNumber) << endl;
}

void drvPCB1::show_text(const TextInfo & textinfo)
{
	pcberrf << "Text String : " << textinfo.thetext.c_str() << endl;
	pcberrf << '\t' << "X " << textinfo.x() << " Y " << textinfo.y() << endl;
	pcberrf << '\t' << "X_END " << textinfo.x_end() << " Y_END " << textinfo.y_end() << endl;
	pcberrf << '\t' << "currentFontName: " <<  textinfo.currentFontName.c_str() << endl;
	pcberrf << '\t' << "is_non_standard_font: " <<  textinfo.is_non_standard_font << endl;
	pcberrf << '\t' << "currentFontFamilyName: " << textinfo.currentFontFamilyName.c_str() << endl;
	pcberrf << '\t' << "currentFontFullName: " << textinfo.currentFontFullName.c_str() << endl;
	pcberrf << '\t' << "currentFontWeight: " << textinfo.currentFontWeight.c_str() << endl;
	pcberrf << '\t' << "currentFontSize: " << textinfo.currentFontSize << endl;
	pcberrf << '\t' << "currentFontAngle: " << textinfo.currentFontAngle << endl;
	pcberrf << '\t' << "currentR: " << textinfo.currentR << endl;
	pcberrf << '\t' << "currentG: " << textinfo.currentG << endl;
	pcberrf << '\t' << "currentB: " << textinfo.currentB << endl;
	const float * CTM = getCurrentFontMatrix();
	pcberrf << '\t' << "currentFontMatrix: [";
	for (int i = 0; i < 6; i++) { pcberrf << " " << CTM[i] ; }
	pcberrf << ']' << endl;
}

void drvPCB1::show_path()
{
	//test for every supported type
	if (lineOut()) return;
	if (filledRectangleOut()) return;
	if (filledCircleOut()) return;
	
	//if the current path is not supported, write it to "pcberror.dat"


	pcberrf << "Path # " << currentNr() ;
    	if (isPolygon()) pcberrf << " (polygon): "  << endl;
		else   pcberrf << " (polyline): " << endl;
	pcberrf << "\tcurrentShowType: ";
    	switch (currentShowType() ) {
    	case drvbase::stroke :
		pcberrf << "stroked";
      		break;
    	case drvbase::fill :
		pcberrf << "filled";
      		break;
    	case drvbase::eofill :
		pcberrf << "eofilled";
      		break;
    	default:
	// cannot happen
      		pcberrf << "unexpected ShowType " << (int) currentShowType() ;
      		break;
    	}
	pcberrf << endl;
	pcberrf << "\tcurrentLineWidth: " <<  currentLineWidth() << endl;
	pcberrf << "\tcurrentR: " << currentR() << endl;
	pcberrf << "\tcurrentG: " << currentG() << endl;
	pcberrf << "\tcurrentB: " << currentB() << endl;
	pcberrf << "\tedgeR:    " << edgeR() << endl;
	pcberrf << "\tedgeG:    " << edgeG() << endl;
	pcberrf << "\tedgeB:    " << edgeB() << endl;
	pcberrf << "\tfillR:    " << fillR() << endl;
	pcberrf << "\tfillG:    " << fillG() << endl;
	pcberrf << "\tfillB:    " << fillB() << endl;
	pcberrf << "\tcurrentLineCap: " << currentLineCap() << endl;
	pcberrf << "\tdashPattern: " << dashPattern() << endl;
	pcberrf << "\tPath Elements 0 to "  <<  numberOfElementsInPath()-1  << endl;
	print_coords();

}

/* void drvPCB1::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{
//	pcberrf << "Rectangle ( " << llx << "," << lly << ") (" << urx << "," << ury << ") equivalent to:" << endl;
// just do show_path for a first guess
	show_path();
}
*/



////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
/*
The following functions have all the same general structure:
1. test if the current path is of the right type, if not return false
   (This tests are not complete, they are only sufficient to distinguish between
   all supported types)
   
2. write the result of the conversion to "outf" and return true
*/
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

struct Lpoint
{
	long x;
	long y;   
};

static Lpoint toLpoint(const Point& p)
{
   Lpoint lp;
   lp.x=long(p.x());   
   lp.y=long(p.y());   
   return lp;
}

static bool isEqual(long a, long b, long tol)
{
   return (abs ((int)(a-b)) <= tol);
}
 
bool drvPCB1::lineOut()
{
   //a line has no hole
   if (drill_data)
      return false;

   //test if:
   // linewidth == 0 -> write "L" (line e.g. board outline)
   // linewidth >  0 -> write "F" (filled line)
   // Showtype == stroke
   // first item "moveto", followed by at least one "lineto"

   char id;
   const long width= long(currentLineWidth());
   if ( width==0 )
      id='L';
   else
      id='F';
   
   if ( currentShowType()!=drvbase::stroke )
      return false;
      
   const int nreip=numberOfElementsInPath();
   if (nreip<2)
      return false;
   if ( pathElement(0).getType()!=moveto )
      return false;
   int i;
   for (i=1; i<nreip; i++)
   {
      if ( pathElement(i).getType()!=lineto )
         return false;
   }
	
	
   //all tests done, write line
   Lpoint p1,p2;
   p1=toLpoint(pathElement(0).getPoint(0));
   for (i=1; i<nreip; i++)
   {
      p2=toLpoint(pathElement(i).getPoint(0));
      outf << id << " " << p1.x << " " << p1.y << " " 
           << p2.x << " " << p2.y;
      if (id=='F')
         outf << " " << width;
      outf << endl;
      p1=p2; 
   }
   return true;
}

bool drvPCB1::filledRectangleOut()
{
   //test if:
   // linewidth == 0
   // Showtype == filled
   // first item "moveto", followed by three "lineto"
   // next and last item ("lineto" and last point=first point) or "closepath"
   // it is really a rectangle 

   if ( currentLineWidth()!=0 )
      return false;
         
   if ( currentShowType()!=drvbase::fill )
      return false;
      
   const int nreip=numberOfElementsInPath();
   if (nreip!=5)
      return false;
   if ( pathElement(0).getType()!=moveto )
      return false;
   Lpoint p[4];
   p[0]=toLpoint(pathElement(0).getPoint(0));
   int i;
   for (i=1; i<4; i++)
   {
      if ( pathElement(i).getType()!=lineto )
         return false;
      p[i]=toLpoint(pathElement(i).getPoint(0));
   }
   
   if (pathElement(4).getType()!=closepath)
   {
      if (pathElement(4).getType()!=lineto)
         return false;
      const Lpoint xp=toLpoint(pathElement(4).getPoint(0));
      if ( !isEqual(xp.x,p[0].x,1) || !isEqual(xp.y,p[0].y,1) )
         return false;
   }

   // p[0] to p[3] contains the corner points
   
   //is it a rectangle?
   Lpoint mn,mx;
   mn=p[0];
   mx=p[0];
   for (i=1; i<4; i++)
   {
      mn.x=std::min(mn.x,p[i].x);
      mn.y=std::min(mn.y,p[i].y);
      mx.x=std::max(mx.x,p[i].x);
      mx.y=std::max(mx.y,p[i].y);
   }

   for (i=0; i<4; i++)
   {
      if ( !isEqual(mn.x,p[i].x,1) && !isEqual(mx.x,p[i].x,1) )
         return false;
      if ( !isEqual(mn.y,p[i].y,1) && !isEqual(mx.y,p[i].y,1) )
         return false;
   }
	
   //all tests done, write rectangle
   if (!drill_data)
   {
      outf << "R " << mn.x << " " << mn.y << " " << mx.x << " " << mx.y << endl;
   }
   else
   {
      if (drill_fixed)
         outf << "D " << (mx.x+mn.x)/2 << " " << (mx.y+mn.y)/2 << " " << drill_diameter << endl;
         //rectangle is only supported with fixed diameter 
   }
   
   return true;
}

bool drvPCB1::filledCircleOut()
{
   //test if:
   // linewidth == 0
   // Showtype == filled
   // first item "moveto", followed by four "curveto" 
   // the surrounding rectangle has equal width and height
   // this is probably not sufficient to be sure that it is a cirle!

   if ( currentLineWidth()!=0 )
      return false;
         
   if ( currentShowType()!=drvbase::fill )
      return false;
      
   const int nreip=numberOfElementsInPath();
   if (nreip!=5)
      return false;
   if ( pathElement(0).getType()!=moveto )
      return false;
   Lpoint p[4];
   p[0]=toLpoint(pathElement(0).getPoint(0));
   int i;
   for (i=1; i<4; i++)
   {
      if ( pathElement(i).getType()!=curveto )
         return false;
      p[i]=toLpoint(pathElement(i).getPoint(2));
   }
   if (pathElement(4).getType()!=curveto)
      return false;

   //get the coordinates of a surrounding rectangle
   Lpoint mn,mx;
   mn=p[0];
   mx=p[0];
   for (i=1; i<4; i++)
   {
      mn.x=std::min(mn.x,p[i].x);
      mn.y=std::min(mn.y,p[i].y);
      mx.x=std::max(mx.x,p[i].x);
      mx.y=std::max(mx.y,p[i].y);
   }

   Lpoint m; 
   m.x= (mn.x+mx.x)/2;
   m.y= (mn.y+mx.y)/2;
   long dx,dy;
   dx= mx.x-mn.x; 
   dy= mx.y-mn.y; 
   if (!isEqual(dx,dy,3))
      return false;

   if (!drill_data)
   {
      outf << "F " << m.x << " " << m.y << " " << m.x << " " << m.y << " " << dx <<endl;
   }
   else
   {
      outf << "D " << m.x << " " << m.y << " ";
      if (drill_fixed)
         outf << drill_diameter << endl;
      else
         outf << dx << endl;
   }
   
   return true;
}


static DriverDescriptionT<drvPCB1> D_pcb("pcbi","engrave data - insulate/PCB format",
										 "",
										 "dat",
								
		false, // backend supports subpaths
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

		true, // backend supports curves
		true, // backend supports elements which are filled and have edges
		false, // backend supports text
		DriverDescription::imageformat::noimage,	// no support for PNG file images
		DriverDescription::opentype::normalopen, 
		true,	// if format supports multiple pages in one file
		false 	/*clipping */
		);

