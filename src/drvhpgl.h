#ifndef __drvHPGL_h
#define __drvHPGL_h

/* 
   drvhpgl.h : This file is part of pstoedit
   Class declaration for hpgl output driver with no additional attributes
   and methods (minimal interface)

   Copyright (C) 1993,1994,1995,1996,1997,1998 Peter Katzmann p.katzmann@thiesen.com 
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

*/

#include "drvbase.h"

   class drvHPGL : public drvbase {
   
protected:
	// = PROTECTED DATA

   public:
   
	derivedConstructor(drvHPGL);
 
   ~drvHPGL() override; // Destructor
	class DriverOptions : public ProgramOptions {
	public:
		OptionT < bool, BoolTrueExtractor > penplotter ;
		OptionT < bool, BoolTrueExtractor > pencolorsfromfile ;
		OptionT < int,  IntValueExtractor > maxPenColors; 
		OptionT < RSString, RSStringValueExtractor> fillinstruction;
	//	OptionT < bool, BoolTrueExtractor > useRGBcolors ;
		OptionT < bool, BoolTrueExtractor > hpgl2 ;
		OptionT < bool, BoolTrueExtractor > rot90 ;
		OptionT < bool, BoolTrueExtractor > rot180 ;
		OptionT < bool, BoolTrueExtractor > rot270 ;

			// penColors(0), maxPenColors(0)
		DriverOptions():
			penplotter(true,"-penplotter","", 0, "plotter is pen plotter (i.e. no support for specific line widths)", nullptr,false),
			pencolorsfromfile(true,"-pencolorsfromfile","", 0, "read pen colors from file drvhpgl.pencolors in pstoedit's data directory", nullptr,false),
			maxPenColors(true,"-pencolors", "number", 0, "maximum number of pen colors to be used by pstoedit (default 0) -" ,nullptr,0),
			fillinstruction(true,"-filltype", "string", 0, "select fill type e.g. FT 1" ,nullptr,(const char*)"FT1"),
			/*
			   Fill Type (FT) Command 
			   ========================================= 
			   This command selects the shading pattern used to fill polygons ( FP ), rectangles 
			   ( RA or RR ), wedges ( WG ), or characters ( CF ). The Fill Type command ( FT 
			   ), can use solid, shading, parallel lines (hatching), cross hatching, patterned 
			   (raster) fill, or PCL user-defined patterns. For more information see the PCL 5 
			   Printer Language Technical Reference Manual. The syntax for this command is 
			   as follows: 

			   FT fill type,[option1,[option2]]; or FT; 
			 */
			// known fill types:
			// FT 1 - solid black
			// FT 3 - parallel lines FT 3[,delta,angle]
			// FT 4 - cross hatching FT 4[,delta,angle]
			// FT 10 - shading FT 10,[percentage]

			hpgl2 (true,"-hpgl2" ,"", 0, "Use HPGL/2 instead of HPGL/1",nullptr,false),
			rot90 (true,"-rot90" ,"", 0, "rotate hpgl by 90 degrees",nullptr,false),
			rot180(true,"-rot180","", 0, "rotate hpgl by 180 degrees",nullptr,false),
			rot270(true,"-rot270","", 0, "rotate hpgl by 270 degrees",nullptr,false)
		{
			ADD( penplotter );
			ADD( pencolorsfromfile );
			ADD( maxPenColors );
			ADD( fillinstruction );
			ADD( hpgl2 );
			ADD( rot90 );
			ADD( rot180 );
			ADD( rot270 );
		}
	}*options;
   
   #include "drvfuncs.h"

      // void show_rectangle(const float llx, const float lly, const float urx, const float ury);
       void show_text(const TextInfo & textinfo) override;

   private:
	   void print_coords();
	   unsigned int readPenColors(ostream & errstream, const char *filename, bool justcount);
	   void SelectPen(float R, float G, float B);

	   //  Start DA hpgl color addition
	   struct HPGLColor { float R; float G; float B; unsigned int intColor;};
       unsigned int prevColor;
       unsigned int maxPen;
	   unsigned int currentPen;
	   HPGLColor * penColors;	
       //  End DA hpgl color addition

	   int rotation;

   public:
       static void rot(double & x, double & y, int angle);

	   NOCOPYANDASSIGN(drvHPGL)
   };

#endif 
 
