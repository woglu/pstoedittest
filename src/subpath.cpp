/*
   subpath.cpp: This file is part of pstoedit
         Reaggangement of paths for backends which
         don't support subpaths

   Copyright (C) 1999 Burkhard Plaum plaum_AT_ipf.uni-stuttgart.de
   Copyright (C) 1999 - 2024  Wolfgang Glunz, wglunz35_AT_pstoedit.net

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

#include "subpath.h"
#include <float.h>

// it would be much effort to check path in all situations. Normally it is OK.
//lint -esym(613,sub_path::path)  // possibly use of null pointer 
//lint -esym(794,sub_path::path)  // possibly use of null pointer 
//lint -esym(613,sub_path_list::paths)
//lint -esym(613,sub_path::points)
//lint -esym(794,sub_path::points)

static inline const Point & end_point(const basedrawingelement * e)
{
//  unsigned int NrOfPoints = e->getNrOfPoints();
//  return e->getPoint( (NrOfPoints > 0) ? (NrOfPoints - 1)  : 0 );
	return (e->getType() == curveto) ? e->getPoint(2) : e->getPoint(0);
}

// Write a subpath (only for debugging)

#if 0

static void write(const basedrawingelement & e, ostream & o)
{
	const Dtype type = e.getType();
	if (type == moveto) {
		const Point& p = e.getPoint(0);
		o << p.x_ << " " << p.y_ << " moveto" << endl;
	} else if (type == lineto) {
		const Point& p = e.getPoint(0);
		o << p.x_ << " " << p.y_ << " lineto" << endl;
	} else if (type == curveto) {
		const Point& p = e.getPoint(0);
		o << p.x_ << " " << p.y_ << " ";
		p = e.getPoint(1);
		o << p.x_ << " " << p.y_ << " ";
		p = e.getPoint(2);
		o << p.x_ << " " << p.y_ << " curveto" << endl;
	} else if (type == closepath) {
		o << "closepath" << endl;
	}
}

static void write(const drvbase::PathInfo & p)
{
	cerr << "numberOfElementsInPath: " << p.numberOfElementsInPath << endl;
	for (unsigned int i = 0; i < p.numberOfElementsInPath; i++)
		write(*p.path[i], cerr);
}

#endif

sub_path::sub_path()
{
	flags = 0;
	num_elements = 0;
	num_points = 0;
	num_children = 0;
	num_outside = 0;

	// pointers
	children = nullptr;
	path = nullptr;
	points = nullptr;
	parents = nullptr;
	parent = nullptr;

	llx = FLT_MAX;
	lly = FLT_MAX;
	urx = -FLT_MAX;
	ury = -FLT_MAX;
}

void sub_path::clean()
{
	{
		// The moveto is always the first element of the subpath
		const Point & p = path[0]->getPoint(0);
		basedrawingelement *newLineto = new Lineto(p.x(), p.y());
		// Now we can delete the path element, we no longer need p (& !!)
		delete path[0];
		path[0] = newLineto;
	}
	// Replace a final closepath with a lineto
	if (path[num_elements - 1]->getType() == closepath) {
		const Point & p = path[0]->getPoint(0);
		basedrawingelement *newLineto = new Lineto(p.x(), p.y());
		// Now we can delete the path element, we no longer need p (& !!)
		delete path[num_elements - 1];
		path[num_elements - 1] = newLineto;
	}

}

void sub_path::adjust_bbox(const Point & p)
{
	if (p.x() < llx)
		llx = p.x();
	if (p.y() < lly)
		lly = p.y();
	if (p.x() > urx)
		urx = p.x();
	if (p.y() > ury)
		ury = p.y();
}

// Read drawingelements
// Arguments:
// main_path:    path to read from
// start:        Starting index 
// Return value: Index of the final closepath
// Calculate the bounding box of the path

int sub_path::read(const drvbase::PathInfo & main_path, int start)
{
	unsigned int i, j;

	llx = FLT_MAX;
	lly = FLT_MAX;
	urx = -FLT_MAX;
	ury = -FLT_MAX;

	// Count the number of elements is this path

	for ( ; ; ) { // while true but without compiler warning
		if (start + num_elements == main_path.numberOfElementsInPath)
			break;
		if (num_elements && (main_path.path[start + num_elements]->getType() == moveto))
			break;
		num_elements++;
	}

	path = new basedrawingelement *[num_elements];

	// Copy the Elements and count the number of ALL points

	for (i = 0; i < num_elements; i++) {
		for (unsigned int pc = 0; pc < main_path.path[start + i]->getNrOfPoints(); pc++) {
			adjust_bbox(main_path.path[start + i]->getPoint(pc));
		}
		// cout << "cloning " << (void *) main_path.path[start+i] << endl;
		path[i] = main_path.path[start + i]->clone();

		num_points += main_path.path[start + i]->getNrOfPoints();
		// cout << num_points << " done" << endl;
	}
	// cout << "done with copy path " << num_points << endl;
	points = new Point[num_points?num_points:1]; // at least 1 - at least for FlexeLint

	// Copy ALL points
	// cout << "now copy points " << num_points << endl;
	j = 0;
	for (i = 0; i < num_elements; i++) {
		// possibly replace by
		// Burkhard: Closepaths are ok here
#if 0
		if (path[i]->getType() == closepath) {
			cerr << "unexpected closepath found in subpath.cpp" << endl;
			exit(1);
		}
#endif
		for (unsigned int pc = 0; pc < path[i]->getNrOfPoints(); pc++) {
			points[j] = path[i]->getPoint(pc);
			j++;
		}
	}
	return start + num_elements;
}

void sub_path::new_points()
{
	// refill the array with the points which can be used
	// for connections
	// Now, only the endpoints of curvetos are stored
	// The size of the array is num_elements

	for (unsigned int i = 0; i < num_elements; i++)
		if (path[i]->getType() != closepath)
			points[i] = end_point(path[i]);
}

// calculate a 2 x 2 determinant

static constexpr double ddet(const double a11, const double a12, const double a21, const double a22)
{
	return a11 * a22 - a21 * a12;
}



// Check if a Point is inside of *this

bool sub_path::point_inside(const Point & p) const
{
	int crossings = 0;

	const double x1 = -1.0;
	//lint -esym(578,y1) // under MSVC math.h contains a y1
	const double y1 = -1.0; 
	const double x2 = p.x();
	const double y2 = p.y();

	// Burkhard: Bugfix 

	for (unsigned int i = 0; i < num_points; i++) {
		const unsigned int j = (i == num_points - 1) ? 0 : i + 1;
		const double x3 = points[i].x();
		const double y3 = points[i].y();
		const double x4 = points[j].x();
		const double y4 = points[j].y();

		// Check wether the lines between (x1,y1)-(x2,y2) and
		// (x3-y3)-(x4,y4) cross each other

		// The system to solve is:
		// (x2 - x1) * t1 - (x4 - x3) * t2 = x3 - x1;
		// (y2 - y1) * t1 - (y4 - y3) * t2 = y3 - y1;
		//
		// The solutions are: t1 = det1/det
		//                    t2 = det2/det
		//
		// The lines cross each other if
		// 0.0 < t1 < 1.0 and 0.0 < t2 < 1.0

		const double det = ddet((x2 - x1), -(x4 - x3),
								(y2 - y1), -(y4 - y3));

		// is det == 0, the lines are parallel

		if (!det)
			continue;

		const double det1 = ddet((x3 - x1), -(x4 - x3),
								 (y3 - y1), -(y4 - y3));
		const double det2 = ddet((x2 - x1), (x3 - x1),
								 (y2 - y1), (y3 - y1));

		if ((det < 0.0)
			&& ((det1 > 0.0) || (det2 > 0.0) || (det1 < det)
				|| (det2 < det)))
			continue;

		if ((det > 0.0)
			&& ((det1 < 0.0) || (det2 < 0.0) || (det1 > det)
				|| (det2 > det)))
			continue;


		// Special case:
		// (t1 == 0.0) || (t1 == 1.0) || (t2 == 0.0) || (t2 == 1.0)
		// t1 == 0.0 should never happen
		// This means that the line (x1, y1)-(x2,y2)
		// goes through either (x3,y3) or (x4,y4) or (x2,y2) lies
		// on the line (x3,y3)-(x4,y4)
		// A proper implementation would handle this correctly

		//    if((det == det1) || (det == det2) || (!det1) || (!det2))
		//  cout << "Blupp!!" << endl;

		crossings++;
	}
	return (crossings & 1);
}


// Check if *this is inside of another path

bool sub_path::is_inside_of(const sub_path & other) const
{
	unsigned int inside = 0;
	unsigned int outside = 0;

	// Check if Bounding Box lies outside the other

	if (!((llx >= other.llx) && (lly >= other.lly) && (urx <= other.urx) && (ury <= other.ury)))
		return false;

	for (unsigned int i = 0; i < num_points; i++) {
		if (other.point_inside(points[i]))
			inside++;
		else
			outside++;
	}
	// Make a democratic decision
	return inside > outside;
}

void sub_path_list::clean_children()
{
	for (unsigned int i = 0; i < num_paths; i++)
		if (paths[i].parent)
			paths[i].clean();
}

void sub_path_list::find_parents()
{
	// Find the parent of each subpath 

	unsigned int i, j;
	sub_path *parent = nullptr;

	// Calculate for each path the number of paths 
	// outside

	for (i = 0; i < num_paths; i++) {
		paths[i].parents = new sub_path *[num_paths - 1];
		for (j = 0; j < num_paths; j++) {
			if (i != j) {
				if (paths[i].is_inside_of(paths[j])) {
					paths[i].parents[paths[i].num_outside] = &paths[j];
					paths[i].num_outside++;
				}
			}
		}
	}

	// Now find the real parent paths
	// A path can be a parent path, if the number of outside paths is even

	// Check which paths can be parents

	for (i = 0; i < num_paths; i++) {
		if (!(paths[i].num_outside & 1)) {
			paths[i].parent = (sub_path *) nullptr;
			paths[i].children = new sub_path *[num_paths - 1];
		}
	}

	// Now bring the kids to their parents

	for (i = 0; i < num_paths; i++) {
		// This can be a parent path
		if (!(paths[i].num_outside & 1))
			continue;
		else {
			for (j = 0; j < paths[i].num_outside; j++) {
				if (paths[i].num_outside == (paths[i].parents[j]->num_outside + 1)) {
					parent = paths[i].parents[j];
					paths[i].parent = parent;
					parent->children[parent->num_children] = &paths[i];
					parent->num_children++;
					break;
				}
			}
		}
	}

}

void sub_path_list::new_points()
{
	for (unsigned int i = 0; i < num_paths; i++)
		paths[i].new_points();
}

void sub_path_list::read(const drvbase::PathInfo & path_info)
{
	unsigned int i, j;
	num_paths = 0;

	// Count the number of movetos

	for (i = 0; i < path_info.numberOfElementsInPath - 1; i++) { // ??? why -1 ???
		if (path_info.path[i]->getType() == moveto)
			num_paths++;
	}

	paths = new sub_path[num_paths];
	j = 0;

	for (i = 0; i < num_paths; i++)
		j = paths[i].read(path_info, j);
}


static float get_min_distance( const basedrawingelement * const * p1,
							   const basedrawingelement * const * p2,
							  unsigned int size1,
							  unsigned int size2, unsigned int &index1, unsigned int &index2)
{
	float ret = FLT_MAX;

	for (unsigned int i = 0; i < size1; i++) {
		if (p1[i]->getType() != closepath) {	// wogl - inserted this check 
			const Point & point1 = end_point(p1[i]);
			for (unsigned int j = 0; j < size2; j++) {
				const Point & point2 = end_point(p2[j]);
				if (p1[i]->getType() != closepath) {
					const float tmp1 = point1.x() - point2.x();
					const float tmp2 = point1.y() - point2.y();
					const float pdistance = tmp1 * tmp1 + tmp2 * tmp2;
					if (pdistance < ret) {
						ret = pdistance;
						index1 = i;
						index2 = j;
					}
				}
			}
		}
	}
	return ret;
}

static void
insert_subpath( basedrawingelement **  parent_path,
			    basedrawingelement * const *  child_path,
			   unsigned int parent_size,
			   unsigned int child_size, unsigned int parent_index, unsigned int child_index)
{
	// First find the point we have to lineto
	const Point & point1 = end_point(child_path[child_index]);
	basedrawingelement *first_lineto = new Lineto(point1.x(), point1.y());
	const Point & point2 = end_point(parent_path[parent_index]);
	basedrawingelement *last_lineto = new Lineto(point2.x(), point2.y());
	// Make a little space
	{for (unsigned int i = parent_size - 1; i >= parent_index + 1; i--)
		parent_path[i + child_size + 2] = parent_path[i]; }
	parent_path[parent_index + 1] = first_lineto;
	unsigned int src_index = child_index;
	{for (unsigned int i = 0; i < child_size; i++) {
		src_index++;
		if (src_index == child_size)
			src_index = 0;
		parent_path[parent_index + 2 + i] = child_path[src_index];
	}}

	parent_path[parent_index + 2 + child_size] = last_lineto;
}

void drvbase::PathInfo::rearrange()
{
//	cerr << " numberOfElementsInPath on input " << numberOfElementsInPath << endl;
	//  write(*this);
	sub_path_list list;
	sub_path *child  = (sub_path *) nullptr;
	sub_path *parent = (sub_path *) nullptr;
	//  cerr << "Reading subpaths" << endl;
	list.read(*this);			// Read the list from this
	//  cerr << "Searching for parents" << endl;
	list.find_parents();		// Find the parents of all children
	list.new_points();			// Make the new point arrays
	list.clean_children();
	
	// temp hack to insert some additional space into the STL vector
	// which might be needed by the code below - since it doesn't check 
	// the index anymore.
	// TBD - fix this.
	const int additional_space = 4;
	for (int s = 0; s < additional_space; s++) {
		addtopath(new Lineto(0.0f,0.0f));
		addtopath(new Lineto(0.0f,0.0f));
	}


	clear();					// Clear the path
	//  cerr << "Rearranging path" << endl;
	// Write the elements back
	{ 
	 unsigned int tmp_num = 0;
 	 for (unsigned int i = 0; i < list.num_paths; i++) {
		// Find the next parent

		if (list.paths[i].parent)
			continue;
		parent = &list.paths[i];
		// Copy the elements of the parent path
		{for (unsigned int j = 0; j < parent->num_elements; j++)
			path[tmp_num + j] = parent->path[j];}
		tmp_num += parent->num_elements;
		{for (unsigned int j = 0; j < parent->num_children; j++) {
			// In this loop, we find the closest child and
			// insert it into the path
			float min_distance = FLT_MAX;
			unsigned int parent_index = 0, child_index = 0;
			for (unsigned int k = 0; k < parent->num_children; k++) {
				unsigned int test_parent_index= 0, test_child_index = 0;
				if (parent->children[k]->flags & PS_PATH_IS_CONNECTED)
					continue;
				const float test_distance =
					get_min_distance(&path
									 [numberOfElementsInPath], 
									 parent->children[k]->path,
									 tmp_num -
									 numberOfElementsInPath,
									 parent->children[k]->
									 num_elements, test_parent_index, test_child_index);
				if (test_distance < min_distance) {
					min_distance = test_distance;
					parent_index = test_parent_index;
					child_index = test_child_index;
					child = parent->children[k];
				}
			}
//			assert(child != 0 && "fatal error in pstoedit::subpath.cpp::drvbase::PathInfo::rearrange");
			if (child) {
				insert_subpath(&path[0], child->path, tmp_num,
						   child->num_elements, parent_index + numberOfElementsInPath, child_index);
				child->flags |= PS_PATH_IS_CONNECTED;
				tmp_num += child->num_elements + 2;
			}
		}}
#if 0
		if (tmp_num > numberOfElementsInPath) {
			cerr << "increasing numberOfElementsInPath from " << numberOfElementsInPath << " to " << tmp_num<< endl;
		}
#endif
		numberOfElementsInPath = tmp_num;
	} }

	// Remove duplicate linetos
#if 1
	{for (unsigned int i = 0; i + 1 < numberOfElementsInPath; i++) {
		if ((path[i]->getType() == lineto)
			&& (path[i + 1]->getType() == lineto)) {
			const Point & pp1 = path[i]->getPoint(0);
			const Point & pp2 = path[i + 1]->getPoint(0);
			if (pp1 == pp2) {
				delete path[i];
				for (unsigned int j = i; j + 1 < numberOfElementsInPath; j++) {
					path[j] = path[j + 1];
				}
				numberOfElementsInPath--;
			}
		}
	}}
#endif
	//  write(*this);
}
