/* 
   drvPDF.cpp : This file is part of pstoedit
   Backend for PDF(TM) format

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
/*
   The PDF format and the corresponding operators are copyrighted
   by Adobe Systems, Inc.
*/

#include "drvpdf.h"
#include I_iostream
#include I_iomanip
#include I_fstream
#include I_fstream
#include I_stdio
#include I_stdlib

#include <time.h>

// for sin and cos
#include <math.h>

USESTD
using std::setw;
using std::setfill;

static constexpr float rnd(const float f, const float roundnumber)
{
	const float roundup = (f < 0.0f) ? -0.5f : 0.5f;
	return ((long int) ((f * roundnumber) + roundup)) / roundnumber;

}

static inline float RND3(const float f)
{
	return rnd(f, 1000.0f);
}

static const char *PDFFonts[] = {	// predefined Fonts (see page 64 PDF Ref. Manual )
	"Courier",
	"Courier-Bold",
	"Courier-Oblique",
	"Courier-BoldOblique",
	"Helvetica",
	"Helvetica-Bold",
	"Helvetica-Oblique",
	"Helvetica-BoldOblique",
	"Symbol",
	"Times-Roman",
	"Times-Bold",
	"Times-Italic",
	"Times-BoldItalic",
	"ZapfDingbats",
	nullptr
};

const unsigned int numberOfFonts = sizeof(PDFFonts) / (sizeof(char *)) - 1;

const char *const *drvPDF::knownFontNames() const
{
	return PDFFonts;
}

unsigned int drvPDF::newobject()
{
	currentobject++;
	if (currentobject >= maxobjects) {
		errf << "Sorry, too many objects in this file" << endl;
		exit(1);
	}
	startPosition[currentobject] = outf.tellp();
	outf << currentobject << " 0 obj" << endl;
	return currentobject;
}

void drvPDF::endobject()
{
	outf << "endobj" << endl;
}

template <class T >inline T Max(T a, T b)
{
	return (a > b ? a : b);
}

template <class T >inline T Min(T a, T b)
{
	return (a < b ? a : b);
}
void drvPDF::adjustbbox(float x, float y)
{
	bb_llx = Min((int) x, bb_llx);
	bb_lly = Min((int) y, bb_lly);
	bb_urx = Max((int) x, bb_urx);
	bb_ury = Max((int) y, bb_ury);
}

// the following two functions avoid unneccessary ET BT sequences
void drvPDF::starttext()
{
	if (!inTextMode) {
		buffer << "BT" << endl;
		inTextMode = true;
	}
}
void drvPDF::endtext()
{
	if (inTextMode) {
		buffer << "ET" << endl;
		inTextMode = false;
	}
}


static std::streampos newlinebytes = 1;	// how many bytes are a newline (1 or 2)

static const char *const stdEncoding = "Standard";

const int largeint = 32000;
drvPDF::derivedConstructor(drvPDF):
constructBase,
currentobject(0),
pagenr(0),
inTextMode(false),
encodingName(stdEncoding),
buffer(tempFile.asOutput()),
bb_llx(largeint), bb_lly(largeint), bb_urx(-largeint), bb_ury(-largeint)
{
	if (&outf == &cout) {
		// Due to tell not working correctly on cout
		errf << "Sorry, PDF cannot be written to standard output. Use a file instead\n";
		exit(1);
	}
	const char *const header = "%PDF-1.1";
	outf << header << endl;
	newlinebytes = (long) outf.tellp() - (long) strlen(header);
	if (Verbose())
		outf << "% Driver options:" << endl;
	for (unsigned int i = 0; i < d_argc; i++) {
		assert(d_argv && d_argv[i]);
		if (Verbose())
			outf << "% " << d_argv[i] << endl;
		if (strcmp(d_argv[i], "-e") == 0) {
			encodingName = d_argv[i + 1];
		}
	}

	errf << "Info: This PDF driver is not very elaborated - consider using -f gs:pdfwrite instead." << endl;

}

drvPDF::~drvPDF()
{
	// print trailer

	endtext();					// close text if open

	const unsigned int outlines = newobject();
	outf << "<<" << endl;
	outf << "/Type /Outlines" << endl;
	outf << "/Count 0" << endl;
	outf << ">>" << endl;
	endobject();

	const unsigned int encoding = newobject();
	// write the diffs between pdf-encoding and WinAnsiEncoding
	outf << "<<" << endl;
	outf << "/Type /Encoding" << endl;

#ifdef basedonwinansi
// For some reasons this does not work.
// I haven't seen a working example using the /BaseEncoding feature
	outf << "/BaseEncoding /WinAnsiEncoding" << endl;
	outf << "%/Differences [" << endl;
	outf << "% 24 /breve/caron/circumflex/dotaccent/hungarumlaut/ogonek/ring/tilde" << endl;
	outf << "%127 /.notdef 129 /dagger/daggerdbl/ellipsis/emdash/endash/florin/fraction" << endl;
	outf << "%/guilsinglleft/guilsinglright/minus/perthousand/quotedblbase/quotedblleft" << endl;
	outf << "%/quotedblright/quoteleft/quoteright/quotesinglbase/trademark/fi/fl/Lslash" << endl;
	outf <<
		"%/OE/Scaron/Ydieresis/Zcaron/dotlessi/lslash/scaron/zcaron/.notdef/.notdef/.notdef"
		<< endl;
	outf << "%]" << endl;
#endif

// The following part (diffs between standard encoding and pdf encoding)
// was generated by a small PostScript program run through gs
	outf << "/Differences [" << endl;
	outf << "24 /breve" << endl;
	outf << "25 /caron" << endl;
	outf << "26 /circumflex" << endl;
	outf << "27 /dotaccent" << endl;
	outf << "28 /hungarumlaut" << endl;
	outf << "29 /ogonek" << endl;
	outf << "30 /ring" << endl;
	outf << "31 /tilde" << endl;
	outf << "39 /quotesingle" << endl;
	outf << "96 /grave" << endl;
	outf << "128 /bullet" << endl;
	outf << "129 /dagger" << endl;
	outf << "130 /daggerdbl" << endl;
	outf << "131 /ellipsis" << endl;
	outf << "132 /emdash" << endl;
	outf << "133 /endash" << endl;
	outf << "134 /florin" << endl;
	outf << "135 /fraction" << endl;
	outf << "136 /guilsinglleft" << endl;
	outf << "137 /guilsinglright" << endl;
	outf << "138 /minus" << endl;
	outf << "139 /perthousand" << endl;
	outf << "140 /quotedblbase" << endl;
	outf << "141 /quotedblleft" << endl;
	outf << "142 /quotedblright" << endl;
	outf << "143 /quoteleft" << endl;
	outf << "144 /quoteright" << endl;
	outf << "145 /quotesinglbase" << endl;
	outf << "146 /trademark" << endl;
	outf << "147 /fi" << endl;
	outf << "148 /fl" << endl;
	outf << "149 /Lslash" << endl;
	outf << "150 /OE" << endl;
	outf << "151 /Scaron" << endl;
	outf << "152 /Ydieresis" << endl;
	outf << "153 /Zcaron" << endl;
	outf << "154 /dotlessi" << endl;
	outf << "155 /lslash" << endl;
	outf << "156 /oe" << endl;
	outf << "157 /scaron" << endl;
	outf << "158 /zcaron" << endl;
	outf << "164 /currency" << endl;
	outf << "166 /brokenbar" << endl;
	outf << "168 /dieresis" << endl;
	outf << "169 /copyright" << endl;
	outf << "170 /ordfeminine" << endl;
	outf << "172 /logicalnot" << endl;
	outf << "174 /registered" << endl;
	outf << "175 /macron" << endl;
	outf << "176 /degree" << endl;
	outf << "177 /plusminus" << endl;
	outf << "178 /twosuperior" << endl;
	outf << "179 /threesuperior" << endl;
	outf << "180 /acute" << endl;
	outf << "181 /mu" << endl;
	outf << "183 /periodcentered" << endl;
	outf << "184 /cedilla" << endl;
	outf << "185 /onesuperior" << endl;
	outf << "186 /ordmasculine" << endl;
	outf << "188 /onequarter" << endl;
	outf << "189 /onehalf" << endl;
	outf << "190 /threequarters" << endl;
	outf << "192 /Agrave" << endl;
	outf << "193 /Aacute" << endl;
	outf << "194 /Acircumflex" << endl;
	outf << "195 /Atilde" << endl;
	outf << "196 /Adieresis" << endl;
	outf << "197 /Aring" << endl;
	outf << "198 /AE" << endl;
	outf << "199 /Ccedilla" << endl;
	outf << "200 /Egrave" << endl;
	outf << "201 /Eacute" << endl;
	outf << "202 /Ecircumflex" << endl;
	outf << "203 /Edieresis" << endl;
	outf << "204 /Igrave" << endl;
	outf << "205 /Iacute" << endl;
	outf << "206 /Icircumflex" << endl;
	outf << "207 /Idieresis" << endl;
	outf << "208 /Eth" << endl;
	outf << "209 /Ntilde" << endl;
	outf << "210 /Ograve" << endl;
	outf << "211 /Oacute" << endl;
	outf << "212 /Ocircumflex" << endl;
	outf << "213 /Otilde" << endl;
	outf << "214 /Odieresis" << endl;
	outf << "215 /multiply" << endl;
	outf << "216 /Oslash" << endl;
	outf << "217 /Ugrave" << endl;
	outf << "218 /Uacute" << endl;
	outf << "219 /Ucircumflex" << endl;
	outf << "220 /Udieresis" << endl;
	outf << "221 /Yacute" << endl;
	outf << "222 /Thorn" << endl;
	outf << "223 /germandbls" << endl;
	outf << "224 /agrave" << endl;
	outf << "225 /aacute" << endl;
	outf << "226 /acircumflex" << endl;
	outf << "227 /atilde" << endl;
	outf << "228 /adieresis" << endl;
	outf << "229 /aring" << endl;
	outf << "230 /ae" << endl;
	outf << "231 /ccedilla" << endl;
	outf << "232 /egrave" << endl;
	outf << "233 /eacute" << endl;
	outf << "234 /ecircumflex" << endl;
	outf << "235 /edieresis" << endl;
	outf << "236 /igrave" << endl;
	outf << "237 /iacute" << endl;
	outf << "238 /icircumflex" << endl;
	outf << "239 /idieresis" << endl;
	outf << "240 /eth" << endl;
	outf << "241 /ntilde" << endl;
	outf << "242 /ograve" << endl;
	outf << "243 /oacute" << endl;
	outf << "244 /ocircumflex" << endl;
	outf << "245 /otilde" << endl;
	outf << "246 /odieresis" << endl;
	outf << "247 /divide" << endl;
	outf << "248 /oslash" << endl;
	outf << "249 /ugrave" << endl;
	outf << "250 /uacute" << endl;
	outf << "251 /ucircumflex" << endl;
	outf << "252 /udieresis" << endl;
	outf << "253 /yacute" << endl;
	outf << "254 /thorn" << endl;
	outf << "255 /ydieresis" << endl;
	outf << "]" << endl;
	outf << ">>" << endl;
	endobject();


	const unsigned int firstFontObject = currentobject + 1;
	// Now define all the 14 standard fonts
	for (unsigned int f = 0; f < numberOfFonts; f++) {
		const unsigned int font = newobject();
		unused(&font);
		outf << "<<" << endl;
		outf << "/Type /Font" << endl;
		outf << "/Subtype /Type1" << endl;
		outf << "/Name /F" << f << endl;
		outf << "/BaseFont /" << PDFFonts[f] << endl;
		if ((f == 8) || (f == 13)) {
			// the special fonts symbol and ZapfDingbats

			// commented out based on a suggestion from Derek Noonburg
			// outf << "/Encoding /" << encodingName << "Encoding" << endl;
		} else {
			outf << "/Encoding " << encoding << " 0 R" << endl;
		}
		outf << ">>" << endl;
		endobject();
	}

	const unsigned int catalog = newobject();
	unsigned int pages = currentobject + 2;	// will be next after resources;
	outf << "<<" << endl;
	outf << "/Type /Catalog" << endl;
	outf << "/Pages " << pages << " 0 R" << endl;
	outf << "/Outlines " << outlines << " 0 R" << endl;
	outf << ">>" << endl;
	endobject();

	const unsigned int nrOfPages = pagenr;

	const unsigned int resources = newobject();
	outf << "<<" << endl;
	outf << "/ProcSet [ /PDF /Text ]" << endl;
	outf << "/Font <<" << endl;
	for (unsigned int f2 = 0; f2 < numberOfFonts; f2++) {
		outf << "/F" << f2 << " " << f2 + firstFontObject << " 0 R" << endl;
	}
	outf << ">>" << endl;		// closing /Font
	outf << ">>" << endl;		// closing /Resources dictionary
	endobject();

	pages = newobject();
	outf << "<<" << endl;
	outf << "/Type /Pages" << endl;
	outf << "/Count " << nrOfPages << endl;
	outf << "/Kids [ ";
	for (unsigned int i = 1; i <= nrOfPages; i++) {
		outf << i + pages << " 0 R ";
	}
	outf << " ] " << endl;
	outf << "/MediaBox [0 0 " << (int) (currentDeviceWidth +
										0.5) << ' ' <<
		(int) (currentDeviceHeight + 0.5) << "]" << endl;
#if 0
	const int width = bb_urx - bb_llx;
	const int height = bb_ury - bb_lly;
	// heuristically increase shown area by 20% in each direction
	outf << "/MediaBox ["
		<< bb_llx - 0.2 * width << ' '
		<< bb_lly - 0.2 * height << ' '
		<< bb_urx + 0.2 * width << ' ' << bb_ury + 0.2 * height << " ]" << endl;
#endif
	outf << "/Resources ";
	outf << resources << " 0 R" << endl;
	outf << ">>" << endl;
	endobject();


	// Now write the Page parts for each page
	for (unsigned int j = 1; j <= nrOfPages; j++) {
		const unsigned int pageobject = newobject();
		unused(&pageobject);
		outf << "<<" << endl;
		outf << "/Type /Page" << endl;
		outf << "/Parent " << pages << " 0 R" << endl;
		outf << "/Contents " << j << " 0 R" << endl;
		outf << "/Resources " << resources << " 0 R" << endl;
		outf << ">>" << endl;
		endobject();
	}

	const unsigned int infoobject = newobject();
	outf << "<<" << endl;

	outf << "/CreationDate (D:" << drvbase::DateString() << ")" << endl;
#if 0
// Comments by Rohan
// This is a hack
// Since Windows CE does not support, I am just putting a dummy date(i.e "01/01/18 09:00:00")
#ifndef OS_WIN32_WCE
	const time_t t = time(nullptr);
	const struct tm * const localt = localtime(&t);
	// (D:YYYYMMDDHHmmSSOHH'mm')
	// All parts of the date after the year are optional.
	if (localt) {
		outf << "/CreationDate (D:" << drvbase::DateString() << ")" << endl;
#if 0
		<< setw(4) << localt->tm_year + 1900
		<< setw(2) << setfill('0') << localt->tm_mon + 1
		<< setw(2) << setfill('0') << localt->tm_mday
		<< setw(2) << setfill('0') << localt->tm_hour
		<< setw(2) << setfill('0') << localt->tm_min
		<< setw(2) << setfill('0') << localt->tm_sec << ")" << endl;
#endif
	}
#else
	   outf << "/CreationDate (D:"
		<< setw(4) << 2018
		<< setw(2) << setfill('0') << 1
		<< setw(2) << setfill('0') << 1
		<< setw(2) << setfill('0') << 9
		<< setw(2) << setfill('0') << 0
		<< setw(2) << setfill('0') << 0 << ")" << endl;
#endif
#endif

	outf << "/Producer (pstoedit by wglunz35_AT_pstoedit.net)" << endl;
	outf << ">>" << endl;
	endobject();

	const std::streampos xrefbegin = outf.tellp();
	outf << "xref" << endl;
	outf << "0 " << currentobject + 1 << endl;
	outf << "0000000000 65535 f";
	if ((long) newlinebytes == 1L) {
		outf << " ";
	}
	outf << endl;

	for (unsigned int x = 1; x <= currentobject; x++) {
		outf.width(10);
		outf.fill('0');
		outf << startPosition[x] << " 00000 n";
		if ((long) newlinebytes == 1L) {
			outf << " ";
		}
		outf << endl;
	}
	outf << "trailer" << endl;
	outf << "<<" << endl;
	outf << "/Size " << currentobject + 1 << endl;
	outf << "/Info " << infoobject << " 0 R" << endl;
	outf << "/Root " << catalog << " 0 R" << endl;
	outf << ">>" << endl;
	outf << "startxref" << endl;
	outf << xrefbegin << endl;
	outf << "%%EOF" << endl;
	options=nullptr;
	encodingName=nullptr;
}

void drvPDF::print_coords()
{
//    buffer.precision(3);
//    buffer.setf(ios::fixed);
//    buffer.width(0); // to force minimal width
//    buffer.unsetf(ios::showpoint);

	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		const basedrawingelement & elem = pathElement(n);
		switch (elem.getType()) {
		case moveto:{
				const Point & p = elem.getPoint(0);
				adjustbbox(p.x() + x_offset, p.y() + y_offset);
				buffer << RND3(p.x() + x_offset) << " " << RND3(p.y() + y_offset) << " ";
				buffer << "m " << endl;
			}
			break;
		case lineto:{
				const Point & p = elem.getPoint(0);
				adjustbbox(p.x() + x_offset, p.y() + y_offset);
				buffer << RND3(p.x() + x_offset) << " " << RND3(p.y() + y_offset) << " ";
				buffer << "l " << endl;
			}
			break;
		case closepath:
			buffer << "h " << endl;
			break;
		case curveto:{
				for (unsigned int cp = 0; cp < 3; cp++) {
					const Point & p = elem.getPoint(cp);
					adjustbbox(p.x() + x_offset, p.y() + y_offset);
					buffer << RND3(p.x() + x_offset) << " " << RND3(p.y() + y_offset) << " ";
				}
				buffer << "c " << endl;
			}
			break;
		default:
			errf << "Fatal: unexpected case in drvpdf " << endl;
			abort();
			break;
		}
	}
}


void drvPDF::open_page()
{
	endtext();					// close text if open
	const unsigned int currentpage = newobject();
	unused(&currentpage);
	pagenr++;
	// provide a temp stream
	(void)tempFile.asOutput();
}

void drvPDF::close_page()
{
	endtext();					// close text if open

	const std::streampos endpos = buffer.tellp();
	outf << "<<" << endl;
	outf << "/Length " << endpos << endl;
	outf << ">>" << endl;
	outf << "stream" << endl;
	ifstream & instream = tempFile.asInput();
	copy_file(instream, outf);
//  int ret = 0;
//  while ( (ret = instream.get()) != EOF ) {
//      outf << (char) ret ;
//  }
	outf << "endstream" << endl;

	endobject();
}

static int getFontNumber(const char *const fontname)
{
	const size_t fntlength = strlen(fontname);
	for (unsigned int i = 0; i < numberOfFonts; i++) {
		const size_t pdfFntLengh = strlen(PDFFonts[i]);
		if (fntlength == pdfFntLengh) {
			if (strncmp(fontname, PDFFonts[i], fntlength) == 0) {
				return i;
			}
		}
	}
	return -1;
}

static int getSubStringFontNumber(const char *const fontname)
{
	// searches for a font name which is the longest substring of the current font name
	int index = -1;
	size_t longest = 0;
	const size_t fntlength = strlen(fontname);
	for (unsigned int i = 0; i < numberOfFonts; i++) {
		const size_t pdfFntLength = strlen(PDFFonts[i]);
		if (fntlength >= pdfFntLength) {
			if (strncmp(fontname, PDFFonts[i], pdfFntLength) == 0) {
				if (pdfFntLength > longest) {
					longest = pdfFntLength;
					index = (int) i;
				}
			}
		}
	}
	return index;
}

void drvPDF::show_text(const TextInfo & textinfo)
{
	const float toRadians = 3.14159265359f / 180.0f;
	const float angleInRadians = textinfo.currentFontAngle * toRadians;
	int PDFFontNum = getFontNumber(textinfo.currentFontName.c_str());
	if (PDFFontNum == -1) {
		PDFFontNum = getSubStringFontNumber(textinfo.currentFontName.c_str());
		if (PDFFontNum == -1) {
			PDFFontNum = getSubStringFontNumber(defaultFontName);
			if (PDFFontNum == -1) {
				errf << "Warning, unsupported font " << textinfo.
					currentFontName.c_str() << ", using Courier instead" << endl;
				PDFFontNum = 0;	// Courier
			} else {
				errf << "Warning, unsupported font " << textinfo.
					currentFontName.c_str() << ", using " << defaultFontName << " instead" << endl;
			}
		} else {
			errf << "Warning, unsupported font " << textinfo.
				currentFontName.c_str() << ", using " << PDFFonts[PDFFontNum] << " instead" << endl;
		}
	}
	starttext();
// define TFALWAYSONE
// note TFALWAYSONE does not work correctly with awidthshow. See showex.ps
#ifdef TFALWAYSONE
	const float Sx = textinfo.currentFontSize;
	const float Sy = textinfo.currentFontSize;
	buffer << "/F" << PDFFontNum << ' ' << 1 << " Tf" << endl;
	// use size 1 and scale via Tm
#else
	// previously we used currentFontSize for SX and SY
	// and fontsize 1 in Tm, now we use the fontsize in tm and set
	// sx and sy to 1
	const float Sx = 1.0f;
	const float Sy = 1.0f;
	buffer << "/F" << PDFFontNum << ' ' << textinfo.currentFontSize << " Tf" << endl;
#endif
	const float cosphi = (float) cos(angleInRadians);
	const float sinphi = (float) sin(angleInRadians);
	// OK, we could get the real transformation matrix from the interpreter,
	// but this approximation should do it in most cases.
//    buffer.precision(3);
//    buffer.setf(ios::fixed);
//    buffer.width(0); // to force minimal width
//    buffer.unsetf(ios::showpoint);

	adjustbbox(textinfo.x() + x_offset, textinfo.y() + y_offset);
	buffer << RND3(Sx * cosphi) << " "
		<< RND3(Sx * sinphi) << " "
		<< RND3(-Sy * sinphi) << " "
		<< RND3(Sy * cosphi) << " "
		<< RND3(textinfo.x() + x_offset) << " " << RND3(textinfo.y() + y_offset) << " Tm" << endl;
	buffer << RND3(textinfo.currentR) << " " << RND3(textinfo.
													 currentG) << " " <<
		RND3(textinfo.currentB) << " rg" << endl;
#ifdef TFALWAYSONE
	buffer << RND3(textinfo.cx /
				   textinfo.currentFontSize) << ' ' << RND3(textinfo.ax / textinfo.currentFontSize)
		<< ' ';
#else
	buffer << RND3(textinfo.cx) << ' ' << RND3(textinfo.ax) << ' ';
#endif
	buffer << "(";
	const char *start_of_text = textinfo.thetext.c_str();
	while (*start_of_text) {
		if ((*start_of_text == '(') || (*start_of_text == ')') || (*start_of_text == '\\')) {
			buffer << '\\';
		}
		buffer << *start_of_text;
		start_of_text++;
	}
	buffer << ") \"" << endl;
//    buffer << ") Tj"  << endl;
	// endtext(); // not done here to avoid ET BT, done on demand 
}

void drvPDF::show_path()
{
	// add_to_page(); // is done in drvbase !! 
	endtext();					// close text if open
	const char *setrgbcolor = nullptr;
	const char *drawingop = nullptr;
	switch (currentShowType()) {
	case drvbase::stroke:
		// it's a stroke
		drawingop = "S";
		setrgbcolor = "RG";
		break;
	case drvbase::fill:
		drawingop = "f";
		setrgbcolor = "rg";
		break;
	case drvbase::eofill:
		drawingop = "f*";
		setrgbcolor = "rg";
		break;
	default:
		// cannot happen
		errf << "unexpected ShowType " << (int) currentShowType() << endl;
		exit(1);
		break;
	}
//    buffer.precision(3);
//    buffer.setf(ios::fixed);
//    buffer.width(0); // to force minimal width
//    buffer.unsetf(ios::showpoint);

	if (Verbose()) {
		buffer << "% path " << currentNr() << endl;
	}
	buffer << RND3(currentR()) << " " << RND3(currentG()) << " " <<
		RND3(currentB()) << " " << setrgbcolor << endl;
	buffer << currentLineWidth() << " w" << endl;
	buffer << currentLineCap() << " J" << endl;
	buffer << currentLineJoin() << " j" << endl;
	buffer << dashPattern() << " d" << endl;
	print_coords();
	buffer << drawingop << endl;
}

static DriverDescriptionT < drvPDF > D_pdf("pdf", "Adobe's Portable Document Format", "","pdf", true,	// if backend supports subpaths, else 0
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
										   1,	// if backend supports curves, else 0
										   false,	// if backend supports elements with fill and edges
										   true,	// if backend supports text, else 0
										   DriverDescription::imageformat::memoryeps,	// no support for PNG file images
										   DriverDescription::opentype::normalopen, true,	// if format supports multiple pages in one file
										   false  /*clipping */
										   );
