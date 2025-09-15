/* 
   drvfuncs.h : This file is part of pstoedit
   This file contains the declarations of the functions, // each driver has to implement

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

private: // yes these are private, library users should use the public interface
	 // provided via drvbase

	void open_page() override;

	void close_page() override;

	void show_path() override; 

//	void show_text(const TextInfo & textinfo);

//	void show_rectangle(const float llx, const float lly, const float urx, const float ury);
