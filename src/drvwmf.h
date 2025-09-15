#ifndef __drvwmf_h__
#define __drvwmf_h__

/*
   drvwmf.h : This file is part of pstoedit
   Class declaration for the WMF output driver.
   The implementation can be found in drvwmf.cpp

   Copyright (C) 1998 Thorsten Behrens and Bjoern Petersen
   Copyright (C) 2000 Thorsten Behrens

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


#ifdef _WIN32

#include <windows.h>
#undef min		// is declared inline in drvbase... (sigh)
#undef max

#else

// use Allen Barnett's libemf
#ifdef OLDLIBEMF
#include <emf.h>
#else
#include <libEMF/emf.h>
#endif

#endif

#include "drvbase.h"


class drvWMF : public drvbase {

public:

	derivedConstructor(drvWMF);		// Constructor

	~drvWMF() override;						// Destructor
	class DriverOptions : public ProgramOptions { 
	public:
		OptionT < bool, BoolTrueExtractor> mapToArial;
		OptionT < bool, BoolTrueExtractor> emulateNarrowFonts;
		OptionT < bool, BoolTrueExtractor> drawBoundingBox;
		OptionT < bool, BoolTrueExtractor> pruneLineEnds;
		OptionT < bool, BoolTrueExtractor> notforWindows;
		OptionT < bool, BoolTrueExtractor> winbb;
		OptionT < bool, BoolTrueExtractor> OpenOfficeMode ;

		DriverOptions() :
		mapToArial(true,"-m","",0,"map to Arial",nullptr,false),
		emulateNarrowFonts(true,"-nf","",0,"emulate narrow fonts",nullptr,false),
		drawBoundingBox(true,"-drawbb","",0,"draw bounding box",nullptr,false),
		pruneLineEnds(true,"-p","",0,"prune line ends",nullptr,false),
		notforWindows(true,"-nfw","",0,"not for MS Windows (meaningful under *nix only)",
		"Newer versions of MS Windows (2000, XP, Vista, 7, ...) will not accept WMF/EMF files generated when this option is set and the input contains text. "
		"But if this option is not set, then the WMF/EMF driver will estimate interletter spacing of text using "
		"a very coarse heuristic. This may result in ugly looking output. On the other hand, OpenOffice "
		"can still read EMF/WMF files where pstoedit delegates the calculation of the inter letter spacing "
		"to the program reading the WMF/EMF file. So if the generated WMF/EMF file shall never be processed "
		"under MS Windows, use this option. If WMF/EMF files with high precision text need to be generated under *nix "
		"the only option is to use the -pta option of pstoedit. However that causes every text to be split into single characters "
		"which makes the text hard to edit afterwards. Hence the -nfw option provides a sort of compromise between "
		"portability and nice to edit but still nice looking text. Again - this option has no meaning when pstoedit "
		"is executed under MS Windows anyway. In that case the output is portable "
		"but nevertheless not split and still looks fine.", false),
		winbb(true,"-winbb","",0,"let the MS Windows API calculate the Bounding Box (MS Windows only)",nullptr,false),
		OpenOfficeMode(true,"-OO", "", 0, "generate OpenOffice compatible EMF file",nullptr,false)

		{
			ADD(mapToArial);
			ADD(emulateNarrowFonts);
			ADD(drawBoundingBox);
			ADD(pruneLineEnds);
			ADD(notforWindows);
			ADD(winbb);
			ADD(OpenOfficeMode);
		}
	
	} * options;

#include "drvfuncs.h"
	void show_rectangle(const float llx, const float lly, const float urx, const float ury) override;
	void show_text(const TextInfo & textinfo) override;

	void show_image(const PSImage & imageinfo) override; 

private:

	enum polyType {TYPE_FILL,TYPE_LINES};

	void drawPoly(polyType type);

	void setDrawAttr();

	int fetchFont(const TextInfo & textinfo, short int, short int);

	float scale() const;
	long transx(float x) const;
	long transy(float y) const;

	void initMetaDC(HDC hdc);

// This contains all private data of drvwmf.

	HDC			metaDC;
	HDC			desktopDC;

	LOGPEN			penData;
	HPEN			coloredPen;
	HPEN			oldColoredPen;

	LOGBRUSH		brushData;
	HBRUSH			coloredBrush;
	HBRUSH			oldColoredBrush;

	HFONT			myFont;
	HFONT			oldFont;

	int 			maxX, maxY;
	int 			minX, minY;
	bool			maxStatus, minStatus;

	bool			enhanced;	

	RSString		tempName;
	FILE*			outFile;	

};

#endif /* #ifndef __drvwmf_h__ */
 
 

 
 
