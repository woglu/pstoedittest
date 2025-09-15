#  This file is part of pstoedit
#   
#  Copyright (C) 2009 Dan McMahill dan_AT_mcmahill_DOT_net
#   
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#   
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#  

# This is a simple makefile for building a small example script which
# demonstrates the use of the cairo driver for pstoedit.


CFLAGS=	-Wall -H `pkg-config --cflags cairo cairo-pdf cairo-xlib pangocairo`
LDFLAGS=`pkg-config --libs cairo cairo-pdf cairo-xlib pangocairo` -Wl,-t

default: cairo-example cairo-example-pango

cairo-example : sample.o cairo-example.o
	$(CC) -o $@  $(CFLAGS) $(CPPFLAGS) cairo-example.o sample.o $(LDFLAGS)

cairo-example-pango : sample-pango.o cairo-example-pango.o
	$(CC) -o $@  $(CFLAGS) $(CPPFLAGS) cairo-example-pango.o sample-pango.o $(LDFLAGS)


cairo-example.o : cairo-example.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) cairo-example.c

cairo-example-pango.o : cairo-example.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) cairo-example.c -o $@


sample.o: sample.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) sample.c

sample-pango.o: sample-pango.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) sample-pango.c


sample.c : sample.eps ../../src/pstoedit ../../src/libp2edrvstd.la
	../../src/pstoedit  -f "cairo: -header sample.h" sample.eps sample.c

sample-pango.c : sample.eps ../../src/pstoedit ../../src/libp2edrvstd.la
	../../src/pstoedit  -f "cairo: -pango -header sample.h" sample.eps sample-pango.c

