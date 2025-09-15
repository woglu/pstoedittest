/*
  drvcairo.cpp : This file is part of pstoedit
  Copyright (C) 2009 - 2024 Dan McMahill dan_AT_mcmahill_DOT_net

  This driver used drvSAMPL.cpp as a reference.
  
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
#include "drvcairo.h"
#include I_fstream
#include I_stdio
#include I_stdlib
#include <iostream>
#include <fstream>
#include <memory>

// TODO
//
// - fix text font selection
//   = When using -pango, the text all appears to be offset in the y-direction from where it
//     should be.  Also bold and italics don't work with -pango.
//
//   = Is it possible to get the postscript bounding box for the text?  If so then one idea is I
//     create the pango or cairo text, get its size and then use cairo_transform to scale it so the pango or
//     cairo bounding box matches the postscript one.  This way even if the fonts are a little different
//     we end up with something as close as possible.
//
//   = What about symbol font?  It looks like the way this has to happen is I need to convert
//     to the greek alphabet in unicode stored in UTF-8 and that sounds like a pretty big hack
//     when the chances of really getting fonts to be right are slim.  The reality is that 
//     fonts probably won't look right unless you use -dt as a pstoedit option
//
// - fix bounding box glitches. 
//   = examples/colrtest.ps
//
// - image and imagemask support
//   Can I implement this directly?
//    = examples/imagetest_1.ps
//    = examples/imagetest_2.ps
//    = examples/imagetest_3.ps
//    = examples/oneimg.ps
//    = examples/woglim.ps
//   If I can figure out how to get at the pixel data, I can generate cairo code that will render it.
//   Cairo can also load png from a file however I'd prefer to not to have to do that since it means
//   the end users program now needs to know how to locate the .png files at runtime and it is not
//   a given that the users code knows how to reliably do this so I'd rather embed the picture.
//
// - Is there a better way of getting the name for the header file that is created?
//   

drvCAIRO::derivedConstructor(drvCAIRO):
  //(const char * driveroptions_p,ostream & theoutStream,ostream & theerrStream): // Constructor
  constructBase //, imgcount(0)
{
  ofstream outh;

  // driver specific initializations
  // and writing of header to output file
  outf << "/* ***** Generated from pstoedit ***** */" << endl;
  outf << "#include <cairo.h>" << endl;
  if (options->pango.value) {
  outf << "#include <pango/pangocairo.h>" << endl;
  }
  outf << "#include <stdio.h>" << endl;
  outf << endl;

  maxw = 0;
  maxh = 0;
  evenoddmode = false;

  
  // Generate the header file
  outh.open(options->header.value.c_str(), ios::out);

  outh << "/* " << options->header.value << " */" << endl;
  outh << "/* ***** Generated from pstoedit ***** */" << endl;
  outh << "#ifndef __" << options->funcname.value << "_H__" << endl;
  outh << "#define __" << options->funcname.value << "_H__" << endl;
  outh << "#include <cairo.h>" << endl;
  outh << "extern cairo_t * (*" << options->funcname.value << "_render[])(cairo_surface_t *, cairo_t *);" << endl;
  outh << "extern int " << options->funcname.value << "_total_pages;" << endl;
  outh << "extern int " << options->funcname.value << "_width[];" << endl;
  outh << "extern int " << options->funcname.value << "_height[];" << endl;
  outh << "extern void " << options->funcname.value << "_init(void);" << endl;
  outh << "#endif /* __" << options->funcname.value << "_H__ */" << endl;
  outh << endl;
  outh.close();

}

drvCAIRO::~drvCAIRO()
{
  unsigned int i;

  // driver specific deallocations
  // and writing of trailer to output file
  outf << endl;
  outf << "/* Total number of pages */" << endl;
  outf << "int " << options->funcname.value << "_" << "total_pages;" << endl;
  outf << endl;
  outf << "/* Array of the individual page render functions */" << endl;
  outf << "cairo_t * (*" << options->funcname.value << "_render[" << totalNumberOfPages() << "])(cairo_surface_t *, cairo_t *);" << endl;
  outf << endl;
  outf << "/* array of pointers to the widths and heights */" << endl;
  outf << "int " << options->funcname.value << "_width[" << totalNumberOfPages() << "];" << endl;
  outf << "int " << options->funcname.value << "_height[" << totalNumberOfPages() << "];" << endl;
  outf << endl;

  outf << "/* This function should be called at the beginning of the user program */" << endl;
  outf << "void " << options->funcname.value << "_init(void)" << endl;
  outf << "{" << endl;
  outf << endl;
  outf << "  " << options->funcname.value << "_" << "total_pages = " << totalNumberOfPages() << ";" << endl;
  outf << endl;
  
  // Now spit out an array of pointers to the render functions, so we can deal with multiple pages
  for (i = 1 ; i <= drvbase::totalNumberOfPages() ; i++) { 
    outf << "  " << options->funcname.value << "_render[" << i-1 << "] = ";
    outf  << options->funcname.value << "_page_" << i << "_render;" << endl;
  }
  outf << endl;

  for (i = 1 ; i <= drvbase::totalNumberOfPages() ; i++) {
    outf << "  " << options->funcname.value << "_width[" << i-1 << "] = ";
    outf << options->funcname.value << "_page_" << i << "_width;" << endl;
    
  }

  for (i = 1 ; i <= drvbase::totalNumberOfPages() ; i++) {
    outf << "  " << options->funcname.value << "_height[" << i-1 << "] = ";
    outf << options->funcname.value << "_page_" << i << "_height;" << endl;
    
  }
  outf << "}" << endl;
  outf << endl;

  outf << "float " << options->funcname.value << "_width_max = " << maxw << ";" << endl;
  outf << "float " << options->funcname.value << "_height_max = " << maxh << ";" << endl;
  
}

void drvCAIRO::print_coords()
{

  for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
    const basedrawingelement & elem = pathElement(n);
    switch (elem.getType()) {
    case moveto:{
      const Point & p = elem.getPoint(0);
      outf << "  cairo_move_to (cr, ";
      outf << p.x() + x_offset << ", " << /*   currentDeviceHeight -  */ -1*p.y() + y_offset << ");";
    }
      break;
    case lineto:{
      const Point & p = elem.getPoint(0);
      outf << "  cairo_line_to (cr, ";
      outf << p.x() + x_offset << ", " << /*   currentDeviceHeight -  */ -1*p.y() + y_offset << ");";
    }
      break;
    case closepath:
      outf << "  cairo_close_path (cr);";
      break;
    case curveto:{
      outf << "  cairo_curve_to (cr";
      for (unsigned int cp = 0; cp < 3; cp++) {
	const Point & p = elem.getPoint(cp);
	outf << 
	  ", " << (p.x() + x_offset) << 
	  ", " << /*   currentDeviceHeight -  */ (-1*p.y() + y_offset);
      }
      outf << ");" << endl;
    }
      break;
    default:
      errf << "\t\tFatal: unexpected case in drvcairo " << endl;
      abort();
      break;
    }
    outf << endl;
  }
}


void drvCAIRO::open_page()
{
  BBox mybox;
  

  mybox = getCurrentBBox();

  x_offset = -mybox.ll.x();
  y_offset = mybox.ur.y();
  //cout << "Set offset to (" << x_offset << ", " << y_offset << ")" << endl;

  outf << "/*" << endl;
  outf << " * Original bounding box = for page # " << currentPageNumber << " is" << endl;
  outf << " * " << mybox << endl;
  outf << " * The figure has been offset by (" << x_offset << ", " << y_offset << ")" << endl;
  outf << " * to move LL to (0,0).  The width and height" << endl;
  outf << " * can be read from the following two variables:" << endl;
  outf << " */" << endl;

  outf << "static int " << options->funcname.value <<"_page_" << currentPageNumber << "_width = " << 
    mybox.ur.x() - mybox.ll.x() << ";" << endl;
  outf << "static int " << options->funcname.value << "_page_" << currentPageNumber << "_height = " << 
       mybox.ur.y() - mybox.ll.y() << ";" << endl;
  outf << endl;

  if (mybox.ur.x() - mybox.ll.x() > maxw) {
    maxw = mybox.ur.x() - mybox.ll.x();
  }

  if (mybox.ur.y() - mybox.ll.y() > maxh) {
    maxh = mybox.ur.y() - mybox.ll.y();
  }

  outf << "static cairo_t * " << options->funcname.value << "_page_" << currentPageNumber << "_render";
  outf << "(cairo_surface_t *cs, cairo_t *cr)" << endl;
  outf << "{" << endl;

  outf << endl;
  outf << "  if (cr == NULL && cs == NULL) {" << endl;
  outf << "    return NULL;" << endl;
  outf << "  } else if(cr == NULL && cs != NULL) {" << endl;
  outf << "    cr = cairo_create (cs);" << endl;
  outf << "  } else if(cr != NULL && cs == NULL) {" << endl;
  outf << "  } else if(cr != NULL && cs != NULL) {" << endl;
  outf << "  }" << endl;
  outf << endl;

  outf << "  cairo_save (cr);" << endl;
  outf << endl;
  if (!options->pango.value) {
    
    outf << "  /* set an initial font */" << endl;
    outf << "  cairo_select_font_face (cr, \"monospace\"," << 
      " CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);" << endl;
  }
  outf << endl;

}

void drvCAIRO::close_page()
{
  outf << "  cairo_restore (cr);" << endl;
  outf << endl;
  outf << "  return cr;" << endl;
  outf << "} /* end of " << options->funcname.value << "_page_" << (currentPageNumber) << "_render() */" ;
  outf << endl;
}

void drvCAIRO::show_text(const TextInfo & textinfo)
{
  outf << "  /*" << endl;
  outf << "   * " << "X " << textinfo.x() << " Y " << textinfo.y() << endl;
  outf << "   * " << "X_END " << textinfo.x_end() << " Y_END " << textinfo.y_end() << endl;
  outf << "   * " << "currentFontName: " << textinfo.currentFontName.c_str() << endl;
  outf << "   * " << "is_non_standard_font: " << textinfo.is_non_standard_font << endl;
  outf << "   * " << "currentFontFamilyName: " << textinfo.currentFontFamilyName.c_str() << endl;
  outf << "   * " << "currentFontFullName: " << textinfo.currentFontFullName.c_str() << endl;
  outf << "   * " << "currentFontWeight: " << textinfo.currentFontWeight.c_str() << endl;
  outf << "   * " << "currentFontAngle: " << textinfo.currentFontAngle << endl;

  const float *CTM = getCurrentFontMatrix();
  const char *family;

  outf << "   * " << "currentFontMatrix: [";
  for (unsigned int i = 0; i < 6; i++) {
    outf << " " << CTM[i];
  }
  outf << ']' << endl;
  outf << "   */" << endl;

  outf << "  {" << endl;
  outf << "    cairo_matrix_t matrix, save_matrix;" << endl;
  if (options->pango.value) {
    outf << "    PangoFontDescription *desc;" << endl;
    outf << "    PangoLayout *layout;" << endl;
  }
  outf << "    const char *text = \"" << textinfo.thetext.c_str() << "\";" << endl;
  outf << endl;

  outf << "    cairo_set_source_rgb (cr, " << textinfo.currentR << "," << 
    textinfo.currentG << "," << textinfo.currentB << ");" << endl;

  outf << "    cairo_get_matrix (cr, &save_matrix);" << endl;
  
  // cairo_matrix_init (xx, yx, xy, yy, x0, y0)
  //    x_new = xx * x + xy * y + x0;
  //    y_new = yx * x + yy * y + y0;
  outf << "    cairo_save (cr);" << endl;
  outf << "    cairo_matrix_init (&matrix," 
       << CTM[0]/textinfo.currentFontSize << ", "
       << -1.0*CTM[1]/textinfo.currentFontSize << ", "
       << -1.0*CTM[2]/textinfo.currentFontSize << ", "
       <<  1.0*CTM[3]/textinfo.currentFontSize << ", "
       << CTM[4] + x_offset << ", "
       << -1.0*CTM[5] + y_offset << ");" << endl;

  outf << "    cairo_transform (cr, &matrix);" << endl;
  outf << "    cairo_move_to (cr, 0, 0);" << endl;
  outf << endl;

  family = "monospace";
  if (strstr(textinfo.currentFontName.c_str(), "Times") ||
      strstr(textinfo.currentFontName.c_str(), "Roman")) {
    family = "serif";
  } else if (strstr(textinfo.currentFontName.c_str(), "Helvetica") ||
	     strstr(textinfo.currentFontName.c_str(), "Sans")) {
    family = "sans-serif";
  } else if (strstr(textinfo.currentFontName.c_str(), "Courier") ||
	     strstr(textinfo.currentFontName.c_str(), "Mono")) {
    family = "monospace";
  } else if (strstr(textinfo.currentFontName.c_str(), "Symbol") ) {
    // In this case, what unfortunately needs to happen is I need to convert the
    // ASCII string to UTF-8 encoded string but with mapping from the 
    family = "symbol";
  } else {
    errf << "currentFontName: " << textinfo.currentFontName.c_str() << " is not known." << endl;
    errf << "                 Defaulting to " << family << endl;
  }
  
  if (options->pango.value) {
    outf << "    /* Set pango font */" << endl;
    outf << "    layout = pango_cairo_create_layout (cr);" << endl;
    outf << "    desc = pango_font_description_from_string (\"" << 
      family << "\");" << endl;
    
    outf << "    /* A size value of 10 * PANGO_SCALE is a 10 point font. */" << endl;
    outf << "    pango_font_description_set_size (desc,  " << 
      textinfo.currentFontSize << " * PANGO_SCALE);" << endl;
    outf << "    pango_layout_set_font_description (layout, desc);" << endl;
    outf << "    pango_font_description_free (desc);" << endl;
    outf << "    pango_layout_set_text (layout, text, -1);" << endl;
    outf << "    pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);" << endl;

    outf << "    pango_cairo_show_layout (cr, layout);" << endl;
    outf << "    g_object_unref (layout);" << endl;

  } else {
    const char *weight, *slant;
    // cairo_select_font_face (cairo_t *cr,
    //                         const char *family,
    //                         cairo_font_slant_t slant,
    //                         cairo_font_weight_t weight);
    //
    // family:
    //  standard CSS2 generic family names, ("serif", "sans-serif",
    //  "cursive", "fantasy", "monospace"), are likely to work as expected.
    //
    
    // FIXME -- figure out how to get the family set is a better way
    
    // slant:
    //   CAIRO_FONT_SLANT_NORMAL  - Upright font style
    //   CAIRO_FONT_SLANT_ITALIC  - Italic font style
    //   CAIRO_FONT_SLANT_OBLIQUE - Oblique font style
    //

    // FIXME -- set the slant more robustly
    slant = "CAIRO_FONT_SLANT_NORMAL";
    if (strstr(textinfo.currentFontFullName.c_str(), "Italic")) {
      slant = "CAIRO_FONT_SLANT_ITALIC";
    } else  if (strstr(textinfo.currentFontFullName.c_str(), "Oblique")) {
      slant = "CAIRO_FONT_SLANT_OBLIQUE";
    }

    // weight:
    //   CAIRO_FONT_WEIGHT_NORMAL - Normal font weight
    //   CAIRO_FONT_WEIGHT_BOLD   - Bold font weight
    //

    // FIXME -- set the weight more robustly
    weight = "CAIRO_FONT_WEIGHT_NORMAL";
    if (strstr(textinfo.currentFontWeight.c_str(), "bold") ||
	strstr(textinfo.currentFontWeight.c_str(), "Bold") ) {
      weight = "CAIRO_FONT_WEIGHT_BOLD";
    }
    
    outf << "    cairo_select_font_face (cr, \"" << family << "\"," << endl;
    outf << "                            " << slant << "," << endl;
    outf << "                            " << weight << ");" << endl;
    
    // outf << "    status = cairo_status (cr);" << endl;
    //outf << "    printf(\"cairo_select_font_face() returned \\\"%s\\\"\\n\", cairo_status_to_string (status));" << endl;
    
    outf << "    cairo_set_font_size (cr, " << textinfo.currentFontSize << ");" << endl;
    outf << "    cairo_show_text (cr, text);" << endl;
  }

  outf << "    cairo_set_matrix (cr, &save_matrix);" << endl;
  outf << "    cairo_restore (cr);" << endl;
  outf << "    cairo_move_to (cr, " << textinfo.x_end() + x_offset 
       << ", " << -1*textinfo.y_end() + y_offset << ");" << endl;
  outf << "  }" << endl;
  outf << endl;

}

void drvCAIRO::ClipPath(cliptype type)
{
  // type is 'clip' or 'eoclip'
  evenoddmode = (type == drvbase::eoclip);

  outf << "  cairo_save (cr);" << endl;
  outf << "  cairo_reset_clip (cr);" << endl;

  if (evenoddmode) {
    outf << "  cairo_set_fill_rule (cr, CAIRO_FILL_RULE_EVEN_ODD);" << endl;
  } else {
    outf << "  cairo_set_fill_rule (cr, CAIRO_FILL_RULE_WINDING);" << endl;
  }

  print_coords();
  outf << "  cairo_clip (cr);" << endl;
  outf << "  cairo_restore (cr);" << endl;
}

void drvCAIRO::show_path()
{
  DashPattern dp(dashPattern());

  outf << endl;
  outf << "  /*" << endl;
  outf << "   * Path # " << currentNr() ;
  if (isPolygon())
    outf << " (polygon):" << endl;
  else
    outf << " (polyline):" << endl;
  outf << "   */" << endl;
  outf << endl;
  
  outf << "  cairo_save (cr);" << endl;
  outf << "  cairo_set_line_width (cr, " << currentLineWidth() << ");" << endl;

  // CAIRO_LINE_CAP_BUTT   - start(stop) the line exactly at the start(end) point
  // CAIRO_LINE_CAP_ROUND  - use a round ending, the center of the circle is the end point
  // CAIRO_LINE_CAP_SQUARE - use squared ending, the center of the square is the end point
  outf << "  cairo_set_line_cap (cr, ";
  switch( currentLineCap() ) {
  case 0:
    outf << "CAIRO_LINE_CAP_BUTT);" << endl;
    break;

  case 1:
    outf << "CAIRO_LINE_CAP_ROUND);" << endl;
    break;

  case 2:
    outf << "CAIRO_LINE_CAP_SQUARE);" << endl;
    break;

  default:
    errf << "Unexpected currentLineCap() in cairo driver:  " << currentLineCap() << endl;
    outf << "CAIRO_LINE_CAP_ROUND);" << endl;
    break;
  }
  // cairo_set_dash (cairo_t *cr, const double *dashes, int num_dashes, double offset);
  // dashes :
  //     an array specifying alternate lengths of on and off stroke portions
  //
  // num_dashes :
  //     the length of the dashes array
  //
  // offset :
  //     an offset into the dash pattern at which the stroke should start
  //
  // dashPattern:  has nrOfEntries, float *numbers, float offset

  if (dp.nrOfEntries > 0) {
    outf << "  {" << endl;
    outf << "    double pat[" << dp.nrOfEntries << "] = {" << endl;
    for (int i = 0; i < dp.nrOfEntries; i++) {
      outf << "                      " << dp.numbers[i] << ", " << endl;
    }
    outf << "                   };" << endl;
    outf << endl;
    outf << "    cairo_set_dash (cr, pat, " << dp.nrOfEntries << ", " << dp.offset << ");" << endl;
    outf << "   }" << endl;
  } else {
    outf << "  cairo_set_dash (cr, NULL, 0, 0.0);" << endl;
  }

  // cairo_move_to (cr, 0.25, 0.25);
  // cairo_line_to (cr, 0.5, 0.375);
  outf << "  /* Path Elements 0 to " << numberOfElementsInPath() - 1 << " */" << endl;
  print_coords();



  switch (currentShowType()) {
  case drvbase::stroke:
    outf << "  cairo_set_source_rgb (cr, " << edgeR() << "," << edgeG() << "," << edgeB() << ");" << endl;
    outf << "  cairo_stroke (cr);" << endl;
    break;
	  
  case drvbase::eofill:
    outf << "  cairo_set_fill_rule (cr, CAIRO_FILL_RULE_EVEN_ODD);" << endl;
    evenoddmode = true;
    /* no break */
    // fall through
  case drvbase::fill:
	  
    outf << "  cairo_set_source_rgb (cr, " << fillR() << "," << fillG() << "," << fillB() << ");" << endl;
    outf << "  cairo_fill_preserve (cr);" << endl;
    if (evenoddmode) {
      outf << "  cairo_set_fill_rule (cr, CAIRO_FILL_RULE_WINDING);" << endl;
      evenoddmode = false;
    }
    outf << "  cairo_set_source_rgb (cr, " << edgeR() << "," << edgeG() << "," << edgeB() << ");" << endl;
    outf << "  cairo_stroke (cr);" << endl;
    break;
	  
  default:
    // cannot happen
    outf << "  // unexpected ShowType " << (int) currentShowType();
    break;
  }
  outf << "  cairo_restore (cr);" << endl;

}

void drvCAIRO::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{
  // FIXME -- I need to get some rectangle calls into my sample so i can test this...
  // cairo_rectangle (cr, x, y, width, height);
  outf << "  cairo_rectangle (cr, " << llx << "," << lly << ", " << urx-llx << "," << ury-lly << ");" << endl;
  // just do show_path for a first guess
  show_path();
}

void drvCAIRO::show_image(const PSImage & imageinfo)
{
  // first retrieve bounding box
  Point lowerLeft, upperRight;
  imageinfo.getBoundingBox(lowerLeft, upperRight);
  
  // not only bounding box must account for scale,
  // but also transformation matrix!

  // scale bounding box
  lowerLeft *= getScale();
  upperRight *= getScale();

  const long width  = abs(i_transX(upperRight.x()) - i_transX(lowerLeft.x()));
  const long height = abs(i_transY(upperRight.y()) - i_transY(lowerLeft.y()));
  
  if (Verbose()) {
    errf << "image.Width:" << imageinfo.width << " image.Height: " << imageinfo.height << endl;
    errf << "Width:" << width << " Height: " << height << endl;
  }
  
#if 0

  // This is an example of how to take image data and get it into a cairo surface.
  // If I ever figure out the pstoedit end, I can try one of these.
  int stride;
  unsigned char *data;
  cairo_surface_t *surface;
  g_double w, h;
    /*
    CAIRO_FORMAT_ARGB32,
    CAIRO_FORMAT_RGB24,
    CAIRO_FORMAT_A8,
    CAIRO_FORMAT_A1
  */

  stride = cairo_format_stride_for_width (format, width);
  data = malloc (stride * height);
  surface = cairo_image_surface_create_for_data (data, format,
						 width, height,
						 stride);
  surface = cairo_image_surface_create_from_png_stream (data, format,
						 width, height,
						 stride);
#endif


  // calc long-padded size of scanline 
  const long scanlineLen = ((width * 3) + 3) & ~3L;

  // now lets get some mem
  std::unique_ptr<unsigned char[]> output (new unsigned char[scanlineLen * height]);

  for (long i = 0; i < scanlineLen * height; i++)
    output[i] = 255;		// default is background (white)    
  
  if (!output) {
    errf << "ERROR: Cannot allocate memory for image" << endl;
    return;
  }
  // setup inverse transformation matrix (scaled, too!)
  const float matrixScale(imageinfo.normalizedImageCurrentMatrix[0] *
			  imageinfo.normalizedImageCurrentMatrix[3] -
			  imageinfo.normalizedImageCurrentMatrix[2] *
			  imageinfo.normalizedImageCurrentMatrix[1]);
  const float inverseMatrix[] = {
    imageinfo.normalizedImageCurrentMatrix[3] / matrixScale / getScale(),
    -imageinfo.normalizedImageCurrentMatrix[1] / matrixScale / getScale(),
    -imageinfo.normalizedImageCurrentMatrix[2] / matrixScale / getScale(),
    imageinfo.normalizedImageCurrentMatrix[0] / matrixScale / getScale(),
    (imageinfo.normalizedImageCurrentMatrix[2] *
     imageinfo.normalizedImageCurrentMatrix[5] -
     imageinfo.normalizedImageCurrentMatrix[4] *
     imageinfo.normalizedImageCurrentMatrix[3]) / matrixScale,
    (imageinfo.normalizedImageCurrentMatrix[4] *
     imageinfo.normalizedImageCurrentMatrix[1] -
     imageinfo.normalizedImageCurrentMatrix[0] *
     imageinfo.normalizedImageCurrentMatrix[5]) / matrixScale
  };

  // now transform image
  for (long ypos = 0; ypos < height; ypos++) {
    // buffer current output scanline (saves us some multiplications)
    unsigned char *const currOutput = &output[scanlineLen * ypos];
    
    for (long xpos = 0; xpos < width; xpos++) {
      // now transform from device coordinate space to image space
      
      // apply transformation
      const Point currPoint = Point(xpos + lowerLeft.x(),
				    ypos + lowerLeft.y()).transform(inverseMatrix);
      
      // round to integers
      const long sourceX = (long) (currPoint.x() + .5);
      const long sourceY = (long) (currPoint.y() + .5);
      
      // is the pixel out of bounds? If yes, no further processing necessary
      if (sourceX >= 0L && (unsigned long) sourceX < imageinfo.width &&
	  sourceY >= 0L && (unsigned long) sourceY < imageinfo.height) {
	// okay, fetch source pixel value into 
	// RGB triplet
	
	unsigned char r(255), g(255), b(255), c, m, y, k;
	
	// how many components?
	switch (imageinfo.ncomp) {
	case 1:
	  r = g = b = imageinfo.getComponent(sourceX, sourceY, 0);
	  break;
	  
	case 3:
	  r = imageinfo.getComponent(sourceX, sourceY, 0);
	  g = imageinfo.getComponent(sourceX, sourceY, 1);
	  b = imageinfo.getComponent(sourceX, sourceY, 2);
	  break;
	  
	case 4:
	  c = imageinfo.getComponent(sourceX, sourceY, 0);
	  m = imageinfo.getComponent(sourceX, sourceY, 1);
	  y = imageinfo.getComponent(sourceX, sourceY, 2);
	  k = imageinfo.getComponent(sourceX, sourceY, 3);
	  
	  // account for key
	  c += k;
	  m += k;
	  y += k;
	  
	  // convert color
	  r = 255 - c;
	  g = 255 - m;
	  b = 255 - y;
	  break;
	  
	default:
	  errf << "\t\tFatal: unexpected case in drvcairo (line "
	       << __LINE__ << ")" << endl;
	  abort();
	  return;
	}
	
	// set color triple
	currOutput[3 * xpos] = b;
	currOutput[3 * xpos + 1] = g;
	currOutput[3 * xpos + 2] = r;
      }
    }
  }
}

static DriverDescriptionT < drvCAIRO > D_cairo("cairo",  // name
					       "cairo driver", // short description
					       "generates compilable c code for rendering with cairo", // long description
					       "c", // output file suffix
					       true,	// backend supports subpaths
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
					       true,	// backend supports curves
					       true,	// backend supports elements which are filled and have edges
					       true,	// backend supports text
					       DriverDescription::imageformat::memoryeps,	// format to be used for raster imagese
					       DriverDescription::opentype::normalopen, // binary or ascii output file
					       true,	// if format supports multiple pages in one file
					       true  /*clipping */ 
					       );
