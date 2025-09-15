/* 
   drvtk.cpp - Driver to output Tcl/Tk canvas
             - written by Christopher Jay Cox (cjcox_AT_acm.org) - 9/22/97
               last updated: 8/09/00
               http://www.ntlug.org/~ccox/impress/
               Based on... 

   drvsample.cpp : Backend for TK

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

#include "drvtk.h"
#include I_iostream
//#include I_stdio	
#include I_string_h

// for sin and cos
#include <math.h>


#include "papersizes.h"


static const char *colorstring(float r, float g, float b)
{
	static char buffer[10];
	sprintf_s(TARGETWITHLEN(buffer,10), "%s%.2x%.2x%.2x", "#", (int) (r * 255), (int) (g * 255), (int) (b * 255));
	return buffer;
}


void drvTK::outputEscapedText(const char *str)
{
	const char *i;

	for (i = str; *i; i++) {
		switch (*i) {
		case '{':
		case '}':
		case '"':
		case '$':
		case '[':
		case ']':
		case '\\':
			buffer << '\\';
			break;
		default:
			break;
		}
		buffer << *i;
	}
}


drvTK::derivedConstructor(drvTK):
constructBase, buffer(tempFile.asOutput()) /*, objectId(1) */ ,paperinfo(nullptr)
{
//  const RSString & l_pagesize = getPageSize();

	// set tk specific values
	x_offset = 0.0;				/* set to fit to tk page      */
	y_offset = 0.0;				/*          "                 */

    const RSString& pagesize = getPageSize();
	paperinfo = getPaperInfo(pagesize.c_str());
	// cout << "Paper Info for " << pagesize << " is " << (void*) paperinfo << endl;
	if (!paperinfo) { paperinfo = getPaperInfo("Letter"); } //default 

	canvasCreate();
}

void drvTK::canvasCreate()
{
	if (!options->noImPress) {
		outf << "#!/bin/sh" << endl
			<< "# restart trick \\" << endl
			<< "exec wish \"$0\" \"$@\"" << endl
			<< "if {![info exists Global(CurrentCanvas)]} {" << endl
			<< "	proc originObject { name } {" << endl
			<< "		global Global" << endl
			<< "		set bb [$Global(CurrentCanvas) bbox $name]" <<
			endl << "		set x [lindex $bb 0]" << endl <<
			"		set y [lindex $bb 1]" << endl <<
			"		if {$x > 0} {" << endl <<
			"			set nx -$x" << endl <<
			"		} else {" << endl <<
			"			set nx [expr abs($x)]"
			<< endl << "		}" <<
			endl <<
			"		if {$y > 0} {"
			<< endl <<
			"			set ny -$y"
			<< endl <<
			"		} else {"
			<<
			endl
			<<
			"			set ny [expr abs($y)]"
			<<
			endl
			<<
			"		}"
			<<
			endl
			<<
			"		$Global(CurrentCanvas) move $name $nx $ny"
			<<
			endl
			<<
			"	}"
			<<
			endl
			<<
			"	proc scaleObject {name ratio width height} {"
			<<
			endl
			<<
			"		global Global"
			<<
			endl
			<<
			"		set bb [eval $Global(CurrentCanvas) bbox $name]"
			<<
			endl
			<<
			"		if {\"$bb\" != \"\"} {"
			<<
			endl
			<<
			"			set x [lindex $bb 2]"
			<<
			endl
			<<
			"			set y [lindex $bb 3]"
			<<
			endl
			<<
			"			if {$ratio == \"variable\"} {"
			<<
			endl
			<<
			"				set scalex [expr ($width + 0.0) / $x]"
			<<
			endl
			<<
			"				set scaley [expr ($height + 0.0) / $y]"
			<<
			endl
			<<
			"				if {$scalex > $scaley} {"
			<<
			endl
			<<
			"					set scale $scaley"
			<<
			endl
			<<
			"				} else {"
			<<
			endl
			<<
			"					set scale $scalex"
			<<
			endl
			<<
			"				}"
			<<
			endl
			<<
			"			} elseif {$ratio != \"\"} {"
			<<
			endl
			<<
			"				set scalex $ratio"
			<<
			endl
			<<
			"				set scaley $ratio"
			<<
			endl
			<<
			"				set scale $ratio"
			<<
			endl
			<<
			"			} else {"
			<<
			endl
			<<
			"				set scalex [expr ($width + 0.0) / $x]"
			<<
			endl
			<<
			"				set scaley [expr ($height + 0.0) / $y]"
			<<
			endl
			<<
			"				if {$scalex > $scaley} {"
			<<
			endl
			<<
			"					set scalex $scaley"
			<<
			endl
			<<
			"					set scale $scaley"
			<<
			endl
			<<
			"				} else {"
			<<
			endl
			<<
			"					set scaley $scalex"
			<<
			endl
			<<
			"					set scale $scalex"
			<<
			endl
			<<
			"				}"
			<<
			endl
			<<
			"			}"
			<<
			endl
			<<
			"			$Global(CurrentCanvas) scale $name 0 0 $scalex $scaley"
			<<
			endl
			<<
			"			foreach i [$Global(CurrentCanvas) find withtag $name] {"
			<<
			endl
			<<
			"				set type [$Global(CurrentCanvas) type $i]"
			<<
			endl
			<<
			"				if {\"$type\" == \"text\"} {"
			<<
			endl
			<<
			"					if {$Global(FontScale)} {"
			<<
			endl
			<<
			"						set fn [$Global(CurrentCanvas) itemcget $i -font]"
			<<
			endl
			<<
			"						regexp \\"
			<<
			endl
			<<
			"						{([-][^-]*-[^-]*-[^-]*-[^-]*-[^-]*-[^-]*-[^-]*-)([^-]*)(-.*)}\\"
			<<
			endl
			<<
			"							$fn dummy d1 size d2;"
			<<
			endl
			<<
			"						if {\"$dummy\" != \"\"} {"
			<<
			endl
			<<
			"							set nsize [expr round($size * $scale)]"
			<<
			endl
			<<
			"							if {$nsize < 20} {"
			<<
			endl
			<<
			"								set nsize 20"
			<<
			endl
			<<
			"							}"
			<<
			endl
			<<
			"							$Global(CurrentCanvas) itemconfigure $i \\"
			<<
			endl
			<<
			"								-font ${d1}${nsize}${d2}"
			<<
			endl
			<<
			"							set fnn [$Global(CurrentCanvas) itemcget $i -font]"
			<<
			endl
			<<
			"							regexp \\"
			<<
			endl
			<<
			"								{([-][^-]*-[^-]*-[^-]*-[^-]*-[^-]*-[^-]*-[^-]*-)([^-]*)(-.*)}\\"
			<<
			endl
			<<
			"								$fnn dummy d1 nsize d2;"
			<<
			endl
			<<
			"							if { ($scale < 1 && $nsize < $size ) || \\"
			<<
			endl
			<<
			"							 	($scale > 1 && $nsize > $size) } {"
			<<
			endl
			<<
			"								$Global(CurrentCanvas) itemconfigure $i \\"
			<<
			endl
			<<
			"									-width [expr [$Global(CurrentCanvas) itemcget $i \\"
			<<
			endl
			<<
			"									-width] * $scale]"
			<<
			endl
			<<
			"							} else {"
			<<
			endl
			<<
			"								$Global(CurrentCanvas) itemconfigure $i \\"
			<<
			endl
			<<
			"									-font $fn"
			<<
			endl
			<<
			"							}"
			<<
			endl
			<<
			"						}"
			<<
			endl
			<<
			"					}"
			<<
			endl
			<<
			"				} else {"
			<<
			endl
			<<
			"					catch {$Global(CurrentCanvas) itemconfigure $i \\"
			<<
			endl
			<<
			"						-width [expr [$Global(CurrentCanvas) itemcget $i -width] \\"
			<<
			endl
			<<
			"							* $scale]}"
			<<
			endl
			<<
			"				}"
			<<
			endl
			<<
			"			}"
			<<
			endl
			<<
			"			return $scale"
			<<
			endl
			<<
			"		}"
			<<
			endl
			<<
			"		return 1.0"
			<<
			endl
			<<
			"	}"
			<<
			endl
			<<
			"	proc nextPage {} {"
			<<
			endl
			<<
			"		global Global"
			<<
			endl
			<<
			"		set Global(CurrentPageId) [expr $Global(CurrentPageId) + 1]"
			<<
			endl
			<<
			"		if {![winfo exists .can.c$Global(CurrentPageId)]} {"
			<<
			endl
			<<
			"			if {$Global(Circulate)} {"
			<<
			endl
			<<
			"				set Global(CurrentPageId) 1 "
			<<
			endl
			<<
			"			} else {"
			<<
			endl
			<<
			"				set Global(CurrentPageId) [expr $Global(CurrentPageId) - 1]"
			<<
			endl
			<<
			"			}"
			<<
			endl
			<<
			"		}"
			<<
			endl
			<<
			"		newCanvas .can c$Global(CurrentPageId)"
			<<
			endl
			<<
			"		update idletasks"
			<<
			endl
			<<
			"	}"
			<<
			endl
			<<
			"	proc prevPage {} {"
			<<
			endl
			<<
			"		global Global"
			<<
			endl
			<<
			"	"
			<<
			endl
			<<
			"		if {$Global(CurrentPageId) > 1} {"
			<<
			endl
			<<
			"			set Global(CurrentPageId) [expr $Global(CurrentPageId) - 1]"
			<<
			endl
			<<
			"			newCanvas .can c$Global(CurrentPageId)"
			<<
			endl
			<<
			"			update idletasks"
			<<
			endl
			<<
			"		}"
			<<
			endl
			<<
			"	}"
			<<
			endl
			<<
			"	proc newCanvas { cf name } {"
			<<
			endl
			<<
			"		global Global"
			<<
			endl
			<<
			"		if {[winfo exists $Global(CurrentCanvas)]} {"
			<<
			endl
			<<
			"			pack forget $Global(CurrentCanvas)"
			<<
			endl
			<<
			"		}"
			<<
			endl
			<<
			"		.can.ezoom configure -textvariable Global(ViewScale_$Global(CurrentPageId))"
			<<
			endl
			<<
			"		.can.lzoom configure -command {set Global(LockScale) $Global(ViewScale_$Global(CurrentPageId));newCanvas .can c$Global(CurrentPageId)}"
			<<
			endl
			<<
			"		set Global(CurrentCanvas) $cf.$name"
			<<
			endl
			<<
			"		if {[winfo exists $Global(CurrentCanvas)]} {"
			<<
			endl
			<<
			"			pack $Global(CurrentCanvas) -in $cf.top -side left -fill both \\"
			<<
			endl
			<<
			"				-expand true"
			<<
			endl
			<<
			"			update idletasks"
			<<
			endl
			<<
			"			if {$Global(Fit) && !$Global(LockZoom)} {"
			<<
			endl
			<<
			"				set newzoom [scaleObject all \"\" \\"
			<<
			endl
			<<
			"				[winfo width $Global(CurrentCanvas)] \\"
			<<
			endl
			<<
			"				[winfo height $Global(CurrentCanvas)]]"
			<<
			endl
			<<
			"				set Global(ViewScale_$Global(CurrentPageId)) \\"
			<<
			endl
			<<
			"					[expr double($newzoom) * \\"
			<<
			endl
			<<
			"					$Global(LastScale_$Global(CurrentPageId))]"
			<<
			endl
			<<
			"			} else {"
			<<
			endl
			<<
			"				if {$Global(LockZoom)} {"
			<<
			endl
			<<
			"					set Global(ViewScale_$Global(CurrentPageId)) \\"
			<<
			endl
			<<
			"						$Global(LockScale)"
			<<
			endl
			<<
			"				}"
			<<
			endl
			<<
			"				scaleObject all \\"
			<<
			endl
			<<
			"					[expr double($Global(ViewScale_$Global(CurrentPageId))) / \\"
			<<
			endl
			<<
			"					double($Global(LastScale_$Global(CurrentPageId)))] {} {}"
			<<
			endl
			<<
			"			}"
			<<
			endl
			<<
			"			set Global(LastScale_$Global(CurrentPageId)) \\"
			<<
			endl
			<<
			"				$Global(ViewScale_$Global(CurrentPageId))"
			<<
			endl
			<<
			"		} else {"
			<<
			endl
			<<
			"			set Global(LastScale_$Global(CurrentPageId)) 100.0"
			<<
			endl
			<<
			"			set Global(ViewScale_$Global(CurrentPageId)) 100.0"
			<<
			endl
			<<
			"			if {$Global(LockZoom)} {"
			<<
			endl
			<<
			"				set Global(ViewScale_$Global(CurrentPageId)) \\"
			<<
			endl
			<<
			"					$Global(LockScale)"
			<<
			endl
			<<
			"			}"
			<<
			endl
			<<
			"			canvas $Global(CurrentCanvas) -bg $Global(SlideBG) \\"
			<<
			endl
			<<
			"				-scrollregion \"0i 0i 50i 50i\" \\"
			<<
			endl
			<<
			"				-xscrollcommand \"$cf.scrollx set\" \\"
			<<
			endl
			<<
			"				-yscrollcommand \"$cf.scrolly set\""
			<<
			endl
			<<
			"			bind $Global(CurrentCanvas) <2> {%W scan mark %x %y}"
			<<
			endl
			<<
			"			bind $Global(CurrentCanvas) <B2-Motion> {%W scan dragto %x %y}"
			<<
			endl
			<<
			"			bind . <Key-Next> {nextPage}"
			<<
			endl
			<<
			"			bind . <Key-Prior> {prevPage}"
			<<
			endl
			<<
			"			pack $Global(CurrentCanvas) -in $cf.top -side left -fill both \\"
			<<
			endl
			<<
			"				-expand true"
			<<
			endl
			<<
			"		}"
			<<
			endl
			<<
			"	}"
			<<
			endl
			<<
			"    set Global(ThisFile) \"Plugin\""
			<<
			endl
			<<
			"    catch {set Global(ThisFile) \"$argv0\"}"
			<<
			endl
			<<
			"	set Global(FontScale) 1"
			<<
			endl
			<<
			"	set Global(Circulate) 0"
			<<
			endl
			<<
			"	set Global(Fit) 0"
			<<
			endl
			<<
			"	set Global(PointsInch) [winfo fpixels . 1i]"
			<<
			endl
			<<
			"	if {[info exists embed_args]} {"
			<<
			endl
			<<
			"		set Global(Plugin) 1"
			<<
			endl
			<<
			"	} else {"
			<<
			endl
			<<
			"		set Global(Plugin) 0"
			<<
			endl
			<<
			"	}"
			<<
			endl
			<<
			"	frame .can"
			<<
			endl
			<<
			"	frame .can.top"
			<<
			endl
			<<
			"	frame .can.bottom"
			<<
			endl
			<<
			"	frame .can.buttons"
			<<
			endl
			<<
			"	button .can.origin -text Origin -command \\"
			<<
			endl
			<<
			"		{originObject all;newCanvas .can c$Global(CurrentPageId)}"
			<<
			endl
			<<
			"	checkbutton .can.lzoom -text \"Lock Zoom\" -variable Global(LockZoom)"
			<<
			endl
			<<
			"	entry .can.ezoom -width 5 -bg #FF00FF00FF00"
			<<
			endl
			<<
			"	checkbutton .can.fit -text Fit -variable Global(Fit) -command {newCanvas .can c$Global(CurrentPageId)}"
			<<
			endl
			<<
			"	checkbutton .can.circ -text Circulate -variable Global(Circulate)"
			<<
			endl
			<<
			"	button .can.next -text Next -command {nextPage}"
			<<
			endl
			<<
			"	button .can.prev -text Prev -command {prevPage}"
			<<
			endl
			<<
			"	button .can.dismiss -text Dismiss -command \"exit\""
			<<
			endl
			<<
			"	scrollbar .can.scrolly -bd 2 -width 15 \\"
			<<
			endl
			<<
			"		-command {$Global(CurrentCanvas) yview}"
			<<
			endl
			<<
			"	scrollbar .can.scrollx -orient h -bd 2 -width 15 \\"
			<<
			endl
			<<
			"		-command {$Global(CurrentCanvas) xview}"
			<<
			endl
			<<
			"	frame .can.corner -height 15 -width 15 -bd 2"
			<<
			endl
			<<
			"	pack .can.scrolly -in .can.top -anchor se -side right -fill y"
			<<
			endl
			<<
			"	pack .can.scrollx -in .can.bottom -anchor se -side left -expand true -fill x"
			<<
			endl
			<<
			"	pack .can.corner -in .can.bottom -anchor ne -in .can.bottom \\"
			<<
			endl
			<<
			"		-side right -fill both"
			<<
			endl
			<<
			"	pack .can.prev .can.next .can.origin .can.ezoom .can.lzoom .can.fit .can.circ -side left \\"
			<<
			endl
			<<
			"		-in .can.buttons"
			<<
			endl
			<<
			"	pack .can.top -side top -in .can -expand true -fill both"
			<<
			endl
			<<
			"	pack .can.bottom -side top -in .can -anchor sw -fill x"
			<<
			endl
			<<
			"	pack .can.buttons -side top -in .can -anchor sw -fill x"
			<<
			endl
			<<
			"	pack .can -fill both -expand true"
			<<
			endl
			<<
			"	bind .can.ezoom <Key-Return> {"
			<<
			endl
			<<
			"		newCanvas .can c$Global(CurrentPageId)"
			<<
			endl
			<<
			"	}"
			<<
			endl
			<<
			"}"
			<<
			endl
			<<
			"if {[info exists Global(EmbedPage)]} {"
			<<
			endl
			<<
			"	if {!$Global(EmbedPage)} {"
			<<
			endl
			<<
			"		set Global(CurrentPageId) 0"
			<<
			endl
			<<
			"		set Global(CurrentCanvas) \"\""
			<<
			endl
			<<
			"	} elseif {[info exists Global(CurrentPageId)]} {"
			<<
			endl
			<<
			"		set Global(CurrentPageId) [expr $Global(CurrentPageId) - 1]"
			<<
			endl
			<<
			"	}"
			<<
			endl
			<<
			"} else {"
			<<
			endl
			<<
			"	set Global(CurrentPageId) 0"
			<<
			endl
			<<
			"	set Global(CurrentCanvas) \"\""
			<<
			endl
			<<
			"}"
			<<
			endl
			<<
			"set Global(LoadPageId) [expr $Global(CurrentPageId) + 1]"
			<<
			endl
			<<
			"set Global(DocPointsInch) 72.0"
			<<
			endl
			<<
			"set Global(SlideBG) #FF00FF00FF00"
			<< endl << "set Global(Circulate) 1" << endl << "set Global(SlideScale) 0" << endl;
	}
}

drvTK::~drvTK()
{
	// now we can copy the buffer the output
	if (!options->noImPress) {
		buffer << "set Global(CurrentPageId) $Global(LoadPageId)" << endl
			<< "newCanvas .can c$Global(CurrentPageId)" << endl;
	}
	ifstream & inbuffer = tempFile.asInput();
	copy_file(inbuffer, outf);
	options=nullptr;
	paperinfo=nullptr;
}

void drvTK::print_coords()
{
	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		const Point & p = pathElement(n).getPoint(0);
		const float pc_x = (p.x() + x_offset) ;
		const float pc_y = (currentDeviceHeight - p.y() + y_offset) ;
		buffer << pc_x;
		buffer << ' ' << pc_y;
		if (n != numberOfElementsInPath() - 1) {
			// it is not the last point
			buffer << ' ';
		}
		if ((!((n + 1) % 8)) && ((n + 1) != numberOfElementsInPath())) {
			buffer << "\\" << endl;
		}
	}
}
void drvTK::close_page()
{
	if (!options->noImPress) {
		buffer <<
			"scaleObject all  [expr $Global(PointsInch)/$Global(DocPointsInch) *  1.0] {} {}"
			<< endl;
	}
}

void drvTK::open_page()
{
	if (!options->noImPress) {
		buffer <<
			"	set Global(CurrentPageId) [expr $Global(CurrentPageId) + 1]" << endl;
		const char * const unit = paperinfo->preferredunit == p_mm ? "c" : "i";
		const double unitfactor = paperinfo->preferredunit == p_mm ? 0.1f : 1.0f; // need to write in cm not in mm
		const double pwidth = paperinfo->preferredunit == p_mm ? paperinfo->mmw*unitfactor : paperinfo->inw;
		const double pheight = paperinfo->preferredunit == p_mm ? paperinfo->mmh*unitfactor : paperinfo->inh;

		if (options->swapHW) { 
			buffer << 
			"	set Global(PageHeight) " << pwidth  << unit << endl <<
			"	set Global(PageWidth) "  << pheight << unit << endl ;
		} else {
			buffer << 
			"	set Global(PageHeight) " << pheight << unit << endl <<
			"	set Global(PageWidth) "  << pwidth  << unit << endl ;
		}
		buffer << 
			"	set Global(Landscape) 0" << endl <<
			"	newCanvas .can c$Global(CurrentPageId)" << endl;
	}
}

void drvTK::show_text(const TextInfo & textinfo)
{
	const int condensedfont = (strstr(textinfo.currentFontName.c_str(), "Condensed") != nullptr);
	const int narrowfont = (strstr(textinfo.currentFontName.c_str(), "Narrow") != nullptr);
	const int boldfont = (strstr(textinfo.currentFontName.c_str(), "Bold") != nullptr);
	const int italicfont = ((strstr(textinfo.currentFontName.c_str(), "Italic") != nullptr)
					  || (strstr(textinfo.currentFontName.c_str(), "Oblique") != nullptr));
	char*  tempfontname = cppstrdup(textinfo.currentFontName.c_str()) ; // char tempfontname[1024];
	char fonttype = 'r';
	char *i;
	int actualFontSize;
	// coverity[uninit_use_in_call]
	i = strchr(tempfontname, '-');
	if (i != nullptr) {
		*i = '\0';
	}
	if (italicfont)
		fonttype = 'i';

	actualFontSize = int ((textinfo.currentFontSize / .95) * 10);

	buffer << "set i [$Global(CurrentCanvas) create text "
		<< textinfo.x() + x_offset
		<< " " << (currentDeviceHeight - textinfo.y() + y_offset + actualFontSize / 7.2);


	buffer << " -text \"";
	outputEscapedText(textinfo.thetext.c_str());
	buffer << endl << "\"";
	buffer << " -font {-*-" << tempfontname << "-";
	if (boldfont)
		buffer << "bold";
	else
		buffer << "medium";
	buffer << "-" << fonttype;
	if (narrowfont)
		buffer << "-narrow--*-";
	else if (condensedfont)
		buffer << "-condensed--*-";
	else
		buffer << "-*--*-";
	buffer << actualFontSize
		<< "-72-72-*-*-*-*"
		<< "}" << " -anchor sw" << " -fill " << colorstring(currentR(), currentG(), currentB())
		<< " -tags \"" << options->tagNames.value << "\" ]" << endl;

	if (strcmp(options->tagNames.value.c_str(), "") && !(options->noImPress)) {
		buffer << "set Group($Global(CurrentCanvas),$i) \"" << options->tagNames.value << "\"" << endl;
	}
	delete [] tempfontname;
}

static const int Fill = 1;
static const int noFill = 0;

void drvTK::show_path()
{
	const int fillpat = (currentShowType() == drvbase::stroke) ? noFill : Fill;
	if (isPolygon()) {
		buffer << "set i [$Global(CurrentCanvas) create polygon ";
		print_coords();
		if (fillpat == 1) {
			buffer << " -fill \"" << colorstring(currentR(), currentG(), currentB())
				<< "\"";
		} else {
			buffer << " -fill \"\"";
		}
		buffer << " -outline \"" << colorstring(currentR(), currentG(), currentB())
			<< "\"" << " -width " << (currentLineWidth()? currentLineWidth() : 1)
			<< "p" << " -tags \"" << options->tagNames.value << "\" ]" << endl;
	} else {
		if (fillpat == 1) {
			buffer << "set i [$Global(CurrentCanvas) create polygon ";
			print_coords();
			buffer << " -fill \"" << colorstring(currentR(), currentG(), currentB())
				<< "\"";
			buffer << " -outline \"" << colorstring(currentR(), currentG(), currentB())
				<< "\"" << " -width " << (currentLineWidth()? currentLineWidth() : 1)
				<< "p" << " -tags \"" << options->tagNames.value << "\" ]" << endl;
		} else {
			buffer << "set i [$Global(CurrentCanvas) create line ";
			print_coords();
			buffer << " -fill \"" << colorstring(currentR(), currentG(), currentB())
				<< "\"" << " -width " << (currentLineWidth()? currentLineWidth() : 1)
				<< "p" << " -tags \"" << options->tagNames.value << "\" ]" << endl;
		}
	}
	if (strcmp(options->tagNames.value.c_str(), "") && !(options->noImPress)) {
		buffer << "set Group($Global(CurrentCanvas),$i) \"" << options->tagNames.value << "\"" << endl;
	}
}


static DriverDescriptionT < drvTK > D_tk("tk", "tk and/or tk applet source code", "","tk", false,	// backend supports subpaths
										 // if subpaths are supported, the backend must deal with
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
										 false,	// backend supports curves
										 false,	// backend supports elements which are filled and have edges
										 true,	// backend supports text
										 DriverDescription::imageformat::noimage,	// no support for PNG file images
										 DriverDescription::opentype::normalopen, true,	// backend support multiple pages
										 false  /*clipping */
										 );
