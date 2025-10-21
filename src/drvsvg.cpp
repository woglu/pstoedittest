/* 
   drvSVG.cpp :
   backend for the SVG (Scalable Vector Graphics) format for pstoedit

   Copyright (C) 1999 - 2024 Wolfgang Glunz, wglunz35_AT_pstoedit.net

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

// TODO: width & height done
// text ?? space:preserve ?? special characters (done), color, 
// image (done)
// merged pathes - how to set the stroke and the fill params seperately ?
//  (is linewidth == 0 in case of pure fill ? -> turn linewithd of 0 -> nostroke)
// add an option to select the DTD - done
// check title, and fill-rule params.
// dash, miter, linecap,  - done

#include "drvsvg.h"
#include "pstoedit_config.h"
#include <ctype.h>
#include I_fstream
#include I_stdio
#include I_stdlib

#if (defined(unix) || defined(__unix__) || defined(_unix) || defined(__unix) || defined (NetBSD) ) && !defined(DJGPP)
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#else
#include <io.h>
#include <fcntl.h>
#endif

#include "base64writer.h"
#include "papersizes.h"

const char *const begincomment = "<!-- ";
const char *const endcomment = "-->";

const int Found = 0;
const int notFound = 1;

//lint -esym(754,Symbolmapping::htmlname) // not used so far
//lint -esym(754,Symbolmapping::dummy) // not used so far

struct Symbolmapping {
	const char *psfontname;
	unsigned char psencoding;
	const char *htmlname;
	const char *htmlcharacters;
	int dummy;
	static const Symbolmapping symbolmapping[];
	static const char *getmapping(unsigned char inchar) {
		// could be made much quicker if we consider the gaps in the table
		unsigned int i = 0;
		while (symbolmapping[i].psfontname != 0) {
			if (symbolmapping[i].psencoding == inchar)
				return symbolmapping[i].htmlcharacters;
			i++;
		}
		return 0;
		//if (inchar >= 0xA0 ) return symbolmapping
	}
};

const Symbolmapping Symbolmapping::symbolmapping[] = {
	// see for example: http://www.iangraham.org/books/xhtml1/entity/en_symbol.html
	// http://w3schools.com/tags/ref_symbols.asp
	//
	// psfontname, psencoding, htmlname, htmlcharacters, dummy,
	{"space", 0x20, "??", " ", Found},
	{"exclam", 0x21, "??", "!", Found},
	{"universal", 0x22, "forall", "&#8704;", Found},
	{"numbersign", 0x23, "??", "#", Found},
	{"existential", 0x24, "??", "??", notFound},
	{"percent", 0x25, "??", "%", Found},
	{"ampersand", 0x26, "??", "&#38;" /* "&" */ , Found},
	{"suchthat", 0x27, "??", "??", notFound},
	{"parenleft", 0x28, "??", "(", Found},
	{"parenright", 0x29, "??", ")", Found},
	{"asteriskmath", 0x2A, "??", "*", Found},	// lowast   CDATA "&#8727;"
	{"plus", 0x2B, "??", "+", Found},
	{"comma", 0x2C, "??", ",", Found},
	{"minus", 0x2D, "minus", "-", Found},	// "&#8722;" 
	{"period", 0x2E, "??", ".", Found},
	{"slash", 0x2F, "??", "/", Found},
	{"zero", 0x30, "??", "0", Found},
	{"one", 0x31, "??", "1", Found},
	{"two", 0x32, "??", "2", Found},
	{"three", 0x33, "??", "3", Found},
	{"four", 0x34, "??", "4", Found},
	{"five", 0x35, "??", "5", Found},
	{"six", 0x36, "??", "6", Found},
	{"seven", 0x37, "??", "7", Found},
	{"eight", 0x38, "??", "8", Found},
	{"nine", 0x39, "??", "9", Found},
	{"colon", 0x3A, "??", ":", Found},
	{"semicolon", 0x3B, "??", ";", Found},
	{"less", 0x3C, "??", "&#60;" /*"<" */ , Found},
	{"equal", 0x3D, "??", "=", Found},
	{"greater", 0x3E, "??", "&#62;" /* ">" */ , Found},
	{"question", 0x3F, "??", "?", Found},
	{"congruent", 0x40, "cong", "&#8773;?????", Found},
	{"Alpha", 0x41, "Alpha", "&#913;", Found},
	{"Beta", 0x42, "Beta", "&#914;", Found},
	{"Chi", 0x43, "Chi", "&#935;", Found},
	{"Delta", 0x44, "Delta", "&#916;", Found},
	{"Epsilon", 0x45, "Epsilon", "&#917;", Found},
	{"Phi", 0x46, "Phi", "&#934;", Found},
	{"Gamma", 0x47, "Gamma", "&#915;", Found},
	{"Eta", 0x48, "Eta", "&#919;", Found},
	{"Iota", 0x49, "Iota", "&#921;", Found},
	{"theta1", 0x4A, "??", "??", notFound},	///
	{"Kappa", 0x4B, "Kappa", "&#922;", Found},
	{"Lambda", 0x4C, "Lambda", "&#923;", Found},
	{"Mu", 0x4D, "Mu", "&#924;", Found},
	{"Nu", 0x4E, "Nu", "&#925;", Found},
	{"Omicron", 0x4F, "Omicron", "&#927;", Found},
	{"Pi", 0x50, "Pi", "&#928;", Found},
	{"Theta", 0x51, "Theta", "&#920;", Found},
	{"Rho", 0x52, "Rho", "&#929;", Found},
	{"Sigma", 0x53, "Sigma", "&#931;", Found},
	{"Tau", 0x54, "Tau", "&#932;", Found},
	{"Upsilon", 0x55, "Upsilon", "&#933;", Found},
	{"sigma1", 0x56, "??", "??", notFound},	////
	{"Omega", 0x57, "Omega", "&#937;", Found},
	{"Xi", 0x58, "Xi", "&#926;", Found},
	{"Psi", 0x59, "Psi", "&#936;", Found},
	{"Zeta", 0x5A, "Zeta", "&#918;", Found},
	{"bracketleft", 0x5B, "??", "[", Found},
	{"therefore", 0x5C, "??", "??", notFound},	//
	{"bracketright", 0x5D, "??", "]", Found},	//
	{"perpendicular", 0x5E, "perp", "&#8869;", Found},
	{"underscore", 0x5F, "??", "_", Found},
	{"radicalex", 0x60, "??", "??", notFound},
	{"alpha", 0x61, "alpha", "&#945;", Found},
	{"beta", 0x62, "beta", "&#946;", Found},
	{"chi", 0x63, "chi", "&#967;", Found},
	{"delta", 0x64, "delta", "&#948;", Found},
	{"epsilon", 0x65, "epsilon", "&#949;", Found},
	{"phi", 0x66, "phi", "&#966;", Found},
	{"gamma", 0x67, "gamma", "&#947;", Found},
	{"eta", 0x68, "eta", "&#951;", Found},
	{"iota", 0x69, "iota", "&#953;", Found},
	{"phi1", 0x6A, "??", "??", notFound},
	{"kappa", 0x6B, "kappa", "&#954;", Found},
	{"lambda", 0x6C, "lambda", "&#955;", Found},
	{"mu", 0x6D, "mu", "&#956;", Found},
	{"nu", 0x6E, "nu", "&#957;", Found},
	{"omicron", 0x6F, "omicron", "&#959;", Found},
	{"pi", 0x70, "pi", "&#960;", Found},
	{"theta", 0x71, "theta", "&#952;", Found},
	{"rho", 0x72, "rho", "&#961;", Found},
	{"sigma", 0x73, "sigma", "&#963;", Found},
	{"tau", 0x74, "tau", "&#964;", Found},
	{"upsilon", 0x75, "upsilon", "&#965;", Found},
	{"omega1", 0x76, "??", "??", notFound},	//
	{"omega", 0x77, "omega", "&#969;", Found},
	{"xi", 0x78, "xi", "&#958;", Found},
	{"psi", 0x79, "psi", "&#968;", Found},
	{"zeta", 0x7A, "zeta", "&#950;", Found},
	{"braceleft", 0x7B, "??", "{", Found},
	{"bar", 0x7C, "??", "|", Found},
	{"braceright", 0x7D, "??", "}", Found},
	{"similar", 0x7E, "??", "~", Found},	// asymp    CDATA "&#8776;" -- almost equal to = asymptotic to, U+2248 ISOamsr -->
	{"Euro", 0xA0, "??", "??", notFound},	//
	{"Upsilon1", 0xA1, "??", "??", notFound},	//
	{"minute", 0xA2, "prime", "&#8242;", Found},
	{"lessequal", 0xA3, "le", "&#8804;", Found},
	{"fraction", 0xA4, "frasl", "&#8260;", Found},
	{"infinity", 0xA5, "infin", "&#8734;", Found},
	{"florin", 0xA6, "fnof", "&#402;", Found},
	{"club", 0xA7, "clubs", "&#9827;", Found},
	{"diamond", 0xA8, "diams", "&#9830;", Found},
	{"heart", 0xA9, "hearts", "&#9829;", Found},
	{"spade", 0xAA, "spades", "&#9824;", Found},
	{"arrowboth", 0xAB, "harr", "&#8596;????", Found},
	{"arrowleft", 0xAC, "larr", "&#8592;", Found},
	{"arrowup", 0xAD, "uarr", "&#8593;", Found},
	{"arrowright", 0xAE, "rarr", "&#8594;", Found},
	{"arrowdown", 0xAF, "darr", "&#8595;", Found},
	{"degree", 0xB0, "??", "&#176;" /*"ø" */ , Found},
	{"plusminus", 0xB1, "plusmn", "&#177;", Found},
	{"second", 0xB2, "Prime", "&#8243;", Found},
	{"greaterequal", 0xB3, "ge", "&#8805;", Found},
	{"multiply", 0xB4, "times", "&#215;", Found},
	{"proportional", 0xB5, "prop", "&#8733;", notFound},
	{"partialdiff", 0xB6, "part", "&#8706;", Found},
	{"bullet", 0xB7, "bull", "&#8226;", Found},
	{"divide", 0xB8, "divide", "&#247;", Found},
	{"notequal", 0xB9, "ne", "&#8800;", Found},
	{"equivalence", 0xBA, "equiv", "&#8801;", Found},
	{"approxequal", 0xBB, "cong", "&#8773;????", Found},
	{"ellipsis", 0xBC, "hellip", "&#8230;", Found},
	{"arrowvertex", 0xBD, "??", "??", notFound},
	{"arrowhorizex", 0xBE, "??", "??", notFound},
	{"carriagereturn", 0xBF, "crarr", "&#8629;", Found},
	{"aleph", 0xC0, " alefsym", "&#8501;??", Found},
	{"Ifraktur", 0xC1, "image", "&#8465;", Found},
	{"Rfraktur", 0xC2, "real", "&#8476;", Found},
	{"weierstrass", 0xC3, "weierp", "&#8472;", Found},
	{"circlemultiply", 0xC4, "otimes", "&#8855;", Found},
	{"circleplus", 0xC5, "oplus", "&#8853;", Found},
	{"emptyset", 0xC6, "empty", "&#8709;", Found},
	{"intersection", 0xC7, "cap", "&#8745;", Found},
	{"union", 0xC8, "cup", "&#8746;", Found},
	{"propersuperset", 0xC9, "sup", "&#8835;", Found},
	{"reflexsuperset", 0xCA, "supe", "&#8839;", Found},
	{"notsubset", 0xCB, "nsub", "&#8836;", Found},
	{"propersubset", 0xCC, "sub", "&#8834;", Found},
	{"reflexsubset", 0xCD, "sube", "&#8838;", Found},
	{"element", 0xCE, "isin", "&#8712;", Found},
	{"notelement", 0xCF, "notin", "&#8713;", Found},
	{"angle", 0xD0, "ang", "&#8736;", Found},
	{"gradient", 0xD1, "??", "??", notFound},
	{"registerserif", 0xD2, "reg", "&#174;", Found},
	{"copyrightserif", 0xD3, "copy", "&#169;", Found},
	{"trademarkserif", 0xD4, "trade", "&#8482;", Found},
	{"product", 0xD5, "prod", "&#8719;", Found},
	{"radical", 0xD6, "radic", "&#8730;", Found},
	{"dotmath", 0xD7, "sdot", "&#8901;??", Found},
	{"logicalnot", 0xD8, "not", "&#172;", Found},
	{"logicaland", 0xD9, "and", "&#8743;", Found},
	{"logicalor", 0xDA, "or", "&#8744;", Found},
	{"arrowdblboth", 0xDB, "hArr", "&#8660;", Found},
	{"arrowdblleft", 0xDC, "lArr", "&#8656;", Found},
	{"arrowdblup", 0xDD, "uArr", "&#8657;", Found},
	{"arrowdblright", 0xDE, "rArr", "&#8658;", Found},
	{"arrowdbldown", 0xDF, "dArr", "&#8659;", Found},
	{"lozenge", 0xE0, "loz", "&#9674;", Found},
	{"angleleft", 0xE1, "lang", "&#9001;??", Found},
	{"registersans", 0xE2, "reg", "&#174;", Found},
	{"copyrightsans", 0xE3, "copy", "&#169;", Found},
	{"trademarksans", 0xE4, "trade", "&#8482;", Found},
	{"summation", 0xE5, "sum", "&#8721;", Found},
	{"parenlefttp", 0xE6, "??", "??", notFound},
	{"parenleftex", 0xE7, "??", "??", notFound},
	{"parenleftbt", 0xE8, "??", "??", notFound},
	{"bracketlefttp", 0xE9, "lceil", "&#8968;", Found}, //2308
	{"bracketleftex", 0xEA, "??", "??", notFound},
	{"bracketleftbt", 0xEB, "lfloor", "&#8969;", Found}, //230A
	{"bracelefttp", 0xEC, "??", "??", notFound},
	{"braceleftmid", 0xED, "??", "??", notFound},
	{"braceleftbt", 0xEE, "??", "??", notFound},
	{"braceex", 0xEF, "??", "??", notFound},
	{"angleright", 0xF1, "ang", "&#8736;", Found},
	{"integral", 0xF2, "int", "&#8747;", Found},
	{"integraltp", 0xF3, "??", "??", notFound},
	{"integralex", 0xF4, "??", "??", notFound},
	{"integralbt", 0xF5, "??", "??", notFound},
	{"parenrighttp", 0xF6, "??", "??", notFound},
	{"parenrightex", 0xF7, "??", "??", notFound},
	{"parenrightbt", 0xF8, "??", "??", notFound},
	{"bracketrighttp", 0xF9, "rceil", "&#8970;", Found}, //2309
	{"bracketrightex", 0xFA, "??", "??", notFound},
	{"bracketrightbt", 0xFB, "rfloor", "&#8971;", Found}, //230b
	{"bracerighttp", 0xFC, "??", "??", notFound},
	{"bracerightmid", 0xFD, "??", "??", notFound},
	{"bracerightbt", 0xFE, "??", "??", notFound},
	{0, 0, 0, 0, notFound}
};


static const char *hexcolorstring(float r, float g, float b)
{
	static char buffer[15];
	sprintf_s(TARGETWITHLEN(buffer,15), "%s%.2x%.2x%.2x", "#", (unsigned int) (r * 255),
				(unsigned int) (g * 255), (unsigned int) (b * 255));
//  cerr << buffer << " ";
//  cerr << r << " ";
//  cerr << (r * 255);
//  cerr << (unsigned int) (r * 255) << " ";
//  cerr << endl;
	return buffer;
}



drvSVG::derivedConstructor(drvSVG):
constructBase,
//imgcount(0), 
clippathid(0)

{
	if (strcmp(driverdesc.symbolicname, "xaml") == 0) {
		xaml = true ; 
	} else {
		xaml = false;
	}
	if (options->border_p < 0) {
		errf << "border argument should be >= 0. Set to 0" << endl;
		options->border_p = 0;
	}
	border =  (float) options->border_p * 0.01f;

	if (xaml) {

		outf << "<Page xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\">" << endl;
		outf << " <Viewbox Stretch=\"Uniform\">" << endl;

	} else {
	if (options->standalone) {
		outf << "<?xml version=\"1.0\" standalone=\"yes\"?>" << endl;
	} else {
		outf << "<?xml version=\"1.0\" standalone=\"no\"?>" << endl;
	}
	if (options->withDTD) {
		if (options->localDTD) {
			outf << "<!DOCTYPE svg SYSTEM \"svg-19990812.dtd\">" << endl;
		} else {
#if 0
			outf << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG August 1999//EN\"" << endl;
			outf << "\"http://www.w3.org/Graphics/SVG/svg-19990812.dtd\">" << endl;
#else

			outf << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20000802//EN\"" << endl;
			outf << "\"http://www.w3.org/TR/2000/CR-SVG-20000802/DTD/svg-20000802.dtd\">" << endl;
#endif
		}
	}
	} 
#if 0
	<?xml version = "1.0" standalone =
		"no" ? ><!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 20000802//EN"
		"http://www.w3.org/TR/2000/CR-SVG-20000802/DTD/svg-20000802.dtd" >
#endif
}

drvSVG::~drvSVG()
{
// driver specific deallocations
// and writing of trailer to output file

	if (xaml) {
		outf << "    </Viewbox>" << endl;
		outf << "</Page> " << endl;
	}
	options=0;
}

void drvSVG::print_coords()
{
	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		const basedrawingelement & elem = pathElement(n);
		switch (elem.getType()) {
		case moveto:{
				const Point & p = elem.getPoint(0);
				outf << "\tM ";
				outf << p.x() + x_offset << " " << currentDeviceHeight + y_offset - p.y() << " ";
			}
			break;
		case lineto:{
				const Point & p = elem.getPoint(0);
				outf << "\tL ";
				outf << p.x() + x_offset << " " << currentDeviceHeight + y_offset - p.y() << " ";
			}
			break;
		case closepath:
			outf << "\tZ ";
			break;
		case curveto:{
				outf << "\tC ";
				for (unsigned int cp = 0; cp < 3; cp++) {
					const Point & p = elem.getPoint(cp);
					outf << (p.x() + x_offset) << " "
						<< currentDeviceHeight - (p.y() + y_offset) << " ";
				}
			}
			break;
		default:
			errf << "\t\tFatal: unexpected case in drvSVG " << endl;
			abort();
			break;
		}
		outf << endl;
	}
}

bool drvSVG::textIsWorthToPrint(const RSString& thetext) const
{
	if (options->texmode) { return true; } else { return drvbase::textIsWorthToPrint(thetext); }
}

void drvSVG::open_page()
{
	// const float framespace = 0.0f; //0.1f; // frame  for BB (as factor of height and witdth
	const BBox & bb = getCurrentBBox();
	   
	const RSString pagesize = getPageSize(); // returns "" if nothing is specified via -pagesize
	const PaperInfo* paperinfo = pagesize.length()
		? getPaperInfo(pagesize.c_str())
		: nullptr;

	if (xaml) {
		outf << "<Canvas>" << endl;
	} else {
	if (paperinfo) {
		const char * const unit = paperinfo->preferredunit == p_mm ? "mm" : "in";
		const double pwidth = paperinfo->preferredunit == p_mm ? paperinfo->mmw : paperinfo->inw;
		const double pheight = paperinfo->preferredunit == p_mm ? paperinfo->mmh : paperinfo->inh;
		outf << "<svg" <<
		" width=\""  << pwidth  << unit << "\"" <<
		" height=\"" << pheight << unit << "\"";
		if (!options->noviewbox) {
			// !! viewBox = "x y width height" !!
			outf << " viewBox=\"" <<
				0 << unit <<
				0 << unit <<
				pwidth  << unit <<
				pheight << unit << "\"";
		}
		if (Verbose()) cout << "Paper Info for " << pagesize << " is " << paperinfo << endl;
	} else {
		const float width  = bb.ur.x() - bb.ll.x();
		const float height = bb.ur.y() - bb.ll.y();
		const float wplus = border * width;
		const float hplus = border * height;
		outf << "<svg" <<
		" width=\"" << width + 2.0f * wplus << "px\"" <<
		" height=\"" << height + 2.0f * hplus << "px\"";
		if (!options->noviewbox) {
			// const char * const unit = "px "; // see mail by Herbert Klein
			// !! viewBox = "x y width height" !!
			const char * const unit = " ";
			outf << " viewBox=\"" <<
				bb.ll.x() - wplus << unit <<
				currentDeviceHeight + y_offset - (bb.ur.y() + hplus) << unit <<
				width + 2.0f * wplus << unit <<
				height + 2.0f * hplus << unit << "\"";
		}
		if (Verbose()) {
			cout << "BB " << bb.ll.x() << " " << bb.ll.y() << " " << bb.ur.x() << " " << bb.ur.y() << " " 
				 << wplus << " " << hplus << endl;
		}
	}
	   	
#if 0
	// now done always - see mail by Herbert Klein
	// this xmlns is at least required by Sketsa 
	if (options->withnamespace) 
#endif
	{
		outf << " xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\"" ;
	}

	outf << ">" << endl;

	outf << "<title>" << options->titlestring.value << "</title>" << endl;
	outf << "<!-- generated by pstoedit version:" << drvbase::VersionString() << " -->" << endl;
	//outf <<"<!-- from " << inFileName << " -->" << endl;
	// cout << "y_offset : " << y_offset << " currentDeviceHeight " << currentDeviceHeight<< endl;
	outf << "<g xml:space='preserve'>" << endl;

	if (options->noviewbox) {
	// global scaling (pixels to points)
		outf << "<g transform=\"scale(1.25)\">" << endl; 
	}
	}

#if 0
	outf << begincomment << endl;
	outf << "Opening page: " << currentPageNumber << endl;
	outf << endcomment << endl;
#endif
}

void drvSVG::close_page()
{
		if (xaml) {
		outf << "</Canvas>" << endl;
	} else {
	
	
	// Maybe remove the deletes and the redefinition of currentsavelevel because this
	// is also done in ~drvbase
#if 1
	if (currentSaveLevel->previous != nullptr) {
		// we should close all open <g> somehow
		// but the check above is not correct or is it ???
		errf <<
			"Warning: possible problem encountered in input file: too few (g)restores before showpage"
			<< endl;

		while (currentSaveLevel->previous != nullptr) {
			// BEGIN SPECIFIC PART
			for (unsigned int i = 0; i < currentSaveLevel->clippathlevel; i++) {
				if (Verbose())
					outf << begincomment << " cliprestore} " << endcomment;
				outf << "</g >" << endl;
			}
			if (options->withgrouping) {
				if (Verbose())
					outf << begincomment << " restore} " << endcomment;
				outf << "</g >" << endl;
			}
			// END SPECIFIC PART
			currentSaveLevel = currentSaveLevel->previous;
			delete currentSaveLevel->next;
		}
	}
#endif
	// close any open clippathes from the top level
	for (unsigned int i = 0; i < currentSaveLevel->clippathlevel; i++) {
		if (Verbose())
			outf << begincomment << " cliprestore} " << endcomment;
		outf << "</g>" << endl;
	}
	outf << "</g>" << endl; // one more for the global text attributes

	if (options->noviewbox) {
		outf << "</g>" << endl; // one more for the global scaling (pixels to points)
	}
	outf << "</svg>" << endl;
	// cout << "y_offsetat end : " << y_offset << " currentDeviceHeight " << currentDeviceHeight<< endl;
#if 0
	outf << begincomment << endl;
	outf << "Closing page: " << (currentPageNumber) << endl;
	outf << endcomment << endl;
#endif

	}
}

// plot-svg generates :
// <text 
//  transform="translate(22.29,-282.16) scale(0.86069,-1.1393) " 
//	style="font-family:Courier-BoldOblique,'Courier',monospace;
//         font-style:oblique;font-weight:bold;font-size:7.8225;stroke:none;fill:black;"
// >
// PHYS ID:
// </text>
#if 0
 if (strcmp (ps_name, css_family) == 0)
    /* no need to specify both */
    css_family_is_ps_name = true;
  else
    css_family_is_ps_name = false;

  if (css_generic_family)
    {
      if (css_family_is_ps_name)
	sprintf (page->point, "font-family:'%s',%s;",
		 css_family, css_generic_family);
      else
	sprintf (page->point, "font-family:%s,'%s',%s;",
		 ps_name, css_family, css_generic_family);
    }
  else
    {
      if (css_family_is_ps_name)
	sprintf (page->point, "font-family:'%s';",
		 css_family);
      else
	sprintf (page->point, "font-family:%s,'%s';",
		 ps_name, css_family);
    }
  _update_buffer (page);
  
  if (strcmp (css_style, "normal") != 0) /* not default */
    {
      sprintf (page->point, "font-style:%s;",
	       css_style);
      _update_buffer (page);
    }

  if (strcmp (css_weight, "normal") != 0) /* not default */
    {
      sprintf (page->point, "font-weight:%s;",
	       css_weight);
      _update_buffer (page);
    }

  if (strcmp (css_stretch, "normal") != 0) /* not default */
    {
      sprintf (page->point, "font-stretch:%s;",
	       css_stretch);
      _update_buffer (page);
    }
#endif


void drvSVG::show_text(const TextInfo & textinfo)
{
	unsigned char *outstring = (unsigned char *) cppstrndup(textinfo.thetext.c_str(),textinfo.thetext.length());
	
// plot-svg generates :
// <text 
//  transform="translate(22.29,-282.16) scale(0.86069,-1.1393) " 
//	style=" font-family:Courier-BoldOblique,'Courier',monospace;font-style:oblique;font-weight:bold; 
//	font-size:7.8225;stroke:none;fill:black;"
	// wogl: the above is a so called font group first try Courier-BoldOblique, then 'Courier', ...
// >

	outf << "<g style=\""
		<< "stroke:none;"
		<< "fill:" << hexcolorstring(textinfo.currentR, textinfo.currentG,
									 textinfo.currentB) << ';' ;
//		<< "text-rendering:optimizeLegibility;" 
	if (!options->notextrendering) outf	<< "text-rendering:geometricPrecision;"  ;
	outf	<< "font-size:" << textinfo.currentFontSize << ";" ;
#if 0
I think you may have more success by changing your svg slightly.  Instead of
using "optimizeLegibility", please substitute "optimizeGeometricPrecision" in
your SVG.  I have tested this with the file you provided and it works fine.

I hope this helps

Best Regards
Ross McGuire
EMEA SVG Developer Support

Herbert Klein:
2. Problem: textrendering Style property
-----------------------------------------
Pstoedit schreibt folgendes:
<g style="stroke:none;fill:#7f7f7f;textrendering:optimizeGeometricPrecision; "
...

"textrendering" hat jedoch laut Spec keinen erlaubten Wert 
textrendering:optimizeGeometricPrecision; 

Ich denke das sollte  textrendering:geometricPrecision; heissen.

#endif


	if (options->texmode) {
		outf << "font-family:";
		const char *const first = textinfo.currentFontName.c_str();
		const char *last = first + strlen(first) - 1;
		while ((last > first) && (isdigit(*last))) {
			last--;
		}
		last++;					// position at first digit or eos
		const char *t = first;
		while (t != last) {
			outf << *t;
			t++;
		};
		//outf << " xx:" << textinfo.currentFontName.c_str() << ":xx ";
		outf << ";";
	} else {
		if (textinfo.remappedfont) {
			// just use the string from the fontmap
			outf << textinfo.currentFontName.c_str();
				//outf << ";";
		} else {
			outf << "font-family:" << textinfo.currentFontName.c_str() << ";";
		}
	}

	outf << "\">" << endl;


#if 0
	float cx;					// next five items correspond to the 
	float cy;					// params for the awidthshow operator
	int Char;					// of PostScript
	float ax;
	float ay;
#endif

	const float *CTM = getCurrentFontMatrix();
	const float fontsize = textinfo.currentFontSize;
	outf << "\t<text "
		<< "transform=\"matrix("
		<<  CTM[0] / fontsize << " "
		<< -CTM[1] / fontsize << " " 
		<< -CTM[2] / fontsize << " " 
		<<  CTM[3] / fontsize << " "
		<<  CTM[4] << " "
		<<  currentDeviceHeight - CTM[5]
		<< ")\""
		<< ">";
	const unsigned char *t = outstring;
	// use something like ischar...- everything else use hex value
	
	// cout << " currentfontname  " << textinfo.currentFontName << endl;
	if (textinfo.currentFontName /* .c_str() */  == RSString("Symbol")) {
		if (Verbose()) cout << "Found a Symbol Font - apply remapping for " << outstring << endl;
		while (*t) {
			const char *alternate = Symbolmapping::getmapping(*t);
			if (alternate) {
				outf << alternate;
			} else {
				errf << "UNKNOWN SYMBOL CHARACTER: '" << std::hex << (int) *t << "'" << endl;
			}
			t++;
		}
	} else {
		if (options->texmode) {
			for (unsigned int i = 0; i < textinfo.thetext.length(); i++) 
			{
				static const char hexkeys[] = "0123456789ABCDEF";
				const unsigned int upperhex = ((unsigned int) t[i]) / 16;
				const unsigned int lowerhex = (unsigned int)  t[i] - upperhex * 16;
				outf << "&#xE0" << hexkeys[upperhex] << hexkeys[lowerhex] << ";";
			}
#if 0
			while (*t) {
				const unsigned int upperhex = ((unsigned int) *t) / 16;
				const unsigned int lowerhex = (unsigned int) *t - upperhex * 16;
				outf << "&#xE0" << hexkeys[upperhex] << hexkeys[lowerhex] << ";";
				t++;
			}
#endif
			//outf << "orig:" << outstring << ":giro" << endl;
		} else {
			while (*t) {
//todo
				if (isalpha(*t) || isdigit(*t) ) {
					outf << *t;
				} else {
//          unsigned int upperhex = ((unsigned int) *t) / 16;
//          unsigned int lowerhex =  (unsigned int) *t - upperhex*16;
					outf << "&#" << (unsigned int) *t << ";";
				}
#if 0
				if (*t == '<')
					outf << "&#60;";
				else if (*t == '>')
					outf << "&#62;";
				else
					outf << *t;
#endif
				t++;
			}
		}
	}
	outf << "</text>" << endl;
	outf << "</g>" << endl;
#if 0
	outf << begincomment << endl;
	outf << "Text String : " << textinfo.thetext.c_str() << endl;
	outf << '\t' << "X " << textinfo.x << " Y " << textinfo.y << endl;
	outf << '\t' << "X_END " << textinfo.x_end << " Y_END " << textinfo.y_end << endl;
	outf << '\t' << "currentFontName: " << textinfo.currentFontName.c_str() << endl;
	outf << '\t' << "is_non_standard_font: " << textinfo.is_non_standard_font << endl;
	outf << '\t' << "currentFontFamilyName: " << textinfo.currentFontFamilyName.c_str() << endl;
	outf << '\t' << "currentFontFullName: " << textinfo.currentFontFullName.c_str() << endl;
	outf << '\t' << "currentFontWeight: " << textinfo.currentFontWeight.c_str() << endl;
	outf << '\t' << "currentFontSize: " << textinfo.currentFontSize << endl;
	outf << '\t' << "currentFontAngle: " << textinfo.currentFontAngle << endl;
	outf << '\t' << "currentR: " << textinfo.currentR << endl;
	outf << '\t' << "currentG: " << textinfo.currentG << endl;
	outf << '\t' << "currentB: " << textinfo.currentB << endl;
	outf << endcomment << endl;
#endif
	delete[]outstring;
}

void drvSVG::show_path()
{
	if (Verbose())
		outf << begincomment << "Path # " << currentNr() << endcomment << endl;
//      if (isPolygon()) outf << " (polygon): "  << endl;
//      else   outf << " (polyline): " << endl;

	// stroke:#000000;stroke-width:1;fill:#66ffff;
	// stroke-meterlimit...;
	// stroke-dasharray:....;
	// stroke-dashoffset:50
	// stroke-linecap: round; 
	// stroke-linejoin: bevel ""

	const char *delim1;
	const char *delim2;
	const char *beginstyle;
	const char *endstyle;

	const bool xmlstyle = false;	// JASC supports "XML and CCS" - don't know the diff so far

	if (xmlstyle) {
		delim1 = ":";
		delim2 = ";";
		beginstyle = "style=\"";
		endstyle = "\"";
	} else {
		delim1 = "=\"";
		delim2 = "\" ";
		beginstyle = "";
		endstyle = "";
	}
	const char * const path = xaml? "<Path " : "<path ";
	const char * const Fill = xaml? "Fill" : "fill";
	const char * const Stroke = xaml? "Stroke" : "stroke";
	if (!options->nogroupedpath) {
		outf << "<g " << beginstyle;
	} else {
		if (xaml) {
			outf << path ;
		} else {
		outf << path << beginstyle;
		}
	}
	switch (currentShowType()) {
	case drvbase::stroke:
		outf << Stroke << delim1 << hexcolorstring(edgeR(), edgeG(),
													 edgeB()) << delim2 <<
			Fill << delim1 << "none" << delim2;
		break;
	case drvbase::fill:
		outf << Fill << delim1 << hexcolorstring(fillR(), fillG(),
												   fillB()) << delim2 <<
			"fill-rule" << delim1 << "nonzero" << delim2;
		break;
	case drvbase::eofill:
		outf << Fill << delim1 << hexcolorstring(fillR(), fillG(),
												   fillB()) << delim2 <<
			"fill-rule" << delim1 << "evenodd" << delim2;
		break;
	default:
		// cannot happen
		outf << "unexpected ShowType " << (int) currentShowType();
		break;
	}

//  if (currentShowType() == drvbase::fill)   outf << "\tcurrentShowType: filled " << endl;
//  if (currentShowType() == drvbase::eofill) outf << "\tcurrentShowType: eofilled " << endl;

	// stroke-meterlimit...;
	// stroke-dasharray:....;
	// stroke-dashoffset:50
	// stroke-linecap: round; 
	// stroke-linejoin: bevel ""


	const unsigned int join = currentLineJoin();	// 0 miter; 1 round; 2 bevel
	const char *joinstring =
		(join == 0) ? "miter" : (join == 1) ? "round" : (join == 2) ? "bevel" : "inherit";
	const float miter = currentMiterLimit();
	const unsigned int cap = currentLineCap();	// 0 butt; 1 round; 2 square
	const char *capstring =
		(cap == 0) ? "butt" : (cap == 1) ? "round" : (cap == 2) ? "square" : "inherit";
	const DashPattern dash(dashPattern());
	if (xaml) {
		outf << "StrokeThickness" << delim1 << currentLineWidth() << delim2;
	outf << "StrokeLineJoin" << delim1 << joinstring << delim2;
	outf << "StrokeStartLineCap" << delim1 << capstring << delim2;
		outf << "StrokeEndLineCap" << delim1 << capstring << delim2;
	outf << "StrokeMiterLimit" << delim1 << miter << delim2;
	} else {
	outf << "stroke-width" << delim1 << currentLineWidth() << delim2;
	outf << "stroke-linejoin" << delim1 << joinstring << delim2;
	outf << "stroke-linecap" << delim1 << capstring << delim2;
	outf << "stroke-miterlimit" << delim1 << miter << delim2;
}
	if (dash.nrOfEntries > 0) {
		outf << "stroke-dasharray" << delim1;
		for (int i = 0; i < dash.nrOfEntries; i++) {
			outf << dash.numbers[i] << " ";
		}
		outf << delim2;
		outf << "stroke-dashoffset" << delim1 << dash.offset << delim2;
	}
#if 0
	outf << begincomment << endl;
	outf << "\tcurrentR: " << currentR() << endl;
	outf << "\tcurrentG: " << currentG() << endl;
	outf << "\tcurrentB: " << currentB() << endl;
	outf << "\tcurrentLineCap: " << currentLineCap() << endl;
	outf << "\tdashPattern: " << dashPattern() << endl;
	outf << "\tPath Elements 0 to " << numberOfElementsInPath() - 1 << endl;
	outf << endcomment << endl;
#endif

	if (!options->nogroupedpath) {
		outf << endstyle << ">" << endl;
		outf << "\t" << path << "d=\"" << endl;
		print_coords();
		outf << "\t\"/>" << endl;
		outf << "</g>" << endl;
	} else {
		outf << endstyle << "\n\td=\"" << endl;
		print_coords();
		outf << "\t\"\n\t";
		outf << "/>" << endl;
	}


}

void drvSVG::show_rectangle(const float /* llx */ , const float /* lly */ ,
							const float /* urx */ , const float /* ury */ )
{
//  outf << "Rectangle ( " << llx << "," << lly << ") (" << urx << "," << ury << ")" << endl;
// just do show_path for a first guess
	show_path();
}

void drvSVG::show_image(const PSImage & imageinfo)
{
	if (imageinfo.isFileImage) {
		// use imageinfo.FileName;
		outf << "<image "		// x=\"" << 0 << "\" y=\"" << 0 << "\"" 
			<< " transform=\"matrix("
			<< imageinfo.normalizedImageCurrentMatrix[0] << ' '
			<< /* - */ -imageinfo.normalizedImageCurrentMatrix[1] << ' '
			// doch doch - zumindest bei im.ps 
			// - no longer needed due to normalization in pstoedit.pro
			<< imageinfo.normalizedImageCurrentMatrix[2] << ' '
			<< -imageinfo.normalizedImageCurrentMatrix[3] << ' '  // THIS IS FLIP SENSITIVE
// transfer
			<< imageinfo.normalizedImageCurrentMatrix[4] << ' '
			<< currentDeviceHeight - imageinfo.normalizedImageCurrentMatrix[5] // THIS AS WELL
			<< ")\"" 
			<< " width=\"" << imageinfo.width << "\"" 
			<< " height=\"" << imageinfo.height << "\"" ;
		if (options->imagetofile) {
			outf << " xlink:href=\"" << imageinfo.FileName << "\"></image>" << endl;
		} else {
			outf << " xlink:href=\"data:image/png;base64," << endl;
			
			int inFile;
#ifdef O_BINARY
			inFile = OPEN(imageinfo.FileName.c_str(), O_RDONLY | O_BINARY);		
#else
			inFile = OPEN(imageinfo.FileName.c_str(), O_RDONLY );		
#endif

			if (inFile < 0) {
				errf << "Could not open file " << imageinfo.FileName.c_str() << " in drvsvg" << endl;
				return ;				// fail
			} else {
/* embedded PNG 
 xlink:href="data:image/jpg;base64,
/9j/4AAQSkZJRgABAQAAAQABAAD/2wBDAAEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEB
...
xThjxf4h4L4C4z4CwGDwVfJONlbOKtfMOLMPi6beFeBoywuGyjijKskm6KqSqyjmOUZhDGPl
oZjDG4KlRwlP/9k="></image>
</g>
*/
				Base64Writer base64writer(outf);
				const unsigned int bufferlen = 1000;
				unsigned char buffer[bufferlen];
				unsigned long totalbytes = 0;
				unsigned int bytesread;
				while( (bytesread = READ(inFile, buffer, bufferlen)) > 0) {
					(void) base64writer.write_base64(buffer,bytesread);
					totalbytes += bytesread;
				}
				if (Verbose()) errf << "Read " << totalbytes << " bytes from file " << imageinfo.FileName.c_str() << " and wrote this to outputfile as base64 data " << endl;
			}
			CLOSE(inFile);
			(void) remove(imageinfo.FileName.c_str());
			outf << "\"></image>" << endl;
		}
	} else {

#if 1
		assert (0 && "SVG should use direct PNG files" );
#else
		char *PNGoutFullFileName = new char[strlen(outDirName) + strlen(outBaseName) + 21];
		sprintf(PNGoutFullFileName, "%s%s%02d.png", outDirName, outBaseName, imgcount++);

//  outf <<  PNGoutFullFileName << endl;
		char *title = new char[strlen(inFileName.c_str()) + 100];
		sprintf(title, "raster image number %d from input file %s", imgcount, inFileName.c_str());
		char generator[100];
		sprintf(generator, "pstoedit version: %s", version);
		imageinfo.writePNGImage(PNGoutFullFileName, inFileName.c_str(), title, generator);
		outf << "<image "		// x=\"" << 0 << "\" y=\"" << 0 << "\"" 
			<< " transform=\"matrix("
			<< imageinfo.normalizedImageCurrentMatrix[0] << ' '
			<< -imageinfo.normalizedImageCurrentMatrix[1] << ' '
			<< imageinfo.normalizedImageCurrentMatrix[2] << ' '
			<< -imageinfo.normalizedImageCurrentMatrix[3] << ' '
// transfer
			<< imageinfo.normalizedImageCurrentMatrix[4] << ' '
			<< currentDeviceHeight - imageinfo.normalizedImageCurrentMatrix[5]
			<< ")\"" << " width=\"" << imageinfo.
			width << "\"" << " height=\"" << imageinfo.
			height << "\"" << " xlink:href=\"" << PNGoutFullFileName << "\"></image>" << endl;
		delete[]title;
		delete[]PNGoutFullFileName;

#endif

	}

#if 0
	outf << begincomment << endl;


	outf << "Image:\n";
	outf << "\ttype: ";
	switch (imageinfo.type) {
	case colorimage:
		outf << " colorimage\n";
		break;
	case normalimage:
		outf << " normalimage\n";
		break;
	case imagemask:
		outf << " imagemask\n";
		break;
	default:
		break;
	}
	outf << "\theight: " << imageinfo.height << endl;
	outf << "\twidth: " << imageinfo.width << endl;
	outf << "\tbits/component: " << imageinfo.bits << endl;
	outf << "\tnumber of color components: " << imageinfo.ncomp << endl;
	outf << "\timageMatrix: ";
	{
		for (unsigned int i = 0; i < 6; i++) {
			outf << imageinfo.imageMatrix[i] << ' ';
		}
	}
	outf << endl;
	outf << "\tnormalizedImageCurrentMatrix: ";
	{
		for (unsigned int i = 0; i < 6; i++) {
			outf << imageinfo.normalizedImageCurrentMatrix[i] << ' ';
		}
	}
	outf << endl;
	outf << "\tpolarity: " << imageinfo.polarity << endl;
	{
		for (unsigned int i = 0; i < imageinfo.nextfreedataitem; i++) {
			if (!(i % 8))
				outf << "\n\t " << i << ":\t";
			outf << (int) imageinfo.data[i] << ' ';
		}
	}
	outf << endl;

	outf << endcomment << endl;

#endif
}

//virtual 
void drvSVG::ClipPath(cliptype clipmode)
{
	currentSaveLevel->clippathlevel++;
	clippathid++;

	// BEGIN SPECIFIC PART
	outf << "<clipPath id=\"clippath" << clippathid
		// (unsigned int) currentSaveLevel << "-" << currentSaveLevel->clippathlevel 
		<< "\"" << "> ";
	outf << "\t<path d=\"" << endl;
	print_coords();				//style="clip-rule:evenodd"
	outf << "\t\" style=\"clip-rule:" << ((clipmode == drvbase::clip) ?
										  "nonzero" : "evenodd") << "\"/>" << endl;
	outf << "</clipPath>" << endl;
	if (Verbose())
		outf << begincomment << " {clipsave " << endcomment;
	outf << "<g style=\"clip-path:url(#clippath" << clippathid
		// (unsigned int) currentSaveLevel << "-" << currentSaveLevel->clippathlevel 
		<< ")\"" << "> " << endl;
	// END SPECIFIC PART
}

//virtual 
void drvSVG::Save()
{
	flushOutStanding();					// first dump all not yet written objects
	add_to_page();
	SaveRestoreInfo *newlevel = new SaveRestoreInfo(currentSaveLevel);
	currentSaveLevel = newlevel;
	// BEGIN SPECIFIC PART
	if (options->withgrouping) {
		if (Verbose())
			outf << begincomment << " {save " << endcomment;
		outf << "<g >" << endl;
	}
	// END SPECIFIC PART
}

//virtual 
void drvSVG::Restore()
{
	if (currentSaveLevel->previous == nullptr) {
		// already at top level, so restore is not allowed
		errf <<
			"Warning: strange input file: too many (g)restores or strange sequence of (g)save/(g)restore/showpage"
			<< endl;
		return;
	}
	flushOutStanding();					// first dump all not yet written object
	add_to_page();
	// BEGIN SPECIFIC PART
	for (unsigned int i = 0; i < currentSaveLevel->clippathlevel; i++) {
		if (Verbose())
			outf << begincomment << " cliprestore} " << endcomment;
		outf << "</g >" << endl;
	}
	if (options->withgrouping) {
		if (Verbose())
			outf << begincomment << " restore} " << endcomment;
		outf << "</g >" << endl;
	}
	// END SPECIFIC PART
	currentSaveLevel = currentSaveLevel->previous;
	delete currentSaveLevel->next;
}

#if 1
static DriverDescriptionT < drvSVG > D_SVG("svg", "Scalable Vector Graphics", "","svg", true,	// backend supports subpathes
										   // if subpathes are supported, the backend must deal with
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
										   true,	// backend supports curves
										   true,	// backend supports elements which are filled and have edges 
										   true,	// backend supports text
										   DriverDescription::imageformat::png,	
										   DriverDescription::opentype::normalopen,
										   false, //  format supports multiple pages in one file
										   true /*clipping */,
										   true // native driver
										   );

static DriverDescriptionT < drvSVG > D_XAML("xaml", "eXtensible Application Markup Language", "","xaml", true,	// backend supports subpathes
										   // if subpathes are supported, the backend must deal with
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
										   true,	// backend supports curves
										   true,	// backend supports elements which are filled and have edges 
										   true,	// backend supports text
										   DriverDescription::imageformat::png,	
										   DriverDescription::opentype::normalopen,
										   false, //  format supports multiple pages in one file
										   true /*clipping */ ,
										   true // native driver
										   );	
#endif
