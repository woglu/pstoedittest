#ifndef __drvCAIRO_h
#define __drvCAIRO_h

/* 
   drvcairo.h : This file is part of pstoedit
   Class declaration for the cairo driver.
   
   Copyright (C) 2009 - 2024 Dan McMahill dan_AT_mcmahill_DOT_net
   
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

class drvCAIRO : public drvbase {

 public:
  
  derivedConstructor(drvCAIRO);
  //(const char * driveroptions_P,ostream & theoutStream,ostream & theerrStream ); // Constructor
  
  ~drvCAIRO() override; // Destructor
  class DriverOptions : public ProgramOptions {
  public:
    OptionT < bool, BoolTrueExtractor > pango;
    OptionT < RSString, RSStringValueExtractor > funcname;
    OptionT < RSString, RSStringValueExtractor > header;
    DriverOptions(): 
      pango(true,"-pango","",0,"use pango for font rendering",nullptr,false),
      funcname(true,"-funcname","string",0,
	       "sets the base name for the generated functions and variables.  e.g. myfig",
	       nullptr,(const char*)"myfig"),
      header(true,"-header","string",0,
	       "sets the output file name for the generated C header file.  e.g. myfig.h",
	       nullptr,(const char*)"myfig.h")
      {
	ADD(pango);
	ADD(funcname);
	ADD(header);
      }
  }*options;
  
#include "drvfuncs.h"
  void show_rectangle(const float llx, const float lly, const float urx, const float ury) override;
  void show_text(const TextInfo & textinfo) override;
  void ClipPath(cliptype) override;
  
 public:
  
  void    show_image(const PSImage & imageinfo) override; 
  
 private:
  void print_coords();
  //int	      imgcount;
  
  float maxw, maxh;

  bool evenoddmode;

};

#endif

 
