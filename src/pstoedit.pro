%//%Title: pstoedit.pro / pstoedit.ph; This file is part ot pstoedit
%// 
%// This file contains some redefinitions of PostScript(TM) operators
%// useful for the conversion of PostScript into a vector format via Ghostscript
%// 
%// Copyright (C) 1993 - 2024 Wolfgang Glunz, wglunz35_AT_pstoedit.net  
%//
%//    This program is free software; you can redistribute it and/or modify
%//    it under the terms of the GNU General Public License as published by
%//    the Free Software Foundation; either version 2 of the License, or
%//    (at your option) any later version.
%//
%//    This program is distributed in the hope that it will be useful,
%//    but WITHOUT ANY WARRANTY; without even the implied warranty of
%//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%//    GNU General Public License for more details.
%//
%//    You should have received a copy of the GNU General Public License
%//    along with this program; if not, write to the Free Software
%//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
%//
/pstoedit.copyright (Copyright \(C\) 1993 - 2024 Wolfgang Glunz) def
systemdict /.addcontrolpath known {
  /PermitFileReading pstoedit.inputfilename  .addcontrolpath
  /PermitFileWriting pstoedit.targetoutputfile .addcontrolpath
  currentdict /pstoedit.nameOfOutputFilewithoutpercentD known { 
	/PermitFileWriting pstoedit.nameOfOutputFilewithoutpercentD (*) concatstrings .addcontrolpath
  } if
  /PermitFileWriting pstoedit.outputfilename .addcontrolpath
  /PermitFileWriting (pscover.txt) .addcontrolpath
  currentdict /pstoedit.nameOfIncludeFile known { 	
    /PermitFileReading pstoedit.nameOfIncludeFile .addcontrolpath 
  } if
  .setsafe
} if
/pstoedit.image.dotranslate true def
currentdict /pstoedit.maptoisolatin1 	known not 
{ 
	/pstoedit.maptoisolatin1 true def
} 
if
currentdict /pstoedit.fontdebugging	known not 
{ 
	/pstoedit.fontdebugging false def 
} 
if
currentdict /pstoedit.useoldnormalization known not
{ 
	/pstoedit.useoldnormalization false def
} 
if
currentdict /pstoedit.pstoedit.textastext known not 
{ 
	/pstoedit.pstoedit.textastext true def
} 
if
currentdict /pstoedit.correctdefinefont known not
{ 
	/pstoedit.correctdefinefont false def
} 
if
currentdict /pstoedit.disabledrawtext known not
{ 
	/pstoedit.disabledrawtext false def
} 
if
currentdict /pstoedit.DrawGlyphBitmaps known not
{ 
	/pstoedit.DrawGlyphBitmaps false def % if true, then overwrite the ignoreimage and ignoredraw variables
} 
if
currentdict /pstoedit.useAGL known not
{ 
	/pstoedit.useAGL false def
} 
if
currentdict /pstoedit.autodrawtext known not 
{ 
	/pstoedit.autodrawtext false def
} 
if
currentdict /pstoedit.t2fontsast1 known not 
{ 
	/pstoedit.t2fontsast1 false def
} 
if
currentdict /pstoedit.passglyphnames known not 
{ 
	/pstoedit.passglyphnames false  def % if true, then the glyphnames are passed instead of the characters
} 
if
currentdict /pstoedit.ashexstring known not 
{ 
	/pstoedit.ashexstring true def % false means clear text
} 
if
currentdict /pstoedit.usefinddevice known not 
{ 
	/pstoedit.usefinddevice false def % true means use the standard operators
} 
if
currentdict /pstoedit.checkfontnames known not
{ 
	/pstoedit.checkfontnames true def
} 
{ 
	/pstoedit.ashexstring true def  % enforce HEX mode
	/pstoedit.maptoisolatin1 false def
} 
ifelse
currentdict /pstoedit.flatnesstouse 		known not 
{ 
	/pstoedit.flatnesstouse 1.0 def
} 
if
currentdict /pstoedit.escapetext 		known not 
{ 
	/pstoedit.escapetext false def
} 
if
currentdict /pstoedit.redirectstdout 	known not 
{ 
	/pstoedit.redirectstdout false def
} 
if
currentdict /pstoedit.usepdfmark 		known not 
{ 
	/pstoedit.usepdfmark false def
} 
if
currentdict /pstoedit.outputfilename 	known not 
{ 
	/pstoedit.outputfilename (%stdout) def
} 
if
currentdict /pstoedit.inputfilename  	known not 
{ 
	/pstoedit.inputfilename  (%stdin) def
} 
if
currentdict /pstoedit.doflatten      	known not 
{ 
	/pstoedit.doflatten      true  def
} 
if
currentdict /pstoedit.withimages     	known not 
{ 
	/pstoedit.withimages     false  def
} 
if
currentdict /pstoedit.backendSupportsFileImages     	known not 
{ 
	/pstoedit.backendSupportsFileImages     false  def
} 
if
currentdict /pstoedit.verbosemode    	known not 
{ 
	/pstoedit.verbosemode    false  def
} 
if
currentdict /pstoedit.replacementfont 	known not 
{ 
	/pstoedit.replacementfont      (Courier)  def
} 
if
currentdict /pstoedit.delaybindversion	known not 
{ 
	/pstoedit.delaybindversion      true  def
} 
if
currentdict /pstoedit.scalefontversion	known not 
{ 
	/pstoedit.scalefontversion      false  def
} 
if
currentdict /pstoedit.simulateclipping	known not 
{ 
	/pstoedit.simulateclipping      false  def
} 
if
currentdict /pstoedit.useRGBcolors	known not 
{ 
	/pstoedit.useRGBcolors      false  def
} 
if
currentdict /pstoedit.useBBfrominput known not 
{ 
	/pstoedit.useBBfrominput      false  def
} 
if
currentdict /pstoedit.dumpclippath	known not 
{ 
	/pstoedit.dumpclippath      false  def
} 
if
currentdict /pstoedit.rotation	known not 
{ 
	/pstoedit.rotation      0  def
} 
if
currentdict /pstoedit.xscale	known not 
{ 
	/pstoedit.xscale      1  def
} 
if
currentdict /pstoedit.yscale	known not 
{ 
	/pstoedit.yscale      1  def
} 
if
currentdict /pstoedit.xshift	known not 
{ 
	/pstoedit.xshift      0  def
} 
if
currentdict /pstoedit.yshift	known not 
{ 
	/pstoedit.yshift      0  def
} 
if
currentdict /pstoedit.centered	known not 
{ 
	/pstoedit.centered false  def
} 
if
currentdict /pstoedit.imagedevicename	known not 
{ 
	/pstoedit.imagedevicename      (bmp16m)  def 
	/pstoedit.imagefilesuffix      (.bmp)  def 
} 
if
currentdict /pstoedit.precisiontext 	known not 
{ 
	/pstoedit.precisiontext         0  def
} 
if
currentdict /pstoedit.globalunmappablecharacter known not 
{ 
	/pstoedit.globalunmappablecharacter ( ) def %Note: we use a space instead of a # since
} 
if
currentdict /pstoedit.pagetoextract      known not 
{ 
	/pstoedit.pagetoextract    0  def
} 
if 
currentdict /pstoedit.withpscover      known not 
{ 
	/pstoedit.withpscover    false  def
} 
if 
pstoedit.withpscover 
{ 
	/covstream (pscover.txt) (w) file def
	/pstoedit.indent 0 store
	/psentry 
	{ 
		0 1 pstoedit.indent 
		{ 
			pop covstream ( ) writestring 
		} 
		for
		covstream ({ ) writestring
		20 string cvs covstream exch writestring
		covstream (\n) writestring
		covstream flushfile
		pstoedit.indent 1 add /pstoedit.indent exch store
	} 
	def
	/psexit 
	{ 
		pstoedit.indent 1 sub /pstoedit.indent exch store
		0 1 pstoedit.indent 
		{ 
			pop covstream ( ) writestring 
		} 
		for
		covstream (}\n) writestring covstream flushfile
	} 
	def
	/PSCOVER 
	{ 
		20 -string cvs covstream exch writestring  covstream (\n) writestring covstream flushfile
	} 
	def
} 
{ 
	/psentry 
	{ 
		pop
	} 
	def
	/psexit 
	{ 
	} 
	def
	/PSCOVER 
	{ 
		pop
	} 
	def
} 
ifelse
pstoedit.delaybindversion
{ 
	revision 353 ge 
	{ 
		/NOBIND where 
		{ 
			pop NOBIND 
		} {
		    false % no NOBIND found
		} ifelse
		/DELAYBIND where 
		{ 
			pop DELAYBIND or % or with NOBIND
		} 
		if
		{ 
			systemdict begin
			/bind /.bind load
			/.forcedef where 
			{ 
				pop .forcedef 
			} 
			{ 
				def 
			} 
			ifelse
			end
		} 
		if
	} 
	if
} 
if
/.forcedef where 
{ 
	pop 
} 
{ 
	/.forcedef /def load def 
} 
ifelse
/pstoedit.currentglobalvalue false def
/pstoedit.setglobal {
	systemdict /.setglobal known 
	{ 
		.setglobal 
	} {
		setglobal
	}
	ifelse
} def
/pstoedit.currentglobal {
	systemdict /.currentglobal known 
	{ 
		.currentglobal 
	} {
		currentglobal
	}
	ifelse
} def
false 
{ 
	/redef 
	{ 
		systemdict begin 1 index exch .makeoperator //.forcedef end 
	} 
	bind def
	/soverload 
	{ 
		bind redef
	} 
	bind def
	/overload  
	{ 
		bind redef
	} 
	bind def
		/pstoedit.currentglobalvalue pstoedit.currentglobal def
		pstoedit.currentglobal 
		true pstoedit.setglobal 
} 
{ 
		/pstoedit.currentglobalvalue pstoedit.currentglobal def 
		pstoedit.currentglobal 
		true pstoedit.setglobal 
	/soverload 
	{ 
		systemdict begin bind def end 
	} 
	def
	/overload 
	{ 
		bind def 
	} 
	bind def
} 
ifelse 
/outstream pstoedit.outputfilename (w) file def
pstoedit.redirectstdout 
{ 
	/errstream (%stderr) (w) file def
	/print 
	{ 
		errstream exch writestring 
	} 
	overload
	/flush 
	{ 
		errstream flushfile 
	} 
	overload
} 
if
/dumpcurrentCTM 
{ 
	(CurrentCTM) matrix currentmatrix  pstack pop pop pop
} 
def
/-printalways  
{ 
	outstream exch writestring 
} 
def
/-print  
{ 
	pstoedit.pagetoextract 0 eq pstoedit.pagetoextract pstoedit.pagenr cvi eq or
	{ 
		-printalways
		1 1 -string cvs pstoedit.somethingprinted copy pop % remember - something was printed
	} 
	{ 
		pop 
	} 
	ifelse
} 
def
/-printhexalways  
{ 
	{ 
		inttosinglechar
		dup
		1 normalizecharacter_wrapper
		{ 
			exch pop % remove plain c since we can use the mapped one.
			pstoedit.fontdebugging 
			{ 
				dup pstoedit.glyphname 1000 -string cvs (normalized:) pstack pop pop pop 
			} 
			if
			outstream exch writehexstring
		} 
		{ 
			outstream exch writehexstring
		} 
		ifelse
	} 
	forall
} 
def
/-printhex  
{ 
	pstoedit.pagetoextract 0 eq pstoedit.pagetoextract pstoedit.pagenr cvi eq or
	{ 
		-printhexalways
		1 1 -string cvs pstoedit.somethingprinted copy pop % remember - something was printed
	} 
	{ 
		pop 
	} 
	ifelse
} 
def
/-printNC  
{ 
	pstoedit.pagetoextract 0 eq pstoedit.pagetoextract pstoedit.pagenr cvi eq or
	{ 
		-printalways
	} 
	{ 
		pop 
	} 
	ifelse
} 
def
/-fill   /fill load def
/-eofill /eofill load def
/-stroke /stroke load def
/-image /image load def
/-imagemask /imagemask load def
/-colorimage /colorimage load def
/-awidthshow   /awidthshow load def
/.pdfwidthshow where {
  pop % the dict
  /-pdfawidthshow  /.pdfwidthshow load def
  /-pdfwidthshow   /.pdfawidthshow load def
} if
/-showpage /showpage load def
/-string /string load def
/-lt /lt load def
/-ge /ge load def
/-charpath /charpath load def
/-stringwidth /stringwidth load def
languagelevel 1 gt 
{ 
	/-glyphshow /glyphshow load def
} 
if
/pstoedit.pagenr (1           ) store
/pstoedit.somethingprinted (0) store % indicates whether something was written.
/pstoedit.replacedfont false store  % if true, font was replaced with Courier
/pstoedit.ignoredraw false store  % if true, ignore all fill, eofill, stroke
/pstoedit.ignoreimage false store  % if true, ignore all *image*
/pstoedit.textshown false store
/pstoedit.fontunit 1000 store % standard fonts have a unit of 1/1000
/pstoedit.x 1 store
/pstoedit.y 1 store
/pstoedit.dumpstring () store
/pstoedit.istype0 false store
/pstoedit.fontselectorbyte 0 store
/pstoedit.currentpositionisfontselector true store % init	
/pstoedit.dumpop 
{ 
} 
store
/pstoedit.y1 1 store
/pstoedit.x1 1 store
/pstoedit.y2 1 store
/pstoedit.x2 1 store
/pstoedit.inputchar 0 store
/pstoedit.outputchar 0 store
/pstoedit.glyphname () store
/pstoedit.glyphname 0 store
/pstoedit.inverseDict 0 store
/pstoedit.currentstring () store
/pstoedit.normalizedstring () store
/pstoedit.stringindex 0 store
/pstoedit.angle 0 store
/pstoedit.pathnumber (0                  ) store
/pstoedit.deviceheight 0 def
/pstoedit.deviceinfoshown false def
/pstoedit.dpiscale 1 def
/pstoedit.currentscale 1 def
/pstoedit.glyphcounter 0 def
/pstoedit.ll.x    (999999            ) def
/pstoedit.ll.y    (999999            ) def
/pstoedit.ur.x    (0                 ) def
/pstoedit.ur.y    (0                 ) def
/pstoedit.bbclear (                  ) def
/pstoedit.dotrace (0) store  % switched on and off dynamically
/pstoedit.unmappablecharacter pstoedit.globalunmappablecharacter 1 string cvs store
/-findfont /findfont load def
/-definefont /definefont load def
/-currentrgbcolor /currentrgbcolor load def
/-currentcmykcolor /currentcmykcolor load def
/-concat /concat load def
/s100 100 -string def
/s1000 1000 -string def
/globals100 
{ 
	currentglobal true setglobal 100 -string exch setglobal 
} 
def
/canIterateThroughString
{ 
	currentfont /FontType get 0 eq
	{ 
		currentfont /FMapType get dup dup 2 eq exch 4 eq or exch 5 eq or
	} 
	{ 
		true % for all other fonts we assume to be able to handle the string char by char
	} 
	ifelse
} 
def
/pstoedit.FORALL 
{ 
	2 dict begin
	currentfont dup /FontType get 0 eq
	{ 
		/FMapType get dup 2 eq exch 5 eq or  % leave ((FmapType == 2) OR (FmapType == 5) ) on stack
	} 
	{ 
		pop % the fontdict
		false
	} 
	ifelse
	{ 
		/pstoedit.FORPROC exch cvx def
		/pstoedit.FORSTRING exch def
		0 2 pstoedit.FORSTRING length 1 sub 
		{ 
			/pstoedit.FORINDEX exch def
			/CHAR2 2 string def
			CHAR2 0 pstoedit.FORSTRING pstoedit.FORINDEX get put
			CHAR2 1 pstoedit.FORSTRING pstoedit.FORINDEX 1 add get put
			CHAR2 
			{ 
				pstoedit.FORPROC 
			} 
			exec
		} 
		for
	} 
	{ 
		/pstoedit.FORPROC exch cvx def
		/pstoedit.FORSTRING exch def
		0 1 pstoedit.FORSTRING length 1 sub 
		{ 
			/pstoedit.FORINDEX exch def
			/CHAR1 1 string def
			CHAR1 0 pstoedit.FORSTRING pstoedit.FORINDEX get put
			CHAR1 
			{ 
				pstoedit.FORPROC 
			} 
			exec
		} 
		for
	} 
	ifelse
	end
} 
def
/-gsave		/gsave		load def
/-grestore  /grestore	load def
/-save		/save		load def
/-restore	/restore	load def
/-clip		/clip		load def
/-eoclip	/eoclip		load def
languagelevel 1 gt 
{ 
	/-rectclip	/rectclip	load def
	/-setgstate /setgstate	load def
} 
if
//systemdict begin
	/CSLAYERDEBUG true store
end
/pstoedit.writesaverestore (0) store
pstoedit.dumpclippath  pstoedit.simulateclipping not and 
{ 837 psentry
	/pstoedit.writesaverestore (1) store % use a string to have it "global"
	/gsave 
	{ 845 psentry
		-gsave pstoedit.writesaverestore (1) eq 
		{ 847 psentry
			(gsave %{\n ) -print 
		 psexit } 
		if
	 psexit } 
	soverload
	/grestore 
	{ 856 psentry
		-grestore pstoedit.writesaverestore (1) eq 
		{ 858 psentry
			(grestore %}\n ) -print 
		 psexit } 
		if
	 psexit } 
	soverload
	/save 
	{ 866 psentry
		-save 
		pstoedit.writesaverestore (1) eq 
		{ 869 psentry
			(save %{\n)	 -print 
		 psexit } 
		if
	 psexit } 
	soverload
	/restore 
	{ 877 psentry
		pstoedit.writesaverestore (1) eq 
		{ 879 psentry
			(restore %}\n)	-print 
		 psexit } 
		{ 883 psentry
			(%HIDDENRESTORE \n) -print
		 psexit } 
		ifelse
		-restore
	 psexit } 
	soverload
	/clip 
	{ 894 psentry
		(% clippath\n) 
		{ 896 psentry
			-clip
		 psexit } 
		(clip) dumppath 
	 psexit } 
	soverload
	/eoclip 
	{ 906 psentry
		(% clippath\n) 
		{ 908 psentry
			-eoclip
		 psexit } 
		(eoclip) dumppath 
	 psexit } 
	soverload
	/rectclip 
	{ 918 psentry
		newpath
		4 2 roll moveto 1 index 0 rlineto 0 exch rlineto neg 0 rlineto
		closepath clip newpath 
	 psexit } 
	soverload
	/dummyrectclip 
	{ 924 psentry
		(Executing rectclip : result may not be correct with respect to clipping\n) true printwarning
		-rectclip
	 psexit } 
	soverload
	/setgstate 
	{ 931 psentry
		(Executing setgstate : result may not be correct with respect to clipping\n) true printwarning
		-setgstate
	 psexit } 
	soverload
 psexit } 
if
languagelevel 1 gt 
{ 941 psentry
	/-rectfill /rectfill load def
	/rectfill 
	{ 944 psentry
		newpath
		4 2 roll moveto 
		1 index 0 rlineto 
		0 exch rlineto 
		neg 0 rlineto
		closepath % normally not needed in fill - just to enable is_rectangle to recognize this rectangle as well.
		pstoedit.fill 
	 psexit } 
	soverload
 psexit } 
if
/dummyrectfill 
{ 957 psentry
	(Executing rectfill : not yet implemented\n) true printwarning
	-rectfill
 psexit } 
soverload
languagelevel 1 gt 
{ 964 psentry
	/-rectstroke /rectstroke load def
	/rectstroke 
	{ 967 psentry
		newpath
		pstoedit.verbosemode 
		{ 969 psentry
			(Executing emulated rectstroke\n) true printwarning 
		 psexit } 
		if
		4 2 roll moveto 1 index 0 rlineto 0 exch rlineto neg 0 rlineto
		closepath pstoedit.stroke 
	 psexit } 
	soverload
 psexit } 
if
/dummyrectstroke 
{ 982 psentry
	pstoedit.verbosemode 
	{ 984 psentry
		(Executing dummyrectstroke : not yet implemented\n) true printwarning 
	 psexit } 
	if
	-rectstroke
 psexit } 
soverload
languagelevel 2 gt 
{ 993 psentry
	/-shfill /shfill load def
	/shfill 
	{ 1012 psentry
		begin
		currentdict /ShadingType known
		ShadingType 7 eq and
		currentdict /ColorSpace known and
		ColorSpace /DeviceRGB eq and
		currentdict /DataSource known and
		DataSource type /arraytype eq and
		DataSource length 45 eq and
		{ 1022 psentry
			gsave
			newpath
			DataSource  1 2 getinterval aload pop moveto
			DataSource  3 6 getinterval aload pop curveto
			DataSource  9 6 getinterval aload pop curveto
			DataSource 15 6 getinterval aload pop curveto
			closepath
			0.0
			33 3 42 
			{ 1037 psentry
				DataSource exch get
				add
			 psexit } 
			for
			4.0 div
			0.0
			34 3 43 
			{ 1045 psentry
				DataSource exch get
				add
			 psexit } 
			for
			4.0 div
			0
			35 3 44 
			{ 1053 psentry
				DataSource exch get
				add
			 psexit } 
			for
			4.0 div
			setrgbcolor
			fill
			grestore
		 psexit } 
		if
		end
	 psexit } 
	soverload
	/formerdummyshfill 
	{ 1077 psentry
		pstoedit.verbosemode 
		{ 1079 psentry
			(Executing shfill operator - this is not yet supported by pstoedit\n) true printwarning 
		 psexit } 
		if
		-shfill
	 psexit } 
	soverload
 psexit } 
if
languagelevel 1 gt 
{ 1090 psentry
	/-ufill /ufill load def
	/ufill 
	{ 1093 psentry
		pstoedit.verbosemode 
		{ 1096 psentry
			(Executing emulated ufill\n) true printwarning 
		 psexit } 
		if
		gsave % Save graphics state
		newpath % Clear current path
		uappend % Interpret userpath
		pstoedit.fill % Stroke the path
		grestore % Restore graphics state
	 psexit } 
	soverload
	/-ueofill /ueofill load def
	/ueofill 
	{ 1110 psentry
		pstoedit.verbosemode 
		{ 1113 psentry
			(Executing emulated ueofill\n) true printwarning 
		 psexit } 
		if
		gsave % Save graphics state
		newpath % Clear current path
		uappend % Interpret userpath
		pstoedit.eofill % Stroke the path
		grestore % Restore graphics state
	 psexit } 
	soverload
 psexit } 
if
/pstoedit.numbertype 
{ 1128 psentry
	type dup /realtype eq exch /integertype eq or
 psexit } 
def
/pstoedit.mattest 
{ 1134 psentry
	dup type /arraytype eq
	{ 1137 psentry
		dup xcheck 
		{ 1140 psentry
			pop false
		 psexit } 
		{ 1143 psentry
			dup length 6 eq 
			{ 1145 psentry
				true exch 
				{ 1148 psentry
					pstoedit.numbertype and  
				 psexit } 
				forall
			 psexit } 
			{ 1153 psentry
				pop false % if length is not 6
			 psexit } 
			ifelse
		 psexit } 
		ifelse
	 psexit } 
	{ 1160 psentry
		pop false
	 psexit } 
	ifelse
 psexit } 
def
languagelevel 1 gt 
{ 1168 psentry
	/-ustroke /ustroke load def
	/ustroke 
	{ 1171 psentry
		pstoedit.verbosemode 
		{ 1174 psentry
			(Executing emulated ustroke\n) true printwarning 
		 psexit } 
		if
		dup pstoedit.mattest
		{ 1185 psentry
			gsave % Save graphics state
			newpath % Clear current path
			exch uappend % Interpret userpath
			concat % Concatenate matrix to CTM
			pstoedit.stroke % Stroke the path
			grestore % Restore graphics state
		 psexit } 
		{ 1194 psentry
			gsave % Save graphics state
			newpath % Clear current path
			uappend % Interpret userpath
			pstoedit.stroke % Stroke the path
			grestore % Restore graphics state
		 psexit } 
		ifelse
	 psexit } 
	soverload
 psexit } 
if
pstoedit.usepdfmark 
{ 1214 psentry
	/currentdistillerparams 1 dict def
	currentdistillerparams begin
	/CoreDistVersion 2000 def
	end
 psexit } 
if
/debugprint 
{ %nc 
	(\% PRINT ) -print 
	-print
	( MSG\n) -print
} 
def
/printwarning 
{ %nc 
	{ %nc 
		(Warning: )  print		
		(%Warning: ) -printalways 
	} 
	if
	dup
	print
	-printalways
} 
def
/P2EVTRACE { %nc
	pstoedit.verbosemode { %nc
		pstack
	} if
	pop % the argument string
} 
def
/onechar 1 -string def
/inttosinglechar 
{ %nc 
	onechar exch 0 exch put 
	onechar 1 -string cvs
} 
def
/InverseISO 1500 256 add dict def 
/InversePDF 1500 256 add dict def
/pstoedit.DEVICEINFO 
{ 1296 psentry
	/deviceinfo where 
	{ 1302 psentry
		pop % the dict
		revision 500 -lt 
		{ 1306 psentry
			deviceinfo
		 psexit } 
		{ 1309 psentry
			systemdict begin currentpagedevice  end
		 psexit } 
		ifelse
	 psexit } 
	{ 1314 psentry
		systemdict begin currentpagedevice  end
	 psexit } 
	ifelse
 psexit } 
def
/pstoedit.computeDPIscale 
{ 1323 psentry
	languagelevel 1 gt 
	{ 1325 psentry
		pstoedit.DEVICEINFO /HWResolution known 
		{ 1327 psentry
			pstoedit.DEVICEINFO /HWResolution get 0 get 
			72 div /pstoedit.dpiscale exch store % normalize dpi to 72
		 psexit } 
		{ 1333 psentry
			/pstoedit.dpiscale 1 store
		 psexit } 
		ifelse
	 psexit } 
	{ 1338 psentry
		/pstoedit.dpiscale 1 store
	 psexit } 
	ifelse
 psexit } 
def
/pstoedit.initialize 
{ 
	0 0 transform 
	/pstoedit.deviceheight exch store
	pop
	/pstoedit.deviceinfoshown false store
	pstoedit.computeDPIscale 
	/AdobeGlyphList where {
		pop % we already have it.
		true % OK to use AGL
	} {
		(gs_agl.ps) .libfile
		{
			pstoedit.verbosemode 
			{ 
				(using gs_agl.ps for normalization ) print
			} 
			if
			run 
			pstoedit.verbosemode 
			{ 
				(done evaluating gs_agl.ps\n) print
			} 
			if	
			true % OK to use AGL
		} {
			false % not OK to use AGL
		} ifelse
	} 
	ifelse
	pstoedit.useAGL and {
		AdobeGlyphList 
		{ 
			dup type
			/arraytype eq 
			{ 
				0 get % in case of an array, just use the first one
			} 
			if
			/pstoedit.glyphcode exch store
			/pstoedit.glyphname exch store
			pstoedit.glyphcode 256 lt 
			{ 
				pstoedit.glyphcode inttosinglechar pop
				InverseISO pstoedit.glyphname pstoedit.glyphcode inttosinglechar put 
				InversePDF pstoedit.glyphname pstoedit.glyphcode inttosinglechar put
			} 
			if
		} 
		forall
	} 
	{ 
		pop % .libfile leaves the filename on the stack in this case
		/pstoedit.glyphcounter 0 store
		ISOLatin1Encoding
		{ 
			/pstoedit.glyphname exch store
			InverseISO pstoedit.glyphname known not
			{ 
				InverseISO pstoedit.glyphname pstoedit.glyphcounter inttosinglechar put 
				InversePDF pstoedit.glyphname pstoedit.glyphcounter inttosinglechar put 
			} 
			if
			/pstoedit.glyphcounter pstoedit.glyphcounter 1 add store
		} 
		forall
	} 
	ifelse
	32 1 127 
	{ 
		dup /pstoedit.glyphcounter exch store 
		inttosinglechar cvn /pstoedit.glyphname exch store
		InverseISO pstoedit.glyphname known not
		{ 
			InverseISO pstoedit.glyphname pstoedit.glyphcounter inttosinglechar put 
			InversePDF pstoedit.glyphname pstoedit.glyphcounter inttosinglechar put 
		} 
		if
	} 
	for 
	InverseISO 11 inttosinglechar cvn  (ff) put 
	InverseISO 12 inttosinglechar cvn  (fi) put 
	InverseISO 13 inttosinglechar cvn  (fl) put 
	InversePDF 11 inttosinglechar cvn  (ff) put 
	InversePDF 12 inttosinglechar cvn  (fi) put 
	InversePDF 13 inttosinglechar cvn  (fl) put 
	InverseISO 	begin
	/.notdef   pstoedit.unmappablecharacter def % wogltmp(@) def
	/quotesinglbase (,) def
	/quotesingle (') def
	/florin (f) def
	/quotedblbase (,,) def
	/ellipsis (...) def
	/perthousand (%o) def
	/guilsinglleft (<) def
	/OE (OE) def
	/quotedblleft (") def
	/quotedblright (") def
	/bullet (*) def
	/endash (-) def
	/emdash (-) def
	/trademark (TM) def
	/guilsinglright (>) def
	/oe (oe) def
	/Ydieresis (Y) def
	/fraction (/) def
	/fi (fi) def
	/fl (fl) def
	/hyphen (-) def
	end
	InversePDF 	begin
	/.notdef pstoedit.unmappablecharacter def
	/breve 24 inttosinglechar def
	/caron 25 inttosinglechar def
	/circumflex 26 inttosinglechar def
	/dotaccent 27 inttosinglechar def
	/hungarumlaut 28 inttosinglechar def
	/ogonek 29 inttosinglechar def
	/ring 30 inttosinglechar def
	/tilde 31 inttosinglechar def
	/quotesingle 39 inttosinglechar def
	/grave 96 inttosinglechar def
	/bullet 128 inttosinglechar def
	/dagger 129 inttosinglechar def
	/daggerdbl 130 inttosinglechar def
	/ellipsis 131 inttosinglechar def
	/emdash 132 inttosinglechar def
	/endash 133 inttosinglechar def
	/florin 134 inttosinglechar def
	/fraction 135 inttosinglechar def
	/guilsinglleft 136 inttosinglechar def
	/guilsinglright 137 inttosinglechar def
	/minus 138 inttosinglechar def
	/perthousand 139 inttosinglechar def
	/quotedblbase 140 inttosinglechar def
	/quotedblleft 141 inttosinglechar def
	/quotedblright 142 inttosinglechar def
	/quoteleft 143 inttosinglechar def
	/quoteright 144 inttosinglechar def
	/quotesinglbase 145 inttosinglechar def
	/trademark 146 inttosinglechar def
	/fi 147 inttosinglechar def
	/fl 148 inttosinglechar def
	/Lslash 149 inttosinglechar def
	/OE 150 inttosinglechar def
	/Scaron 151 inttosinglechar def
	/Ydieresis 152 inttosinglechar def
	/Zcaron 153 inttosinglechar def
	/dotlessi 154 inttosinglechar def
	/lslash 155 inttosinglechar def
	/oe 156 inttosinglechar def
	/scaron 157 inttosinglechar def
	/zcaron 158 inttosinglechar def
	/hyphen 45 inttosinglechar def
	/caron 25 inttosinglechar def
	/space 32 inttosinglechar def
	end
} 
def
/pstoedit.quit 
{ 
	pstoedit.somethingprinted cvi 0 eq pstoedit.pagetoextract 0 ne and 
	{ 
		(no graphics found on selected page, maybe pagenr is too high?\n) true printwarning
	} 
	if
	flush
	(\% normal end reached by pstoedit.pro\n) -printalways
	(\%\%Trailer\n) -printalways
	(\%\%Pages: ) -printalways pstoedit.pagenr cvi 1 sub 100 -string cvs -printalways (\n) -printalways
	(\%\%EOF\n) -printalways
	pstoedit.quitprog
} 
def
/getfrominverse	 
{ 1604 psentry
	/pstoedit.inverseDict exch store	
	0 get % get first and only char of string as int
	/pstoedit.inputchar exch store
	currentfont /Encoding get 
	dup length pstoedit.inputchar gt
	{ 1618 psentry
		pstoedit.inputchar
		get 
		/pstoedit.glyphname exch store
		pstoedit.inverseDict pstoedit.glyphname known 
		{ 1626 psentry
			pstoedit.inverseDict pstoedit.glyphname get 
			dup 0 get 0 eq 
			{ 1630 psentry
				(unable to map [c1] glyph name ) print pstoedit.glyphname  
				1000 -string cvs print ( encoded as ) print 
				pstoedit.inputchar 1000 -string cvs print (\n) print
				pop false
			 psexit } 
			{ 1636 psentry
				true % mapped !
			 psexit } 
			ifelse
		 psexit } 
		{ 1641 psentry
			(unable to map [c2] glyph name ) print pstoedit.glyphname  
			1000 -string cvs print ( encoded as ) print 
			pstoedit.inputchar 1000 -string cvs print ( - using the PostScript code value instead.\n) print
			false
		 psexit } 
		ifelse
	 psexit } 
	{ 1649 psentry
		(problem with Encoding vector of font \n) print 
		pop % the Encoding vector
		false
	 psexit } 
	ifelse
 psexit } 
def
/CheckForTextFont
{ 1672 psentry
	currentfont /Encoding get dup length 66 -ge 
	{ 1677 psentry
		65 get /A eq
	 psexit } 
	{ 1681 psentry
		pop false
	 psexit } 
	ifelse 
 psexit } 
def
/GetCurrentFontName 
{ 1689 psentry
	currentfont /FontName known 
	{ 1691 psentry
		currentfont /FontName get 
	 psexit } 
	{ 1694 psentry
		("font without FontName entry")
	 psexit } 
	ifelse
 psexit } 
def
/CheckForTextFontVerbose 
{ 1702 psentry
	CheckForTextFont
	{ 1704 psentry
		true
	 psexit } 
	{ 1707 psentry
		pstoedit.verbosemode 
		{ 1709 psentry
			GetCurrentFontName
			100 -string cvs true printwarning
			( seems to be a non text font - you may want to use -dt or -adt.\n) false printwarning 
		 psexit } 
		if
		false
	 psexit } 
	ifelse
 psexit } 
def
/oldnormalizecharacter 
{ 1724 psentry
	CheckForTextFont pstoedit.maptoisolatin1 and 
	{ 1735 psentry
		pstoedit.usepdfmark 
		{ 1737 psentry
			InversePDF 
		 psexit } 
		{ 1740 psentry
			InverseISO 
		 psexit } 
		ifelse 
		getfrominverse
	 psexit } 
	{ 1749 psentry
		pstoedit.fontdebugging 
		{ 1751 psentry
			(unmapped pfad fuer non text fonts - old ) pstack pop 
		 psexit } 
		if
		true % we tag this case as mapped as well
	 psexit } 
	ifelse
 psexit } 
def
/pstoedit.FontsForWhichToBlockRemapping 10 dict def
pstoedit.FontsForWhichToBlockRemapping begin
/Symbol true def
end
/normalizecharacter 
{ 1767 psentry
	currentfont /FontName get pstoedit.FontsForWhichToBlockRemapping exch known
	not 
	pstoedit.maptoisolatin1 and
	{ 1778 psentry
		dup  % keep the char for later "true" path
		pstoedit.usepdfmark 
		{ 1781 psentry
			InversePDF 
		 psexit } 
		{ 1784 psentry
			InverseISO 
		 psexit } 
		ifelse 
		getfrominverse
		{ 1791 psentry
			exch pop
			true % return true, i.e. 
		 psexit } 
		{ 1796 psentry
			CheckForTextFont 
			{ 1800 psentry
				pop % remove the kept char
				false
			 psexit } 
			{ 1804 psentry
				true
			 psexit } 
			ifelse
		 psexit } 
		ifelse
	 psexit } 
	{ 1811 psentry
		pstoedit.fontdebugging 
		{ 1813 psentry
			(unmapped pfad fuer non text fonts ) pstack pop 
		 psexit } 
		if
		true % we tag this case as mapped as well
	 psexit } 
	ifelse
 psexit } 
def
/normalizecharacter_wrapper
{ 1824 psentry
	pstoedit.fontdebugging 
	{ 1826 psentry
		(kontext: ) pstack pop 
	 psexit } 
	if
	pop % the last pop pops the context id
	pstoedit.useoldnormalization
	{ 1832 psentry
		oldnormalizecharacter
	 psexit } 
	{ 1835 psentry
		normalizecharacter
	 psexit } 
	ifelse
 psexit } 
def
/normalizestring 
{ 1843 psentry
	dup 
	length 2 mul -string /pstoedit.normalizedstring exch store
	/pstoedit.stringindex 0 store % position in output string
	{ 1849 psentry
		inttosinglechar
		2 normalizecharacter_wrapper 
		not	
		{ 1855 psentry
			pstoedit.unmappablecharacter		
		 psexit } 
		if
		dup length exch % save for later adjustment of index
		pstoedit.normalizedstring exch
		pstoedit.stringindex exch
		putinterval
		pstoedit.stringindex add /pstoedit.stringindex exch store 
	 psexit } 
	forall
	pstoedit.normalizedstring 0 pstoedit.stringindex getinterval
 psexit } 
def
/pstoedit.indexinarray 
{ 1874 psentry
	exch % v a
	false % v a false
	1 index length 1 sub -1 0 
	{ 1878 psentry
		2 index  % v a false i a
		1 index  % v a false i a i 
		get % v a false i av
		4 index  % v a false i av v
		eq 
		{ 1886 psentry
			exch pop % v a i
			true
			exit % v a i true
		 psexit } 
		{ 1892 psentry
			pop % the index leaving v a false
		 psexit } 
		ifelse
	 psexit } 
	for
	{ 1899 psentry
		exch pop exch pop 
	 psexit } 
	{ 1902 psentry
		pop pop -1 
	 psexit } 
	ifelse
 psexit } 
def
/getglyphaschar 
{ 1910 psentry
	/pstoedit.glyphname exch store
	currentfont /Encoding get pstoedit.glyphname pstoedit.indexinarray
	dup -1 eq 
	{ 1916 psentry
		pop
		(unable to map [c3] glyph name ) print pstoedit.glyphname  
		1000 -string cvs print %debugwogl( from ) print 
		pstoedit.unmappablecharacter
	 psexit } 
	{ 1924 psentry
		inttosinglechar % just return the index as result
	 psexit } 
	ifelse
 psexit } 
def
/oldgetglyphaschar 
{ 1932 psentry
	/pstoedit.glyphname exch store
	pstoedit.glyphname
	pstoedit.usepdfmark 
	{ 1938 psentry
		InversePDF 
	 psexit } 
	{ 1941 psentry
		InverseISO 
	 psexit } 
	ifelse 
	2 copy
	exch known 
	{ 1948 psentry
		exch get
	 psexit } 
	{ 1951 psentry
		pop pop
		(unable to map [c3] glyph name ) print pstoedit.glyphname  
		1000 -string cvs print %debugwogl( from ) print 
		pstoedit.unmappablecharacter
	 psexit } 
	ifelse
 psexit } 
def
/FontHistory 256 dict def
/pstoedit.lastfindfont (lastfindfont is undefined) def  
/pstoedit.basefontname (null) def
/pstoedit.currentfontkey () def
/findfont 
{ 1976 psentry
	dup /pstoedit.lastfindfont exch store
	-findfont
 psexit } 
overload
/p2egetbasefontname 
{ 1985 psentry
	true
	{ 1989 psentry
		dup
		getorigfont
		dup
		dup /FontName known 
		{ 2000 psentry
			3 1 roll
			ne  % and getorig returned a different font
			{ 2005 psentry
				/FontName get 
			 psexit } 
			{ 2009 psentry
				pstoedit.lastfindfont (lastfindfont is undefined) eq 
				{ 2013 psentry
					dup /FontName known 
					{ 2015 psentry
						/FontName get % this consumes the font on the stack
					 psexit } 
					{ 2018 psentry
						pop % the font returned from getorig
						pstoedit.replacementfont
					 psexit } 
					ifelse
				 psexit } 
				{ 2025 psentry
					pop  % the font returned from getorig
					pstoedit.lastfindfont
				 psexit } 
				ifelse
			 psexit } 
			ifelse
		 psexit } 
		{ 2034 psentry
			pop pop pop
			pstoedit.lastfindfont (lastfindfont is undefined) eq 
			{ 2039 psentry
				pstoedit.replacementfont
			 psexit } 
			{ 2043 psentry
				pstoedit.lastfindfont
			 psexit } 
			ifelse
		 psexit } 
		ifelse
	 psexit } 
	{ 2051 psentry
		pop
		pstoedit.lastfindfont (lastfindfont is undefined) eq 
		{ 2054 psentry
			pstoedit.replacementfont
		 psexit } 
		{ 2058 psentry
			pstoedit.lastfindfont
		 psexit } 
		ifelse
	 psexit } 
	ifelse
 psexit } 
def
/definefont 
{ 2070 psentry
	exch  % font key
	dup type /dicttype eq 
	{ 2073 psentry
	 psexit } 
	{ 2077 psentry
		dup globals100 cvs /pstoedit.currentfontkey exch store
		pstoedit.correctdefinefont 
		{ 2088 psentry
			exch % key font
			dup /FontName known 
			{ 2091 psentry
				dup /FontName get 100 -string cvs pstoedit.currentfontkey eq not 
				{ 2093 psentry
					(FontName and Key do not match: Found ) true printwarning 
					pstoedit.currentfontkey false printwarning 
					( and ) false printwarning
					dup /FontName get 100 string cvs false printwarning (\n) false printwarning
					dup dup /FontName get findfont 
					/OrigFont exch put
					dup /FontName pstoedit.currentfontkey put
				 psexit } 
				if
			 psexit } 
			if
			exch
		 psexit } 
		if
		false
		FontHistory 
		{ 2116 psentry
			exch pop % remove key
			pstoedit.currentfontkey eq 
			{ 2119 psentry
				pop true % remove false and push new result
				exit
			 psexit } 
			if
		 psexit } 
		forall
		{ 2128 psentry
			pstoedit.verbosemode 
			{ 2130 psentry
				dup ( second font definition for ) print s100 cvs print ( ignored\n) print
			 psexit } 
			if
		 psexit } 
		{ 2136 psentry
			(\() -printalways
			dup s100 cvs -printalways
			(\) cvn) -printalways
			( /) -printalways 
			exch 
			dup p2egetbasefontname 
			/pstoedit.basefontname exch store
			exch
			pstoedit.basefontname
			s100 cvs -printalways 
			( pstoedit.newfont\n) -printalways
			dup FontHistory 
			exch 
			pstoedit.basefontname
			globals100 cvs 
			put
		 psexit } 
		ifelse
	 psexit } 
	ifelse
	exch
	-definefont
 psexit } 
overload
/rootFont 
{ 2192 psentry
	dup 
	FontHistory exch known 
	{ 2200 psentry
		dup dup FontHistory exch get 
		ne 
		{ 2205 psentry
			FontHistory exch get rootFont 
		 psexit } 
		if           
	 psexit } 
	if
 psexit } 
def
/printTOS 
{ 2222 psentry
	( ) -print 
	dup type /realtype eq false and
	{ 2225 psentry
		dup 
		cvi dup s100 cvs -print % no we have the int part
		sub % now the delta
		(.) -print
		10000 mul cvi abs 
		dup 1000 gt % can be 0.9999 * 10000 = 9999
		{ 2234 psentry
			s100 cvs -print
		 psexit } 
		{ 2237 psentry
			dup 100 gt
			{ 2239 psentry
				(0) -print s100 cvs -print
			 psexit } 
			{ 2242 psentry
				dup 10 gt
				{ 2244 psentry
					(00) -print s100 cvs -print
				 psexit } 
				{ 2247 psentry
					(000) -print s100 cvs -print
				 psexit } 
				ifelse 
			 psexit } 
			ifelse 
		 psexit } 
		ifelse 
	 psexit } 
	{ 2256 psentry
		s100 cvs -print
	 psexit } 
	ifelse
 psexit } 
def
/TracePoints 
{ 2264 psentry
	-1 1 
	{ 2266 psentry
		2 mul -2 roll     % use the loop control variable to roll up a point (x,y) from bottom
		transform  % transform to device coordinates
		matrix defaultmatrix 
		itransform
		exch
		true % pstoedit.dotrace cvi 1 eq 
		{ 2275 psentry
			dup dup pstoedit.ll.x cvi -lt 
			{ 2277 psentry
				pstoedit.bbclear pstoedit.ll.x copy pop 100 -string cvs pstoedit.ll.x copy pop 
			 psexit } 
			{ 2280 psentry
				pop 
			 psexit } 
			ifelse
			dup dup pstoedit.ur.x cvi gt 
			{ 2285 psentry
				pstoedit.bbclear pstoedit.ur.x copy pop 100 -string cvs pstoedit.ur.x copy pop 
			 psexit } 
			{ 2288 psentry
				pop 
			 psexit } 
			ifelse
		 psexit } 
		if
		pop %printTOS   % the x value
		true % pstoedit.dotrace cvi 1 eq 
		{ 2296 psentry
			dup dup pstoedit.ll.y cvi -lt 
			{ 2298 psentry
				pstoedit.bbclear pstoedit.ll.y copy pop 100 -string cvs pstoedit.ll.y copy pop 
			 psexit } 
			{ 2301 psentry
				pop 
			 psexit } 
			ifelse
			dup dup pstoedit.ur.y cvi gt 
			{ 2306 psentry
				pstoedit.bbclear pstoedit.ur.y copy pop 100 -string cvs pstoedit.ur.y copy pop 
			 psexit } 
			{ 2309 psentry
				pop 
			 psexit } 
			ifelse
		 psexit } 
		if
		pop %printTOS   % the y value
	 psexit } 
	for
 psexit } 
def
/prpoints 
{ 2322 psentry
	-1 1 
	{ 2324 psentry
		2 mul -2 roll     % use the loop control variable to roll up a point (x,y) from bottom
		transform  % transform to device coordinates
		matrix defaultmatrix 
		itransform
		exch
		pstoedit.dotrace cvi 1 eq 
		{ 2333 psentry
			dup dup pstoedit.ll.x cvi -lt 
			{ 2335 psentry
				pstoedit.bbclear pstoedit.ll.x copy pop 100 -string cvs pstoedit.ll.x copy pop 
			 psexit } 
			{ 2338 psentry
				pop 
			 psexit } 
			ifelse
			dup dup pstoedit.ur.x cvi gt 
			{ 2343 psentry
				pstoedit.bbclear pstoedit.ur.x copy pop 100 -string cvs pstoedit.ur.x copy pop 
			 psexit } 
			{ 2346 psentry
				pop 
			 psexit } 
			ifelse
		 psexit } 
		if
		printTOS   % the x value %woglmul
		pstoedit.dotrace cvi 1 eq 
		{ 2354 psentry
			dup dup pstoedit.ll.y cvi -lt 
			{ 2356 psentry
				pstoedit.bbclear pstoedit.ll.y copy pop 100 -string cvs pstoedit.ll.y copy pop 
			 psexit } 
			{ 2359 psentry
				pop 
			 psexit } 
			ifelse
			dup dup pstoedit.ur.y cvi gt 
			{ 2364 psentry
				pstoedit.bbclear pstoedit.ur.y copy pop 100 -string cvs pstoedit.ur.y copy pop 
			 psexit } 
			{ 2367 psentry
				pop 
			 psexit } 
			ifelse
		 psexit } 
		if
		printTOS   % the y value
	 psexit } 
	for
 psexit } 
def
/prpointswithtrace 
{ 2381 psentry
	1 1 -string cvs pstoedit.dotrace copy pop
	prpoints
 psexit } 
def
/prpointsnotrace 
{ 2389 psentry
	1 1 -string cvs pstoedit.dotrace copy pop
	prpoints
 psexit } 
def
/getcurrentscale 
{ 2397 psentry
	100 0 transform % x1' y1'
	/pstoedit.y1 exch store
	/pstoedit.x1 exch store
	0 0 transform % x2' y2'
	/pstoedit.y2 exch store
	/pstoedit.x2 exch store
	pstoedit.x1 pstoedit.x2 sub dup mul 
	pstoedit.y1 pstoedit.y2 sub dup mul 
	add sqrt
	100 div abs
	pstoedit.dpiscale div
 psexit } 
def
/printstate 
{ 2416 psentry
	pstoedit.computeDPIscale 
	-gsave
	pstoedit.deviceinfoshown not 
	{ 2421 psentry
		/pstoedit.deviceinfoshown true store
		languagelevel 1 gt 
		{ 2424 psentry
			pstoedit.DEVICEINFO /PageSize known 
			{ 2426 psentry
				pstoedit.DEVICEINFO /PageSize get
				pstoedit.deviceheight 0.0 ne 
				{ 2432 psentry
					dup 1 get 
					pstoedit.dpiscale mul 
					/pstoedit.deviceheight exch store
				 psexit } 
				if
				{ 2448 psentry
					printTOS
				 psexit } 
				forall
				( setPageSize\n) -print
			 psexit } 
			if
		 psexit } 
		if
	 psexit } 
	if
	currentlinecap
	printTOS
	( setlinecap\n) -print 
	currentmiterlimit
	printTOS
	( setmiterlimit\n) -print 
	currentlinejoin
	printTOS
	( setlinejoin\n) -print 
	currentdash
	exch
	( [) -print
	{ 2475 psentry
		getcurrentscale mul
		printTOS
	 psexit } 
	forall % dump array
	( ]) -print
	printTOS %index
	( setdash\n) -print
	currentlinewidth
	getcurrentscale mul
	printTOS
	( setlinewidth\n) -print
	pstoedit.useRGBcolors 
	{ 2490 psentry
		-currentrgbcolor
		3 1 roll exch
		printTOS
		printTOS
		printTOS
		( setrgbcolor\n) -print
	 psexit } 
	{ 2499 psentry
		-currentcmykcolor	% c m y k
		exch 4 1 roll		% y c m k
		4 1 roll			% k y c m
		exch				% k y m c
		printTOS 
		printTOS
		printTOS
		printTOS
		( setcmykcolor\n) -print
	 psexit } 
	ifelse
	currentcolorspace 0 get /Separation eq 
	{ 2513 psentry
		currentcolorspace 1 get
		(\() -print s100 cvs -print (\)) -print 
		( p2esetcolorname\n) -print
	 psexit } 
	{ 2520 psentry
		(\(\)) -print ( p2esetcolorname\n) -print
	 psexit } 
	ifelse
	false {
	  (got layer info) pstack pop 	
	} {
	} ifelse
	-grestore
 psexit } 
def
/print-path 
{ 2530 psentry
	{ 2532 psentry
		{ 2533 psentry
			1 prpointswithtrace ( moveto\n)    -print 
		 psexit } 
		{ 2536 psentry
			1 prpointswithtrace ( lineto\n)    -print 
		 psexit } 
		{ 2539 psentry
			3 prpointswithtrace ( curveto\n)   -print 
		 psexit } 
		{ 2542 psentry
			( closepath\n) -print 
		 psexit } 
		pathforall
	 psexit } 
	stopped 
	{ 2548 psentry
	 psexit } 
	if
 psexit } 
def
/dumppath 
{ 2557 psentry
	( \n) -print
	/pstoedit.dumpstring exch store  % (clip)
	/pstoedit.dumpop exch store  % {clip}
	( \n) -print
	(\% ) -print
	pstoedit.pathnumber cvi 1 add 100 -string cvs pstoedit.pathnumber copy pop
	pstoedit.pathnumber -print 
	( pathnumber\n) -print
	-print % new place
	printstate
	-gsave
	{ 2571 psentry
		pstoedit.doflatten 
		{ 2573 psentry
			pstoedit.flatnesstouse setflat
			flattenpath % replaces curves by lines
		 psexit } 
		if
	 psexit } 
	stopped 
	{ 2580 psentry
		-grestore
		-gsave
	 psexit } 
	if
	(newpath\n) -print
	pstoedit.simulateclipping 
	{ 2590 psentry
		clip clippath 
	 psexit } 
	if
	print-path
	pstoedit.dumpstring -print (\n) -print
	-grestore
	pstoedit.dumpop
 psexit } 
def
false % use the clip tracing instead
{ 2603 psentry
	/-showclippath 
	{ 2605 psentry
		-gsave ( gsave \n) -print  
		clippath clip clippath 
		(% clippath\n) 
		{ 2610 psentry
			clip
		 psexit } 
		(clip) dumppath 
		-grestore
	 psexit } 
	def
	/-endshowclippath 
	{ 2619 psentry
		(\ngrestore\n) -print
	 psexit } 
	def
 psexit } 
{ 2624 psentry
	/-showclippath 
	{ 2626 psentry
	 psexit } 
	def
	/-endshowclippath 
	{ 2630 psentry
	 psexit } 
	def
 psexit } 
ifelse
/pstoedit.charpathwarningdone false store
/pstoedit.charpatherrorcaught false store
/charpath 
{ 2639 psentry
	/pstoedit.ignoredraw true  store
	/pstoedit.charpatherrorcaught false store
	pstoedit.fontdebugging 
	{ 2643 psentry
		(% start charpath ) pstack pop
		(% start charpath \n) -print 
	 psexit } 
	if
	true % with error handling
	{ 2649 psentry
		{ 2650 psentry
			-charpath 
		 psexit } 
		stopped
		{ 2654 psentry
			/pstoedit.charpatherrorcaught true store %% for later usage in place where charpath was called
			pstoedit.charpathwarningdone not pstoedit.verbosemode or
			{ 2658 psentry
				pstoedit.verbosemode 
				{ 2660 psentry
					errordict begin handleerror end
				 psexit } 
				if
				(Problem occurred in charpath (maybe due to protected fonts) - this warning is given only once per page!! Try again using -ndt option\n) true printwarning %% wogl remove? ?
				/pstoedit.charpathwarningdone true store
			 psexit } 
			if
		 psexit } 
		if
	 psexit } 
	{ 2672 psentry
		-charpath 
	 psexit } 
	ifelse
	pstoedit.fontdebugging 
	{ 2678 psentry
		(% end charpath ) pstack pop
		(% end charpath \n) -print 
	 psexit } 
	if
	/pstoedit.ignoredraw false  store
 psexit } 
overload
/stringwidth 
{ 2688 psentry
	/pstoedit.ignoredraw true  store
	-stringwidth
	/pstoedit.ignoredraw false  store
 psexit } 
overload
/eofill	
{ 2697 psentry
	pstoedit.ignoredraw 
	{ 2700 psentry
		-eofill
	 psexit } 
	{ 2704 psentry
		-showclippath
		(\% eofilledpath\n) 
		{ 2707 psentry
			-eofill
		 psexit } 
		(eofill) dumppath
		-endshowclippath
	 psexit } 
	ifelse
 psexit } 
overload
/fill  	
{ 2718 psentry
	pstoedit.ignoredraw 
	{ 2722 psentry
		-fill
	 psexit } 
	{ 2726 psentry
		-showclippath
		(\% filledpath\n) 
		{ 2729 psentry
			-fill
		 psexit } 
		(fill) dumppath
		-endshowclippath
	 psexit } 
	ifelse
 psexit } 
overload
/pstoedit.fill 
{ 2740 psentry
	fill 
 psexit } 
def %% needed to avoid a too early binding e.g. in rectfill
/pstoedit.eofill 
{ 2745 psentry
	eofill 
 psexit } 
def %% needed to avoid a too early binding e.g. in rectfill
/stroke	
{ 2751 psentry
	pstoedit.ignoredraw 
	{ 2753 psentry
		-stroke
	 psexit } 
	{ 2757 psentry
		-showclippath
		(\% strokedpath\n) 
		{ 2760 psentry
			/pstoedit.hidestroke where %% this could come from an additional prologue file via -include 
			{ 2762 psentry
				pop % the dict returned by where
				pstoedit.hidestroke not
				{ 2765 psentry
					-stroke
				 psexit } 
				if		
			 psexit } 
			{ 2770 psentry
				-stroke
			 psexit } 
			ifelse
		 psexit } 
		(stroke) dumppath
		-endshowclippath
	 psexit } 
	ifelse
 psexit } 
overload
/pstoedit.stroke 
{ 2782 psentry
	stroke 
 psexit } 
def 
/pstoeditdummystroke 
{ 2788 psentry
	pstoedit.ignoredraw 
	{ 2791 psentry
		-stroke
	 psexit } 
	{ 2795 psentry
		(\% dummystrokedpath\n) 
		{ 2798 psentry
			newpath
		 psexit } 
		(pstoeditdummystroke) dumppath
	 psexit } 
	ifelse
 psexit } 
def
/pstoeditdummystrokewithoutput	
{ 2809 psentry
	{ 2812 psentry
		{ 2813 psentry
			1 TracePoints  
		 psexit } 
		{ 2817 psentry
			1 TracePoints  
		 psexit } 
		{ 2821 psentry
			3 TracePoints  
		 psexit } 
		{ 2825 psentry
		 psexit } 
		pathforall
	 psexit } 
	stopped 
	{ 2831 psentry
	 psexit } 
	if
 psexit } 
def
/pstoedit.forcedrawtext 
{ 2838 psentry
	currentfont begin
	/FontName where 
	{ 2841 psentry
		pop % the dict
		FontName 100 -string cvs % FontName is a name or is already a string
		cvn dup 
		/pstoedit.knownFontNames where 
		{ 2851 psentry
			pop % the dict
			pstoedit.knownFontNames exch known 
			{ 2857 psentry
				pop
				false % do not force drawing mode
			 psexit } 
			{ 2863 psentry
				rootFont 
				pstoedit.knownFontNames exch known 
				{ 2868 psentry
					false % do not force drawing mode
				 psexit } 
				{ 2872 psentry
					true
				 psexit } 
				ifelse
			 psexit } 
			ifelse
		 psexit } 
		{ 2879 psentry
			pop pop
			false % if pstoedit.knownFontNames is not defined, 'show' text
		 psexit } 
		ifelse
	 psexit } 
	{ 2886 psentry
		true % if FontName is not known
	 psexit } 
	ifelse 
	end
	CheckForTextFontVerbose not
	pstoedit.autodrawtext   % if no text font and autodraw = true, then draw the text 
	and
	or 
 psexit } 
def
/getorigfont 
{ 2911 psentry
	dup /OrigFont known
	{ 2918 psentry
		dup % for comparison
		dup /OrigFont get ne  % ne is a weak check against recursion but is was needed from gs8.56
		{ 2921 psentry
			/OrigFont get getorigfont 
		 psexit } 
		if
	 psexit } 
	if
 psexit } 
def
/isKnownFontName 
{ 2932 psentry
	/Fontmap where 
	{ 2939 psentry
		pop % the dict from where
		dup % for second check
		Fontmap exch known
	 psexit } 
	{ 2944 psentry
		false % nothing found so far
	 psexit } 
	ifelse
	{ 2952 psentry
		pop % the fontname
		true % just repush true
	 psexit } 
	{ 2956 psentry
		/pstoedit.knownFontNames where 
		{ 2958 psentry
			pop % the dict from where
			pstoedit.knownFontNames exch known
		 psexit } 
		{ 2962 psentry
			pop % the fontname
			false % nothing found so far
		 psexit } 
		ifelse
	 psexit } 
	ifelse
 psexit } 
def 
/pstoedit.handlepstoedit.textastext
{ 2973 psentry
	pstoedit.fontdebugging 
	{ 2977 psentry
		(pstoedit.handlepstoedit.textastext ) pstoedit.currentstring pstack pop pop
	 psexit } 
	if
	currentfont begin
	/FontName where 
	{ 2984 psentry
		pop FontName 
		pstoedit.checkfontnames 
		{ 2987 psentry
			dup 
			isKnownFontName  
		 psexit } 
		{ 2993 psentry
			true
		 psexit } 
		ifelse
		{ 2999 psentry
			100 -string cvs % FontName is a name or is already a string
		 psexit } 
		{ 3003 psentry
			100 -string cvs % FontName is a name or is already a string
			dup
			cvn
			rootFont 
			(\% ) -print
			s100 cvs -print ( ) -print dup -print 
			( nonstandard font\n) -print
		 psexit } 
		ifelse
		/pstoedit.replacedfont false store  
	 psexit } 
	{ 3019 psentry
		pstoedit.fontdebugging 
		{ 3021 psentry
			(using replacement font because currentfont has no FontName) pstack pop 
		 psexit } 
		if
		pstoedit.replacementfont
		/pstoedit.replacedfont true  store  
	 psexit } 
	ifelse 
	(\() -print
	-print
	(\) cvn) -print
	end
	( findfont ) -print
	pstoedit.replacedfont 
	{ 3038 psentry
		pstoedit.currentstring -stringwidth
		dtransform
		dup mul exch dup mul add sqrt
		-gsave
		pstoedit.replacementfont cvn findfont 10 scalefont setfont
		pstoedit.currentstring normalizestring -stringwidth 
		dtransform
		dup mul exch dup mul add sqrt
		-grestore
		div 10 mul getcurrentscale mul 
		pstoedit.scalefontversion 
		{ 3062 psentry
			s100 cvs -print ( scalefont setfont\n) -print
		 psexit } 
		{ 3065 psentry
			([ ) -print dup printTOS 0 printTOS 0 printTOS printTOS 
			-gsave
			currentpoint translate
			currentfont /FontMatrix get -concat 0 0 transform
			-grestore
			itransform
			1 prpointsnotrace
			( ] makefont setfont\n) -print
		 psexit } 
		ifelse
	 psexit } 
	{ 3077 psentry
		currentfont begin
		currentfont /FontType get 42 eq 
		{ 3087 psentry
			/pstoedit.fontunit 1 store
		 psexit } 
		{ 3091 psentry
			/pstoedit.fontunit 1000 store
		 psexit } 
		ifelse
		pstoedit.scalefontversion %wogl
		{ 3098 psentry
			FontMatrix 0 get % get the x-scale of the font matrix
			0
			dtransform
			dup mul exch dup mul add sqrt
			pstoedit.fontunit mul 
			pstoedit.dpiscale div
			s100 cvs -print ( scalefont setfont\n) -print
		 psexit } 
		{ 3110 psentry
			-gsave
			currentpoint 1 TracePoints % tracing also the starting point of text into BB information
			currentpoint translate
			currentfont /FontMatrix get -concat
			currentfont getorigfont /FontMatrix get matrix invertmatrix -concat
			matrix currentmatrix matrix defaultmatrix matrix invertmatrix matrix concatmatrix %xxtemp
			-grestore
			([ ) -print 
			{ 3122 psentry
				printTOS 
			 psexit } 
			forall
			( ] makefont setfont\n) -print
		 psexit } 
		ifelse
		end
	 psexit } 
	ifelse
	currentfont begin
	/FontInfo where 
	{ 3139 psentry
		pop 
		FontInfo /FamilyName known 
		{ 3145 psentry
			FontInfo /FamilyName get 100 -string cvs
		 psexit } 
		{ 3148 psentry
			(unknown) 
		 psexit } 
		ifelse
		(\% ) -print -print  ( FamilyName\n) -print
		FontInfo /FullName   known 
		{ 3154 psentry
			FontInfo /FullName get 100 -string cvs
		 psexit } 
		{ 3157 psentry
			(unknown) 
		 psexit } 
		ifelse   
		(\% ) -print -print  ( FullName\n) -print
		FontInfo /Weight     known 
		{ 3163 psentry
			FontInfo /Weight get 100 -string cvs
		 psexit } 
		{ 3166 psentry
			(unknown) 
		 psexit } 
		ifelse
		(\% ) -print -print  ( Weight\n) -print
	 psexit } 
	if
	end
	pstoedit.scalefontversion 
	{ 3176 psentry
		/pstoedit.angle 1 0 dtransform exch atan -1 mul 360 add store
		languagelevel 1 gt 
		{ 3182 psentry
			pstoedit.DEVICEINFO dup /Orientation known 
			{ 3184 psentry
				/Orientation get 
				-90 mul pstoedit.angle add /pstoedit.angle exch store 
				pstoedit.angle 0 -lt 
				{ 3188 psentry
					pstoedit.angle 360 add /pstoedit.angle exch store 
				 psexit } 
				if
			 psexit } 
			{ 3193 psentry
				pop % the duped DEVICEINFO
			 psexit } 
			ifelse
		 psexit } 
		if
		-gsave
		currentpoint translate
		currentfont /FontMatrix get -concat 0 0 transform
		-grestore 
		itransform
		/pstoedit.y exch store /pstoedit.x exch store
		pstoedit.x pstoedit.y 1 prpointswithtrace ( moveto\n)    -print
		pstoedit.angle 360.0 ne 
		{ 3216 psentry
			pstoedit.x pstoedit.y 1 prpointswithtrace ( translate\n) -print
			( ) -print
			pstoedit.angle s100 cvs -print ( rotate\n) -print
		 psexit } 
		{ 3221 psentry
			( ) -print
			0             s100 cvs -print ( rotate\n) -print
		 psexit } 
		ifelse
	 psexit } 
	{ 3228 psentry
		(  0 0 moveto\n)    -print
	 psexit } 
	ifelse 
	getcurrentscale dup /pstoedit.currentscale exch store
	mul
	exch
	pstoedit.currentscale mul
	5 -2 roll 
	pstoedit.currentscale mul
	exch
	pstoedit.currentscale mul
	printTOS printTOS % c
	3 -1 roll
	printTOS	% char
	printTOS printTOS % a
 psexit } 
def
/dumptextstring 
{ 3259 psentry
	pstoedit.escapetext
	{ 3261 psentry
		pstoedit.currentstring 
		{ 3264 psentry
			inttosinglechar
			dup
			3 normalizecharacter_wrapper
			{ 3273 psentry
				exch pop % remove plain c since we can use the mapped one.
				dup dup (\r) eq exch (\n) eq or
				{ 3280 psentry
					pop 
				 psexit } 
				{ 3283 psentry
					dup (\() eq 
					{ 3285 psentry
						(\\) -print 
					 psexit } 
					if
					dup (\)) eq 
					{ 3290 psentry
						(\\) -print 
					 psexit } 
					if
					dup (\\) eq 
					{ 3295 psentry
						(\\) -print 
					 psexit } 
					if
					-print 
				 psexit } 
				ifelse
			 psexit } 
			{ 3304 psentry
				dup (\r) eq exch (\n) eq or 
				{ 3309 psentry
				 psexit } 
				{ 3312 psentry
					pstoedit.unmappablecharacter -print
				 psexit } 
			 psexit } 
			ifelse
		 psexit } 
		forall
	 psexit } 
	{ 3321 psentry
		pstoedit.currentstring 
		{ 3324 psentry
			inttosinglechar
			dup
			4 normalizecharacter_wrapper
			{ 3331 psentry
				exch pop % remove plain c since we can use the mapped one.
				dup dup (\r) eq exch (\n) eq or
				{ 3338 psentry
					pop 
				 psexit } 
				{ 3341 psentry
					-print 
				 psexit } 
				ifelse
			 psexit } 
			{ 3347 psentry
				dup (\r) eq exch (\n) eq or 
				{ 3352 psentry
				 psexit } 
				{ 3355 psentry
					pstoedit.unmappablecharacter -print
				 psexit } 
				ifelse 
			 psexit } 
			ifelse
		 psexit } 
		forall
	 psexit } 
	ifelse
 psexit } 
def			
/effectiveawidthshow 
{ 3371 psentry
	/pstoedit.ignoreimage true  store %ignore all image* ops called from here
	pstoedit.fontdebugging
	{ 3374 psentry
		(start awidthshow) pstack pop 
		(% start awidthshow \n) -print
	 psexit } 
	if
	( \n) -print
	printstate
	/pstoedit.currentstring exch store
	pstoedit.fontdebugging
	{ 3384 psentry
		GetCurrentFontName pstoedit.currentstring (unnormalized:) pstack pop pop pop
		currentfont /Encoding get pstack pop
		(glphynames:) pstack pop
		pstoedit.currentstring 
		{ 3389 psentry
			currentfont /Encoding get exch 
			saveGET % get 
			(gl:) pstack pop pop
		 psexit } 
		forall
	 psexit } 
	if
	5 copy % save for real operator
	-gsave
	pstoedit.textastext 			% text shall be shown as text and
	currentfont /FontType get 1 eq 	% current font is type 1
	pstoedit.t2fontsast1 % we cannot do this all times - at least up to gs6.0 this sometimes leads
	currentfont /FontType get 2 eq 	% current font is type 2 (embedded reduced type 1)
	and
	or
	currentfont /FontType get 42 eq % current font is type 42 (TT)
	or
	pstoedit.forcedrawtext not	% and not forced draw due to font restrictions
	and
	currentfont /FontType get 3 eq 	% current font is type 3
	currentfont /BitMaps known   	% and contains a BitMaps entry (as dvips provides)
	and				% Bitmap fonts cannot be "drawn", anyway
	or				% (T1 or T2 or T42 and not force)  or (T3 and Bitmap)
	and				% pstoedit.textastext
	pstoedit.disabledrawtext or % if drawtext is fully disabled
	dup /pstoedit.textshown exch store %
	{ 3435 psentry
		pstoedit.handlepstoedit.textastext
	 psexit } 
	{ 3438 psentry
		-gsave
		currentpoint
		newpath
		moveto
		mark
		6 1 roll % first move the mark at begin
		5 -1 roll % then roll back the last 5
		pstoedit.currentstring 
		pstoedit.fontdebugging 
		{ 3462 psentry
			(pstoedit.currentstring 1) pstack pop
			currentfont /FontType get (FontType: ) pstack pop pop
			currentfont /FMapType get (FMapType: ) pstack pop pop
		 psexit } 
		if
		canIterateThroughString  %true means "draw each char separately"
		{ 3469 psentry
			{ 3478 psentry
				dup dup length 1 sub get % now last char is on stack as int.
				exch
				mark % insert a mark right before the two args to charpath. So whatever charpath does, we can clean up
				exch %
				true
				charpath 
				cleartomark
				3 index  % (compare :) pstack pop % wogldebug 
				eq 
				{ 3492 psentry
					4 index 4 index 
					rmoveto 
				 psexit } 
				if
				2 copy rmoveto
			 psexit } 
			pstoedit.FORALL % in effectiveawithshow
		 psexit } 
		{ 3504 psentry
			pstoedit.fontdebugging 
			{ 3506 psentry
				(Cannot iterate through string - need to handle string as a whole) pstack pop
				currentfont /FontType get (FontType: ) pstack pop pop
				currentfont /FMapType get (FMapType: ) pstack pop pop
			 psexit } 
			if
			true charpath 
		 psexit } 
		ifelse
		pop pop pop pop pop
		cleartomark
		pstoedit.charpatherrorcaught
		{ 3522 psentry
			(error caught during charpath. Trying to dump text as text - even if it may not be useful\n) true printwarning
			0 0 32 0 0 
			/pstoedit.textshown true store
			pstoedit.handlepstoedit.textastext
		 psexit } 
		{ 3530 psentry
			eofill 
		 psexit } 
		ifelse
		-grestore 
	 psexit } 
	ifelse
	-grestore
	pstoedit.currentstring
	/pstoedit.ignoredraw true  store
	pstoedit.fontdebugging 
	{ 3544 psentry
		(vor -awidthshow ) (top) pstack pop pop 
	 psexit } 
	if
	mark 7 1 roll % put mark in front of params
	{ 3549 psentry
		-awidthshow 
	 psexit } 
	stopped 
	{ 3554 psentry
		pstoedit.verbosemode 
		{ 3556 psentry
			errordict begin handleerror end
			(Problem occurred in awidthshow) print 
		 psexit } 
		if
	 psexit } 
	if
	cleartomark
	pstoedit.fontdebugging 
	{ 3565 psentry
		(nach -awidthshow ) (top) pstack pop pop 
	 psexit } 
	if
	/pstoedit.ignoredraw false store
	/pstoedit.ignoreimage false store 
	pstoedit.textshown
	{ 3574 psentry
		-gsave
		currentpoint translate
		currentfont /FontMatrix get -concat 0 0 transform
		-grestore 
		itransform
		1 prpointswithtrace
		CheckForTextFont pstoedit.maptoisolatin1 and 
		{ 3590 psentry
			( 1 )
		 psexit } 
		{ 3595 psentry
			( 0 ) 
		 psexit } 
		ifelse -print
		( setshowparams\n) -print
		CheckForTextFontVerbose pop % we do not need the result here
		pstoedit.fontdebugging 
		{ 3603 psentry
			(vor dumping text ) pstoedit.currentstring (top) pstack pop pop pop 
		 psexit } 
		if
		pstoedit.passglyphnames
		{ 3608 psentry
			(\() -print
			pstoedit.currentstring -printhex
			(\) \[) -print
			pstoedit.currentstring 
			{ 3613 psentry
				currentfont /Encoding get 
				exch get
				( /) -print
				1000 -string cvs -print
			 psexit } 
			forall
			( \] awidthshowhexwithglyphs\n) -print
		 psexit } 
		{ 3624 psentry
			pstoedit.ashexstring 
			{ 3627 psentry
				(\() -print
				pstoedit.currentstring -printhex
				(\) awidthshowhex\n) -print
			 psexit } 
			{ 3633 psentry
				(\() -print
				dumptextstring
				(\) awidthshow\n) -print
			 psexit } 
			ifelse
		 psexit } 
		ifelse % write glyphnames
		pstoedit.fontdebugging 
		{ 3644 psentry
			(nach dumping text ) pstoedit.currentstring (top) pstack pop pop pop 
		 psexit } 
		if
		pstoedit.scalefontversion 
		{ 3650 psentry
			pstoedit.angle 360.0 ne 
			{ 3652 psentry
				( ) -print
				pstoedit.angle neg s100 cvs -print ( rotate\n) -print
				pstoedit.x pstoedit.y 1 prpointswithtrace
				( ntranslate\n) -print
			 psexit } 
			if
		 psexit } 
		if
	 psexit } 
	if
	pstoedit.fontdebugging
	{ 3664 psentry
		(end awidthshow) pstack pop 
		(% end awidthshow \n) -print
	 psexit } 
	if
 psexit } 
def
/precisionshowchar 
{ 3674 psentry
	0 0 32 0 0   6 -1 roll 
	effectiveawidthshow 
 psexit } 
def
/pstoedit.showcharacter ( ) def % needed because forall in awidthshow gives ints, but show needs char
/pstoedit.showcharacter2 ( ) def % needed for type 0 fonts 
/awidthshow 
{ 3684 psentry
	-showclippath
	pstoedit.precisiontext 0 eq   % no precision text    		% cx cy char ax ay string b1
	pstoedit.precisiontext 1 eq   % pti and cx cy ax ay are all 0	% cx cy char ax ay string b1 b2
	7 index 0 eq 						% cx cy char ax ay string b1 b2 b3
	7 index 0 eq 						% cx cy char ax ay string b1 b2 b3 b4
	and						% cx cy char ax ay string b1 b2 b3*b4
	5 index 0 eq 						% cx cy char ax ay string b1 b2 b3*b4 b5
	5 index 0 eq 						% cx cy char ax ay string b1 b2 b3*b4 b5 b6
	and 						% cx cy char ax ay string b1 b2 b3*b4 b5*b6
	and 						% cx cy char ax ay string b1 b2 b3*b4*b5*b6
	and 						% cx cy char ax ay string b1 b2*b3*b4*b5*b6
	or 						% cx cy char ax ay string b1+b2*b3*b4*b5*b6
	canIterateThroughString not or % or if we cannot iterate anyway
	{ 3701 psentry
		effectiveawidthshow 
	 psexit } 
	{ 3705 psentry
		{ 3709 psentry
			dup dup length 1 sub get % now last char is on stack as int.
			inttosinglechar
			precisionshowchar 
			3 index  % (compare :) pstack pop % wogldebug 
			eq 
			{ 3719 psentry
				4 index 4 index 
				rmoveto  
			 psexit } 
			if
			2 copy rmoveto
		 psexit } 
		pstoedit.FORALL % in awidthshow
		pop pop pop pop pop
	 psexit } 
	ifelse
	-endshowclippath
 psexit } 
overload
/widthshow  
{ 3738 psentry
	0 exch 0 exch awidthshow 
 psexit } 
overload
/.pdfwidthshow {
  widthshow
} overload
/.pdfawidthshow {
  awidthshow
} overload
/ashow      
{ 3745 psentry
	0 0 32 
	6 3 roll awidthshow 
 psexit } 
overload
/show	    
{ 3752 psentry
	0 0 32 0 0   6 -1 roll awidthshow 
 psexit } 
overload
/debugcshow 
{ 3759 psentry
	(in cshow1 :) pstack pop
	-cshow
	(out cshow :) pstack pop
 psexit } 
overload
/pstoedit.kshowproc 
{ 3772 psentry
 psexit } 
store
/pstoedit.kshowclast 1  store
/kshow  
{ 3778 psentry
	exch /pstoedit.kshowproc exch store 
	dup length 0 gt 
	{ 3784 psentry
		true  % for first
		exch
		{ 3789 psentry
			exch
			{ 3793 psentry
				dup /pstoedit.kshowclast exch store
				inttosinglechar
				show
				pstoedit.kshowclast
				false
			 psexit } 
			{ 3800 psentry
				dup /pstoedit.kshowclast exch store
				pstoedit.kshowproc
				pstoedit.kshowclast inttosinglechar
				show
				pstoedit.kshowclast
				false
			 psexit } 
			ifelse
		 psexit } 
		forall
		pop pop % char and first
	 psexit } 
	{ 3815 psentry
		(ignoring kshow for empty string\n) true printwarning
		pop % pstoedit.kshowproc % just pop the empty string
	 psexit } 
	ifelse
 psexit } 
overload
/pstoedit.xyshowindex 0  store
/pstoedit.xyshowarray [ 0 ]  store
/saveGET 
{ 3828 psentry
	2 copy % a i a i
	exch % a i i a 
	length % a i i l
	lt 
	{ 3840 psentry
		get 
	 psexit } 
	{ 3843 psentry
		pop pop % throw away the args and return 0
		0
	 psexit } 
	ifelse
 psexit } 
def
/xshow 
{ 3853 psentry
	dup type 
	/arraytype eq 
	canIterateThroughString and % and if we can iterate 
	{ 3862 psentry
		/pstoedit.xyshowarray exch store
		/pstoedit.xyshowindex 0  store
		{ 3866 psentry
			currentpoint
			3 -1 roll
			show
			exch
			pstoedit.xyshowarray pstoedit.xyshowindex 
			saveGET add exch
			moveto
			/pstoedit.xyshowindex pstoedit.xyshowindex 1 add store
		 psexit } 
		pstoedit.FORALL % in xshow - needed because every char is placed with individual with as given in array.
	 psexit } 
	{ 3883 psentry
		pstoedit.verbosemode 
		{ 3886 psentry
			(numstrings in xshow are not fully supported yet or iteration through string not supported yet\n) true printwarning 
		 psexit } 
		if
		pop show
	 psexit } 
	ifelse
 psexit } 
overload
/yshow 
{ 3899 psentry
	dup type /arraytype eq 
	canIterateThroughString and % and if we can iterate 
	{ 3905 psentry
		/pstoedit.xyshowarray exch store
		/pstoedit.xyshowindex 0  store
		{ 3908 psentry
			currentpoint
			3 -1 roll
			show
			pstoedit.xyshowarray pstoedit.xyshowindex saveGET add 
			moveto
			/pstoedit.xyshowindex pstoedit.xyshowindex 1 add store
		 psexit } 
		pstoedit.FORALL % in yshow - needed because every char is placed with individual with as given in array.
	 psexit } 
	{ 3922 psentry
		pstoedit.verbosemode 
		{ 3925 psentry
			(numstrings in yshow are not fully supported yet or iteration through string not supported yet\n) true printwarning 
		 psexit } 
		if
		pop show
	 psexit } 
	ifelse
 psexit } 
overload
/xyshow 
{ 3936 psentry
	dup type
	/arraytype eq 
	canIterateThroughString and % and if we can iterate 
	{ 3945 psentry
		/pstoedit.xyshowarray exch store
		/pstoedit.xyshowindex 0  store
		{ 3948 psentry
			currentpoint
			3 -1 roll
			show
			pstoedit.xyshowarray pstoedit.xyshowindex 1 add saveGET add exch
			pstoedit.xyshowarray pstoedit.xyshowindex saveGET add exch
			moveto
			/pstoedit.xyshowindex pstoedit.xyshowindex 2 add store
		 psexit } 
		pstoedit.FORALL % in xyshow - needed because every char is placed with individual with as given in array.
	 psexit } 
	{ 3967 psentry
		pstoedit.verbosemode 
		{ 3970 psentry
			(numstrings in xyshow are not fully supported yet or iteration through string not supported yet\n) true printwarning  
		 psexit } 
		if
		pop show
	 psexit } 
	ifelse
 psexit } 
overload
/glyphshow 
{ 3981 psentry
	getglyphaschar show
 psexit } 
overload
/showpage 
{ 3990 psentry
	false 
	{ 3992 psentry
		gsave
		(\%\% Bounding Box Trace\n) -print
		newpath
		0.5 0.6 0.7 setrgbcolor
		pstoedit.ll.x cvi pstoedit.ll.y cvi moveto 
		pstoedit.ur.x cvi pstoedit.ur.y cvi lineto
		pstoedit.ll.x cvi pstoedit.ur.y cvi lineto
		pstoedit.ur.x cvi pstoedit.ll.y cvi lineto
		closepath pstoedit.stroke
		grestore
	 psexit } 
	if
	(showpage\n) -print
	-showpage
	pstoedit.pagetoextract pstoedit.pagenr cvi eq 
	pstoedit.useBBfrominput {
	/get_any_box where {
	    pop % the dict
		1 pdffindpage
		get_any_box 
		exch
		(%% used from PDF:) -printNC 100 -string cvs -printNC ( \n) -printNC
		dup 3 get exch
		dup 2 get exch
		dup 1 get exch
		0 get 
	} {
	  pstoedit.ur.y  cvi 
          pstoedit.ur.x  cvi 
	  pstoedit.ll.y  cvi 
	  pstoedit.ll.x  cvi 
	} ifelse
	} {
	  pstoedit.ur.y  cvi 
          pstoedit.ur.x  cvi 
	  pstoedit.ll.y  cvi 
	  pstoedit.ll.x  cvi 
	}ifelse
	true
	{ 4014 psentry
		(\%\%BoundingBox: ) -printNC %
		100 -string cvs -printNC ( ) -printNC % LLX
		100 -string cvs -printNC ( ) -printNC % LLY
		100 -string cvs -printNC ( ) -printNC % URX
		100 -string cvs -printNC ( \n) -printNC % URY
	 psexit 
	 } 
	if
	(\%\%Page: ) -printNC 
	pstoedit.pagenr cvi 100 -string cvs 
	dup -printNC 
	( ) -printNC 
	-printNC 
	(\n) -printNC
	pstoedit.pagenr cvi 1 add 100 -string cvs pstoedit.pagenr copy pop 
	pstoedit.verbosemode 
	{ 4037 psentry
		([) print pstoedit.pagenr cvi 100 -string cvs print (]) print 
	 psexit } 
	if
	{ 4043 psentry
		pstoedit.quit
	 psexit } 
	if
	pstoedit.ll.x    (999999            ) exch copy pop
	pstoedit.ll.y    (999999            ) exch copy pop
	pstoedit.ur.x    (0                 ) exch copy pop
	pstoedit.ur.y    (0                 ) exch copy pop
	pstoedit.bbclear (                  ) exch copy pop
	0 1 -string cvs pstoedit.somethingprinted copy pop % reset
 psexit } 
soverload
/copypage 
{ 4059 psentry
	(copypage is being handled as showpage. This may lead to different results if copypage was used to overlay multiple pages to one\n) true printwarning
	showpage
 psexit } 
soverload
pstoedit.withimages 
{ 4066 psentry
	/pstoedit.image.ncomp   0 store
	/pstoedit.image.nrds   0 store % number of data sources
	/pstoedit.image.multi   0 store
	/pstoedit.image.datasrc 0 store
	/pstoedit.image.mat	  0 store
	/pstoedit.image.bits    0 store
	/pstoedit.image.decode  0 store
	/pstoedit.image.height  0 store
	/pstoedit.image.width   0 store 
	/pstoedit.image.fourthparam 0 store
	/pstoedit.image.procname 0 store
	/pstoedit.image.isfile  true store
	/pstoedit.image.stringprefix 0 store
	/pstoedit.image.realproc 0 store
	/pstoedit.image.nr (0                 ) store
	/pstoedit.image.filesuffix (                  ) store
	/pstoedit.image.isimagemask true store % f for image - t for mask
	/pstoedit.image.outputfile 1000 -string store
	/pstoedit.imagestring 1 -string def
	/pstoedit.image.saveobject1 0 store %% these are used for save/restore
	/pstoedit.image.saveobject2 0 store
	/printdumponly 
	{ 4090 psentry
		pstoedit.escapetext 
		{ 4093 psentry
			pop 
		 psexit } 
		{ 4096 psentry
			-print 
		 psexit } 
		ifelse
	 psexit } 
	def
	/ImageTraceBoundingBox 
	{ 4104 psentry
		-gsave
		newpath
		0 0 moveto
		1 0 lineto
		1 1 lineto
		0 1 lineto
		closepath
		pstoeditdummystrokewithoutput
		-grestore
	 psexit } 
	def
	pstoedit.backendSupportsFileImages 
	{ 4122 psentry
		/pstoedit.image.imageproc 0 store
		/processAsFILEimage_core 
		{ 4125 psentry
			(before processAsFILEImage ) P2EVTRACE 
			/pstoedit.image.filesuffix exch store 
			/pstoedit.image.imageproc exch store
			pstoedit.image.nr cvi 1 add 100 -string cvs pstoedit.image.nr copy pop
			/pstoedit.image.outputfile pstoedit.nameOfOutputFilewithoutpercentD 
			pstoedit.image.filesuffix   %(_mdsi) 
			concatstrings 
			pstoedit.image.nr cvi 20 -string cvs concatstrings pstoedit.imagefilesuffix concatstrings store
			/pstoedit.image.stringprefix (%fileimage:) store
			pstoedit.escapetext not 
			{ 4144 psentry
				(\%fileimage:begin\n) -print
				-gsave
				pstoedit.image.mat matrix invertmatrix concat
				matrix currentmatrix matrix defaultmatrix matrix invertmatrix matrix concatmatrix
				(% DC ) -print 
				{ 4151 psentry
					printTOS 
				 psexit } 
				forall (\n) -print
				pstoedit.image.stringprefix printdumponly 
				(normalized image currentmatrix\n) printdumponly  
				-grestore
				(% DC ) -print pstoedit.image.mat  
				{ 4160 psentry
					printTOS 
				 psexit } 
				forall (\n) -print
				pstoedit.image.stringprefix -print (imagematrix\n) -print
				pstoedit.image.stringprefix -print (filename ) -print
				pstoedit.image.outputfile -print  (\n) -print
				(% DC ) printdumponly pstoedit.image.width  printTOS (\n) -print
				pstoedit.image.stringprefix printdumponly 
				(width\n) printdumponly 
				(% DC ) printdumponly pstoedit.image.height printTOS (\n) -print
				pstoedit.image.stringprefix printdumponly 
				(height\n) printdumponly 
				(\%fileimage:end\n) -print
			 psexit } 
			if
			(after dump1 ) P2EVTRACE 
			(0) pstoedit.writesaverestore  copy pop
			pstoedit.verbosemode 
			{ 4205 psentry
				(vor save) dumpcurrentCTM 
			 psexit } 
			if
			pstoedit.verbosemode 
			{ 4210 psentry
				(vm vor save 1) vmstatus pstack pop pop pop pop 
			 psexit } 
			if
			/pstoedit.image.saveobject1 -save store
			pstoedit.verbosemode 
			{ 4216 psentry
				(vm vor save 2) vmstatus pstack pop pop pop pop 
			 psexit } 
			if
			/pstoedit.image.saveobject2 -save store
			pstoedit.verbosemode 
			{ 4222 psentry
				(nach save) dumpcurrentCTM 
			 psexit } 
			if
			pstoedit.verbosemode 
			{ 4227 psentry
				(vm nach save2) vmstatus pstack pop pop pop pop 
			 psexit } 
			if
			true
			{ 4234 psentry
				pstoedit.verbosemode 
				{ 4236 psentry
					(opening image file:) print
					pstoedit.image.outputfile print (\n) print
					pstoedit.image.width pstoedit.image.height pstack pop pop
				 psexit } 
				if
				pstoedit.usefinddevice not % default dont use finddevice anymore use setpagedevice instead
				{ 4242 psentry
					currentcolorspace 
					<<
					/OutputFile pstoedit.image.outputfile
					/OutputDevice pstoedit.imagedevicename cvn
					/HWResolution [ 72 72 ]
					/PageSize [ pstoedit.image.width pstoedit.image.height ]
					>>	
					(before setpage device ) P2EVTRACE
					setpagedevice	
					(after  setpage device ) P2EVTRACE 
					setcolorspace 
				 psexit } 
				{ 4260 psentry
					mark
					/OutputFile pstoedit.image.outputfile
					/HWResolution [ 72 72 ]
					/PageSize [ pstoedit.image.width pstoedit.image.height ]
					pstoedit.imagedevicename
					finddevice % in processAsFILEimage_core
					pstoedit.verbosemode 
					(after finddevice ) P2EVTRACE
					putdeviceprops
					setdevice
				 psexit } 
				ifelse
			 psexit } 
			if
			false 
			{ 4311 psentry
				pstoedit.image.mat 3 get 0 -lt 
				{ 4314 psentry
					pstoedit.image.mat 5 get 0 eq 
					{ 4316 psentry
						(Info: correcting image matrix - did not follow PS conventions\n) print
						pstoedit.image.mat 5 pstoedit.image.mat 3 get -1 mul put
					 psexit } 
					if
				 psexit } 
				if
			 psexit } 
			if
			pstoedit.image.width pstoedit.image.height scale
			(before pstoedit.image.imageproc ) P2EVTRACE
			pstoedit.image.imageproc %wogl -colorimage
			(after pstoedit.image.imageproc ) P2EVTRACE 
			(image showpage) P2EVTRACE
			-showpage
			pstoedit.image.saveobject2 -restore
			pstoedit.image.saveobject1 -restore
			(1) pstoedit.writesaverestore copy pop
			(%end dealing with dumping image to file\n)  P2EVTRACE 
		 psexit } 
		def
		/processAsFILEimage_errhandling 
		{ 4410 psentry
			{ 4411 psentry
				processAsFILEimage_core
			 psexit } 
			stopped
			{ 4415 psentry
				(caught an error during processAsFILEimage) pstack pop
			 psexit } 
			if
		 psexit } 
		def
		/processAsFILEimage 
		{ 4422 psentry
			processAsFILEimage_core
		 psexit } 
		def
	 psexit } 
	if
	/colorimage 
	{ 4433 psentry
		printstate
		pstoedit.ignoredraw pstoedit.ignoreimage or 
		pstoedit.DrawGlyphBitmaps not and % if DrawGlyphBitmaps is true then do not suppress the images coming from glyphs
		pstoedit.DEVICEINFO /HWResolution known not or 
		{ 4442 psentry
			pstoedit.verbosemode 
			{ 4445 psentry
				(colorimage operation ignored (called from some glyph drawing function or HWResolution unknown)\n) print
				(ignoredraw : )     print pstoedit.ignoredraw 
				{ 4450 psentry
					(T)
				 psexit } 
				{ 4453 psentry
					(F)
				 psexit } 
				ifelse print (\n) print
				(ignoreimage : )    print pstoedit.ignoreimage 
				{ 4458 psentry
					(T)
				 psexit } 
				{ 4461 psentry
					(F)
				 psexit } 
				ifelse print (\n) print
				(HWResolution known : ) print pstoedit.DEVICEINFO /HWResolution known 
				{ 4466 psentry
					(T)
				 psexit } 
				{ 4469 psentry
					(F)
				 psexit } 
				ifelse print (\n) print
			 psexit } 
			if
			-colorimage
		 psexit } 
		{ 4479 psentry
			pstoedit.backendSupportsFileImages 
			{ 4485 psentry
				ImageTraceBoundingBox
				-showclippath
				/pstoedit.image.ncomp exch store
				/pstoedit.image.multi exch store
				pstoedit.image.multi 
				{ 4499 psentry
					pstoedit.image.ncomp
				 psexit } 
				{ 4503 psentry
					1
				 psexit } 
				ifelse
				/pstoedit.image.nrds  exch store
				pstoedit.image.nrds 4 add pstoedit.image.nrds roll
				/pstoedit.image.mat		exch store
				/pstoedit.image.bits	exch store
				/pstoedit.image.height	exch store
				/pstoedit.image.width	exch store
				pstoedit.image.width pstoedit.image.height pstoedit.image.bits 
				[ pstoedit.image.width 0 0 pstoedit.image.height -1 mul 0 pstoedit.image.height  ]
				pstoedit.image.nrds 4 add 4 roll
				pstoedit.image.multi pstoedit.image.ncomp 
				{ %nc 
					-colorimage 
				} 
				(_cim) % color image
				processAsFILEimage
			 psexit } 
			{ 4548 psentry
				exch dup 
				{ 4551 psentry
					(colorimage with more than one datasrc not supported for this backend due to lack of support for images on raster files\n) true printwarning
					exch -colorimage
				 psexit } 
				{ 4557 psentry
					ImageTraceBoundingBox
					-showclippath
					pstoedit.escapetext 
					{ 4562 psentry
						(gsave\n)  -print  
					 psexit } 
					{ 4565 psentry
						(\%colorimage:begin\n) -print 
					 psexit } 
					ifelse
					exch
					/pstoedit.image.ncomp   exch store
					/pstoedit.image.multi   exch store
					/pstoedit.image.datasrc exch store
					/pstoedit.image.mat	  exch store
					/pstoedit.image.bits    exch store
					/pstoedit.image.height  exch store
					/pstoedit.image.width   exch store 
					(% DC ) printdumponly pstoedit.image.width  printTOS (\n) -print
					(\%colorimage:width\n) printdumponly 
					(% DC ) printdumponly pstoedit.image.height printTOS (\n) -print
					(\%colorimage:height\n) printdumponly 
					(% DC ) printdumponly pstoedit.image.bits   printTOS (\n) -print
					(\%colorimage:bits\n) printdumponly 
					pstoedit.escapetext not 
					{ 4599 psentry
						(% DC ) -print pstoedit.image.mat 
						{ 4601 psentry
							printTOS 
						 psexit } 
						forall (\n) -print
						(\%colorimage:imagematrix\n) -print 
					 psexit } 
					if
					-gsave
					pstoedit.image.mat matrix invertmatrix concat
					matrix currentmatrix matrix defaultmatrix matrix invertmatrix matrix concatmatrix
					pstoedit.escapetext 
					{ 4613 psentry
						([ ) -print 
					 psexit } 
					{ 4616 psentry
						(% DC ) -print 
					 psexit } 
					ifelse 
					{ 4620 psentry
						printTOS 
					 psexit } 
					forall 
					pstoedit.escapetext 
					{ 4625 psentry
						(]  concat [ 1 0 0 1 0 0 ] ) -print 
					 psexit } 
					if
					(\n) -print
					(\%colorimage:normalized image currentmatrix\n) printdumponly  
					-grestore
					pstoedit.escapetext 
					{ 4634 psentry
						({ currentfile imagestring readhexstring pop }\n)  -print 
					 psexit } 
					if
					(% DC ) printdumponly  pstoedit.image.multi   printTOS (\n) -print
					(\%colorimage:multi\n) printdumponly 
					(% DC ) printdumponly pstoedit.image.ncomp   printTOS (\n) -print
					(\%colorimage:ncomp\n) printdumponly
					pstoedit.escapetext 
					{ 4643 psentry
						(colorimage\n) -print 
					 psexit } 
					if
					pstoedit.image.width pstoedit.image.height pstoedit.image.bits pstoedit.image.mat 
					{ 4649 psentry
						pstoedit.image.datasrc 
						dup type /filetype eq 
						{ 4652 psentry
							read % returns: int bool
							{ 4655 psentry
								pstoedit.imagestring exch 0 exch put pstoedit.imagestring
							 psexit } 
							{ 4658 psentry
								0 -string % return 0 string if eof is found
							 psexit } 
							ifelse
						 psexit } 
						if
						(\%colorimage:data ) printdumponly 
						dup 
						{ 4667 psentry
							dup 16 -lt 
							{ 4669 psentry
								(0) -print 
							 psexit } 
							if
							16 2 -string cvrs -print 
						 psexit } 
						forall
						(\n) -print 
					 psexit } 
					pstoedit.image.multi pstoedit.image.ncomp -colorimage
					pstoedit.escapetext 
					{ 4680 psentry
						(grestore\n)  -print  
					 psexit } 
					{ 4683 psentry
						(\%colorimage:end\n) -print 
					 psexit } 
					ifelse
				 psexit } 
				ifelse
			 psexit } 
			ifelse % raster file support
			-endshowclippath
		 psexit } 
		ifelse % ignoredraw
	 psexit } 
	soverload
	/ImageAndImagemaskdummy
	{ 4700 psentry
		(executing dummy ImageAndImagemask \n) print
		/pstoedit.ignoredraw true  store
		pop pop pop %pop
		pstack
		exec % -image
		/pstoedit.ignoredraw false  store
		(executing dummy ImageAndImagemask done \n) print
	 psexit } 
	def
	/ImageAndImagemask
	{ 4712 psentry
		/pstoedit.image.isimagemask exch store
		/pstoedit.image.fourthparam exch store
		/pstoedit.image.procname exch store
		/pstoedit.image.stringprefix exch store
		/pstoedit.image.realproc exch store
		pstoedit.ignoredraw pstoedit.ignoreimage or 
		pstoedit.DrawGlyphBitmaps not and % if DrawGlyphBitmaps is true then do not suppress the images coming from glyphs
		pstoedit.DEVICEINFO /HWResolution known not or %%%% test only pop false
		{ 4722 psentry
			pstoedit.verbosemode 
			{ 4724 psentry
				(image operation ignored (called from some glyph drawing function or HWResolution unknown)\n) print
				(ignoredraw : )     print pstoedit.ignoredraw 
				{ 4729 psentry
					(T)
				 psexit } 
				{ 4732 psentry
					(F)
				 psexit } 
				ifelse print (\n) print
				(pstoedit.DrawGlyphBitmaps : ) print pstoedit.DrawGlyphBitmaps
				{ 4738 psentry
					(T)
				 psexit } 
				{ 4741 psentry
					(F)
				 psexit } 
				ifelse print (\n) print
				(ignoreimage : )    print pstoedit.ignoreimage 
				{ 4748 psentry
					(T)
				 psexit } 
				{ 4751 psentry
					(F)
				 psexit } 
				ifelse print (\n) print
				(HWResolution known : ) print pstoedit.DEVICEINFO /HWResolution known 
				{ 4757 psentry
					(T)
				 psexit } 
				{ 4760 psentry
					(F)
				 psexit } 
				ifelse print (\n) print
			 psexit } 
			if
			pstoedit.image.realproc
		 psexit } 
		{ 4769 psentry
			pstoedit.backendSupportsFileImages 
			{ 4771 psentry
				ImageTraceBoundingBox
				-showclippath
				dup type
				pstoedit.verbosemode 
				{ 4776 psentry
					dup 40 -string cvs (handling datasource of type ) print print (\n) print
				 psexit } 
				if
				dup /filetype eq /pstoedit.image.isfile exch store
				/dicttype eq 
				{ 4782 psentry
					(%begin dealing with image from dict\n) -print
					dup /Width get /pstoedit.image.width exch store
					dup /Height get /pstoedit.image.height exch store
					dup /ImageMatrix get /pstoedit.image.mat exch store
					dup /ImageMatrix
					[ pstoedit.image.width 0 0 pstoedit.image.height -1 mul 0 pstoedit.image.height  ]
					put
					pstoedit.image.isimagemask 
					{ 4810 psentry
						dup /Decode get 0 get /pstoedit.image.decode exch store
					 psexit } 
					if
					{ %nc 
						pstoedit.image.realproc
					} 
					pstoedit.image.isimagemask 
					{ 4823 psentry
						pstoedit.image.decode 0 eq 
						{ 4825 psentry
							(_mfd) % mask from dict - polarity false
						 psexit } 
						{ 4828 psentry
							(_mtd) % mask from dict - polarity true
						 psexit } 
						ifelse 
					 psexit } 
					{ 4833 psentry
						(_imd) % image from dict
					 psexit } 
					ifelse
					processAsFILEimage
				 psexit } 
				{ 4839 psentry
					false 
					{ 4843 psentry
						/pstoedit.image.datasrc exch store	
						/pstoedit.image.mat	    exch store
						/pstoedit.image.bits    exch store
						/pstoedit.image.height  exch store
						/pstoedit.image.width   exch store 
						pstoedit.image.width pstoedit.image.height pstoedit.image.bits pstoedit.image.mat 
						{ 4854 psentry
							pstoedit.image.isfile  
							{ 4856 psentry
								pstoedit.image.datasrc  read 
								{ 4859 psentry
									inttosinglechar 
								 psexit } 
								{ 4863 psentry
									0 -string 
								 psexit } 
								ifelse  
							 psexit } 
							{ 4869 psentry
								pstoedit.image.datasrc 
								(after calling datasrc2\n) pstack print flush
							 psexit } 
							ifelse
						 psexit } 
					 psexit } 
					{ 4876 psentry
						exch					/pstoedit.image.mat	    exch store
						exch					/pstoedit.image.bits    exch store  %  this is polarity (bool) for imagemask
						exch					/pstoedit.image.height  exch store
						exch					/pstoedit.image.width   exch store 
						pstoedit.image.width exch
						pstoedit.image.height exch 
						pstoedit.image.isimagemask 
						{ 4890 psentry
							1 % overwrite "polarity" with 1 if we use -image always 
						 psexit } 
						{ 4895 psentry
							pstoedit.image.bits
						 psexit } 
						ifelse  exch 
						[ pstoedit.image.width 0 0 pstoedit.image.height -1 mul 0 pstoedit.image.height  ]
						exch
					 psexit } 
					ifelse
					true 
					{ 4913 psentry
						{ %nc 
							-image %%%wogl fixme  pstoedit.image.realproc 
						} 
						pstoedit.image.isimagemask 
						{ 4918 psentry
							pstoedit.image.bits % note bits hold the polarity value (bool) in this case
							{ 4920 psentry
								(_mtn) % imagemask  - normal (non dict ds) - polarity true
							 psexit } 
							{ 4923 psentry
								(_mfn) % imagemask  - normal (non dict ds) - polarity false
							 psexit } 
							ifelse
						 psexit } 
						{ 4928 psentry
							(_imn) % image - normal (non dict ds)
						 psexit } 
						ifelse
						processAsFILEimage
					 psexit } 
					{ 4936 psentry
						(TEST!! : not processing as raster file image \n) print
						pstoedit.image.realproc
					 psexit } 
					ifelse
				 psexit } 
				ifelse
			 psexit } 
			{ 4944 psentry
				dup type
				dup /filetype eq /pstoedit.image.isfile exch store
				/dicttype eq 
				{ 4948 psentry
					(Level 2 version of image and imagemask not supported or implemented (yet) for this backend \n) true printwarning
					pstoedit.image.realproc
				 psexit } 
				{ 4953 psentry
					ImageTraceBoundingBox
					-showclippath
					pstoedit.escapetext	
					{ 4957 psentry
						(gsave\n)  -print  
					 psexit } 
					{ 4960 psentry
						pstoedit.image.stringprefix -print (begin\n) -print 
					 psexit } 
					ifelse
					/pstoedit.image.datasrc exch store
					/pstoedit.image.mat	    exch store
					pstoedit.escapetext not 
					{ 4968 psentry
						(% DC ) -print pstoedit.image.mat    
						{ 4970 psentry
							printTOS 
						 psexit } 
						forall (\n) -print
						pstoedit.image.stringprefix -print (imagematrix\n) -print 
					 psexit } 
					if
					/pstoedit.image.bits    exch store
					/pstoedit.image.height  exch store
					/pstoedit.image.width   exch store 
					(% DC ) printdumponly pstoedit.image.width  printTOS (\n) -print
					pstoedit.image.stringprefix printdumponly (width\n) printdumponly 
					(% DC ) printdumponly pstoedit.image.height printTOS (\n) -print
					pstoedit.image.stringprefix printdumponly (height\n) printdumponly 
					(% DC ) printdumponly pstoedit.image.bits   printTOS (\n) -print
					pstoedit.image.stringprefix printdumponly pstoedit.image.fourthparam printdumponly (\n) printdumponly
					-gsave
					pstoedit.image.mat matrix invertmatrix concat
					matrix currentmatrix matrix defaultmatrix matrix invertmatrix matrix concatmatrix
					pstoedit.escapetext 
					{ 4992 psentry
						([ ) -print 
					 psexit } 
					{ 4995 psentry
						(% DC ) -print 
					 psexit } 
					ifelse 
					{ 4999 psentry
						printTOS 
					 psexit } 
					forall 
					pstoedit.escapetext 
					{ 5004 psentry
						(]  concat [ 1 0 0 1 0 0 ] ) -print 
					 psexit } 
					if
					(\n) -print
					pstoedit.image.stringprefix printdumponly (normalized image currentmatrix\n) printdumponly  
					pstoedit.escapetext 
					{ 5011 psentry
						({ currentfile imagestring readhexstring pop }\n)  -print pstoedit.image.procname -print (\n) -print  
					 psexit } 
					if
					-grestore
					pstoedit.image.width pstoedit.image.height pstoedit.image.bits pstoedit.image.mat 
					{ 5017 psentry
						pstoedit.image.isfile  
						{ 5019 psentry
							pstoedit.image.datasrc  read 
							{ 5022 psentry
								pstoedit.image.stringprefix printdumponly (data ) printdumponly  
								dup  
								dup 16 -lt 
								{ 5027 psentry
									(0) -print 
								 psexit } 
								if 
								16 2 -string cvrs -print
								inttosinglechar 
							 psexit } 
							{ 5036 psentry
								0 -string 
							 psexit } 
							ifelse  
						 psexit } 
						{ 5042 psentry
							pstoedit.image.datasrc 
							pstoedit.image.stringprefix printdumponly (data ) printdumponly 
							dup 
							{ 5046 psentry
								dup 16 -lt 
								{ 5048 psentry
									(0) -print 
								 psexit } 
								if
								16 2 -string cvrs -print 
							 psexit } 
							forall
						 psexit } 
						ifelse
						(\n) -print
					 psexit } 
					pstoedit.image.realproc
					pstoedit.escapetext 
					{ 5061 psentry
						(grestore\n)  -print  
					 psexit } 
					{ 5064 psentry
						pstoedit.image.stringprefix -print (end\n) -print 
					 psexit } 
					ifelse
					-endshowclippath
				 psexit } 
				ifelse % level 2
			 psexit } 
			ifelse % raster file images supported
		 psexit } 
		ifelse % ignoredraw
	 psexit } 
	def
	/image 
	{ 5088 psentry
		printstate
		{ %nc 
			-image 
		} 
		(\%image:) (image) (bits) false ImageAndImagemask 
	 psexit } 
	soverload
	/imagemask 
	{ 5098 psentry
		printstate
		{ %nc 
			-imagemask 
		} 
		(\%imagemask:) (imagemask) (polarity) true ImageAndImagemask 
	 psexit } 
	soverload
 psexit } 
if
pstoedit.initialize
(\%!PS-Adobe-3.0\n) -printalways
(\%\%Title: flattened PostScript generated from file: ) -printalways
pstoedit.inputfilename -printalways (\n) -printalways
(\%\%Creator: pstoedit\n) -printalways
(\%\%BoundingBox: \(atend\)\n) -printalways
(\%\%Pages: \(atend\)\n) -printalways
(\%\%EndComments\n) -printalways
(\%\%BeginProlog\n) -printalways
(/setPageSize { pop pop } def\n) -printalways
(/ntranslate { neg exch neg exch translate } def\n) -printalways
(/p2esetcolorname { pop } def\n) -printalways
(/setshowparams { pop pop pop} def\n) -printalways
(/awidthshowhex { dup length 0 gt { dup /ASCIIHexDecode filter exch length 2 div cvi string readstring pop } if awidthshow } def\n) -printalways
(/awidthshowhexwithglyphs { true { pop awidthshowhex } { { glyphshow } forall pop } ifelse } def\n) -printalways
(/backendconstraints { pop pop } def\n) -printalways
(/pstoedit.newfont { ) -printalways
(80 string cvs ) -printalways
( findfont ) -printalways
( dup length dict begin {1 index /FID ne {def} {pop pop} ifelse} forall ) -printalways
( /Encoding ISOLatin1Encoding def  ) -printalways
( dup 80 string cvs /FontName exch def ) -printalways
( currentdict end ) -printalways
( definefont pop) -printalways
( } def\n) -printalways
(/imagestring 1 string def\n) -printalways
(\%\%EndProlog\n) -printalways
(\%\%BeginSetup\n) -printalways
(\% pstoedit.textastext pstoedit.doflatten backendconstraints   \n) -printalways
pstoedit.textastext 
{ 5154 psentry
	(1 ) 
 psexit } 
{ 5157 psentry
	(0 ) 
 psexit } 
ifelse -printalways
pstoedit.doflatten  
{ 5162 psentry
	(1 ) 
 psexit } 
{ 5165 psentry
	(0 ) 
 psexit } 
ifelse -printalways
(backendconstraints\n) -printalways
(\%\%EndSetup\n) -printalways
(\%\%Page: 1 1\n) -print
pstoedit.delaybindversion 
{ 5175 psentry
	revision 353 ge 
	{ 5180 psentry
		.bindoperators
		/NOBIND where 
		{ 
			pop NOBIND 
		} {
		    false % no NOBIND found
		} 
		ifelse 
		currentdict systemdict ne and
		{ 5183 psentry
			systemdict begin .bindoperators end 
		 psexit } 
		if
		/DELAYBIND where 
		{ 5188 psentry
			pop DELAYBIND 
			{ 5190 psentry
			  .bindnow 
			 psexit } 
			if 
		 psexit } 
		if
	 psexit } 
	if
	systemdict readonly pop
	pstoedit.currentglobalvalue pstoedit.setglobal 
 psexit } 
{ 5211 psentry
		pstoedit.currentglobalvalue pstoedit.setglobal 
	/NOBIND where 
	{ 
	   pop NOBIND 
	} {
	   false % no NOBIND found
	} 
	ifelse
	{ 5231 psentry
		/bind /.bind load def 
	 psexit } 
	if
 psexit } 
ifelse
languagelevel 1 gt 
{ 5240 psentry
	<< /PageSize [612 792 ] >> setpagedevice
 psexit } 
if
currentdict /pstoedit.nameOfIncludeFile known 
{ 5254 psentry
	pstoedit.nameOfIncludeFile run 
 psexit } 
if
4 dict begin
/RotateDict currentdict def
/angle pstoedit.rotation def
angle 0 ne pstoedit.xscale 1 ne or pstoedit.yscale 1 ne or pstoedit.xshift 0 ne or pstoedit.yshift 0 ne or
{ 5287 psentry
	/languagelevel where 
	{ 5289 psentry
		pop languagelevel 2 -ge 
	 psexit } 
	{ 5292 psentry
		FALSE 
	 psexit } 
	ifelse 
	{ 5296 psentry
		/lastW 0 def
		/lastH 0 def
		<<
		/BeginPage 
		{ 5302 psentry
			//RotateDict begin
			pop % page number
			currentpagedevice /PageSize get aload pop % stack: W H
			lastH ne exch lastW ne or 
			{ 5311 psentry
				<<
				/PageSize [
				currentpagedevice /PageSize get aload pop % stack: W H
				exch % stack: newW newH
				/lastW 2 index def
				/lastH 1 index def
				]
				>> setpagedevice % calls BeginPage recursively
			 psexit } 
			{ 5324 psentry
				pstoedit.centered 
				{ 5328 psentry
					currentpagedevice /PageSize get aload pop % stack: width height
					2 div exch 2 div exch % stack: center
					2 copy translate  
				 psexit } 
				if
				//angle rotate
				pstoedit.centered 
				{ 5336 psentry
					neg exch neg translate
				 psexit } 
				if
				pstoedit.xscale pstoedit.yscale scale
				pstoedit.xshift pstoedit.yshift translate
			 psexit } 
			ifelse
			end % //RotateDict
		 psexit } 
		>> setpagedevice
	 psexit } 
	{ 5348 psentry
		currentdevice getdeviceprops >> /HWSize get
		aload pop exch 2 array astore
		mark exch /HWSize exch currentdevice putdeviceprops pop
		/adbFixup 
		{ 5356 psentry
			pstoedit.centered 
			{ 5358 psentry
				currentdevice getdeviceprops >>
				dup /HWSize get aload pop 3 -1 roll
				/HWResolution get aload pop
				exch 4 -1 roll exch div 72 mul
				3 1 roll div 72 mul
				2 div exch 2 div exch
				2 copy translate 
			 psexit } 
			if
			//angle rotate 
			pstoedit.centered 
			{ 5373 psentry
				neg exch neg translate
			 psexit } 
			if
			pstoedit.xscale pstoedit.yscale scale
			pstoedit.xshift pstoedit.yshift translate
		 psexit } 
		bind odef
		/adbShowpage /showpage load def
		userdict begin /showpage 
		{ 5383 psentry
			adbShowpage adbFixup 
		 psexit } 
		bind odef end
		adbFixup
		statusdict begin
		/adbSet /.setpagesize load def
		/.setpagesize 
		{ 5392 psentry
			exch adbSet adbFixup
		 psexit } 
		bind def
		end
	 psexit } 
	ifelse
 psexit } 
if
end % //RotateDict
pstoedit.inputfilename run
pstoedit.somethingprinted cvi 1 eq 
{ 5411 psentry
	showpage % do one since no showpage was issued regularly
 psexit } 
if
pstoedit.quit
