/*
   drvIDRAW.cpp : This file is part of pstoedit
   Backend for idraw files
   Contributed by: Scott Pakin <scott+ps2ed_AT_pakin.org>
   Image Support added by Scott Johnston

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
#include "drvidraw.h"
#include I_fstream
#include I_stdio
#include I_stdlib
#include <math.h>
#include <algorithm>


// Add a point unless it's the same as the previous point
// Used by drvIDRAW::print_coords() for convenience
#define ADDPOINT(X,Y) do {						\
  if (!newtotalpoints ||						\
      iscale(X) != iscale(newpointlist[newtotalpoints-1]->x()) ||	\
      iscale(Y) != iscale(newpointlist[newtotalpoints-1]->y())){		\
    newpointlist[newtotalpoints++] = new Point(X,Y);			\
    assert(newpointlist[newtotalpoints-1] != nullptr); }		        \
} while (0)


drvIDRAW::derivedConstructor(drvIDRAW):
//(const char * driveroptions_p,ostream & theoutStream,ostream & theerrStream): // Constructor
constructBase,  buffer(tempFile.asOutput()) //, imgcount(1)
{
	IDRAW_SCALING = 0.799705f ;
	// Initialize the color table
	color[0].red = 0;
	color[0].green = 0;
	color[0].blue = 0;
	color[0].name = "Black";

	color[1].red = 0.647059;
	color[1].green = 0.164706;
	color[1].blue = 0.164706;
	color[1].name = "Brown";

	color[2].red = 1;
	color[2].green = 0;
	color[2].blue = 0;
	color[2].name = "Red";

	color[3].red = 1;
	color[3].green = 0.647059;
	color[3].blue = 0;
	color[3].name = "Orange";

	color[4].red = 1;
	color[4].green = 1;
	color[4].blue = 0;
	color[4].name = "Yellow";

	color[5].red = 0;
	color[5].green = 1;
	color[5].blue = 0;
	color[5].name = "Green";

	color[6].red = 0;
	color[6].green = 0;
	color[6].blue = 1;
	color[6].name = "Blue";

	color[7].red = 0.74902;
	color[7].green = 0;
	color[7].blue = 1;
	color[7].name = "Indigo";

	color[8].red = 0.933333;
	color[8].green = 0.509804;
	color[8].blue = 0.933333;
	color[8].name = "Violet";

	color[9].red = 1;
	color[9].green = 1;
	color[9].blue = 1;
	color[9].name = "White";

	color[10].red = 0.764706;
	color[10].green = 0.764706;
	color[10].blue = 0.764706;
	color[10].name = "LtGray";

	color[11].red = 0.501961;
	color[11].green = 0.501961;
	color[11].blue = 0.501961;
	color[11].name = "DkGray";


	// Write idraw PostScript header
	outf << "%!PS-Adobe-2.0 EPSF-1.2\n"
	     << "%%Creator: idraw\n"
	     << "%%DocumentFonts: Times-Roman Times-Bold Times-Italic\n"
	     << "%%+ Helvetica Helvetica-Bold Helvetica-Oblique Helvetica-BoldOblique\n"
	     << "%%+ Courier Courier-Bold Courier-Oblique Courier-BoldOblique\n"
	     << "%%+ Symbol\n"
	     << "%%Pages: 1\n"
	     << "%%BoundingBox: 0 0 612 792\n"  // Assume full page
	     << "%%EndComments\n"
	     << "\n"
	     << "%%BeginIdrawPrologue\n"
	     << "/arrowhead {\n"
	     << "0 begin\n"
	     << "transform originalCTM itransform\n"
	     << "/taily exch def\n"
	     << "/tailx exch def\n"
	     << "transform originalCTM itransform\n"
	     << "/tipy exch def\n"
	     << "/tipx exch def\n"
	     << "/dy tipy taily sub def\n"
	     << "/dx tipx tailx sub def\n"
	     << "/angle dx 0 ne dy 0 ne or { dy dx atan } { 90 } ifelse def\n"
	     << "gsave\n"
	     << "originalCTM setmatrix\n"
	     << "tipx tipy translate\n"
	     << "angle rotate\n"
	     << "newpath\n"
	     << "arrowHeight neg arrowWidth 2 div moveto\n"
	     << "0 0 lineto\n"
	     << "arrowHeight neg arrowWidth 2 div neg lineto\n"
	     << "patternNone not {\n"
	     << "originalCTM setmatrix\n"
	     << "/padtip arrowHeight 2 exp 0.25 arrowWidth 2 exp mul add sqrt brushWidth mul\n"
	     << "arrowWidth div def\n"
	     << "/padtail brushWidth 2 div def\n"
	     << "tipx tipy translate\n"
	     << "angle rotate\n"
	     << "padtip 0 translate\n"
	     << "arrowHeight padtip add padtail add arrowHeight div dup scale\n"
	     << "arrowheadpath\n"
	     << "ifill\n"
	     << "} if\n"
	     << "brushNone not {\n"
	     << "originalCTM setmatrix\n"
	     << "tipx tipy translate\n"
	     << "angle rotate\n"
	     << "arrowheadpath\n"
	     << "istroke\n"
	     << "} if\n"
	     << "grestore\n"
	     << "end\n"
	     << "} dup 0 9 dict put def\n"
	     << "\n"
	     << "/arrowheadpath {\n"
	     << "newpath\n"
	     << "arrowHeight neg arrowWidth 2 div moveto\n"
	     << "0 0 lineto\n"
	     << "arrowHeight neg arrowWidth 2 div neg lineto\n"
	     << "} def\n"
	     << "\n"
	     << "/leftarrow {\n"
	     << "0 begin\n"
	     << "y exch get /taily exch def\n"
	     << "x exch get /tailx exch def\n"
	     << "y exch get /tipy exch def\n"
	     << "x exch get /tipx exch def\n"
	     << "brushLeftArrow { tipx tipy tailx taily arrowhead } if\n"
	     << "end\n"
	     << "} dup 0 4 dict put def\n"
	     << "\n"
	     << "/rightarrow {\n"
	     << "0 begin\n"
	     << "y exch get /tipy exch def\n"
	     << "x exch get /tipx exch def\n"
	     << "y exch get /taily exch def\n"
	     << "x exch get /tailx exch def\n"
	     << "brushRightArrow { tipx tipy tailx taily arrowhead } if\n"
	     << "end\n"
	     << "} dup 0 4 dict put def\n"
	     << "\n"
	     << "%%EndIdrawPrologue\n"
	     << "\n"
	     << "/arrowHeight 10 def\n"
	     << "/arrowWidth 5 def\n"
	     << "\n"
	     << "/IdrawDict 100 dict def\n"
	     << "IdrawDict begin\n"
	     << "\n"
	     << "/reencodeISO {\n"
	     << "dup dup findfont dup length dict begin\n"
	     << "{ 1 index /FID ne { def }{ pop pop } ifelse } forall\n"
	     << "/Encoding ISOLatin1Encoding def\n"
	     << "currentdict end definefont\n"
	     << "} def\n"
	     << "\n"
	     << "/ISOLatin1Encoding [\n"
	     << "/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n"
	     << "/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n"
	     << "/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n"
	     << "/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n"
	     << "/space/exclam/quotedbl/numbersign/dollar/percent/ampersand/quoteright\n"
	     << "/parenleft/parenright/asterisk/plus/comma/minus/period/slash\n"
	     << "/zero/one/two/three/four/five/six/seven/eight/nine/colon/semicolon\n"
	     << "/less/equal/greater/question/at/A/B/C/D/E/F/G/H/I/J/K/L/M/N\n"
	     << "/O/P/Q/R/S/T/U/V/W/X/Y/Z/bracketleft/backslash/bracketright\n"
	     << "/asciicircum/underscore/quoteleft/a/b/c/d/e/f/g/h/i/j/k/l/m\n"
	     << "/n/o/p/q/r/s/t/u/v/w/x/y/z/braceleft/bar/braceright/asciitilde\n"
	     << "/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n"
	     << "/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef\n"
	     << "/.notdef/dotlessi/grave/acute/circumflex/tilde/macron/breve\n"
	     << "/dotaccent/dieresis/.notdef/ring/cedilla/.notdef/hungarumlaut\n"
	     << "/ogonek/caron/space/exclamdown/cent/sterling/currency/yen/brokenbar\n"
	     << "/section/dieresis/copyright/ordfeminine/guillemotleft/logicalnot\n"
	     << "/hyphen/registered/macron/degree/plusminus/twosuperior/threesuperior\n"
	     << "/acute/mu/paragraph/periodcentered/cedilla/onesuperior/ordmasculine\n"
	     << "/guillemotright/onequarter/onehalf/threequarters/questiondown\n"
	     << "/Agrave/Aacute/Acircumflex/Atilde/Adieresis/Aring/AE/Ccedilla\n"
	     << "/Egrave/Eacute/Ecircumflex/Edieresis/Igrave/Iacute/Icircumflex\n"
	     << "/Idieresis/Eth/Ntilde/Ograve/Oacute/Ocircumflex/Otilde/Odieresis\n"
	     << "/multiply/Oslash/Ugrave/Uacute/Ucircumflex/Udieresis/Yacute\n"
	     << "/Thorn/germandbls/agrave/aacute/acircumflex/atilde/adieresis\n"
	     << "/aring/ae/ccedilla/egrave/eacute/ecircumflex/edieresis/igrave\n"
	     << "/iacute/icircumflex/idieresis/eth/ntilde/ograve/oacute/ocircumflex\n"
	     << "/otilde/odieresis/divide/oslash/ugrave/uacute/ucircumflex/udieresis\n"
	     << "/yacute/thorn/ydieresis\n"
	     << "] def\n"
	     << "/Times-Roman reencodeISO def\n"
	     << "/Times-Bold reencodeISO def\n"
	     << "/Times-Italic reencodeISO def\n"
	     << "/Helvetica reencodeISO def\n"
	     << "/Helvetica-Bold reencodeISO def\n"
	     << "/Helvetica-Oblique reencodeISO def\n"
	     << "/Helvetica-BoldOblique reencodeISO def\n"
	     << "/Courier reencodeISO def\n"
	     << "/Courier-Bold reencodeISO def\n"
	     << "/Courier-Oblique reencodeISO def\n"
	     << "/Courier-BoldOblique reencodeISO def\n"
	     << "/Symbol reencodeISO def\n"
	     << "\n"
	     << "/none null def\n"
	     << "/numGraphicParameters 17 def\n"
	     << "/stringLimit 65535 def\n"
	     << "\n"
	     << "/Begin {\n"
	     << "save\n"
	     << "numGraphicParameters dict begin\n"
	     << "} def\n"
	     << "\n"
	     << "/End {\n"
	     << "end\n"
	     << "restore\n"
	     << "} def\n"
	     << "\n"
	     << "/SetB {\n"
	     << "dup type /nulltype eq {\n"
	     << "pop\n"
	     << "false /brushRightArrow idef\n"
	     << "false /brushLeftArrow idef\n"
	     << "true /brushNone idef\n"
	     << "} {\n"
	     << "/brushDashOffset idef\n"
	     << "/brushDashArray idef\n"
	     << "0 ne /brushRightArrow idef\n"
	     << "0 ne /brushLeftArrow idef\n"
	     << "/brushWidth idef\n"
	     << "false /brushNone idef\n"
	     << "} ifelse\n"
	     << "} def\n"
	     << "\n"
	     << "/SetCFg {\n"
	     << "/fgblue idef\n"
	     << "/fggreen idef\n"
	     << "/fgred idef\n"
	     << "} def\n"
	     << "\n"
	     << "/SetCBg {\n"
	     << "/bgblue idef\n"
	     << "/bggreen idef\n"
	     << "/bgred idef\n"
	     << "} def\n"
	     << "\n"
	     << "/SetF {\n"
	     << "/printSize idef\n"
	     << "/printFont idef\n"
	     << "} def\n"
	     << "\n"
	     << "/SetP {\n"
	     << "dup type /nulltype eq {\n"
	     << "pop true /patternNone idef\n"
	     << "} {\n"
	     << "dup -1 eq {\n"
	     << "/patternGrayLevel idef\n"
	     << "/patternString idef\n"
	     << "} {\n"
	     << "/patternGrayLevel idef\n"
	     << "} ifelse\n"
	     << "false /patternNone idef\n"
	     << "} ifelse\n"
	     << "} def\n"
	     << "\n"
	     << "/BSpl {\n"
	     << "0 begin\n"
	     << "storexyn\n"
	     << "newpath\n"
	     << "n 1 gt {\n"
	     << "0 0 0 0 0 0 1 1 true subspline\n"
	     << "n 2 gt {\n"
	     << "0 0 0 0 1 1 2 2 false subspline\n"
	     << "1 1 n 3 sub {\n"
	     << "/i exch def\n"
	     << "i 1 sub dup i dup i 1 add dup i 2 add dup false subspline\n"
	     << "} for\n"
	     << "n 3 sub dup n 2 sub dup n 1 sub dup 2 copy false subspline\n"
	     << "} if\n"
	     << "n 2 sub dup n 1 sub dup 2 copy 2 copy false subspline\n"
	     << "patternNone not brushLeftArrow not brushRightArrow not and and { ifill } if\n"
	     << "brushNone not { istroke } if\n"
	     << "0 0 1 1 leftarrow\n"
	     << "n 2 sub dup n 1 sub dup rightarrow\n"
	     << "} if\n"
	     << "end\n"
	     << "} dup 0 4 dict put def\n"
	     << "\n"
	     << "/Circ {\n"
	     << "newpath\n"
	     << "0 360 arc\n"
	     << "closepath\n"
	     << "patternNone not { ifill } if\n"
	     << "brushNone not { istroke } if\n"
	     << "} def\n"
	     << "\n"
	     << "/CBSpl {\n"
	     << "0 begin\n"
	     << "dup 2 gt {\n"
	     << "storexyn\n"
	     << "newpath\n"
	     << "n 1 sub dup 0 0 1 1 2 2 true subspline\n"
	     << "1 1 n 3 sub {\n"
	     << "/i exch def\n"
	     << "i 1 sub dup i dup i 1 add dup i 2 add dup false subspline\n"
	     << "} for\n"
	     << "n 3 sub dup n 2 sub dup n 1 sub dup 0 0 false subspline\n"
	     << "n 2 sub dup n 1 sub dup 0 0 1 1 false subspline\n"
	     << "patternNone not { ifill } if\n"
	     << "brushNone not { istroke } if\n"
	     << "} {\n"
	     << "Poly\n"
	     << "} ifelse\n"
	     << "end\n"
	     << "} dup 0 4 dict put def\n"
	     << "\n"
	     << "/Elli {\n"
	     << "0 begin\n"
	     << "newpath\n"
	     << "4 2 roll\n"
	     << "translate\n"
	     << "scale\n"
	     << "0 0 1 0 360 arc\n"
	     << "closepath\n"
	     << "patternNone not { ifill } if\n"
	     << "brushNone not { istroke } if\n"
	     << "end\n"
	     << "} dup 0 1 dict put def\n"
	     << "\n"
	     << "/Line {\n"
	     << "0 begin\n"
	     << "2 storexyn\n"
	     << "newpath\n"
	     << "x 0 get y 0 get moveto\n"
	     << "x 1 get y 1 get lineto\n"
	     << "brushNone not { istroke } if\n"
	     << "0 0 1 1 leftarrow\n"
	     << "0 0 1 1 rightarrow\n"
	     << "end\n"
	     << "} dup 0 4 dict put def\n"
	     << "\n"
	     << "/MLine {\n"
	     << "0 begin\n"
	     << "storexyn\n"
	     << "newpath\n"
	     << "n 1 gt {\n"
	     << "x 0 get y 0 get moveto\n"
	     << "1 1 n 1 sub {\n"
	     << "/i exch def\n"
	     << "x i get y i get lineto\n"
	     << "} for\n"
	     << "patternNone not brushLeftArrow not brushRightArrow not and and { ifill } if\n"
	     << "brushNone not { istroke } if\n"
	     << "0 0 1 1 leftarrow\n"
	     << "n 2 sub dup n 1 sub dup rightarrow\n"
	     << "} if\n"
	     << "end\n"
	     << "} dup 0 4 dict put def\n"
	     << "\n"
	     << "/Poly {\n"
	     << "3 1 roll\n"
	     << "newpath\n"
	     << "moveto\n"
	     << "-1 add\n"
	     << "{ lineto } repeat\n"
	     << "closepath\n"
	     << "patternNone not { ifill } if\n"
	     << "brushNone not { istroke } if\n"
	     << "} def\n"
	     << "\n"
	     << "/Rect {\n"
	     << "0 begin\n"
	     << "/t exch def\n"
	     << "/r exch def\n"
	     << "/b exch def\n"
	     << "/l exch def\n"
	     << "newpath\n"
	     << "l b moveto\n"
	     << "l t lineto\n"
	     << "r t lineto\n"
	     << "r b lineto\n"
	     << "closepath\n"
	     << "patternNone not { ifill } if\n"
	     << "brushNone not { istroke } if\n"
	     << "end\n"
	     << "} dup 0 4 dict put def\n"
	     << "\n"
	     << "/Text {\n"
	     << "ishow\n"
	     << "} def\n"
	     << "\n"
	     << "/idef {\n"
	     << "dup where { pop pop pop } { exch def } ifelse\n"
	     << "} def\n"
	     << "\n"
	     << "/ifill {\n"
	     << "0 begin\n"
	     << "gsave\n"
	     << "patternGrayLevel -1 ne {\n"
	     << "fgred bgred fgred sub patternGrayLevel mul add\n"
	     << "fggreen bggreen fggreen sub patternGrayLevel mul add\n"
	     << "fgblue bgblue fgblue sub patternGrayLevel mul add setrgbcolor\n"
	     << "eofill\n"
	     << "} {\n"
	     << "eoclip\n"
	     << "originalCTM setmatrix\n"
	     << "pathbbox /t exch def /r exch def /b exch def /l exch def\n"
	     << "/w r l sub ceiling cvi def\n"
	     << "/h t b sub ceiling cvi def\n"
	     << "/imageByteWidth w 8 div ceiling cvi def\n"
	     << "/imageHeight h def\n"
	     << "bgred bggreen bgblue setrgbcolor\n"
	     << "eofill\n"
	     << "fgred fggreen fgblue setrgbcolor\n"
	     << "w 0 gt h 0 gt and {\n"
	     << "l w add b translate w neg h scale\n"
	     << "w h true [w 0 0 h neg 0 h] { patternproc } imagemask\n"
	     << "} if\n"
	     << "} ifelse\n"
	     << "grestore\n"
	     << "end\n"
	     << "} dup 0 8 dict put def\n"
	     << "\n"
	     << "/istroke {\n"
	     << "gsave\n"
	     << "brushDashOffset -1 eq {\n"
	     << "[] 0 setdash\n"
	     << "1 setgray\n"
	     << "} {\n"
	     << "brushDashArray brushDashOffset setdash\n"
	     << "fgred fggreen fgblue setrgbcolor\n"
	     << "} ifelse\n"
	     << "brushWidth setlinewidth\n"
	     << "originalCTM setmatrix\n"
	     << "stroke\n"
	     << "grestore\n"
	     << "} def\n"
	     << "\n"
	     << "/ishow {\n"
	     << "0 begin\n"
	     << "gsave\n"
	     << "fgred fggreen fgblue setrgbcolor\n"
	     << "/fontDict printFont printSize scalefont dup setfont def\n"
	     << "/descender fontDict begin 0 /FontBBox load 1 get FontMatrix end\n"
	     << "transform exch pop def\n"
	     << "/vertoffset 1 printSize sub descender sub def {\n"
	     << "0 vertoffset moveto show\n"
	     << "/vertoffset vertoffset printSize sub def\n"
	     << "} forall\n"
	     << "grestore\n"
	     << "end\n"
	     << "} dup 0 3 dict put def\n"
	     << "/patternproc {\n"
	     << "0 begin\n"
	     << "/patternByteLength patternString length def\n"
	     << "/patternHeight patternByteLength 8 mul sqrt cvi def\n"
	     << "/patternWidth patternHeight def\n"
	     << "/patternByteWidth patternWidth 8 idiv def\n"
	     << "/imageByteMaxLength imageByteWidth imageHeight mul\n"
	     << "stringLimit patternByteWidth sub min def\n"
	     << "/imageMaxHeight imageByteMaxLength imageByteWidth idiv patternHeight idiv\n"
	     << "patternHeight mul patternHeight max def\n"
	     << "/imageHeight imageHeight imageMaxHeight sub store\n"
	     << "/imageString imageByteWidth imageMaxHeight mul patternByteWidth add string def\n"
	     << "0 1 imageMaxHeight 1 sub {\n"
	     << "/y exch def\n"
	     << "/patternRow y patternByteWidth mul patternByteLength mod def\n"
	     << "/patternRowString patternString patternRow patternByteWidth getinterval def\n"
	     << "/imageRow y imageByteWidth mul def\n"
	     << "0 patternByteWidth imageByteWidth 1 sub {\n"
	     << "/x exch def\n"
	     << "imageString imageRow x add patternRowString putinterval\n"
	     << "} for\n"
	     << "} for\n"
	     << "imageString\n"
	     << "end\n"
	     << "} dup 0 12 dict put def\n"
	     << "\n"
	     << "/min {\n"
	     << "dup 3 2 roll dup 4 3 roll lt { exch } if pop\n"
	     << "} def\n"
	     << "\n"
	     << "/max {\n"
	     << "dup 3 2 roll dup 4 3 roll gt { exch } if pop\n"
	     << "} def\n"
	     << "\n"
	     << "/midpoint {\n"
	     << "0 begin\n"
	     << "/y1 exch def\n"
	     << "/x1 exch def\n"
	     << "/y0 exch def\n"
	     << "/x0 exch def\n"
	     << "x0 x1 add 2 div\n"
	     << "y0 y1 add 2 div\n"
	     << "end\n"
	     << "} dup 0 4 dict put def\n"
	     << "\n"
	     << "/thirdpoint {\n"
	     << "0 begin\n"
	     << "/y1 exch def\n"
	     << "/x1 exch def\n"
	     << "/y0 exch def\n"
	     << "/x0 exch def\n"
	     << "x0 2 mul x1 add 3 div\n"
	     << "y0 2 mul y1 add 3 div\n"
	     << "end\n"
	     << "} dup 0 4 dict put def\n"
	     << "\n"
	     << "/subspline {\n"
	     << "0 begin\n"
	     << "/movetoNeeded exch def\n"
	     << "y exch get /y3 exch def\n"
	     << "x exch get /x3 exch def\n"
	     << "y exch get /y2 exch def\n"
	     << "x exch get /x2 exch def\n"
	     << "y exch get /y1 exch def\n"
	     << "x exch get /x1 exch def\n"
	     << "y exch get /y0 exch def\n"
	     << "x exch get /x0 exch def\n"
	     << "x1 y1 x2 y2 thirdpoint\n"
	     << "/p1y exch def\n"
	     << "/p1x exch def\n"
	     << "x2 y2 x1 y1 thirdpoint\n"
	     << "/p2y exch def\n"
	     << "/p2x exch def\n"
	     << "x1 y1 x0 y0 thirdpoint\n"
	     << "p1x p1y midpoint\n"
	     << "/p0y exch def\n"
	     << "/p0x exch def\n"
	     << "x2 y2 x3 y3 thirdpoint\n"
	     << "p2x p2y midpoint\n"
	     << "/p3y exch def\n"
	     << "/p3x exch def\n"
	     << "movetoNeeded { p0x p0y moveto } if\n"
	     << "p1x p1y p2x p2y p3x p3y curveto\n"
	     << "end\n"
	     << "} dup 0 17 dict put def\n"
	     << "\n"
	     << "/storexyn {\n"
	     << "/n exch def\n"
	     << "/y n array def\n"
	     << "/x n array def\n"
	     << "n 1 sub -1 0 {\n"
	     << "/i exch def\n"
	     << "y i 3 2 roll put\n"
	     << "x i 3 2 roll put\n"
	     << "} for\n"
	     << "} def\n"
	     << "\n"
	     << "/SSten {\n"
	     << "fgred fggreen fgblue setrgbcolor\n"
	     << "dup true exch 1 0 0 -1 0 6 -1 roll matrix astore\n"
	     << "} def\n"
	     << "\n"
	     << "/FSten {\n"
	     << "dup 3 -1 roll dup 4 1 roll exch\n"
	     << "newpath\n"
	     << "0 0 moveto\n"
	     << "dup 0 exch lineto\n"
	     << "exch dup 3 1 roll exch lineto\n"
	     << "0 lineto\n"
	     << "closepath\n"
	     << "bgred bggreen bgblue setrgbcolor\n"
	     << "eofill\n"
	     << "SSten\n"
	     << "} def\n"
	     << "\n"
	     << "/Rast {\n"
	     << "exch dup 3 1 roll 1 0 0 -1 0 6 -1 roll matrix astore\n"
	     << "} def\n"
	     << "\n"
	     << "%%EndProlog\n"
	     << "\n"
	     << "%I Idraw 12 Grid 8 8 \n"
	     << "\n"
	     << "%%Page: 1 1\n"
	     << "\n"
	     << "Begin\n"
	     << "%I b u\n"
	     << "%I cfg u\n"
	     << "%I cbg u\n"
	     << "%I f u\n"
	     << "%I p u\n"
	     << "%I t\n"
	     << "[ 0.834861 0 0 0.834861 0 0 ] concat\n"
	     << "/originalCTM matrix currentmatrix def\n"
	     << "\n";
}

drvIDRAW::~drvIDRAW()
{
	// Write idraw PostScript trailer
	outf << "End %I eop\n\n";
	outf << "showpage\n\n";
	outf << "%%Trailer\n\n";
	outf << "end\n";
	options=nullptr;
}

// Return the name of closest matching color
const char *drvIDRAW::rgb2name(float red, float green, float blue) const
{
	const char *name = nullptr;
	double bestquality = 1e+100;	// Arbitrary large number

	// Linear search for a match
	for (int i = 0; i < IDRAW_NUMCOLORS; i++) {
		const double quality = (red - color[i].red) * (red - color[i].red) +
			(green - color[i].green) * (green - color[i].green) +
			(blue - color[i].blue) * (blue - color[i].blue);
		if (quality < bestquality) {
			bestquality = quality;
			name = color[i].name;
		}
	}
	return name;
}

// Output a standard idraw object header
void drvIDRAW::print_header(const char *objtype)
{

	// Object type
	outf << "Begin %I " << objtype << endl;

	// Special case for text
	if (!strcmp(objtype, "Text")) {
		// Foreground color
		outf << "%I cfg " << rgb2name(currentR(), currentG(), currentB()) << endl;
		outf << currentR() << ' ' << currentG() << ' ' << currentB() << " SetCFg\n";
		return;
	}
	// Dash pattern
	outf << "%I b ";
	double dash[4];
	const int dashpieces = sscanf_s(dashPattern(), "[ %lf %lf %lf %lf",
							&dash[0], &dash[1], &dash[2], &dash[3]);
	if (dashpieces) {
		unsigned int dashbits = 0;  // was unsigned short initially
		for (int i = 0; i < 4; i++) {
			const unsigned int numbits = std::min((unsigned int)sizeof(dashbits)*8,iscale((float)dash[i % dashpieces]));
			for (unsigned int j = 0; j < numbits; j++) {
				dashbits = dashbits << 1 | (~i & 1);
			}
		}
		outf << dashbits << endl;
		outf << iscale(currentLineWidth()) << " 0 0 [";
		for (int i = 0; i < dashpieces - 1; i++)
			outf << iscale((float)dash[i]) << ' ';
		outf << iscale((float)dash[dashpieces - 1]) << "] ";
	} else {
		outf << 65535 << endl;
		outf << iscale(currentLineWidth()) << " 0 0 [] ";
	}
	outf << "0 SetB" << endl;	// Hard-code an offset of 0 into the pattern

	// Foreground color
	outf << "%I cfg " << rgb2name(currentR(), currentG(), currentB()) << endl;
	outf << currentR() << ' ' << currentG() << ' ' << currentB() << " SetCFg\n";

	// Background color
	outf << "%I cbg " << rgb2name(currentR(), currentG(), currentB()) << endl;
	outf << currentR() << ' ' << currentG() << ' ' << currentB() << " SetCBg\n";

	// Pattern
	if (currentShowType() == drvbase::stroke)
		outf << "none SetP %I p n" << endl;
	else {
		outf << "%I p" << endl;
		outf << "0 SetP" << endl;
	}

	// Transposition matrix
	outf << "%I t" << endl;
	outf << "[ 1 -0 -0 1 0 0 ] concat" << endl;	// No translation/rotation
}

void drvIDRAW::print_coords()
{
	const unsigned int pathelts = numberOfElementsInPath();
	bool closed;				// True if shape is closed
	bool curved;				// True if shape is curved
	const Point *firstpoint;	// First and last points in shape
	const Point *lastpoint;
	unsigned int totalpoints;	// Total number of points in shape
	const Point dummypoint(-123.456f, -789.101112f);	// Used to help eliminate duplicates

	unsigned int i, j;

	// First, try to figure out what type of shape we have
	closed = false;
	curved = false;
	for (i = 0; i < pathelts; i++) {
		if (pathElement(i).getType() == curveto)
			curved = true;
		else if (pathElement(i).getType() == closepath)
			closed = true;
	}
	auto pointlist = new const Point *[pathelts * 3];	// List of points
	 	// Allocate a conservative amount
	assert(pointlist != nullptr);
	firstpoint = nullptr;
	lastpoint = &dummypoint;
	totalpoints = 0;
	for (i = 0; i < pathelts; i++) {
		const basedrawingelement & pelt = pathElement(i);

		if ((pelt.getType() == moveto || pelt.getType() == lineto) &&
			!(pelt.getPoint(0) == *lastpoint))
			lastpoint = pointlist[totalpoints++] = &pelt.getPoint(0);
		else if (pelt.getType() == curveto)
			for (j = 0; j < 3; j++)
				lastpoint = pointlist[totalpoints++] = &pelt.getPoint(j);
	}
	if (totalpoints) {
		firstpoint = pointlist[0];
		if (firstpoint->x() == lastpoint->x() && firstpoint->y() == lastpoint->y())
			closed = true;

		// Find points on the curve for curved lines
		if (curved) {
			const unsigned int pt_per_cp = 5;	// PostScript points per control point
			const unsigned int min_innerpoints = 2;	// Minimum # of points to add
			
			unsigned int newtotalpoints = 0;	// Number of points in curve

			// ASSUMPTION: Curve is moveto+curveto+curveto+curveto+...
			// List of points on curve
			auto newpointlist = new const Point *[pathelts * 3000 / pt_per_cp];	// Allocate a conservative amount
			assert(newpointlist != nullptr);
			for (i = 0; i < totalpoints - 3; i += 3) {
				const float x0 = pointlist[i]->x();
				const float y0 = pointlist[i]->y();
				const float x1 = pointlist[i + 1]->x();
				const float y1 = pointlist[i + 1]->y();
				const float x2 = pointlist[i + 2]->x();
				const float y2 = pointlist[i + 2]->y();
				const float x3 = pointlist[i + 3]->x();
				const float y3 = pointlist[i + 3]->y();
				const float cx = (x1 - x0) * 3;
				const float cy = (y1 - y0) * 3;
				const float bx = (x2 - x1) * 3 - cx;
				const float by = (y2 - y1) * 3 - cy;
				const float ax = x3 - x0 - cx - bx;
				const float ay = y3 - y0 - cy - by;
				

				// Longer lines get more control points
				// Number of points to add
				unsigned int innerpoints =(unsigned int) (
							pythagoras((y1 - y0),(x1 - x0) ) + 
							pythagoras((y2 - y1),(x2 - x1) ) + 
							pythagoras((y3 - y2),(x3 - x2) ) ) / pt_per_cp;
				if (innerpoints < min_innerpoints)
					innerpoints = min_innerpoints;

				// Add points to the list
				ADDPOINT(x0, y0);
				for (j = 1; j <= innerpoints; j++) {
					const float t = (float) j / (float) innerpoints;
					const float newx = (((ax * t) + bx) * t + cx) * t + x0;
					const float newy = (((ay * t) + by) * t + cy) * t + y0;
					ADDPOINT(newx, newy);
				}
				ADDPOINT(x3, y3);
			}

			delete[]pointlist;
			pointlist = newpointlist;
			totalpoints = newtotalpoints;
		}
		// Straight lines, not closed
		if (!closed && !curved) {
			if (totalpoints == 2) {	// Special case for single line
				print_header("Line");
				outf << "%I" << endl;
				outf << iscale(firstpoint->x()) << ' ' << iscale(firstpoint->y()) << ' ';
				outf << iscale(lastpoint->x()) << ' ' << iscale(lastpoint->y()) << ' ';
				outf << "Line" << endl;
				outf << "%I 1" << endl;
				outf << "End" << endl << endl;
			} else {			// Otherwise, output a multiline
				print_header("MLine");	// (Should have a special case for Rect)
				outf << "%I " << totalpoints << endl;
				for (i = 0; i < totalpoints; i++) {
					outf << iscale(pointlist[i]->x()) << ' ';
					outf << iscale(pointlist[i]->y()) << endl;
				}
				outf << totalpoints << " MLine" << endl;
				outf << "%I 1" << endl;
				outf << "End" << endl << endl;
			}
		}
		// Straight lines, closed */
		if (closed && !curved) {
			unsigned int numpoints;

			numpoints = totalpoints == 1 ? 1 : totalpoints - 1;
			print_header("Poly");	// Output a polygon
			outf << "%I " << numpoints << endl;
			for (i = 0; i < numpoints; i++) {
				outf << iscale(pointlist[i]->x()) << ' ';
				outf << iscale(pointlist[i]->y()) << endl;
			}
			outf << numpoints << " Poly" << endl;
			outf << "End" << endl << endl;
		}
		// Curved lines, not closed
		if (!closed && curved) {
			print_header("BSpl");	// Output a B-spline
			outf << "%I " << totalpoints << endl;
			for (i = 0; i < totalpoints; i++) {
				outf << iscale(pointlist[i]->x()) << ' ';
				outf << iscale(pointlist[i]->y()) << endl;
			}
			outf << totalpoints << " BSpl" << endl;
			outf << "%I 1" << endl;
			outf << "End" << endl << endl;
		}
		// Curved lines, closed
		if (closed && curved) {
			unsigned int numpoints;

			numpoints = totalpoints == 1 ? 1 : totalpoints - 1;
			print_header("CBSpl");	// Output a closed B-spline
			outf << "%I " << numpoints << endl;
			for (i = 0; i < numpoints; i++) {
				outf << iscale(pointlist[i]->x()) << ' ';
				outf << iscale(pointlist[i]->y()) << endl;
			}
			outf << numpoints << " CBSpl" << endl;
			outf << "End" << endl << endl;
		}
		if (curved) {
			//
			// in this case we have created the pointlist newly with Points on the heap
			//
			if (pointlist)
				for (unsigned int pindex = 0; pindex < totalpoints; pindex++) {
					//cout << "pindex / totalpoints " << pindex  << " " << totalpoints << " " << pointlist[pindex]->x_ << " " << pointlist[pindex]->y_ << " " << pointlist[pindex]<< endl;
#if defined (_MSC_VER) && (_MSC_VER < 1100)
					// MSVC < 6 needs cast here
					delete (Point *)	(pointlist[pindex]);	
#else
					delete				(pointlist[pindex]);	
#endif
				}

		}
	}
	delete[]pointlist;
}


// idraw doesn't support more than one page of graphics
void drvIDRAW::open_page()
{
}

// idraw doesn't support more than one page of graphics
void drvIDRAW::close_page()
{
}

// Convert a PostScript font name to an XLFD font name
const char *drvIDRAW::psfont2xlfd(const char *psfont)
{
	// Times Roman family
	if (!strcmp(psfont, "Times-Roman"))
		return "-*-times-medium-r-*-*-";
	if (!strcmp(psfont, "Times-Bold"))
		return "-*-times-bold-r-*-*-";
	if (!strcmp(psfont, "Times-Italic"))
		return "-*-times-medium-i-*-*-";
	if (!strcmp(psfont, "Times-BoldItalic"))
		return "-*-times-bold-i-*-*-";

	// Helvetica family
	if (!strcmp(psfont, "Helvetica"))
		return "-*-helvetica-medium-r-*-*-";
	if (!strcmp(psfont, "Helvetica-Bold"))
		return "-*-helvetica-bold-r-*-*-";
	if (!strcmp(psfont, "Helvetica-Oblique"))
		return "-*-helvetica-medium-o-*-*-";
	if (!strcmp(psfont, "Helvetica-BoldOblique"))
		return "-*-helvetica-bold-o-*-*-";

	// Courier family
	if (!strcmp(psfont, "Courier"))
		return "-*-courier-medium-r-*-*-";
	if (!strcmp(psfont, "Courier-Bold"))
		return "-*-courier-bold-r-*-*-";
	if (!strcmp(psfont, "Courier-Oblique"))
		return "-*-courier-medium-o-*-*-";
	if (!strcmp(psfont, "Courier-BoldOblique"))
		return "-*-courier-bold-o-*-*-";

	// Symbol family
	if (!strcmp(psfont, "Symbol"))
		return "-*-symbol-medium-r-*-*-";

	// None of the above -- assume Times Roman is close enough
	return "-*-times-medium-r-*-*-";
}

void drvIDRAW::show_text(const TextInfo & textinfo)
{
	// Output the text header
	print_header("Text");

	// Output the name of the font to use on screen
	outf << "%I f " << psfont2xlfd(textinfo.currentFontName.c_str());
	outf << iscale(textinfo.currentFontSize);
	outf << "-*-*-*-*-*-*-*" << endl;

	// Output the name of the font to print
	outf << textinfo.currentFontName.c_str() << ' ';
	outf << iscale(textinfo.currentFontSize);
	outf << " SetF" << endl;

	// Output the next part of the text setup boilerplate
	outf << "%I t" << endl;
	const float toRadians = 3.14159265359f / 180.0f;
	const float angle = textinfo.currentFontAngle * toRadians;
	const float xoffset = textinfo.currentFontSize * (float) -sin(angle);
	const float yoffset = textinfo.currentFontSize * (float) cos(angle);
	outf << "[ " << cos(angle) << ' ' << sin(angle) << ' ';
	outf << -sin(angle) << ' ' << cos(angle) << ' ';
	outf << (unsigned int) (0.5 + xoffset + textinfo.x() / IDRAW_SCALING) << ' ';
	outf << (unsigned int) (0.5 + yoffset + textinfo.y() / IDRAW_SCALING);
	outf << " ] concat" << endl;
	outf << "%I" << endl;
	outf << "[" << endl;

	// Output the string, escaping parentheses with backslashes
	outf << '(';
	for (const char *c = textinfo.thetext.c_str(); *c; c++)
		switch (*c) {
		case '(':
			outf << "\\(";
			break;
		case ')':
			outf << "\\)";
			break;
		default:
			outf << *c;
			break;
		}
	outf << ')' << endl;
	outf << "] Text" << endl;
	outf << "End" << endl << endl;
}

// Show the current path (i.e. shape)
void drvIDRAW::show_path()
{
	print_coords();
}


void drvIDRAW::show_image(const PSImage & imageinfo)
{
	if (outBaseName == "") {
		errf << "images cannot be handled via standard output. Use an output file " << endl;
		return;
	}

	imageinfo.writeIdrawImage(outf, 1.0f / IDRAW_SCALING);
}

static DriverDescriptionT < drvIDRAW > D_idraw("idraw", "Interviews draw format (EPS)", "", "idraw", false,	// if backend supports subpaths, else 0
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
											   true,	// if backend supports elements with fill and edges
											   true,	// if backend supports text, else 0
											   DriverDescription::imageformat::memoryeps,	// no support for PNG file images
											   DriverDescription::opentype::normalopen, false,	// if format supports multiple pages in one file
											   false /*clipping */ );
 
