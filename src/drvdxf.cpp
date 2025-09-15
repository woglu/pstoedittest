/* 
   drvDXF.cpp : This file is part of pstoedit 

   Copyright (C) 1993 - 2024 Wolfgang Glunz, wglunz35_AT_pstoedit.net

	DXF Backend Version 0.9 ( LINEs only, no Text, no color, no linewidth )
	(see if polyaslines )
	Carsten Hammer	chammer_AT_post.uni-bielefeld.de
   	CATS Gesellschaft fuer technische DV-Anwendungen mbH
	Carl-Zeiss-Strasse 65
	33334 Guetersloh

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

#include "drvdxf.h"
#include I_stdio
#include I_string_h
#include I_iostream
#include "pstoedit_config.h"

static void write_DXF_handle(ostream& outs, int handle) {
	outs << "  5\n" << std::hex << handle << std::dec << endl;
}

// not used - the acad header also works with icad. later we may introduce an option
// which header to use #include "dxficad.h"
#include "dxfacad.h"

// try to use shorter headers
// but this doesn't work so far
#if 0
static const char dxf14shortheader[] = 
			"  0\n"
			"SECTION\n"
			"2\nHEADER\n"
			"  9\n"
			"$ACADVER\n"
			"1\n"
			"AC1014\n"
			"  9\n"
			"$EXTMIN\n"
			"10\n"
			"0.0\n"
			"20\n"
			"0.0\n"
			"30\n"
			"0.0\n"
			"  9\n"
			"$EXTMAX\n"
			"10\n"
			"1000.0\n"
			"20\n"
			"1000.0\n"
			"30\n"
			"0.0\n"
			"  9\n"
			"$FILLMODE\n"
			"70\n"
			"0\n"
			"  9\n"
			"$SPLFRAME\n"
			"70\n"
			"1\n"
			"  0\n"
			"ENDSEC\n"
			"  0\n"
			"SECTION\n"
			"2\n"
			"TABLES\n"
// Layers
			"  0\n"
			"TABLE\n"
			"2\n"
			"LAYER\n"
			"70\n"
			"255\n" // 255 is max number of layers - but can be greater

			// Layer "0" definition
			"  0\n"
			"LAYER\n"
			"2\n"
			"0\n" // layername
			"70\n" 
			"0\n" // unfrozen
			"62\n"
			"7\n" // color
			"6\n"
			"CONTINUOUS\n" // linestyle
			// End Layer 0


			"  0\n"
			"ENDTAB\n"
			"  0\n"
			"ENDSEC\n"
			"  0\n"
			"SECTION\n"
			"2\n"
			"ENTITIES\n";



static const char dxf14shorttrailer[] =
	"  0\n"
	"ENDSEC\n"
	"  0\n"
	"EOF\n";
#endif

static const char dxf9shortheader_prelayer1[] =
			"  0\n"
			"SECTION\n"
			"  2\n"
			"HEADER\n"
			"  9\n"
			"$ACADVER\n"
			"  1\n"
			"AC1009\n"
			"  9\n"
			"$FILLMODE\n"
			" 70\n"
			" 0\n"
			"  9\n"
			"$SPLFRAME\n"
			" 70\n"
			" 1\n";

static const char dxf9shortheader_prelayer2[] =
			"  0\n"
			"ENDSEC\n"

			"  0\n"
			"SECTION\n"
			"  2\n"
			"TABLES\n"
// Layertable
			"  0\n"
			"TABLE\n"
			"  2\n"
			"LAYER\n"
			" 70\n";
			// "   255\n" // max num of layers - but can be greater

static const char dxf9layer0def[] =
			// layer 0
			"  0\n"
			"LAYER\n"
			"  2\n"
			"0\n"		// layer name "0"
			" 70\n"
			"     0\n"	// unfrozen
			" 62\n"	  
			"     7\n"	// color
			"  6\n"
			"CONTINUOUS\n"; // line style
			// end layer 0


static const char dxf9shortheader_postlayer[] =
			"  0\n"
			"ENDTAB\n"
			"  0\n"
			"ENDSEC\n"
			"  0\n"
			"SECTION\n"
			"  2\n"
			"ENTITIES\n";


static const char dxf9shorttrailer[] =
	"  0\n"
	"ENDSEC\n"
	"  0\n"
	"EOF\n";



class DXFColor {
public:
// The SparcCompiler wants to have this public in order to initialize DXFColors
	struct rgbcolor {
		constexpr rgbcolor(unsigned short r_p, unsigned short g_p, unsigned short b_p) : r(r_p),g(g_p),b(b_p) {}
		const unsigned short r;
		const unsigned short g;
		const unsigned short b;
	};

private:
	static const rgbcolor DXFColors[];
public:
	static const unsigned short numberOfColors;
	static unsigned int getDXFColor(float r, float g, float b, unsigned int firstcolor = 0);
};


const DXFColor::rgbcolor DXFColor::DXFColors[] = {
	rgbcolor(0x00, 0x00, 0x00),			//0    // black - but reserved layer
	rgbcolor(0xFF, 0x00, 0x00),
	rgbcolor(0xFF, 0xFF, 0x00),
	rgbcolor(0x00, 0xFF, 0x00),
	rgbcolor(0x00, 0xFF, 0xFF),
	rgbcolor(0x00, 0x00, 0xFF),			//5
	rgbcolor(0xFF, 0x00, 0xFF),
	rgbcolor(0xFF, 0xFF, 0xFF),			//7 is the "visible" color - so what normally in PostScript is black (in CAD "white" is visible)
	rgbcolor(0x41, 0x41, 0x41),
	rgbcolor(0x80, 0x80, 0x80),
	rgbcolor(0xFF, 0x00, 0x00),			//10
	rgbcolor(0xFF, 0xAA, 0xAA),
	rgbcolor(0xBD, 0x00, 0x00),
	rgbcolor(0xBD, 0x7E, 0x7E),
	rgbcolor(0x81, 0x00, 0x00),
	rgbcolor(0x81, 0x56, 0x56),			//15
	rgbcolor(0x68, 0x00, 0x00),
	rgbcolor(0x68, 0x45, 0x45),
	rgbcolor(0x4F, 0x00, 0x00),
	rgbcolor(0x4F, 0x35, 0x35),
	rgbcolor(0xFF, 0x3F, 0x00),			//20
	rgbcolor(0xFF, 0xBF, 0xAA),
	rgbcolor(0xBD, 0x2E, 0x00),
	rgbcolor(0xBD, 0x8D, 0x7E),
	rgbcolor(0x81, 0x1F, 0x00),
	rgbcolor(0x81, 0x60, 0x56),			//25
	rgbcolor(0x68, 0x19, 0x00),
	rgbcolor(0x68, 0x4E, 0x45),
	rgbcolor(0x4F, 0x13, 0x00),
	rgbcolor(0x4F, 0x3B, 0x35),
	rgbcolor(0xFF, 0x7F, 0x00),			//30
	rgbcolor(0xFF, 0xD4, 0xAA),
	rgbcolor(0xBD, 0x5E, 0x00),
	rgbcolor(0xBD, 0x9D, 0x7E),
	rgbcolor(0x81, 0x40, 0x00),
	rgbcolor(0x81, 0x6B, 0x56),			//35
	rgbcolor(0x68, 0x34, 0x00),
	rgbcolor(0x68, 0x56, 0x45),
	rgbcolor(0x4F, 0x27, 0x00),
	rgbcolor(0x4F, 0x42, 0x35),
	rgbcolor(0xFF, 0xBF, 0x00),			//40
	rgbcolor(0xFF, 0xEA, 0xAA),
	rgbcolor(0xBD, 0x8D, 0x00),
	rgbcolor(0xBD, 0xAD, 0x7E),
	rgbcolor(0x81, 0x60, 0x00),
	rgbcolor(0x81, 0x76, 0x56),			//45
	rgbcolor(0x68, 0x4E, 0x00),
	rgbcolor(0x68, 0x5F, 0x45),
	rgbcolor(0x4F, 0x3B, 0x00),
	rgbcolor(0x4F, 0x49, 0x35),
	rgbcolor(0xFF, 0xFF, 0x00),			//50
	rgbcolor(0xFF, 0xFF, 0xAA),
	rgbcolor(0xBD, 0xBD, 0x00),
	rgbcolor(0xBD, 0xBD, 0x7E),
	rgbcolor(0x81, 0x81, 0x00),
	rgbcolor(0x81, 0x81, 0x56),			//55
	rgbcolor(0x68, 0x68, 0x00),
	rgbcolor(0x68, 0x68, 0x45),
	rgbcolor(0x4F, 0x4F, 0x00),
	rgbcolor(0x4F, 0x4F, 0x35),
	rgbcolor(0xBF, 0xFF, 0x00),			//60
	rgbcolor(0xEA, 0xFF, 0xAA),
	rgbcolor(0x8D, 0xBD, 0x00),
	rgbcolor(0xAD, 0xBD, 0x7E),
	rgbcolor(0x60, 0x81, 0x00),
	rgbcolor(0x76, 0x81, 0x56),			//65
	rgbcolor(0x4E, 0x68, 0x00),
	rgbcolor(0x5F, 0x68, 0x45),
	rgbcolor(0x3B, 0x4F, 0x00),
	rgbcolor(0x49, 0x4F, 0x35),
	rgbcolor(0x7F, 0xFF, 0x00),			//70
	rgbcolor(0xD4, 0xFF, 0xAA),
	rgbcolor(0x5E, 0xBD, 0x00),
	rgbcolor(0x9D, 0xBD, 0x7E),
	rgbcolor(0x40, 0x81, 0x00),
	rgbcolor(0x6B, 0x81, 0x56),			//75
	rgbcolor(0x34, 0x68, 0x00),
	rgbcolor(0x56, 0x68, 0x45),
	rgbcolor(0x27, 0x4F, 0x00),
	rgbcolor(0x42, 0x4F, 0x35),
	rgbcolor(0x3F, 0xFF, 0x00),			//80
	rgbcolor(0xBF, 0xFF, 0xAA),
	rgbcolor(0x2E, 0xBD, 0x00),
	rgbcolor(0x8D, 0xBD, 0x7E),
	rgbcolor(0x1F, 0x81, 0x00),
	rgbcolor(0x60, 0x81, 0x56),			//85
	rgbcolor(0x19, 0x68, 0x00),
	rgbcolor(0x4E, 0x68, 0x45),
	rgbcolor(0x13, 0x4F, 0x00),
	rgbcolor(0x3B, 0x4F, 0x35),
	rgbcolor(0x00, 0xFF, 0x00),			//90
	rgbcolor(0xAA, 0xFF, 0xAA),
	rgbcolor(0x00, 0xBD, 0x00),
	rgbcolor(0x7E, 0xBD, 0x7E),
	rgbcolor(0x00, 0x81, 0x00),
	rgbcolor(0x56, 0x81, 0x56),			//95
	rgbcolor(0x00, 0x68, 0x00),
	rgbcolor(0x45, 0x68, 0x45),
	rgbcolor(0x00, 0x4F, 0x00),
	rgbcolor(0x35, 0x4F, 0x35),
	rgbcolor(0x00, 0xFF, 0x3F),			//100
	rgbcolor(0xAA, 0xFF, 0xBF),
	rgbcolor(0x00, 0xBD, 0x2E),
	rgbcolor(0x7E, 0xBD, 0x8D),
	rgbcolor(0x00, 0x81, 0x1F),
	rgbcolor(0x56, 0x81, 0x60),			//105
	rgbcolor(0x00, 0x68, 0x19),
	rgbcolor(0x45, 0x68, 0x4E),
	rgbcolor(0x00, 0x4F, 0x13),
	rgbcolor(0x35, 0x4F, 0x3B),
	rgbcolor(0x00, 0xFF, 0x7F),			//110
	rgbcolor(0xAA, 0xFF, 0xD4),
	rgbcolor(0x00, 0xBD, 0x5E),
	rgbcolor(0x7E, 0xBD, 0x9D),
	rgbcolor(0x00, 0x81, 0x40),
	rgbcolor(0x56, 0x81, 0x6B),			//115
	rgbcolor(0x00, 0x68, 0x34),
	rgbcolor(0x45, 0x68, 0x56),
	rgbcolor(0x00, 0x4F, 0x27),
	rgbcolor(0x35, 0x4F, 0x42),
	rgbcolor(0x00, 0xFF, 0xBF),			//120
	rgbcolor(0xAA, 0xFF, 0xEA),
	rgbcolor(0x00, 0xBD, 0x8D),
	rgbcolor(0x7E, 0xBD, 0xAD),
	rgbcolor(0x00, 0x81, 0x60),
	rgbcolor(0x56, 0x81, 0x76),			//125
	rgbcolor(0x00, 0x68, 0x4E),
	rgbcolor(0x45, 0x68, 0x5F),
	rgbcolor(0x00, 0x4F, 0x3B),
	rgbcolor(0x35, 0x4F, 0x49),
	rgbcolor(0x00, 0xFF, 0xFF),			//130
	rgbcolor(0xAA, 0xFF, 0xFF),
	rgbcolor(0x00, 0xBD, 0xBD),
	rgbcolor(0x7E, 0xBD, 0xBD),
	rgbcolor(0x00, 0x81, 0x81),
	rgbcolor(0x56, 0x81, 0x81),			//135
	rgbcolor(0x00, 0x68, 0x68),
	rgbcolor(0x45, 0x68, 0x68),
	rgbcolor(0x00, 0x4F, 0x4F),
	rgbcolor(0x35, 0x4F, 0x4F),
	rgbcolor(0x00, 0xBF, 0xFF),			//140
	rgbcolor(0xAA, 0xEA, 0xFF),
	rgbcolor(0x00, 0x8D, 0xBD),
	rgbcolor(0x7E, 0xAD, 0xBD),
	rgbcolor(0x00, 0x60, 0x81),
	rgbcolor(0x56, 0x76, 0x81),			//145
	rgbcolor(0x00, 0x4E, 0x68),
	rgbcolor(0x45, 0x5F, 0x68),
	rgbcolor(0x00, 0x3B, 0x4F),
	rgbcolor(0x35, 0x49, 0x4F),
	rgbcolor(0x00, 0x7F, 0xFF),			//150
	rgbcolor(0xAA, 0xD4, 0xFF),
	rgbcolor(0x00, 0x5E, 0xBD),
	rgbcolor(0x7E, 0x9D, 0xBD),
	rgbcolor(0x00, 0x40, 0x81),
	rgbcolor(0x56, 0x6B, 0x81),			//155
	rgbcolor(0x00, 0x34, 0x68),
	rgbcolor(0x45, 0x56, 0x68),
	rgbcolor(0x00, 0x27, 0x4F),
	rgbcolor(0x35, 0x42, 0x4F),
	rgbcolor(0x00, 0x3F, 0xFF),			//160
	rgbcolor(0xAA, 0xBF, 0xFF),
	rgbcolor(0x00, 0x2E, 0xBD),
	rgbcolor(0x7E, 0x8D, 0xBD),
	rgbcolor(0x00, 0x1F, 0x81),
	rgbcolor(0x56, 0x60, 0x81),			//165
	rgbcolor(0x00, 0x19, 0x68),
	rgbcolor(0x45, 0x4E, 0x68),
	rgbcolor(0x00, 0x13, 0x4F),
	rgbcolor(0x35, 0x3B, 0x4F),
	rgbcolor(0x00, 0x00, 0xFF),			//170
	rgbcolor(0xAA, 0xAA, 0xFF),
	rgbcolor(0x00, 0x00, 0xBD),
	rgbcolor(0x7E, 0x7E, 0xBD),
	rgbcolor(0x00, 0x00, 0x81),
	rgbcolor(0x56, 0x56, 0x81),			//175
	rgbcolor(0x00, 0x00, 0x68),
	rgbcolor(0x45, 0x45, 0x68),
	rgbcolor(0x00, 0x00, 0x4F),
	rgbcolor(0x35, 0x35, 0x4F),
	rgbcolor(0x3F, 0x00, 0xFF),			//180
	rgbcolor(0xBF, 0xAA, 0xFF),
	rgbcolor(0x2E, 0x00, 0xBD),
	rgbcolor(0x8D, 0x7E, 0xBD),
	rgbcolor(0x1F, 0x00, 0x81),
	rgbcolor(0x60, 0x56, 0x81),			//185
	rgbcolor(0x19, 0x00, 0x68),
	rgbcolor(0x4E, 0x45, 0x68),
	rgbcolor(0x13, 0x00, 0x4F),
	rgbcolor(0x3B, 0x35, 0x4F),
	rgbcolor(0x7F, 0x00, 0xFF),			//190
	rgbcolor(0xD4, 0xAA, 0xFF),
	rgbcolor(0x5E, 0x00, 0xBD),
	rgbcolor(0x9D, 0x7E, 0xBD),
	rgbcolor(0x40, 0x00, 0x81),
	rgbcolor(0x6B, 0x56, 0x81),			//195
	rgbcolor(0x34, 0x00, 0x68),
	rgbcolor(0x56, 0x45, 0x68),
	rgbcolor(0x27, 0x00, 0x4F),
	rgbcolor(0x42, 0x35, 0x4F),
	rgbcolor(0xBF, 0x00, 0xFF),			//200
	rgbcolor(0xEA, 0xAA, 0xFF),
	rgbcolor(0x8D, 0x00, 0xBD),
	rgbcolor(0xAD, 0x7E, 0xBD),
	rgbcolor(0x60, 0x00, 0x81),
	rgbcolor(0x76, 0x56, 0x81),			//205
	rgbcolor(0x4E, 0x00, 0x68),
	rgbcolor(0x5F, 0x45, 0x68),
	rgbcolor(0x3B, 0x00, 0x4F),
	rgbcolor(0x49, 0x35, 0x4F),
	rgbcolor(0xFF, 0x00, 0xFF),			//210
	rgbcolor(0xFF, 0xAA, 0xFF),
	rgbcolor(0xBD, 0x00, 0xBD),
	rgbcolor(0xBD, 0x7E, 0xBD),
	rgbcolor(0x81, 0x00, 0x81),
	rgbcolor(0x81, 0x56, 0x81),			//215
	rgbcolor(0x68, 0x00, 0x68),
	rgbcolor(0x68, 0x45, 0x68),
	rgbcolor(0x4F, 0x00, 0x4F),
	rgbcolor(0x4F, 0x35, 0x4F),
	rgbcolor(0xFF, 0x00, 0xBF),			//220
	rgbcolor(0xFF, 0xAA, 0xEA),
	rgbcolor(0xBD, 0x00, 0x8D),
	rgbcolor(0xBD, 0x7E, 0xAD),
	rgbcolor(0x81, 0x00, 0x60),
	rgbcolor(0x81, 0x56, 0x76),			//225
	rgbcolor(0x68, 0x00, 0x4E),
	rgbcolor(0x68, 0x45, 0x5F),
	rgbcolor(0x4F, 0x00, 0x3B),
	rgbcolor(0x4F, 0x35, 0x49),
	rgbcolor(0xFF, 0x00, 0x7F),			//230
	rgbcolor(0xFF, 0xAA, 0xD4),
	rgbcolor(0xBD, 0x00, 0x5E),
	rgbcolor(0xBD, 0x7E, 0x9D),
	rgbcolor(0x81, 0x00, 0x40),
	rgbcolor(0x81, 0x56, 0x6B),			//235
	rgbcolor(0x68, 0x00, 0x34),
	rgbcolor(0x68, 0x45, 0x56),
	rgbcolor(0x4F, 0x00, 0x27),
	rgbcolor(0x4F, 0x35, 0x42),
	rgbcolor(0xFF, 0x00, 0x3F),			//240
	rgbcolor(0xFF, 0xAA, 0xBF),
	rgbcolor(0xBD, 0x00, 0x2E),
	rgbcolor(0xBD, 0x7E, 0x8D),
	rgbcolor(0x81, 0x00, 0x1F),
	rgbcolor(0x81, 0x56, 0x60),			//245
	rgbcolor(0x68, 0x00, 0x19),
	rgbcolor(0x68, 0x45, 0x4E),
	rgbcolor(0x4F, 0x00, 0x13),
	rgbcolor(0x4F, 0x35, 0x3B),
	rgbcolor(0x33, 0x33, 0x33),			//250
	rgbcolor(0x50, 0x50, 0x50),
	rgbcolor(0x69, 0x69, 0x69),
	rgbcolor(0x82, 0x82, 0x82),
	rgbcolor(0xBE, 0xBE, 0xBE),
	rgbcolor(0xFF, 0xFF, 0xFF)			//255
};

const unsigned short DXFColor::numberOfColors =
sizeof(DXFColor::DXFColors) / sizeof(DXFColor::rgbcolor);


unsigned int DXFColor::getDXFColor(float r, float g, float b,unsigned int firstcolor)
{
	float mindist = 2.0f;  // theoretically the distance is max 1.0f - but to avoid rounding problems.
	unsigned int best = firstcolor;
	for (unsigned int i = firstcolor; i < numberOfColors; i++) {
		const float dxfr = DXFColors[i].r / 255.0f;
		const float dxfg = DXFColors[i].g / 255.0f;
		const float dxfb = DXFColors[i].b / 255.0f;
		const float dr = dxfr - r;
		const float dg = dxfg - g;
		const float db = dxfb - b;
		const float dist = dr * dr + dg * dg + db * db;
		if (dist == 0.0) {
//          errf << "Found exact match for " << r << " "
//              << g << " " << b << " " << i  << " " << numberOfColors << endl;
			return i;
		} else if (dist < mindist) {
			best = i;
			mindist = dist;
		}
	}
//  errf << "Found approximation for " << r << " "
//      << g << " " << b << " " << best  << " " << mindist  << " " << numberOfColors << endl;
	return best;
}


static constexpr unsigned short floatColTointCol(float fcol) {
	return (unsigned short) (fcol*255);
}

class DXFLayers {
public:

	static RSString normalizeColorName(const RSString & s){
		// normalizes color names such that they can be used as DXF layer names
		// all characters are converted to upper case
		// all non-alphanumeric characters are replaced by '_'

		char * s_copy = cppstrdup(s.c_str());
		assert(s_copy);
		char * cp = s_copy;
		while (cp && *cp) {
			if(islower(*cp) && isascii(*cp)) {
				*cp = static_cast<char>(toupper(*cp));
			}
			if (!isalnum(*cp)) {
				*cp = '_'; // replace all not alphanumeric characters with _
			}
			cp++;
		}
		const RSString normalized(s_copy);
		delete [] s_copy;
		return normalized;
	}

	struct Layer {
		Layer(float r, float g, float b, struct Layer * next_p = nullptr): rgb(floatColTointCol(r),floatColTointCol(g),floatColTointCol(b)), next(next_p) {}
		const DXFColor::rgbcolor rgb;
		struct Layer * next;
	};
	struct NamedLayer {
		explicit NamedLayer(const RSString& s, struct NamedLayer * next_p=nullptr): layerName(s), next(next_p) {}
		const RSString layerName;
		struct NamedLayer* next;
	};

	Layer * LayerTable[DXFColor::numberOfColors];
	
	// for each color as defined in dxfcolor, we can have a linked list of layers
	// This acts like a hash table with numberofcolors entries and the LayerList as buckets.
	// we may at the end have several layers for the same color ("same" modulo the number of
	// entries in DXFColor, i.e. "best match") - but the layerNames are still representing the exact color.

	unsigned int numberOfLayers;

	NamedLayer * namedLayers; // just a list of Layernames - these names from from PostScript Separation names
	// and are used in the DXF without any color values (or better said - mapped to B&W color)


	DXFLayers() : numberOfLayers(0), namedLayers(nullptr) { 
		for (unsigned int i = 0; i < DXFColor::numberOfColors; i++) LayerTable[i]= nullptr; 
	}

	~DXFLayers() {
		for (unsigned int i = 0; i < DXFColor::numberOfColors; i++) {
			Layer * p = LayerTable[i];
			while (p) {
				Layer* pnext = p->next;
				delete p;
				p = pnext;
			}
			LayerTable[i]= nullptr;
		}
		NamedLayer* nl = namedLayers;
		while (nl) {
			NamedLayer* pnext = nl->next;
			delete nl;
			nl = pnext;
		}
	}

	void rememberLayerName(const RSString & s) {
		NamedLayer* nl = namedLayers;
		while (nl) {
			if (nl->layerName == s) return;
			nl = nl->next;
		}
		// not found - so prepend to list;
		namedLayers = new NamedLayer(s,namedLayers); // prepend to list;
		numberOfLayers++;
	}

	bool alreadyDefined(float r, float g, float b, unsigned int index) const {
		assert(index < DXFColor::numberOfColors);

		const unsigned short R = floatColTointCol(r);
		const unsigned short G = floatColTointCol(g);
		const unsigned short B = floatColTointCol(b);

		const Layer * ptr = LayerTable[index];
		while(ptr) {
			if( (ptr->rgb.r == R) && 
				(ptr->rgb.g == G) && 
				(ptr->rgb.b == B) ) {
					// cout << "already defined" << R << " " << G << " " << B << endl;
					return true;
				}
			ptr = ptr->next;
		}
		// cout << "not yet defined" << R << " " << G << " " << B << endl;
		return false;
	}

	void defineLayer(float r, float g, float b, unsigned int index) {
		assert(index < DXFColor::numberOfColors);

		Layer * headptr = LayerTable[index];

		// prepend new layer to the linked list
		LayerTable[index] = new Layer(r, g, b, headptr);
		numberOfLayers++;
	}

	static const char * getLayerName(unsigned short r,unsigned short g,unsigned short b) {
		static char stringbuffer[20]; // format: "Cxx-xx-xx" (10 chars)
		sprintf_s(TARGETWITHLEN(stringbuffer,20),"C%02X-%02X-%02X",r,g,b);
		return stringbuffer;
	}
	static const char * getLayerName(float r, float g, float b) {
		// rgb is in range 0..1.0f
		const unsigned short R = floatColTointCol(r);
		const unsigned short G = floatColTointCol(g);
		const unsigned short B = floatColTointCol(b);
		return getLayerName(R,G,B);
	}
};



/* results: (25.12.2001) (v - VoloView ; i - IntelliCAD)
p2ed -f dxf_s:-splineasbezier 			curvetest.ps curve_bz.dxf v ok  i   ok
p2ed -f dxf_s:-splineaspolyline 		curvetest.ps curve_pl.dxf v ok  i   ok
#p2ed -f dxf_s:-splineasnurb 			curvetest.ps curve_nu.dxf v no  i   syntaktisch ja, aber die koordinaten werden falsch interpretiert (y als z)
#p2ed -f dxf_s:-splineasbspline 		curvetest.ps curve_bs.dxf v no  i   syntaktisch ja, aber die koordinaten werden falsch interpretiert (y als z)
#p2ed -f dxf_s:-splineassinglespline 	curvetest.ps curve_ss.dxf v no  i   syntaktisch ja, aber die koordinaten werden falsch interpretiert (y als z)
#p2ed -f dxf_s:-splineasmultispline 	curvetest.ps curve_ms.dxf v ok, but a bit strange  i same as acad
p2ed -f dxf 							curvetest.ps curve_np.dxf v ok  i   ok
p2ed -f dxf:-polyaslines 				curvetest.ps curve_nl.dxf v ok  i   ok
*/

drvDXF::derivedConstructor(drvDXF): 
	constructBase, 
	splinemode(asbezier),
	formatis14(true),  // !!!! THESE TWO NEED TO BE CONSISTENT
	layers(new DXFLayers),
	handleint(0xFF),
	scalefactor(1.0f),
	buffer(tempFile.asOutput())

{
#ifdef withshortheaders	
	header(dxf14shortheader),
	trailer(dxf14shorttrailer),
#endif
	
	const bool withcurves = driverdesc.backendSupportsCurveto;
	formatis14 = withcurves; // if no curves are generated by the frontent, no splines can occur and thus the old header is sufficient

	if (options->splineasbezier) {
		splinemode = asbezier;
		formatis14 = true;
		if (!withcurves) { errf << "Option -splineasbezier ignored - only valid for dxf_14 format" << endl; }
	} else if (options->splineaspolyline) {
		splinemode = aspolyline;	
		formatis14 = true;
		if (!withcurves) { errf << "Option -splineaspolyline ignored - only valid for dxf_14 format" << endl; }
	} else if (options->splineasnurb) {
		splinemode = asnurb;		
		formatis14 = true;
		if (!withcurves) { errf << "Option -splineasmultispline ignored - only valid for dxf_14 format" << endl; }
	} else if (options->splineasbspline) {
		splinemode = asbspline;		
		formatis14 = true;
		if (!withcurves) { errf << "Option -splineasmultispline ignored - only valid for dxf_14 format" << endl; }
	} else if (options->splineasmultispline) {
		splinemode = asmultispline;	
		formatis14 = true;
		if (!withcurves) { errf << "Option -splineasmultispline ignored - only valid for dxf_14 format" << endl; }
	} else if (options->splineassinglespline) {
		splinemode = assinglespline;
		formatis14 = true;
		if (!withcurves) { errf << "Option -splineassinglespline ignored - only valid for dxf_14 format" << endl; }
	}

	if (options->splineprecision < 2) {
		errf << "splineprecision argument should be >=2. Set to 2" << endl;
		options->splineprecision = 2;
	}

	if (options->mm) {
		//debug cout << "mm enabled " << endl;
		scalefactor = 25.4f / 72.0f;
	} else {
		//debug cout << "inch enabled " << endl;
		scalefactor = 1.0f / 72.0f;
	}
	DXF_LineType::scalefactor = scalefactor;

	if (formatis14) {
		// voloview doesn't like this header    
		outf << "999\nDXF generated by pstoedit version " << drvbase::VersionString() << "\n";
		outf << dxf14acadheader_prelayer1;
		/*
		$MEASUREMENT 70 Sets drawing units: 
		                0 = English; 1 = Metric
		$INSUNITS    70 Default drawing units for AutoCAD DesignCenter blocks :
		                0 = Unitless; 1 = Inches; 2 = Feet; 3 = Miles; 4 = Millimeters;
		$LUNITS      70 Units format for coordinates and distances
		*/

		if(options->mm) {
			outf <<
				"  9\n"
				"$MEASUREMENT\n"
				" 70\n"
				"1\n" // 1 means metric - 0 english
				"  9\n"
				"$INSUNITS\n"
				" 70\n"
				"4\n"
				"  9\n"
				"$LUNITS\n"
				" 70\n"
				"4\n";
		} else {
			outf <<
				"  9\n"
				"$MEASUREMENT\n"
				" 70\n"
				"0\n" // 1 means metric - 0 english
				"  9\n"
				"$INSUNITS\n"
				" 70\n"
				"1\n" 
				"  9\n"
				"$LUNITS\n"
				" 70\n"
				"1\n";
		}
		outf <<
		    "  9\n"
			"$EXTMIN\n";
		printPoint(outf, Point(0.0f, 0.0), 10);
		outf <<
			"  9\n"
			"$EXTMAX\n";
		printPoint(outf, Point(3000.0f, 3000.0), 10);

		outf << dxf14acadheader_prelayer2;

		// only these are supported currently
		outf << dxf_dotted;
		outf << dxf_dashed;
		outf << dxf_dashdot;
		outf << dxf_dashdotdot;

		outf << dxf14_line_type_tab_trailer_and_begin_of_layer_tab; 
	} else {

		outf << dxf9shortheader_prelayer1;
		outf <<
			"  9\n"
			"$EXTMIN\n";
		printPoint(outf, Point(0.0f, 0.0), 10);
		outf <<
			"  9\n"
			"$EXTMAX\n";
		printPoint(outf, Point(3000.0f, 3000.0), 10);
		outf << dxf9shortheader_prelayer2;
		//
		// unfortunately we cannot write the $MEASUREMENT statement into old style files
		// otherwise voloview does not accept the file
		//
		// so this may lead to "wrong" scaling. The -mm option turns on mm, but the file is 
		// still displayed in inch-mode since that seems to be the default in DXF.
		//
	}
}

void drvDXF::writelayerentry(ostream & outs, unsigned int color, const char * layername) {
	outs <<
		"  0\n"
		"LAYER\n";
	if (formatis14) {
		writeHandle(outs);
		outs <<
			"100\n"
			"AcDbSymbolTableRecord\n"
			"100\n"
			"AcDbLayerTableRecord\n";
	}
	outs << "  2\n" 
		<< layername << endl; // layername
	outs <<
		" 70\n"
		"0\n"  // unfrozen
		" 62\n";
	outs << color << endl; // color
	outs << "  6\n"
			"CONTINUOUS\n" ;		// linestyle
}

drvDXF::~drvDXF()
{
	// write size of color definition table
	if (options->colorsToLayers) {
		outf << layers->numberOfLayers + 4 << endl; // 2 layers for C00-00-00-BLACK and CFF-FF-FF-WHITE and one predefined and one reserve
	} else {
		outf << "1" << endl;
	}
	if (formatis14) {
		outf << dxf14layer0def;
	} else {
		outf << dxf9layer0def;
	}
	if (options->colorsToLayers) {
		// write the layer definitions
		writelayerentry(outf,7,"C00-00-00-BLACK"); // both black and white have color 7 ("visible")
		writelayerentry(outf,7,"CFF-FF-FF-WHITE");
		for (unsigned int i = 0; i < DXFColor::numberOfColors; i++) {
			DXFLayers::Layer * layer = layers->LayerTable[i];
			while (layer) {
				DXFLayers::Layer* pnext = layer->next;
				if (options->dumplayernames) {
					cout << "Layer (generated): " << DXFLayers::getLayerName(layer->rgb.r,layer->rgb.g,layer->rgb.b) << endl;
				}
				writelayerentry(outf,i,DXFLayers::getLayerName(layer->rgb.r,layer->rgb.g,layer->rgb.b));
				layer = pnext;
			}
		}
		DXFLayers::NamedLayer * nl = layers->namedLayers;
		while (nl) {
			if (options->dumplayernames) {
				cout << "Layer (defined in input): " << nl->layerName.c_str() << endl;
			}
			writelayerentry(outf,7,nl->layerName.c_str());
			nl = nl->next;
		}
	}

	if (formatis14) {
		outf << dxf14acadheader_postlayer;
	}
	else {
		outf << dxf9shortheader_postlayer;
	}
	
	// now we can copy the buffer the output
	ifstream & inbuffer = tempFile.asInput();
	copy_file(inbuffer, outf);

	if (formatis14) {
		outf << dxf14acadtrailer;
	} else {
		outf << dxf9shorttrailer;
	}
	
	delete layers; layers = nullptr;
	options=nullptr;
}



void drvDXF::writeHandle(ostream & outs) {

	// VoloView needs the handle, intellicad doesn't need it but they don't harm
	write_DXF_handle(outs, handleint);
	handleint++;
}

void drvDXF::writeLayer(float r, float g, float b, const RSString& colorName) 
{
	//
	// Some notes about layers:
	//
	// If layers are not defined in the header, they implicitly get color white.
	// An object drawn on a specific layer can either get the color by the layer (BYLAYER) 
	// or define its own color (so far BYLAYER is not used by pstoedit)
	//

	buffer << "  8\n";
	buffer << calculateLayerString(r,g,b,colorName) << endl;
}
RSString drvDXF::calculateLayerString(float r, float g, float b, const RSString& colorName) 
{
	//
	// Some notes about layers:
	//
	// If layers are not defined in the header, they implicitly get color white.
	// An object drawn on a specific layer can either get the color by the layer (BYLAYER) 
	// or define its own color (so far BYLAYER is not used by pstoedit)
	//

	if (options->colorsToLayers) {		
		constexpr float roundinglimit = 0.001f;

		// map black (0,0,0) to layer 7-black and white (1,1,1) to 7-white

		if (colorName != "") {
			layers->rememberLayerName(colorName);
			return colorName ;
		} else if ((r < roundinglimit) && 
			(g < roundinglimit) && 
			(b < roundinglimit) ) {
					// black
			return RSString( "C00-00-00-BLACK");
		} else if (	(r > (1.0f- roundinglimit)) && 
					(g > (1.0f- roundinglimit)) && 
					(b > (1.0f- roundinglimit)) ) {
			return RSString( "CFF-FF-FF-WHITE");
						// white
		} else {
			const unsigned int dxfcolor = DXFColor::getDXFColor(r,g,b,1);
			const char * layerString = DXFLayers::getLayerName(r,g,b);
			if (! (layers->alreadyDefined(r,g,b,dxfcolor))) {
				layers->defineLayer(r,g,b,dxfcolor);
	//			cout << "defined new layer " << layerString << endl;
			}
			return RSString(layerString);
		}
	} else {
		return RSString("0");
	}
}

bool drvDXF::wantedLayer(float r, float g, float b,const RSString& colorName)  // layer shall be written
{ 
	// inspect layerpositivfilter and layerpositivfilter
	// use colorname if set, else calculate layername from color
	// then if positivfilter is set, then check if contained in that one -> true
	// else if negativfilter is set, then check if contained in that one -> false
	// else -> true
	static const RSString comma(","); // used as delimiter for string based containment test
	if (options->layerpositivfilter.value != "" ) {
		static const RSString compareto = comma + options->layerpositivfilter.value + comma;
		const RSString layerstring = comma + calculateLayerString(r,g,b,colorName) + comma ;
		// cout << "L: " << layerstring << " P:" << options->layerpositivfilter << " " << str_contains(options->layerpositivfilter.value,layerstring) << endl;
		return string_contains(compareto,layerstring);
	} else if (options->layernegativfilter.value != "" ) {
		const RSString layerstring = comma + calculateLayerString(r,g,b,colorName) + comma;
		static const RSString compareto = comma + options->layerpositivfilter.value + comma;
		// cout << "L: " << layerstring << " N:" << options->layernegativfilter << " " << !str_contains(options->layernegativfilter.value,layerstring) << endl;
		return !string_contains(compareto,layerstring);
	} else return true;
}

void drvDXF::close_page()
{
/*buffer << "#Seite beendet.\n";*/
}

void drvDXF::open_page()
{
/*buffer << "#Seite Nr. " << currentPageNumber << "\n";*/
}

void drvDXF::writeColorAndStyle() {
  static const char * const LineStyle[] =
	{ "CONTINUOUS", "DASHED", "DOT", "DASHDOT", "DIVIDE" };
	//solid = 0, dashed, dotted, dashdot, dashdotdot
	// https://dxfwrite.readthedocs.io/en/latest/entities/linepattern.html
  if (!options->colorsToLayers) buffer << " 62\n     " << DXFColor::getDXFColor(currentR(), currentG(), currentB()) << '\n';
  if (formatis14) {
	  buffer << "  6\n" << LineStyle[currentLineType()] << '\n';
  }
}
void drvDXF::show_text(const TextInfo & textinfo)
{
  if (wantedLayer(textinfo.currentR, textinfo.currentG, textinfo.currentB,DXFLayers::normalizeColorName(textinfo.colorName))) {
	buffer << "  0\n"
			  "TEXT\n";

	if (formatis14) {
		writeHandle(buffer);
		buffer << "100\n" "AcDbEntity\n";
		writeLayer(textinfo.currentR, textinfo.currentG, textinfo.currentB,DXFLayers::normalizeColorName(textinfo.colorName)); //"  8\n" "0\n"
		buffer << "100\n" "AcDbText\n";
	} else {
		writeLayer(textinfo.currentR, textinfo.currentG, textinfo.currentB,DXFLayers::normalizeColorName(textinfo.colorName)); //buffer << "  8\n" "0\n";
	}

	// color
	if (!options->colorsToLayers) buffer << " 62\n     " << DXFColor::getDXFColor(textinfo.currentR,
												  textinfo.currentG, 
												  textinfo.currentB)
		<< "\n";
	printPoint(buffer,textinfo.p, 10);
	buffer << " 40\n" << textinfo.currentFontSize * scalefactor << "\n";
	buffer << "  1\n" << textinfo.thetext.c_str() << "\n";
	buffer << " 50\n" << textinfo.currentFontAngle << "\n";

	// FIXME: we need a STYLE table for this. So far not supported.
	// buffer << "  7\n" << textinfo.currentFontName << "\n";

	if (formatis14) {
	  buffer << "100\n"
				"AcDbText\n";
	}
  }
}

void drvDXF::printPoint(ostream & out, const Point & p, unsigned short offset, bool with_z)
{
	out << " " << offset << "\n" << p.x()* scalefactor << "\n";
	out << " " << 10 + offset << "\n" << p.y()* scalefactor << "\n";
	if (with_z) {
		out << " " << 20 + offset << "\n" << "0.0" << "\n";
	}
}

void drvDXF::drawVertex(const Point & p, bool withlinewidth, int val70)
{
  if (wantedLayer(currentR(), currentG(), currentB(),DXFLayers::normalizeColorName(currentColorName()))) {
	buffer << "  0\nVERTEX\n"; // "  8\n0\n";
	writeLayer(currentR(), currentG(), currentB(),DXFLayers::normalizeColorName(currentColorName()));
	printPoint(buffer,p, 10);
	if (withlinewidth) {
		const double lineWidth = currentLineWidth()* scalefactor;
		buffer << " 40\n" << lineWidth << "\n 41\n" << lineWidth << "\n";
	}
	if (val70)					// used only for try with spline type polylines (which doesn't work anyway)
		buffer << " 70\n    16\n";
  }
}

void drvDXF::drawLine(const Point & start_p, const Point & end_p)
{
  if (wantedLayer(currentR(), currentG(), currentB(),DXFLayers::normalizeColorName(currentColorName()))) {
	buffer << "  0\nLINE\n"; // "  8\n0\n";
	if (formatis14) {
		writeHandle(buffer);
		//entity 
		buffer <<	"100\n" "AcDbEntity\n";
		writeLayer(currentR(), currentG(), currentB(),DXFLayers::normalizeColorName(currentColorName())); //buffer <<	"  8\n" "0\n" ;
		buffer <<	"100\n" "AcDbLine" << endl;
	} else {
		writeLayer(currentR(), currentG(), currentB(),DXFLayers::normalizeColorName(currentColorName()));
	}
	// color
	//if (!options->colorsToLayers) buffer << " 62\n     " << DXFColor::getDXFColor(currentR(), currentG(), currentB()) << '\n';
	writeColorAndStyle();
	printPoint(buffer,start_p, 10);
	printPoint(buffer,end_p, 11);
  }
}

#if 0
0 LINE
	5
	64
	100
	AcDbEntity
	8 0 100 AcDbLine 10 - 13.272601 20 347.062801 30 0.0 11 89.636603 21 270.615964 31 0.0
#endif

#if 0
/* 
	Spline group codes Group codes Description 
100
 Subclass marker (AcDbSpline) 
 
210
 Normal vector (omitted if the spline is nonplanar)
DXF: X value; APP: 3D vector
 
220, 230
 DXF: Y and Z values of normal vector
 
70
 Spline flag (bit coded):
1 = Closed spline
2 = Periodic spline
4 = Rational spline
8 = Planar
16 = Linear (planar bit is also set) 
 
71
 Degree of the spline curve
 
72
 Number of knots
 
73
 Number of control points
 
74
 Number of fit points (if any)
 
42
 Knot tolerance (default = 0.0000001)
 
43
 Control-point tolerance (default = 0.0000001)
 
44
 Fit tolerance (default = 0.0000000001)
 
12
 Start tangent--may be omitted (in WCS).
DXF: X value; APP: 3D point.
 
22, 32
 DXF: Y and Z values of start tangent--may be omitted (in WCS).
 
13
 End tangent--may be omitted (in WCS).
DXF: X value; APP: 3D point.
 
23, 33
 DXF: Y and Z values of end tangent--may be omitted (in WCS)
 
40
 Knot value (one entry per knot)
 
41
 Weight (if not 1); with multiple group pairs, are present if all are not 1
 
10
 Control points (in WCS) one entry per control point.
DXF: X value; APP: 3D point
 
20, 30
 DXF: Y and Z values of control points (in WCS) (one entry per control point)
 
11
 Fit points (in WCS) one entry per fit point.
DXF: X value; APP: 3D point
 
21, 31
 DXF: Y and Z values of fit points (in WCS) (one entry per fit point)
 

*/

#endif

//NU const unsigned short ClosedSpline	= 1 ; //= Closed spline
//NU const unsigned short PeriodicSpline = 2 ; //= Periodic spline
constexpr unsigned short RationalSpline = 4 ; //= Rational spline
//NU const unsigned short PlanarSpline	= 8 ; //= Planar
//NU const unsigned short LinearSpline	= 16 ; //= Linear (planar bit is also set) 


void drvDXF::writesplinetype(const unsigned short stype = 0) {
	buffer << " 70\n     " << stype << "\n";	  /* spline type: */
}


void drvDXF::curvetoAsBezier(const basedrawingelement & elem, const Point & currentpoint)
{
	// single spline representation - 
	// each curveto is one SPLINE element
	// only the 4 points of the curveto are used
	//
  if (wantedLayer(currentR(), currentG(), currentB(),DXFLayers::normalizeColorName(currentColorName()))) {
	buffer << "  0\nSPLINE\n";

	writeHandle(buffer);
	buffer << "100\n" "AcDbEntity\n";
	writeLayer(currentR(), currentG(), currentB(),DXFLayers::normalizeColorName(currentColorName())); //buffer << "  8\n0\n";			/* Layer */
	buffer << "100\n" "AcDbSpline\n";

	buffer << "210\n0.0\n220\n0.0\n230\n1.0\n";	/* Norm vector */

	//if (!options->colorsToLayers) buffer << " 62\n     " << DXFColor::getDXFColor(currentR(), currentG(), currentB()) << '\n';
	writeColorAndStyle();
	writesplinetype( 0 );

	buffer << " 71\n     3\n";            /* degree of curve */
	buffer << " 72\n     8\n";            /* no of knots */
	buffer << " 73\n" << 4 << "\n";	/* no of control points */

	
	// knot values 
	buffer << " 40\n0.0\n"; 
	buffer << " 40\n0.0\n";
	buffer << " 40\n0.0\n";
	buffer << " 40\n0.0\n";

	buffer << " 40\n1.0\n"; 
	buffer << " 40\n1.0\n";
	buffer << " 40\n1.0\n";
	buffer << " 40\n1.0\n";


	//  START_POINT(first).x, START_POINT(first).y);
	/* 10 control points
	   11 fit points
	   40 knot values
	 */
	const Point & cp1 = elem.getPoint(0);
	const Point & cp2 = elem.getPoint(1);
	const Point & ep  = elem.getPoint(2);

	// control points - same as the bezier points
	printPoint(buffer,currentpoint, 10);	
	printPoint(buffer,cp1, 10);		
	printPoint(buffer,cp2, 10);	
	printPoint(buffer,ep,  10);		
  }
}

void drvDXF::curvetoAsNurb(const basedrawingelement & elem, const Point & currentpoint)
{
	// single spline representation - 
	// each curveto is one SPLINE element
	// only the 4 points of the curveto are used
	//
	// Here we use the mapping to Nurbs as described in Schneiders Nurbs tutorial
	// 
	// 
  if (wantedLayer(currentR(), currentG(), currentB(),DXFLayers::normalizeColorName(currentColorName()))) {
	buffer << "  0\nSPLINE\n";
	writeHandle(buffer);
	buffer << "100\n" "AcDbEntity\n";
	writeLayer(currentR(), currentG(), currentB(),DXFLayers::normalizeColorName(currentColorName())); //buffer << "  8\n0\n";			/* Layer */
	buffer << "100\n" "AcDbSpline\n";

	buffer << "210\n0.0\n220\n0.0\n230\n1.0\n";	/* Norm vector */

	//if (!options->colorsToLayers) buffer << " 62\n     " << DXFColor::getDXFColor(currentR(), currentG(), currentB()) << '\n';
	writeColorAndStyle();
	writesplinetype( RationalSpline );

	buffer << " 71\n     3\n";            /* degree of curve */
	buffer << " 72\n     8\n";            /* no of knots */

	buffer << " 73\n" << 4 << "\n";	/* no of control points */

	// knot values 
	buffer << " 40\n0.0\n"; 
	buffer << " 40\n0.0\n";
	buffer << " 40\n0.0\n";
	buffer << " 40\n0.0\n";

	buffer << " 40\n1.0\n"; 
	buffer << " 40\n1.0\n";
	buffer << " 40\n1.0\n";
	buffer << " 40\n1.0\n";


	//  START_POINT(first).x, START_POINT(first).y);
	/* 10 control points
	   11 fit points
	   40 knot values
	 */
	const Point & cp1 = elem.getPoint(0);
	const Point & cp2 = elem.getPoint(1);
	const Point & ep  = elem.getPoint(2);

	// control points - same as the bezier points
	printPoint(buffer,currentpoint, 10);	
	printPoint(buffer,cp1, 10);		
	printPoint(buffer,cp2, 10);	
	printPoint(buffer,ep,  10);		

  }
}

void drvDXF::curvetoAsBSpline(const basedrawingelement & elem, const Point & currentpoint)
{
	// single spline representation - 
	// each curveto is one B-SPLINE element
	// only the 4 points of the curveto are used
	//
	// The Bezier points are transformed to the corresponding B-Spline control points.
	// 
  if (wantedLayer(currentR(), currentG(), currentB(),DXFLayers::normalizeColorName(currentColorName()))) {
	buffer << "  0\nSPLINE\n";
	writeHandle(buffer);
	buffer << "100\n" "AcDbEntity\n";
	writeLayer(currentR(), currentG(), currentB(),DXFLayers::normalizeColorName(currentColorName())); //buffer << "  8\n0\n";			/* Layer */
	buffer << "100\n" "AcDbSpline\n";

	buffer << "210\n0.0\n220\n0.0\n230\n1.0\n";	/* Norm vector */

	//if (!options->colorsToLayers) buffer << " 62\n     " << DXFColor::getDXFColor(currentR(), currentG(), currentB()) << '\n';
	writeColorAndStyle();
	writesplinetype(RationalSpline);     
	buffer << " 71\n     3\n";        /* degree of curve */
#define bsplinewithknots
#ifdef bsplinewithknots
	buffer << " 72\n     8\n";	    /* no of knots */
#else
	buffer << " 72\n     0\n";	    /* no of knots */
#endif
	buffer << " 73\n" << 4 << "\n";	/*  no of control points */


#ifdef bsplinewithknots
		// knot values 
	buffer << " 40\n0.0\n"; 
	buffer << " 40\n1.0\n";
	buffer << " 40\n2.0\n";
	buffer << " 40\n3.0\n";

	buffer << " 40\n4.0\n"; 
	buffer << " 40\n5.0\n";
	buffer << " 40\n6.0\n";
	buffer << " 40\n7.0\n";

#endif



	/* 10 control points
	   11 fit points
	   40 knot values
	 */
	const Point & bp0 = currentpoint;
	const Point & bp1 = elem.getPoint(0);
	const Point & bp2 = elem.getPoint(1);
	const Point & bp3 = elem.getPoint(2);
/*
 bezier to bspline control point transformation matrix
 according to mail from Steve Noskow
6 -7 2 0
0 2 -1 0
0 -1 2 0
0 2 -7 6
*/
	//lint -save -e747
	//The conversions from int to float are OK here.
	const Point sp0 = (bp0* ( 6)) + (bp1* (-7)) + (bp2* ( 2)) ;
	const Point sp1 =               (bp1* ( 2)) + (bp2* (-1)) ;
	const Point sp2 =               (bp1* (-1)) + (bp2* ( 2)) ;
	const Point sp3 =               (bp1* ( 2)) + (bp2* (-7)) + (bp3* ( 6)) ;
	//lint -restore

		// b-spline control points 
	printPoint(buffer,sp0, 10);	
	printPoint(buffer,sp1, 10);		
	printPoint(buffer,sp2, 10);	
	printPoint(buffer,sp3, 10);	
  }
}



void drvDXF::curvetoAsOneSpline(const basedrawingelement & elem, const Point & currentpoint)
{
	// single spline representation - 
	// each curveto is one SPLINE element
	// only the 4 points of the curveto are used
	//
	// this doesn's seem to work stable enough. It is unclear what are control points in DXF and
	// what are fit points. In all viewers I tried, the control points had no effect.
	// 


	// 
  if (wantedLayer(currentR(), currentG(), currentB(),DXFLayers::normalizeColorName(currentColorName()))) {
	buffer << "  0\nSPLINE\n";
	writeHandle(buffer);
	buffer << "100\n" "AcDbEntity\n";
	writeLayer(currentR(), currentG(), currentB(),DXFLayers::normalizeColorName(currentColorName())); // << "  8\n0\n";			/* Layer */
	buffer << "100\n" "AcDbSpline\n";

	buffer << "210\n0.0\n220\n0.0\n230\n1.0\n";	/* Norm vector */


	//if (!options->colorsToLayers) buffer << " 62\n     " << DXFColor::getDXFColor(currentR(), currentG(), currentB()) << '\n';
	writeColorAndStyle();
	writesplinetype(RationalSpline);
	/* 8: planar; 1 closed; 2 periodic; 16 linear */
	buffer << " 71\n     3\n";             /* degree of curve */



#ifdef bsplinewithknots
	buffer << " 72\n    10\n";	    /* no of knots */
			// knot values 
	buffer << " 40\n0.0\n"; 
	buffer << " 40\n1.0\n";
	buffer << " 40\n2.0\n";
	buffer << " 40\n3.0\n";

	buffer << " 40\n4.0\n"; 
	buffer << " 40\n5.0\n";
	buffer << " 40\n6.0\n";
	buffer << " 40\n7.0\n";
	buffer << " 40\n8.0\n";
	buffer << " 40\n9.0\n";

#else
	buffer << " 72\n     0\n";	    /* no of knots */
#endif


// approach with the phantom nodes
		// (abcd)   ->   d1=(b-a);d2=(d-c) ; (a-d1),(a),(a+d1==b),(d-d2==c),(d),(d+d2)

	buffer << " 73\n" << 6 << "\n";	  /* no of control points */


//	buffer << " 74\n" << 2 << "\n";	  /* no of fit points */
//	buffer << " 44\n0.0000000001\n";	//, 0.0000000001);           /* fit tolerance */


	/* 10 control points
	   11 fit points
	   40 knot values
	 */
	const Point & a = currentpoint;
	const Point & b = elem.getPoint(0);
	const Point & c = elem.getPoint(1);
	const Point & d = elem.getPoint(2);

	const Point delta1= b + (a * -1.0f);
	const Point delta2= d + (c * -1.0f);


	//  printPoint(buffer,cp1,12) ; // 12 22 32 // start tanget
	//  printPoint(buffer,cp2,13) ; // 13 23 33 // end tanget
	printPoint(buffer,a + (delta1 * -1.0f), 10);		// 12 22 32
	printPoint(buffer,a, 10);		// 13 23 33
	printPoint(buffer,b, 10);		// 13 23 33

	printPoint(buffer,c, 10);		// 12 22 32
	printPoint(buffer,d, 10);		// 13 23 33
	printPoint(buffer,d + delta2 , 10);		// 13 23 33
  }
}

void drvDXF::curvetoAsMultiSpline(const basedrawingelement & elem, const Point & currentpoint)
{
  if (wantedLayer(currentR(), currentG(), currentB(),DXFLayers::normalizeColorName(currentColorName()))) {
	// multiple fit points on a single SPLINE
	const unsigned int fitpoints = options->splineprecision.value ; // 4;

	buffer << "  0\nSPLINE\n";
	writeHandle(buffer);
	buffer << "100\n" "AcDbEntity\n";
	writeLayer(currentR(), currentG(), currentB(),DXFLayers::normalizeColorName(currentColorName())); // << "  8\n0\n";			/* Layer */
	buffer << "100\n" "AcDbSpline\n";

	buffer << "210\n0.0\n220\n0.0\n230\n1.0\n";	/* Norm vector */


	//if (!options->colorsToLayers) buffer << " 62\n     " << DXFColor::getDXFColor(currentR(), currentG(), currentB()) << '\n';
	writeColorAndStyle();
	writesplinetype( 0 );
	/* 8: planar; 1 closed; 2 periodic; 16 linear */
	buffer << " 71\n     3\n";	// , 3);             /* degree of curve */
	buffer << " 72\n     0\n";	//, 0);             /* no of knots */
	buffer << " 73\n" << 0 << "\n";	// ????????? , SPLINE_LIST_LENGTH (list));  /* no of control points */
	buffer << " 74\n" << fitpoints << "\n";	// , SPLINE_LIST_LENGTH (list));  /* no of fit points */


//wogl                      buffer << " 42\n0.0000001\n"; // , 0.0000001);                /* knot tolerance */
//wogl                      buffer << " 43\n0.0000001\n"; //, 0.0000001);             /* control-point tolerance */
	buffer << " 44\n0.0000000001\n";	//, 0.0000000001);           /* fit tolerance */

	//  START_POINT(first).x, START_POINT(first).y);
	/* 10 control points
	   11 fit points
	   40 knot values
	 */
	const Point & cp1 = elem.getPoint(0);
	const Point & cp2 = elem.getPoint(1);
	const Point & ep  = elem.getPoint(2);

	for (unsigned int s = 0; s < fitpoints; s++) {
		const float t = 1.0f * s / (fitpoints - 1);
		const Point pt = PointOnBezier(t, currentpoint, cp1, cp2, ep);
		printPoint(buffer,pt, 11);		// 12 22 32   fit points
	}

  }
}

void drvDXF::curvetoAsPolyLine(const basedrawingelement & elem, const Point & currentpoint)
{
  if (wantedLayer(currentR(), currentG(), currentB(),DXFLayers::normalizeColorName(currentColorName()))) {
	// approximate spline with lines.
	const unsigned int sections = options->splineprecision.value ; // 20;

	buffer << "  0\nLWPOLYLINE\n";
	writeHandle(buffer);
	buffer << "100\n" "AcDbEntity\n";
	writeLayer(currentR(), currentG(), currentB(),DXFLayers::normalizeColorName(currentColorName())); // buffer << "  8\n0\n";
	buffer << "100\n" "AcDbPolyline\n";
	buffer << " 90\n"; // number of edges
	buffer << (sections+1) << endl;
	buffer << " 70\n"
			" 0\n";

	// color
	//if (!options->colorsToLayers) buffer << " 62\n     " << DXFColor::getDXFColor(currentR(), currentG(), currentB()) << "\n";
	writeColorAndStyle();
	// the following lines marked with nolw are not needed with the lwpolyline object.
	// vertex flag (always 1)
//nolw	buffer << " 66\n     1\n";
	// base point
	// buffer << " 10\n0.0\n 20\n0.0\n 30\n0.0\n";
//nolw	printPoint(buffer,Point(0.0f,0.0f), 10);

	const Point & cp1 = elem.getPoint(0);
	const Point & cp2 = elem.getPoint(1);
	const Point & ep = elem.getPoint(2);

	for (unsigned int s = 0; s <= sections; s++) {
		const float t = 1.0f * s / sections;
		const Point pt = PointOnBezier(t, currentpoint, cp1, cp2, ep);
		//nolw drawVertex(pt, false);
		printPoint(buffer,pt,10);
	}
//nolw	buffer << "  0\nSEQEND\n  8\n0\n";
  }
}

#if 0
const polysplines = false;		// old try using polyline and splines - but doesn't seem to work
					// Polylines with type spline seem to have as well the approximating lines.

					// draw them just as polygon through the edge and control points (I know, this is not correct)

if (0) {
	buffer << "  0\nPOLYLINE\n  8\n0\n";
	// color
	//if (!colorsToLayers) buffer << " 62\n     " << DXFColor::getDXFColor(currentR(), currentG(), currentB()) << "\n";
	writeColorAndStyle();
	// vertex flag (always 1)
	buffer << " 66\n     1\n";
	// base point
	printPoint(buffer,Point(0.0f,0.0f) 10));
	// buffer << " 10\n0.0\n 20\n0.0\n 30\n0.0\n";
	if (polysplines) {
		// start and end line width
		buffer << " 40\n" << lineWidth << "\n 41\n" << lineWidth << "\n";
		// polytype flag : always open curve
		buffer << " 70\n     4\n";
		// degree 5 means spline of degree 3
		buffer << " 75\n     5\n";
	}
	drawVertex(currentPoint, polysplines, polysplines ? 48 : 0);
	for (unsigned int cp = 0; cp < 3; cp++) {
		const Point & p = elem.getPoint(cp);
		drawVertex(p, polysplines, polysplines ? 16 : 0);
		currentPoint = p;
	}
	buffer << "  0\nSEQEND\n  8\n0\n";

#endif
void drvDXF::showHatch() {
	if (wantedLayer(currentR(), currentG(), currentB(), DXFLayers::normalizeColorName(currentColorName()))) {
		if (formatis14) {
			buffer << "  0\n" "HATCH\n";
			writeHandle(buffer);
			buffer << "100\n" "AcDbEntity\n";
			writeLayer(currentR(), currentG(), currentB(), DXFLayers::normalizeColorName(currentColorName()));
			writeColorAndStyle();
			buffer << "100\n" "AcDbHatch\n";
			// elevation "points"
			const Point hatch_dummy(0, 0);
			printPoint(buffer, hatch_dummy, 10, false);

			// TrueView wants Extrusion directions
			buffer << "210\n" "0\n";
			buffer << "220\n" "0\n";
			buffer << "230\n" "1\n"; // 1 taken from example - not sure it is correct

			buffer << "  2\n" "SOLID\n";
			buffer << " 70\n" "1\n"; //solid fill flag
			buffer << " 71\n" "0\n"; //non associative
			buffer << " 91\n" "1\n"; //number of boundary paths
			buffer << " 92\n" "0\n"; //Boundary path type flag(bit coded)
			/* 	0 = Default; 1 = External; 2 = Polyline
				4 = Derived; 8 = Textbox; 16 = Outermost */
			constexpr bool usepolyline = false;
			if (usepolyline) {
				//TODO Polyline boundary type data(only if boundary = polyline).
			} else {
				// hatch needs the path to be closed
				buffer << " 93\n" << numberOfElementsInPath() << "\n";
				// Number of edges in this boundary path(only if boundary is not a polyline)
				// loop for as many edges
				// Starting from 1. Element 0 will be used when t is at numberOfElementsInPath
				// in order to close the path as needed for HATCH
				for (unsigned int t = 1; t <= numberOfElementsInPath(); t++) {
					buffer << " 72\n" << "1\n";
					//Edge type(only if boundary is not a polyline) :
					//	1 = Line; 2 = Circular arc; 3 = Elliptic arc; 4 = Spline
					const Point& p = pathElement(t - 1).getLastPoint();
					// wrap around to first element
					const Point& q = pathElement((t == numberOfElementsInPath()) ? 0 : t).getLastPoint();
					//write start and end point for each line
					printPoint(buffer, p, 10, false);
					printPoint(buffer, q, 11, false);
				}
			}
			buffer << " 97\n" "0\n"; // Number of source boundary objects
			buffer << " 75\n" "0\n"; //hatch style
			buffer << " 76\n" "1\n"; //hatch pattern type
			buffer << " 98\n" "0\n"; //number of seed points
		} // only supported with version 14
	}
}

void drvDXF::show_path()
{
	if (options->fillToHatch && (currentShowType() != stroke)) {
		showHatch();
	}
	if (driverdesc.backendSupportsCurveto) {
		Point currentPoint(0.0f, 0.0f);
		const Point firstPoint = pathElement(0).getPoint(0);
		for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
			const basedrawingelement & elem = pathElement(n);
			switch (elem.getType()) {
			case moveto:{
					const Point & p = elem.getPoint(0);
					currentPoint = p;
				}
				break;
			case lineto:{
					const Point & p = elem.getPoint(0);
					drawLine(currentPoint, p);
					currentPoint = p;
				}
				break;
			case closepath:
				drawLine(currentPoint, firstPoint);
				break;
			case curveto:{
					switch (splinemode) {
					case aspolyline:
						curvetoAsPolyLine(elem, currentPoint);
						break;
					case assinglespline:
						curvetoAsOneSpline(elem, currentPoint);
						break;
					case asmultispline:
						curvetoAsMultiSpline(elem, currentPoint);
						break;
					case asnurb:
						curvetoAsNurb(elem, currentPoint);
						break;
					case asbspline:
						curvetoAsBSpline(elem, currentPoint);
						break;
					case asbezier:
						curvetoAsBezier(elem, currentPoint);
						break;

					default:; // nothing
					}
					currentPoint = elem.getPoint(2);
				}
				break;
			default:
				errf << "\t\tFatal: unexpected case in drvdxf " << endl;
				abort();
				break;
			}					// end switch
		}						// end for
	} else if (options->polyaslines) {
// Curveto not supported

// not used, since lines can't have a width
//             ||
		// it's just a simple line
//             ((numberOfElementsInPath() == 2)
//              && (pathElement(0).getType() == moveto)
//              && (pathElement(1).getType() == lineto))


		for (unsigned int t = 1; t < numberOfElementsInPath(); t++) {
			const Point & p = pathElement(t - 1).getPoint(0);
			const Point & q = pathElement(t).getPoint(0);
			drawLine(p, q);
		}
	} else {
	  if (wantedLayer(currentR(), currentG(), currentB(),DXFLayers::normalizeColorName(currentColorName()))) {
		// no curveto and not forced to draw LINEs - use PolyLine/VERTEX then
		buffer << "  0\nPOLYLINE\n";
		// layer
		writeLayer(currentR(), currentG(), currentB(),DXFLayers::normalizeColorName(currentColorName()));
		// color
		//if (!options->colorsToLayers) buffer << " 62\n     " << DXFColor::getDXFColor(currentR(), currentG(), currentB()) << "\n";
		writeColorAndStyle();
		// vertex flag (always 1)
		buffer << " 66\n     1\n";
		// base point
		printPoint(buffer,Point(0.0f,0.0f), 10);
		// buffer << " 10\n0.0\n 20\n0.0\n 30\n0.0\n";
		if (isPolygon() || (currentShowType() != stroke) ) {
                  // DXF output doesn't handle filling, so it seem better to close polyline in this case, isn't it ? - F.Ourgaud
			buffer << " 70\n     1\n";
		}
		// start and end line width
		{
			const float lineWidth = currentLineWidth();
			buffer << " 40\n" << lineWidth << "\n 41\n" << lineWidth << "\n";
		}
		for (unsigned int t = 0; t < numberOfElementsInPath(); t++) {
			const Point & p = pathElement(t).getPoint(0);
			drawVertex(p, true, 0);
		}
		buffer << "  0\nSEQEND\n"" 8\n0\n";
	  }
	}
}




static DriverDescriptionT < drvDXF > D_dxf("dxf", "CAD exchange format version 9 - only limited features. Consider using dxf_14 instead.","", "dxf", false,	// if backend supports subpaths, else 0
										   // if subpaths are supported, the backend must deal with
										   // sequences of the following form
										   // moveto (start of subpath)
										   // lineto (a line segment)
										   // lineto 
										   // moveto (start of a new subpath)
										   // lineto (a line segment)
										   // lineto 
										   //
										   // If this argument is set to 0 each subpath is drawn 
										   // individually which might not necessarily represent
										   // the original drawing.
										   false,	// if backend supports curves, else 0
										   false,	// if backend supports elements with fill and edges
										   true,	// if backend supports text, else 0
										   DriverDescription::imageformat::noimage,	// no support for PNG file images
										   DriverDescription::opentype::normalopen, false,	// if format supports multiple pages in one file
										   false /*clipping */ 
										   );

static DriverDescriptionT < drvDXF > D_dxf_14("dxf_14", "CAD exchange format version 14 supporting splines and linetypes","", "dxf", false,	// if backend supports subpaths, else 0
											 // if subpaths are supported, the backend must deal with
											 // sequences of the following form
											 // moveto (start of subpath)
											 // lineto (a line segment)
											 // lineto 
											 // moveto (start of a new subpath)
											 // lineto (a line segment)
											 // lineto 
											 //
											 // If this argument is set to 0 each subpath is drawn 
											 // individually which might not necessarily represent
											 // the original drawing.
											 true,	// if backend supports curves, else 0
											 false,	// if backend supports elements with fill and edges
											 true,	// if backend supports text, else 0
											 DriverDescription::imageformat::noimage,	// no support for PNG file images
											 DriverDescription::opentype::normalopen, false,	// if format supports multiple pages in one file
											 false /*clipping */ 
											 );
// same as above.
static DriverDescriptionT < drvDXF > D_dxf_s("dxf_s", "CAD exchange format version 14 supporting splines and linetypes", "", "dxf", false,	
	true,	// if backend supports curves, else 0
	false,	// if backend supports elements with fill and edges
	true,	// if backend supports text, else 0
	DriverDescription::imageformat::noimage,	// no support for PNG file images
	DriverDescription::opentype::normalopen, false,	// if format supports multiple pages in one file
	false /*clipping */
);
