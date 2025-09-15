/*
   subpath.h: This file is part of pstoedit
         Reaggangement of paths for backends which
         do not support subpaths

   Copyright (C) 1999 Burkhard Plaum plaum@ipf.uni-stuttgart.de

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

class sub_path_list; // forward

// Constants for flags

const int PS_PATH_IS_CONNECTED = 1;

// Class for a subpath

class sub_path
  {
public:
  basedrawingelement ** path;            // Basedrawingelements

  int flags;
  
  sub_path ** parents;                   // Pointers to possible parents
  sub_path * parent;                     // Parent path
  sub_path ** children;                  // Children paths
  Point * points;
  
  unsigned int num_outside;              // Number of paths outside of this
  unsigned int num_elements;             // Number of elements
  unsigned int num_children;             // Number of children
  unsigned int num_points;               // Number of Points

  // Bounding box of the sub path

  float llx, lly, urx, ury;
  
  int read(const drvbase::PathInfo &, int start); // Read drawingelements
  void adjust_bbox(const Point & p);
  
  // Check if the path is inside of another path

  bool is_inside_of(const sub_path & other) const ;
  
  // Check if a point is inside of *this

  bool point_inside(const Point &) const;

  void new_points();
  
  ~sub_path() {
    delete [] children; children = nullptr;
    delete [] path; path = nullptr;
    delete [] points; points = nullptr;
    delete [] parents; parents = nullptr;
	parent = nullptr;
  };

  sub_path();
    
  // Replace every moveto by a lineto in a child path
  
  void clean();
    
  friend class sub_path_list;
//  friend struct drvbase::PathInfo;

  NOCOPYANDASSIGN(sub_path)
};

class sub_path_list
  {
  sub_path * paths;  // Array of subpaths
  unsigned int num_paths;     // Number of subpaths
public:

  sub_path_list() : paths((sub_path*)nullptr), num_paths(0) {}
  ~sub_path_list() {    
      delete [] paths;
  }
  void find_parents();
  void read(const drvbase::PathInfo &);
  void new_points();
  void clean_children();
  friend struct drvbase::PathInfo;

  NOCOPYANDASSIGN(sub_path_list)
};

 
 
 
