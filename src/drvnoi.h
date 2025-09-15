/*
  drvnoi.h
  
  noixml driver interface
  
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

#ifndef __drvNOI_h
#define __drvNOI_h

#include "drvbase.h"
#include <dynload.h>

#define NOI_XML_Proxy_DLL	  "pstoed_noi"

#define	RESOURCE_FILE_OPTION  "-r"
#define	RESOURCE_FILE_DESCR	  "Allplan resource file"  

#define	BEZIER_SPLIT_LEVEL_OPTION	"-bsl"
#define DEFAULT_BEZIER_SPLIT_LEVEL	3
#define BEZIER_SPLIT_LEVEL_DESCR	"Bezier Split Level (default 3)"

#define DRIVER_NAME			  "noixml"
#define	DRIVER_DESCR		  "Nemetschek NOI XML format"
#define	DRIVER_LONG_DESCR	  "Nemetschek Object Interface XML format"
#define FILE_SUFFIX			  "xml"
#define DEFAULT_FONT_NAME	  "Arial"

class drvNOI: public drvbase
  {
  public:
	derivedConstructor(drvNOI);
	~drvNOI() override;
	class DriverOptions: public ProgramOptions 
	  {	
	  public:
		OptionT <RSString, RSStringValueExtractor> ResourceFile;
		OptionT <int, IntValueExtractor> BezierSplitLevel;
		DriverOptions(): 
		  ResourceFile(true, RESOURCE_FILE_OPTION, "string", 0, 
			RESOURCE_FILE_DESCR, nullptr, (const char*) ""),
		  BezierSplitLevel(true, BEZIER_SPLIT_LEVEL_OPTION, "number", 0, 
			BEZIER_SPLIT_LEVEL_DESCR, nullptr, DEFAULT_BEZIER_SPLIT_LEVEL)
		  { 
		  ADD (ResourceFile); 
		  ADD (BezierSplitLevel); 
		  }
		} *options;	
	
	void show_rectangle(const float llx, const float lly, const float urx, const float ury) override;
	void show_text(const TextInfo &textinfo) override;
	void show_image(const PSImage &imageinfo) override;
	// void translate(Point &p, float x, float y);
  private:
	// int imgcount; // not used
	
    DynLoader hProxyDLL;               // Handle to DLL
	void LoadNOIProxy();
	void draw_polyline();
	void draw_polygon();
	// const char * ResourceFile;
	// int BezierSplitLevel;
	// declare other private methods using include - pstoedit style
	#include "drvfuncs.h"
  };

#endif 
// __drvNOI_h
