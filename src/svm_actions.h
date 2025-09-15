/*************************************************************************
 *
 *  $RCSfile: metaact.hxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: kz $ $Date: 2005/01/13 17:40:40 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef __svm_actions_h__
#define __svm_actions_h__

// The content of this file was taken from OpenOffice.org's codebase,
// specifically from vcl/inc/metaact.hxx

// --------------------------
// - The metaaction defines -
// --------------------------

#define META_NULL_ACTION					(0)
#define META_PIXEL_ACTION					(100)
#define META_POINT_ACTION					(101)
#define META_LINE_ACTION					(102)
#define META_RECT_ACTION					(103)
#define META_ROUNDRECT_ACTION				(104)
#define META_ELLIPSE_ACTION 				(105)
#define META_ARC_ACTION 					(106)
#define META_PIE_ACTION 					(107)
#define META_CHORD_ACTION					(108)
#define META_POLYLINE_ACTION				(109)
#define META_POLYGON_ACTION 				(110)
#define META_POLYPOLYGON_ACTION 			(111)
#define META_TEXT_ACTION					(112)
#define META_TEXTARRAY_ACTION				(113)
#define META_STRETCHTEXT_ACTION 			(114)
#define META_TEXTRECT_ACTION				(115)
#define META_BMP_ACTION 					(116)
#define META_BMPSCALE_ACTION				(117)
#define META_BMPSCALEPART_ACTION			(118)
#define META_BMPEX_ACTION					(119)
#define META_BMPEXSCALE_ACTION				(120)
#define META_BMPEXSCALEPART_ACTION			(121)
#define META_MASK_ACTION					(122)
#define META_MASKSCALE_ACTION				(123)
#define META_MASKSCALEPART_ACTION			(124)
#define META_GRADIENT_ACTION				(125)
#define META_HATCH_ACTION					(126)
#define META_WALLPAPER_ACTION				(127)
#define META_CLIPREGION_ACTION				(128)
#define META_ISECTRECTCLIPREGION_ACTION 	(129)
#define META_ISECTREGIONCLIPREGION_ACTION	(130)
#define META_MOVECLIPREGION_ACTION			(131)
#define META_LINECOLOR_ACTION				(132)
#define META_FILLCOLOR_ACTION				(133)
#define META_TEXTCOLOR_ACTION				(134)
#define META_TEXTFILLCOLOR_ACTION			(135)
#define META_TEXTALIGN_ACTION				(136)
#define META_MAPMODE_ACTION 				(137)
#define META_FONT_ACTION					(138)
#define META_PUSH_ACTION					(139)
#define META_POP_ACTION 					(140)
#define META_RASTEROP_ACTION				(141)
#define META_TRANSPARENT_ACTION 			(142)
#define META_EPS_ACTION 					(143)
#define META_REFPOINT_ACTION				(144)
#define META_TEXTLINECOLOR_ACTION			(145)
#define META_TEXTLINE_ACTION				(146)
#define META_FLOATTRANSPARENT_ACTION		(147)
#define META_GRADIENTEX_ACTION				(148)
#define META_LAYOUTMODE_ACTION 				(149)
#define META_TEXTLANGUAGE_ACTION 			(150)

#define META_COMMENT_ACTION 				(512)

#endif
