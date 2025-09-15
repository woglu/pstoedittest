#ifndef __drvGCODE_H
#define __drvGCODE_H

/* 
   drvGCODE.h : This file is part of pstoedit
   Contributed / Copyright 2008 by: Lawrence Glaister VE7IT

   Copyright (C) 1993 - 2024 Wolfgang Glunz, wglunz@pstoedit.net

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

class drvGCODE : public drvbase {

public:

	derivedConstructor(drvGCODE);
	~drvGCODE() override; // Destructor

	class DriverOptions : public ProgramOptions {
	public:
		DriverOptions() 
		{
		}
	}*options;

private:

#include "drvfuncs.h"

};
#endif

 
 
