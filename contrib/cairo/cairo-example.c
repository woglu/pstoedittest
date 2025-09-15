/* 
   This file is part of pstoedit
   
   Copyright (C) 2009 Dan McMahill dan_AT_mcmahill_DOT_net
   
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
 * This is a toy example of rendering an image with cairo.
 *
 * To use the example, run
 *
 *   pstoedit -f somefile.ps sample.c
 *
 * to generate the C code file "sample.c"
 *
 * Now compile the program with:
 *
 *   gcc -g -Wall -O2 `pkg-config --cflags cairo cairo-pdf cairo-xlib` -c sample.c
 *   gcc -g -Wall -O2 `pkg-config --cflags cairo cairo-pdf cairo-xlib` -c test.c
 *   gcc -g -Wall -O2 -o testprog test.o sample.o `pkg-config --libs cairo cairo-pdf cairo-xlib`
 */


#include<cairo.h>
#include<cairo-pdf.h>
#include<cairo-xlib.h>
#include<X11/Xlib.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>



/*
 * here "myfig" is the default for the cairo driver
 * "myfig" could have been changed to something else by using
 *
 *   pstoedit -f "cairo: -funcname othername" somefile.ps sample.c
 *
 * when running pstoedit
 */
#include "sample.h"

void do_xlib(void)
{
  Display *dpy;
  Window rootwin;
  Window win;
  XEvent e;
  int scr;
  cairo_surface_t *cs;
  int i;
  char tmps[128];

  if(!(dpy = XOpenDisplay(NULL))) {
    fprintf(stderr, "ERROR: Could not open display\n");
    exit(1);
  }
  
  scr = DefaultScreen(dpy);
  rootwin = RootWindow(dpy, scr);
  
  win = XCreateSimpleWindow(dpy, rootwin, 1, 1, myfig_width[0], myfig_height[0], 0, 
			    WhitePixel(dpy, scr), WhitePixel(dpy, scr));
  
  XStoreName(dpy, win, "pstoedit -- cairo driver");
  XSelectInput(dpy, win, ExposureMask|ButtonPressMask);
  XMapWindow(dpy, win);
  
  
  cs = cairo_xlib_surface_create(dpy, win, DefaultVisual(dpy, 0), myfig_width[0], myfig_height[0]);
  

  for (i = 0 ; i < myfig_total_pages ; i++) {
    printf ("Xlib rendering page #%d of %d (%d x %d)\n", i+1, myfig_total_pages,
	    myfig_width[i], myfig_height[i]);
    XResizeWindow(dpy, win, myfig_width[i], myfig_height[i]);
    sprintf(tmps, "pstoedit/cairo page %d of %d", i+1, myfig_total_pages);
    XStoreName(dpy, win, tmps);

    cairo_xlib_surface_set_size (cs, myfig_width[i], myfig_height[i]);
    while(1) {
      XNextEvent(dpy, &e);
      if(e.type==Expose && e.xexpose.count<1) {
	myfig_render[i](cs, NULL);
      } else if(e.type==ButtonPress) break;
    }
  }
  cairo_surface_destroy(cs);
  XCloseDisplay(dpy);
}

void do_pdf(void)
{
  cairo_surface_t *cs;
  cairo_t *cr;
  int i;

  /* We will resize the pages later */
  printf ("Creating initial cairo PDF surface with dimensions %d x %d\n",
	  myfig_width[0], myfig_height[0]);

  cs = cairo_pdf_surface_create("test.pdf", myfig_width[0], myfig_height[0]);

  for (i = 0 ; i < myfig_total_pages ; i++) {
    printf ("PDF rendering page #%d of %d (%d x %d)\n", i+1, myfig_total_pages,
	    myfig_width[i], myfig_height[i]);

    /* 
     * Note:  If you have older versions of cairo (1.4.10 and older I
     * believe) then this set_size screws up the y size some.
     */
    cairo_pdf_surface_set_size (cs, myfig_width[i], myfig_height[i]);

    cr = myfig_render[i](cs, NULL);
    cairo_show_page(cr);
    cairo_destroy(cr);
    cairo_surface_flush(cs);
  }

  cairo_surface_flush(cs);
  cairo_surface_destroy(cs);
}

int main(int argc, char *argv[])
{

  myfig_init();
  do_pdf();
  do_xlib();
  return 0;
}
