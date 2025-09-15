#ifndef PSTOEDITOPTIONS_H
#define PSTOEDITOPTIONS_H

/*
   pstoeditoptions.h : This file is part of pstoedit
   definition of program options

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


#include "miscutil.h"
#include "poptions.h"

// with support for -DREALLYDELAYBIND - needed for gs9.22
#define WITHRDB 1

enum advancedTypeIDs { start_ty=char_ty, rsstring_ty, charstring_ty, constcharstring_ty, argv_ty };

class DLLEXPORT RSStringValueExtractor {
public:
	static bool getvalue(const char *optname, const char *instring, unsigned int &currentarg, RSString &result) {
	if (instring) {
		result.assign(instring);
		currentarg++;
		return true;
	} else {
		cout << "missing string argument for " << optname << " option" << endl;
		return false;
	}
	}
	static const char *gettypename() { return "string"; }
	static unsigned int gettypeID()  { return rsstring_ty; }
};
#if 0
// not really usable - because when using a Dialog, the char * would afterwards point into the data owned by the dialog
//
class DLLEXPORT charstringValueExtractor {
public:
	static bool getvalue(const char *optname, const char *instring, unsigned int &currentarg, char* &result) {
	if (instring) {
		result = cppstrdup(instring);
		currentarg++;
		return true;
	} else {
		cout << "missing string argument for " << optname << " option" << endl;
		return false;
	}
	}
	static const char *gettypename() { return "string"; }
	static unsigned int gettypeID() { return charstring_ty; }
};

template < >
inline ostream & Option< char *, charstringValueExtractor>::writevalue(ostream & out) const {
		out << (value ? value : "");
		return out;
}

class DLLEXPORT constcharstringValueExtractor {
public:
	static bool getvalue(const char *optname, const char *instring, unsigned int &currentarg, const char* &result) {
	if (instring) {
		result = instring;
		currentarg++;
		return true;
	} else {
		cout << "missing string argument for " << optname << " option" << endl;
		return false;
	}
	}
	static const char *gettypename() { return "string"; }
	static unsigned int gettypeID() { return constcharstring_ty; }
};

template < >
inline ostream & Option< const char *, constcharstringValueExtractor>::writevalue(ostream & out) const {
	out << (value ? value : "");
		return out;
}

#endif

class DLLEXPORT ArgvExtractor {
public:
	static bool getvalue(const char *optname, const char *instring, unsigned int &currentarg, Argv  &result) {
		if (instring) {
			result.addarg(instring);
			currentarg++;
			return true;
		} else {
			cout << "missing string argument for " << optname << " option" << endl;
			return false;
		}
	}
	static const char *gettypename() { return "string"; }
	static unsigned int gettypeID() { return argv_ty; }
};

static const char * const emptyString = "";
static const char * const noArgument = emptyString;
static const char * const UseDefaultDoku = emptyString;


class DLLEXPORT PsToEditOptions : public ProgramOptions {
public:
	static PsToEditOptions& theOptions(); // singleton

	// option categories
	const unsigned int g_t, t_t, d_t, b_t, h_t;

	// cannot be const  because it needs to be changed on non UNIX systems (convertBackSlashes)
	char * nameOfInputFile  ; //= 0;
	char * nameOfOutputFile ; //= 0;	// can contain %d for page splitting

	OptionT < RSString, RSStringValueExtractor> nameOfIncludeFile ;// = 0;	// name of an option include file
	OptionT < RSString, RSStringValueExtractor> replacementfont;// = "Courier";
 	OptionT < bool, BoolTrueExtractor > nomaptoisolatin1 ;//= false;
	OptionT < bool, BoolTrueExtractor > withdisplay ;//= false;
	OptionT < RSString, RSStringValueExtractor> pngimage ;
	OptionT < bool, BoolTrueExtractor > quiet ;//= false;
	OptionT < bool, BoolTrueExtractor > noquit ;//= false;
	OptionT < bool, BoolTrueExtractor > nocurves ;//= false;		// normally curves are shown as curves if backend supports
//	OptionT < bool, BoolTrueExtractor > nosubpaths ;//= false;	// normally we use subpaths if the backend support them
	OptionT < bool, BoolTrueExtractor > mergelines ;//= false;
	OptionT < bool, BoolTrueExtractor > convertFilledRectToStroke; //= false;
	OptionT < bool, BoolTrueExtractor > mergetext ;//= false;
	OptionT < bool, BoolTrueExtractor > drawtext ;//= false;
	OptionT < bool, BoolTrueExtractor > autodrawtext ;//= false;
	OptionT < bool, BoolTrueExtractor > disabledrawtext ;//= false;
	OptionT < bool, BoolTrueExtractor > DrawGlyphBitmaps ;//= false;
	OptionT < bool, BoolTrueExtractor > correctdefinefont ;//= false;

	OptionT < bool, BoolTrueExtractor > ptioption;
	OptionT < bool, BoolTrueExtractor > ptaoption;
	unsigned int precisiontext ;//= 0; // derived from the above pti ->1 pta -> 2 otherwise 0

	OptionT < double, DoubleValueExtractor > xscale;
	OptionT < double, DoubleValueExtractor > yscale;
	OptionT < double, DoubleValueExtractor > xshift;
	OptionT < double, DoubleValueExtractor > yshift;
	OptionT < bool, BoolTrueExtractor > centered ;
	OptionT < double, DoubleValueExtractor > minlinewidth;


	OptionT < RSString, RSStringValueExtractor> pagenumberformat ;
	OptionT < bool, BoolTrueExtractor > splitpages ;//= false;
	OptionT < bool, BoolTrueExtractor > verboseflag ;//= false;
	OptionT < int, IntValueExtractor> verboselevel; //= 0;
	OptionT < bool, BoolTrueExtractor > useBBfrominput; //= false;
	OptionT < bool, BoolTrueExtractor > simulateSubPaths ;//= false;
	OptionT < bool, BoolTrueExtractor > simulateFill ;//= false;
	OptionT < char, CharacterValueExtractor> unmappablecharstring ;//= 0;
	OptionT < bool, BoolTrueExtractor > dontloadplugins ;//= false;
	OptionT < bool, BoolTrueExtractor > nobindversion ;//= false;	// use old NOBIND instead of DELAYBIND
#if WITHRDB
	OptionT < bool, BoolTrueExtractor > reallydelaybindversion ;//= false;	// use REALLYDELAYBIND instead of DELAYBIND which not supported in gs 9.22
#endif
	OptionT < int, IntValueExtractor > pagetoextract ;//= 0;		// 0 stands for all pages
	OptionT < double, DoubleValueExtractor > flatness ;//= 1.0f;		// used for setflat
	OptionT < bool, BoolTrueExtractor > simulateClipping ;//= false;	// simulate clipping most useful in combination with -dt
	OptionT < bool, BoolTrueExtractor > usePlainStrings; //= false;
	OptionT < bool, BoolTrueExtractor > useFindDevice; //= false;
	OptionT < bool, BoolTrueExtractor > useRGBcolors ;//= false;
	OptionT < bool, BoolTrueExtractor > useAGL ;//= false;
	OptionT < bool, BoolTrueExtractor > noclip ;//= false;
	OptionT < bool, BoolTrueExtractor > t2fontsast1 ;//= false;	// handle T2 fonts (often come as embedded fonts in PDF files) same as T1
	OptionT < bool, BoolTrueExtractor > keepinternalfiles ;//= false;
	OptionT < bool, BoolTrueExtractor > fontdebug ;//= false;
	OptionT < bool, BoolTrueExtractor > justgstest ;//= false;
	OptionT < bool, BoolTrueExtractor > fake_date_and_version;//= false;
	OptionT < bool, BoolTrueExtractor > pscover ;//= false;
	OptionT < bool, BoolTrueExtractor > nofontreplacement ;//= false;
	OptionT < bool, BoolTrueExtractor > passglyphnames;
	OptionT < bool, BoolTrueExtractor > useoldnormalization;
	OptionT < int, IntValueExtractor > rotation ;//= 0;
	OptionT < RSString, RSStringValueExtractor> explicitFontMapFile ;//= 0;
	OptionT < RSString, RSStringValueExtractor > outputPageSize;//("");
	OptionT < bool, BoolTrueExtractor > fromgui;
	OptionT < bool, BoolTrueExtractor > showdialog;
	

//	OptionT < double, DoubleValueExtractor >  magnification ;//= 1.0f;
	OptionT < bool, BoolTrueExtractor > showdrvhelp ;//= false;
	OptionT < bool, BoolTrueExtractor > showdocu_long ;//= false;
	OptionT < bool, BoolTrueExtractor > showdocu_short ;//= false;
	OptionT < bool, BoolTrueExtractor > dumphelp ;//= false;
	OptionT < bool, BoolTrueExtractor > listdrivers ;//= false;

	OptionT < bool, BoolTrueExtractor > backendonly ;//= false;	// used for easier debugging of backends
	// directly read input file by backend
	// bypass Ghostscript. The input file
	// is assumed to be produced by a
	// previous call with -f debug

	OptionT < Argv, ArgvExtractor > psArgs;				// Pass through arguments to PostScript interpreter
	OptionT < int, IntValueExtractor > psLanguageLevel;
	OptionT < RSString, RSStringValueExtractor> drivername ;//= 0; // cannot be const char * because it is changed in pstoedit.cpp
	OptionT < RSString, RSStringValueExtractor> GSToUse;
	OptionT < RSString, RSStringValueExtractor > gsregbase;

	int verbose() const { return verboselevel() ? verboselevel() : verboseflag(); }
	
private:
	PsToEditOptions() :

	ProgramOptions(true), // expect additional parameters

	g_t(add_category("General options")),
	t_t(add_category("Text and font handling related options")),
	d_t(add_category("Drawing related options")),
	b_t(add_category("Diagnostic and debug options")),
	h_t(add_category("Hidden options")),

	nameOfInputFile(nullptr),
	nameOfOutputFile(nullptr),	// can contain %d for page splitting

	nameOfIncludeFile	(true, "-include","filename",g_t,"name of PostScript file to be included",
		"This option allows specifying an additional PostScript file that will be "
		"executed just before the normal input is read. This is helpful for "
		"including specific page settings or for disabling potentially unsafe "
		"PostScript operators, e.g., file, renamefile, or deletefile.",		
		emptyString),	//
	replacementfont		(true, "-df","font name",t_t,"default replacement font for raster fonts",
		" Sometimes fonts embedded in a PostScript "
		"program do not have a fontname. For example, this happens in PostScript "
		"files generated by \\Cmd{dvips}{1}. In such a case \\Prog{pstoedit} uses a "
		"replacement font. The default for this is Courier. Another font can be "
		"specified using the \\Opt{-df} option. \\Opt{-df Helvetica} causes all "
		"unnamed fonts to be replaced by Helvetica.",
		(const char *) "Courier"),
	nomaptoisolatin1	(true, "-nomaptoisolatin1",noArgument,t_t,"do not map to ISO Latin 1 encoding",
		"Normally \\Prog{pstoedit} maps all character "
		"codes to the ones defined by the ISO Latin1 encoding. If you specify "
		"\\Opt{-nomaptoisolatin1} then the encoding from the input PostScript is "
		"passed unchanged to the output. This may result in strange text output but "
		"on the other hand may be the only way to get some fonts converted appropriately. "
		"Try what fits best to your concrete case.",
		false),
	withdisplay			(true, "-dis",noArgument,b_t,"let Ghostscript display the file during conversion",
		"Open a display during processing by Ghostscript. Some files "
		"only work correctly this way.",
		false, true /* hide in GUI */),

	pngimage	        (true, "-pngimage","filename",t_t,
	                     "for debugging purpose mainly. Write result of processing also to a PNG file",
						 UseDefaultDoku,emptyString, true /* hide from GUI */),

	quiet				(true, "-q",noArgument,b_t,"quiet mode - do not write startup message",
		UseDefaultDoku,
		false,
		true /* hide in GUI */),
	noquit				(true, "-nq",noArgument,b_t,"do not quit Ghostscript after PostScript processing - for debugging only",
		"no exit from the PostScript interpreter. Normally Ghostscript "
		"exits after processing the pstoedit input-file. For debugging it can be "
		"useful to avoid this. If you do, you will have to type quit at the "
		"\\verb+GS>+ prompt to exit from Ghostscript.",
		false, true /* hide in GUI */),
	nocurves			(true, "-nc",noArgument,d_t,"normally curves are shown as curves if the output format does support it. This options forces curves to be always converted to line segments.",
		"no curves.\n"
		"Normally pstoedit tries to keep curves from the input and transfers them to "
		"the output if the output format supports curves. If the output format does not "
		"support curves, then pstoedit replaces curves by a series of lines (see "
		"also \\Opt{-flat} option). However, in some cases the user might wish to "
		"have this behavior also for output formats that originally support curves. This "
		"can be forced via the \\Opt{-nc} option.",
		false),
/*
	nosubpaths			(true, "-nsp",noArgument,d_t,"normally subpaths are used if the output format support them. This option turns off subpaths.",
		UseDefaultDoku,
		false),	
*/
	mergelines			(true, "-mergelines",noArgument,d_t,"merge adjacent paths if one is a stroke and the other is a fill. This depends on the capabilities of the selected output format",
		"Some output formats permit the representation of filled "
		"polygons with edges that are in a different color than the fill color. "
		"Since PostScript does not support this by the standard drawing primitives directly, drawing programs typically "
		"generate two objects (the outline and the filled polygon) into the "
		"PostScript output. \\Prog{pstoedit} is able to recombine these, if they "
		"follow each other directly and you specify \\Opt{-mergelines}. "
		"However, this merging is not supported by all output formats due to restrictions in the target format.",
		false),
	convertFilledRectToStroke(true,"-filledrecttostroke",noArgument,d_t,"convert filled rectangles to a stroke",
		"Rectangles filled with a solid color can be converted to a stroked line with a width that corresponds to "
		"the width of the rectangle. This is of primary interest for output formats which do not "
		"support filled polygons at all. But it is restricted to rectangles only, i.e. it is not "
		"supported for general polygons",false),
	mergetext			(true, "-mergetext",noArgument,d_t,"merge adjacent text pieces",
		"In order to produce nice looking text output, programs producing PostScript files "
		"often split words into smaller pieces which are then placed individually on adjacent positions. "
		"However, such split text is hard to edit later on and hence it is sometime better "
		"to recombine these pieces again to form a word (or even sequence of words). "
		"For this pstoedit implements some heuristics about what text pieces are to be "
		"considered parts of a split word. This is based on the geometrical proximity of the different "
		"parts and seems to work quite well so far. But there are certainly cases "
		"where this simple heuristic fails. So please check the results carefully.",
		false),
	drawtext			(true, "-dt",noArgument,t_t,"draw text, i.e. convert text to polygons",
		"draw text. Text is drawn as polygons. This might produce a large output file. This option is automatically "
		"switched on if the selected output format does not support text, e.g. "
		"\\Cmd{gnuplot}{1}.",
		false),
	autodrawtext		(true, "-adt",noArgument,t_t,"automatic draw text. This draws text only for text that uses fonts with non standard encodings",
	"automatic draw text. This option turns on the \\Opt{-dt} option selectively for fonts that seem to be no normal text fonts, e.g. Symbol.",
		false),
	disabledrawtext		(true, "-ndt",noArgument,t_t,"fully disable any \"intelligence\" for drawing text",
		"never draw text. Fully disable the heuristics used by pstoedit to decide when to \"draw\" text "
		"instead of showing it as text. This may produce incorrect results, but in some cases it might nevertheless be useful. \"Use at own risk\".",
		false),

	DrawGlyphBitmaps		(true, "-dgbm",noArgument,t_t,"experimental - draw also bitmaps generated by fonts/glyphs",
		UseDefaultDoku,
		false,
		true /* hide in GUI */),

	correctdefinefont	(true, "-correctdefinefont",noArgument,t_t,"apply some \"corrective\" actions to definefont - use this for ChemDraw generated PostScript files",
		"Some PostScript files, e.g. such as generated by ChemDraw, "
		"use the PostScript definefont operator in a way that is incompatible with "
		"pstoedit's assumptions. The new font is defined by copying an old font "
		"without changing the FontName of the new font. When this option is applied, "
		"some \"patches\" are done after a definefont in order to make it again "
		"compatible with pstoedit's assumptions. This option is not enabled by default, "
		"since it may break other PostScript files. It is tested only with ChemDraw generated files.",
		false),

	ptioption			(true, "-pti",noArgument,t_t,"precision text - individual. Places text character by character - but only if non standard chararater widths are used",
		"precision text. "
     	"Normally a text string is drawn as it occurs in the input file. However, in some situations, this might "
		"produce wrongly positioned characters. This is due to limitations in most output formats of "
		"pstoedit. They cannot represent text with arbitrary inter-letter spacing which is easily "
		"possible in PDF and PostScript. "
		"With \\Opt{-pta}, each character of a text string is placed "
		"separately. With \\Opt{-pti}, this is done only in cases when there is a non zero inter-letter spacing. "
		"The downside of \"precision text\" is a bigger file size and hard to edit "
		"text.",
		false),
	ptaoption			(true, "-pta",noArgument,t_t,"precision text - always. Places text character by character",
		"see -pti",
		false),
	precisiontext (0), //= 0; // derived from the above pti ->1 pta -> 2 sonst 0

	xscale				(true, "-xscale","number",g_t,"scale by a factor in x-direction",
		UseDefaultDoku,
		1.0),
	yscale				(true, "-yscale","number",g_t,"scale by a factor in y-direction",
		UseDefaultDoku,
		1.0),

	xshift				(true, "-xshift","number",g_t,"shift image in x-direction",
		UseDefaultDoku,
		0.0f),
	yshift				(true, "-yshift","number",g_t,"shift image in y-direction",
		UseDefaultDoku,
		0.0f),

	centered		(true,"-centered", noArgument ,g_t,"center image before scaling or shifting",
		UseDefaultDoku,
		false),

	minlinewidth		(true, "-minlinewidth","number",g_t,"minimal line width. All lines thinner than this will be drawn in this line width - especially zero-width lines",
		UseDefaultDoku,
		0.0f),

	pagenumberformat		(true, "-pagenumberformat","page number format specification",g_t,"format specification for page numbers in filename if -split is used. "
		"The specification is used to create the page number using sprintf. "
		"The specification shall not include the leading \\% nor the trailing d. "
		"Default is empty string which results in formatting the page number using \\%d. "
		"This results in page numbers like 1, 2, ..., 10. "
		"Sometimes you may want to have fixed length with leading 0, "
		"so you might want to specify 02 which means 2 digits with leading 0.",
		UseDefaultDoku,
		emptyString),
		
	splitpages			(true, "-split",noArgument,g_t,"split multipage documents into single pages",
		"Create a new file for each page of the input. For this the "
		"output filename must contain a \\%d which is replaced with the current page "
		"number. This option is automatically switched on for output formats that do not "
		"support multiple pages within one file, e.g. fig or gnuplot.",
		false),
	verboseflag			(true, "-v",noArgument,b_t,"turns on verbose mode",
		"Switch on verbose mode. Some additional information is shown "
		"during processing.",
		false),
	verboselevel(true, "-vl", noArgument, b_t, "turns on verbose mode with a given verbosity level",
		"Switch on verbose mode with a given level. Some additional information is shown "
		"during processing.",
		0),
	useBBfrominput		(true, "-usebbfrominput",noArgument,g_t,"extract BoundingBox from input file rather than determining it during processing",
		"If specified, pstoedit uses the BoundingBox as is (hopefully) found in the input file instead of one that is calculated by its own.",
		false),
	simulateSubPaths	(true, "-ssp",noArgument,d_t,"simulate subpaths",
		"simulate subpaths.\n"
		"Several output formats do not support PostScript paths containing subpaths, i.e. "
		"paths with intermediate movetos. In the normal case, each subpath is "
		"treated as an independent path for such output formats. This can lead to bad "
		"looking results. The most common case where this happens is if you use the "
		"\\Opt{-dt} option and show some text with letters like e, o, or b, i.e. "
		"letters that have a \"hole\". When the \\Opt{-ssp} option is set, pstoedit "
		"tries to eliminate these problems. However, this option is CPU time "
		"intensive!",
		false),
	simulateFill	(true, "-sfill",noArgument,d_t,"simulate fill",
		"simulate filling by individual strokes.\n"
		"",
		false),
	unmappablecharstring(true, "-uchar","character",t_t,"when a character cannot be mapped to one of the standard encoding vector, use this as replacement",
		"Sometimes pstoedit cannot map a character "
		"from the encoding used by the PostScript file to the font encoding of the target "
		"format. In this case pstoedit replaces the input character by a special character "
		"in order to show all the places that could not be mapped correctly. The default "
		"for this is a \"\\#\". Using the \\Opt{-uchar} option it is possible to specify another character "
		"to be used instead. If you want to use a space, use -uchar \" \".",
		'#'),
	dontloadplugins		(true, "-dontloadplugins",noArgument,h_t,"internal option - not relevant for normal user",
		UseDefaultDoku,
		false,
		true /* hide in GUI */),
	nobindversion		(true, "-nb",noArgument,b_t,"use old NOBIND instead of DELAYBIND - try this if Ghostscript has problems",
		"Since version 3.10 \\Prog{pstoedit} uses the "
		"\\texttt{-dDELAYBIND} option when calling Ghostscript. Previously the "
		"\\texttt{-dNOBIND} option was used instead but that sometimes caused "
		"problems if a user's PostScript file overloaded standard PostScript "
		"operator with totally new semantic, e.g. lt for lineto instead of the standard meaning of \"less than\". Using \\Opt{-nb} the "
		"old style can be activated again in case the \\texttt{-dDELAYBIND} gives "
		"different results as before. In such a case please also contact the "
		"author.",
		false),	//
#if WITHRDB
	reallydelaybindversion	(true, "-rdb",noArgument,b_t,"use REALLYDELAYBIND option for gs, instead of the former DELAYBIND which is not supported anymore since gs 9.2x",
		"Since version 3.10 \\Prog{pstoedit} uses the "
		"\\texttt{-dDELAYBIND} option when calling Ghostscript. But in version 9.22 of "
		"Ghostscript, that option is not supported anymore because of security reasons. " 
		"As a fallback, that version provides the REALLYDELAYBIND option "
		"and pstoedit can use this if you supply the \\Opt{-rdb} option. "
		"Use this with caution as it might open security risks, e.g. a PostScript file "
		"injecting some malicious code into PostScript standard operators. " 
		"However, not using this option can cause some of the PostScript drawings operations "
		"to be not seen by pstoedit, hence causing missing artefacts in the output. "
		"Later versions of Ghostscript will probably support -dDELAYBIND again. "
		"But also in that case the security risk remains. So be careful with what "
		"files you process with pstoedit and Ghostscript.",
		false),	//
#endif
	pagetoextract		(true, "-page","page number",g_t,"extract a specific page: 0 means all pages",
		"Select a single page from a multi-page PostScript or PDF file.",
		0),		// 0 stands for all pages
	flatness			(true, "-flat","flatness factor",d_t,"the precision used for approximating curves by lines if needed",
		"If the output format does not support curves in the way "
		"PostScript does or if the \\Opt{-nc} option is specified, all curves are "
		"approximated by lines. Using the \\Opt{-flat} option one can control this "
		"approximation. This parameter is directly converted to a PostScript "
		"\\textbf{setflat} command. Higher numbers, e.g. 10 give rougher, lower "
		"numbers, e.g. 0.1, give finer approximations.",
		1.0),		// used for setflat
	simulateClipping	(true, "-sclip",noArgument,d_t,"simulate clipping - probably you need to set this if you use -dt",
		"simulate clipping.\n"
		"Most output formats of pstoedit do not have native support for clipping. For that "
		"\\Prog{pstoedit} offers an option to perform the clipping of the graphics "
		"directly without passing the clippath to the output driver. However, this "
		"results in curves being replaced by a lot of line segments and thus larger "
		"output files. So use this option only if your output looks different from "
		"the input due to clipping. In addition, this \"simulated clipping\" is not "
		"exactly the same as defined in PostScript. There might be lines drawn at "
		"double size. Also clipping of text is not supported unless you also use "
		"the \\Opt{-dt} option.",
		false),	// simulate clipping
	usePlainStrings		(true, "-ups",noArgument,b_t,"write text as plain string instead of hex string in intermediate format - normally useful for trouble shooting and debugging only.",
		UseDefaultDoku,
		false),
	useFindDevice(true, "-usefinddevice", noArgument, b_t, "use the ghostscript internal finddevice operator during image handling instead of standard operators - normally useful for trouble shooting and debugging only.",
		UseDefaultDoku,
		false),
	useRGBcolors		(true, "-rgb",noArgument,g_t,"use RGB colors instead of CMYK",
		"Since version 3.30 pstoedit uses the CMYK colors internally. The -rgb option turns on the old behavior to use RGB values.",
		false),
	useAGL		(true, "-useagl",noArgument,g_t,"use Adobe Glyph List instead of the ISO Latin-1 table (this is experimental)",
		UseDefaultDoku,
		false),
	noclip				(true, "-noclip",noArgument,g_t,"do not use clipping (relevant only if output format supports clipping at all)",
		UseDefaultDoku,
		false),
	t2fontsast1			(true, "-t2fontsast1",noArgument,t_t,"handle T2 fonts (often come as embedded fonts in PDF files) same as T1",
		"Handle Type 2 fonts same as Type 1. Type 2 fonts sometimes occur as "
		"embedded fonts within PDF files. In the default mode, text using such fonts is drawn as polygons "
		"since pstoedit assumes that such a font is not available on the user's machine. If this option "
		"is set, pstoedit assumes that the internal encoding follows the same as for a standard font "
		"and generates normal text output. This assumption may not be true in all cases. But it "
		"is nearly impossible for pstoedit to verify this assumption - it would have to do a sort of OCR.",
		false),	// handle T2 fonts (often come as embedded fonts in PDF files) same as T1
	keepinternalfiles	(true, "-keep",noArgument,b_t,"keep the intermediate files produced by pstoedit - for debug purposes only",
		UseDefaultDoku,
		false),
	fontdebug	(true, "-debugfonthandling",noArgument,b_t,"writes verbose messages related to internal font processing - for debug purposes only",
		UseDefaultDoku,
		false),
	justgstest			(true, "-gstest",noArgument,b_t,"perform a basic test of the interworking with Ghostscript",
		UseDefaultDoku,
		false),
	fake_date_and_version(true, "-fakedateandversion", noArgument, b_t, "Just for regression testing - uses a constant date and version string.",
		UseDefaultDoku,
		false,
		true /* hide in GUI */),
	pscover				(true, "-pscover",noArgument,h_t,"perform coverage statistics about the pstoedit PostScript proloque - for debug and test only",
		UseDefaultDoku,
		false,
		true /* hide in GUI */),
	nofontreplacement	(true, "-nfr",noArgument,t_t,"do not replace non standard encoded fonts with a replacement font",
		"In normal mode pstoedit replaces bitmap fonts with a font as defined by the \\Opt{-df} option. This is done, because most output formats cannot handle such fonts. This behavior can be "
		"switched off using the \\Opt{-nfr} option but then it strongly depends on the application reading the generated file whether the file is usable and correctly interpreted or not. Any problems are then out of control of pstoedit.",
		false),
	passglyphnames		(true, "-glyphs",noArgument,t_t,"pass glyph names to output format driver",
		"pass glyph names to the output format driver. So far no output format driver really uses the glyph names, so this does not have any effect at the moment. "
		"It is a preparation for future work.",
		false),
	useoldnormalization		(true, "-useoldnormalization",noArgument,t_t,"use legacy (pre 3.50) method for normalizing font encodings",
			"Just use this option in case the new heuristic introduced in 3.5 does not produce correct results - however, this normalization of font encoding will always be a best-effort approach since there is no real general solution to it with reasonable effort",
		false),
	rotation			(true, "-rotate","angle (0-360)",g_t,"rotate the image",
		"Rotate image by angle.",
		0),
#define LINEBREAK "\\\\"
	explicitFontMapFile	(true, "-fontmap","name of font map file for pstoedit",t_t,"use a font mapping from a file",
		"The font map is a "
		"simple text file containing lines in the following format:" LINEBREAK "\n"
		"\n\n"
		"\\verb+document_font_name    target_font_name+" LINEBREAK "\n"
		"Lines beginning with \\verb+%+ are considered comments." LINEBREAK "\n"
		"For font names with spaces use the "
		"\\verb+\"font name with spaces\"+ notation.\n"
		"\n"
		"If a target\\_font\\_name starts with /, it is regarded as alias to a former entry.\n "
		"\n"
		"Each font name found in the document is checked against this mapping and if "
		"there is a corresponding entry, the new name is used for the output. "
		"\n\n"
		"If  the \\Opt{-fontmap} option is not specified, \\Prog{pstoedit} "
		"automatically looks for the file \\emph{drivername}.fmp in the installation "
		"directory and uses that file as a default fontmap file if available. The "
		"installation directory is:"
		"\n\n"
		"\\begin{itemize}\n"
		"  \\item MS Windows: The same directory where the \\Prog{pstoedit} executable is located\n"
		"  \\item Unix:" LINEBREAK "\n"
		"  The default installation directory. If it fails, then $<$\\emph{The directory where the pstoedit executable is located}$>$\\verb+/../lib/+\n"
		"\\end{itemize}\n"
		"\n"
		"The mpost.fmp in the misc directory of the pstoedit distribution is a sample "
		"map file with mappings from over 5000 PostScript font names to their \\TeX\\ "
		"equivalents. This is useful because MetaPost is frequently used with "
		"\\TeX/\\LaTeX\\ and those programs do not use standard font names. This file and "
		"the MetaPost output format driver are provided by Scott Pakin "
		"(\\Email{scott+ps2ed\\_AT\\_pakin.org})."
		"\n"
		"Another example is wemf.fmp to be used under Windows. See the misc "
		"directory of the pstoedit source distribution."
		"\n"
		"After loading the implicit (based on driver name) or explicit (based on the -fontmap option) font map file, a system specific "
		"map file is searched and loaded from the installation directory (unix.fmp or windows.fmp). "
		"This file can be used to redirect certain fonts to system specific names using the /AliasName notation described above."
		,
		emptyString),
	outputPageSize		(true, "-pagesize","page format",g_t,"set page size (e.g. a4) - used by TK and libplot output format driver only",
		"set page size for output medium.\n"
		"This option sets the page size for the output medium. Currently this "
		"is just used by the libplot output format driver, but might be used by other "
		"output format drivers in future. The page size is specified in terms of the usual "
		"page size names, e.g. letter or a4.",
		emptyString),
	fromgui				(true, "-fromgui",noArgument,h_t,"internal - not for normal user",
		UseDefaultDoku,
		false,
		true /* hide in GUI */),
	showdialog			(true, "-showdialog",noArgument,h_t,"internal - not for normal user",
		UseDefaultDoku,
		false,
		true /* hide in GUI */),
	showdrvhelp			(true, "-help",noArgument,g_t,"show the help information",
		UseDefaultDoku,
		false, true /* hide in GUI */),
	showdocu_long			(true, "-doculong",noArgument,h_t,"show help information in TeX format - long version",
		UseDefaultDoku,
		false) ,
	showdocu_short			(true, "-docushort",noArgument,h_t,"show help information in TeX format - short version",
		UseDefaultDoku,
		false) ,
	dumphelp			(true, "-dumphelp",noArgument,h_t,"show all options of all drivers in TeX format",
		UseDefaultDoku,
		false),
	listdrivers			(true, "-listdrivers",noArgument,h_t,"list all available drivers",
		UseDefaultDoku,
		false),
		
	backendonly			(true, "-bo",noArgument,g_t,"backend only - This option is not useful for a \"normal\" user. "
													"It is useful for programs which use pstoedit as output format generator "
													"and can provide an input file which adheres to pstoedit's internal dump format.",
		"You can run backend processing only (without the PostScript "
		"interpreter frontend) by first running \\textbf{pstoedit} \\Opt{-f dump} "
		"\\Arg{infile} \\Arg{dumpfile} and then running \\textbf{pstoedit} "
		"\\OptArg{-f}{~format}  \\Opt{-bo} \\Arg{dumpfile} \\Arg{outfile}.",
		false,
		true /* hide in GUI */),	// used for easier debugging of backends
	psArgs				(true, "-psarg","argument string",g_t,"additional arguments to be passed to Ghostscript directly",
		"The string given with this option is passed "
		"directly to Ghostscript when Ghostscript is called to process the "
		"PostScript file for \\Prog{pstoedit}. For example:      "
		"\\textbf{-psarg}\\textbf{~\"}\\textbf{-r300x300}\\textbf{\"}. "
		"This causes the resolution to be changed to "
		"300x300 dpi. (With older versions of Ghostscript, changing the resolution "
		"this way has an effect only if the \\Opt{-dis} option is given.) "
		"If you want to pass multiple options to Ghostscript you can use multiple "
		"-psarg options \\Opt{-psarg opt1} \\Opt{-psarg opt2} \\Opt{-psarg opt2}. "
		"See the Ghostscript manual for other possible options.",
		OptionBase::ctorToUseForValue::useDefaultCtor
		),

	psLanguageLevel		(true, "-pslanguagelevel","PostScript Language Level 1, 2, or 3 to be used.", g_t,
		"PostScript Language Level 1, 2, or 3 to be used. "
		"You can switch Ghostscript into PostScript Level 1 only mode by "
		"\\Opt{-pslanguagelevel 1}. This can be useful for example if the PostScript file to be "
		"converted uses some Level 2 specific custom color models that are not supported "
		"by pstoedit. However, this requires that the PostScript program checks for the "
		"PostScript level supported by the interpreter and \"acts\" accordingly. "
		"The default language level is 3.",
		UseDefaultDoku,
		3),

	drivername			(false,"-f","\"format[:options]\"",g_t,"target format identifier",
		"target output format recognized by "
		"\\Prog{pstoedit}. Since other format drivers can be loaded dynamically, "
		"type  \\texttt{pstoedit -help} to get a full list of formats. See "
		"\"Available formats and their specific options\" "
		"below for an explanation of the \\oArg{:options} to "
		"\\Opt{-f} format. If the format option is not given, pstoedit tries to guess the target format "
		" from the suffix of the output filename. However, in a lot of cases, this is not a unique "
		"mapping and hence pstoedit demands the \\Opt{-f} option.",
		emptyString),
	GSToUse(true, "-gs", "either full path to the Ghostscript executable/DLL or - for Windows - just a version number (e.g. 10.01.0), "
		"in which case the version is used to look up the path from the registry.", g_t, 
		"tells pstoedit which Ghostscript executable/DLL to use - overwrites the internal search heuristic",
				UseDefaultDoku, emptyString),
	gsregbase (true, "-gsregbase", "Ghostscript base registry path", g_t,
	  "use this registry key as a subkey to search for Ghostscript",
	  "registry path to use as a base path when searching Ghostscript interpreter.\n"
	  "This option provides means to specify a registry key under "
	  "HKLM/Software where to search for GS interpreter key, version "
	  "and \\verb+GS_DLL / GS_LIB+ values. Example: \"-gsregbase MyCompany\" means "
	  "that HKLM/Software/MyCompany/GPL Ghostscript would be searched "
	  "instead of HKLM/Software/GPL Ghostscript.",
	  emptyString,
	  true /* hide in GUI */)
	{

#define MAKESTRING(x) #x
#define ADD(x) add(&x,MAKESTRING(x))

	ADD(nameOfIncludeFile);
	ADD(replacementfont);
	ADD(nomaptoisolatin1);
	ADD(withdisplay);
	ADD(pngimage);
	ADD(quiet);
	ADD(noquit);
	ADD(nocurves);
//	ADD(nosubpaths);	
	ADD(mergelines);
	ADD(convertFilledRectToStroke);
	ADD(mergetext);
	ADD(drawtext);
	ADD(autodrawtext);
	ADD(disabledrawtext);
	ADD(DrawGlyphBitmaps);
	ADD(correctdefinefont);

	ADD(ptioption);
	ADD(ptaoption);
	// ADD(precisiontext (0); //= 0; // derived from the above pti ->1 pta -> 2 sonst 0

	ADD(xscale);
	ADD(yscale);
	ADD(xshift);
	ADD(yshift);
	ADD(centered);
	ADD(minlinewidth);

	ADD(pagenumberformat);
	ADD(splitpages);
	ADD(verboseflag);
	ADD(verboselevel);
	ADD(useBBfrominput);
	ADD(simulateSubPaths);
	ADD(simulateFill);
	ADD(unmappablecharstring);
	ADD(dontloadplugins);
	ADD(nobindversion );
#if WITHRDB
	ADD(reallydelaybindversion );
#endif
	ADD(pagetoextract);	
	ADD(flatness);		
	ADD(simulateClipping);	
	ADD(usePlainStrings);	
	ADD(useFindDevice);
	ADD(useRGBcolors);
	ADD(useAGL);
	ADD(noclip);
	ADD(t2fontsast1);	
	ADD(keepinternalfiles);
	ADD(fontdebug);
	ADD(justgstest);
	ADD(pscover);
	ADD(fake_date_and_version);
	ADD(nofontreplacement);
	ADD(passglyphnames);
	ADD(useoldnormalization);
	ADD(rotation );
	ADD(explicitFontMapFile);
	ADD(outputPageSize);
	ADD(fromgui);
//	ADD(magnification);
	ADD(showdrvhelp) ;
	ADD(showdocu_long) ;
	ADD(showdocu_short) ;

	ADD(dumphelp);
	ADD(listdrivers);
	ADD(backendonly);	
	ADD(psArgs);	
	ADD(psLanguageLevel);

	ADD(drivername);
	ADD(GSToUse);
	ADD(gsregbase);
}

	~PsToEditOptions() {
		// delete drivername.value; // this crashes under Windows (heap problem) //lint !e605
		delete [] nameOfInputFile ;
		delete [] nameOfOutputFile  ;	
	}
	virtual bool hideSheetFromGui(unsigned int sheet) const { return (sheet == h_t); }
	virtual bool hideFromDoku(const OptionBase& opt) const { return (opt.propsheet == h_t); }
	NOCOPYANDASSIGN(PsToEditOptions)
};

#endif
