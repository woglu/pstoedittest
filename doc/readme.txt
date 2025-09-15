PSTOEDIT 
Copyright (C) 1993 - 2024 Wolfgang Glunz, wglunz35_AT_pstoedit.net

pstoedit converts PostScript(TM) and PDF files to other vector graphic
formats so that they can be edited graphically. See pstoedit.htm or
index.htm for more details on which formats are supported by pstoedit.

The architecture of pstoedit consists of a PostScript frontend which
needs to call a PostScript interpreter like Ghostscript and the
individual backends which are plugged into a kind of framework.

This framework can be used independently from the PostScript frontend
from any other program. The framework provides a uniform interface to
all different backends. Get in contact with the author if you need
more information on how to use this framework.

If you just find this program useful, have made some improvements or 
implemented other backends please send an email to wglunz35_AT_pstoedit.net.

If this programs saves you a lot of work consider sending a contribution
of any kind.

If you include this program on a CDROM, please send me a copy of the CD,
or if it goes with a book, of the book. 

My home address is:

	Dr. Wolfgang Glunz             
	81825 Muenchen / Germany  
	Josef Brueckl Str. 32    

Compiling pstoedit unter Linux:
-------------------------------
You need a C++ compiler, e.g., g++ to compile pstoedit.

Enabling additional drivers (optionally):

It is recommended to have libplotter installed. Then you
get a lot of additional formats for free. 
do "sudo apt-get install libplot-dev" to install libplotter.

Similarly, to enable the PowerPoint backend, you need libzip.
do "sudo apt-get install libzip-dev" to install libzip.

And for support of EMF, the libEMF is needed.
do "sudo apt-get install libemf-dev" to install libEMF.

See the file pstoedit.linux.prerequisites.txt for more prerequisites you might need to install before under Linux

Then just run these commands:
sh configure; 
make
sudo make install; 


There are several test cases included. To run them type `make test'.
This works under *nix only.


When building pstoedit under cygwin/Linux you may need to set LDFLAGS to /usr/local/lib in case you have some libraries (e.g. libEMF) installed there.

Under some systems (e.g. cygwin) it is not possible to link static libraries (.a) into a dynamic library (.so/.dll). In this case, you need to have also a shared version of the relevant libs, e.g. of libEMF. In order to get a shared version, you normally need to set the option "--enabled-shared" during the "configure" run for the library.


Installing pstoedit under Windows 
---------------------------------

Best use the  pstoedit_gui_setup_x64.exe for installation.


Using pstoedit:
---------------
Before you can use pstoedit you must have a working installation
of Ghostscript (either GNU or Aladdin).

The rest is described in the manual page in pstoedit.htm.

pstoedit works reasonable with PostScript files containing
	* line drawings
	* text with standard fonts 

Try to run it on golfer.ps or tiger.ps that comes with Ghostscript, e.g., 
pstoedit -f <your format> <local path where GhostScript is installed>/examples/tiger.ps tiger.<suffix>

Since versiomn 4.0, there is also a GUI for pstoedit - PstoeditQtGui

Some features that are not supported by every backend of pstoedit:
	* bitmap images (just for some backends and only a subset of the PostScript image operators)
	* general fill patterns
	* clipping (only partially via the -sclip option)
	* ... 

Special note about the Java backend:
------------------------------------
The Java backends generate a Java source file that needs other files
in order to be compiled and usable. See the files contrib/java/java1/readme_java.txt 
and contrib/java/java2/readme_java2.htm for more details.

Extending pstoedit:
-------------------
To implement a new backend you can start from drvsampl.cpp.
Please don't forget to send any new backend that might be of interest
for others as well to the author (wglunz35_AT_pstoedit.net) so that
it can be incorporated into future versions of pstoedit. Such
new backends will then be available with the GPL as well.

Acknowledgements:
-----------------

See manual page in pstoedit.htm and the changelog.htm for a list of contributors.

License: 
--------

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



----------------------------------------------------------------------------
 
