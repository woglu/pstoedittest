#ifndef __drvPPTX_h
#define __drvPPTX_h

/*
   drvooxml.h : This file is part of pstoedit
   Backend for Office Open XML files
   Contributed by: Scott Pakin <scott+ps2ed_AT_pakin.org>
   Thie code uses the libzip library from https://libzip.org

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

#include "drvbase.h"
#include I_strstream
#include <set>
#include <list>
using std::set;
using std::string;

class drvPPTX : public drvbase {
public:
        derivedConstructor(drvPPTX);
        ~drvPPTX() override; // Destructor
        class DriverOptions : public ProgramOptions {
        public:
                OptionT < RSString, RSStringValueExtractor > colortype;
                OptionT < RSString, RSStringValueExtractor > fonttype;
                OptionT < RSString, RSStringValueExtractor > embeddedfonts;
                OptionT < bool, BoolTrueExtractor> keepImageFiles;
        DriverOptions():
                colortype(true, "-colors", "string", 0,
                          "\"original\" to retain original colors (default), \"theme\" to convert randomly to theme colors, or \"theme-lum\" also to vary luminance",
                          nullptr, "original"),
                  fonttype(true, "-fonts", "string", 0,
                           "use \"windows\" fonts (default), \"native\" fonts, or convert to the \"theme\" font",
                           nullptr, "windows"),
                  embeddedfonts(true, "-embed", "string", 0,
                                "embed fonts, specified as a comma-separated list of EOT-format font files",
                                nullptr, ""),
                  keepImageFiles(true, "-keepimagefiles", "", 0,
                                "do not remove the temporary PNG image files.",
                                nullptr, false)
                {
                        ADD(colortype);
                        ADD(fonttype);
                        ADD(embeddedfonts);
                        ADD(keepImageFiles);
                }
        }*options;

#include "drvfuncs.h"
        void show_rectangle(const float llx, const float lly, const float urx, const float ury) override;
        void show_text(const TextInfo & textinfo) override;

public:

        void    show_image(const PSImage & imageinfo) override;

private:
        struct zip * outzip;        // pptx file to generate (zip format)
        std::ostringstream slidef;       // Contents of current slide
        BBox slideBBox;             // Bounding box (all slides)
        Point center_offset;        // Translation needed to center the current slide
        unsigned long next_id;      // Next ID number for naming objects
        unsigned long page_images;  // Number of images on the current page
        unsigned long total_images; // Number of images in the entire document
        std::set<std::string> eotlist;        // Set of EOT files to embed

        // OOXML requires a *lot* of boilerplate text.
        static const char * const xml_rels;
        static const char * const xml_slideLayout1_xml;
        static const char * const xml_slideLayout1_xml_rels;
        static const char * const xml_slideMaster1_xml;
        static const char * const xml_slideMaster1_xml_rels;
        static const char * const xml_theme1_xml;

        void create_pptx_file(const char * relname, const char * contents);
        void create_pptx();
        void print_coords(const BBox & pathBBox);
        Point pathCentroid();
        static Point rotate_pt_around (const Point & pt, float angle, const Point & pivot) ;
		static float angle_between(Point first, Point second);
		static void parse_xform_matrix(const float * origMatrix,
				bool * mirrored,
				float * xscale, float * yscale,
				float * rotation,
				float * x_trans, float * y_trans);
        void print_connections(const BBox & pathBBox);
        void print_color(int baseIndent, float redF, float greenF, float blueF);
        void print_join();
        void print_dash();
        enum {C_ORIGINAL, C_THEME, C_THEME_PURE} color_type;
        enum {F_WINDOWS, F_NATIVE, F_THEME} font_type;
        const char * pt2emu(float x_bp, float y_bp,
                            long int xshift_emu=0, long int yshift_emu=0,
                            RSString x_name="x", RSString y_name="y",
                            bool scaleOnly=false) const;

        static long int bp2emu (float bp); // implemented in cpp file because it used lroundf which is not visible from h file

        long int xtrans (float x_bp) const {
          // Convert an x coordinate from PostScript to OOXML.  To do
          // this we shift the page to the origin, center it on the
          // slide, and scale it from PostScript points to OOXML EMUs.
          const BBox pageBBox = getCurrentBBox();
          return bp2emu(x_bp - pageBBox.ll.x() + center_offset.x());
        }

        long int ytrans (float y_bp) const {
          // Convert a y coordinate from PostScript to OOXML.  To do
          // this we shift the page to the origin, center it on the
          // slide, flip it upside down, and scale it from PostScript
          // points to OOXML EMUs.
          const BBox pageBBox = getCurrentBBox();
          const float pageHeight = pageBBox.ur.y() - pageBBox.ll.y();
          return bp2emu(pageHeight - (y_bp - pageBBox.ll.y()) + center_offset.y());
        }

        static unsigned char panose2pitch (const unsigned int * panose_vals);
        void get_font_props(const TextInfo & textinfo,
                            RSString * typeface, RSString * panose,
                            bool * isBold, bool * isItalic,
                            unsigned char * pitchFamily) const;
        void eot2texinfo(const string& eotfilename, TextInfo & textinfo);

        // Describe a theme color and modifications to it.
        class ThemeColor {
        public:
          string name;            // Color name
          unsigned int lum;       // New luminance (thousandths of a %)
          explicit ThemeColor(const string& tName="unknown", unsigned int tLum=~0U) :
            name(tName),
            lum(tLum)
          {}
        };

        // Map an RGB value to a theme color.
        typedef KeyValuePair<unsigned int, ThemeColor> RGB2Theme;
		Mapper<RGB2Theme /* , unsigned int, ThemeColor */ > rgb2theme;

        // Map a PostScript core font to a Windows name + PANOSE characterization.
        Mapper<FontMapping /*, RSString, RSString*/ > ps2win;

        std::list<RSString> temporary_image_files;
};

#endif
