/* this file is taken from the hp2xx project (http://www.gnu.org/software/hp2xx/hp2xx.html) 
   and slightly adapted to fit into pstoedit s framework */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define PSTOEDIT 1
#ifdef PSTOEDIT
#include "drvbase.h"
enum action_type { MOVE_TO, DRAW_TO };
static drvbase* backend; // avoid making all the local functions members.
static void Pen_action_to_tmpfile(action_type a, const struct HPGL_Pt2* p, int scale_flag);


#else

#include "bresnham.h"
#include "hp2xx.h"
#include "hpgl.h"
#include "lindef.h"
#include "pendef.h"
#endif

#include <assert.h>

struct HPGL_Pt2 {
    double  x, y;
	HPGL_Pt2() : x(0), y(0) {}
};

struct HPGL_Mat {
    double  xx, xy, xt;
    double  yx, yy, yt;
};

#define HPGL_Pt HPGL_Pt2

inline HPGL_Pt transform(const HPGL_Mat& m, const double x, const double y)
{
    HPGL_Pt result;
    result.x = static_cast<float>(m.xt + m.xx * x + m.xy * y);
    result.y = static_cast<float>(m.yt + m.yx * x + m.yy * y);
    return result;
}

static void transform_points(const HPGL_Mat& m, HPGL_Pt2 *const dst, const HPGL_Pt *const src, const size_t count)
{
    const double xx = m.xx, xy = m.xy, xt = m.xt,
                 yx = m.yx, yy = m.yy, yt = m.yt;

	for (size_t i = 0; i < count; i++) {
        const double x = src[i].x,
                     y = src[i].y;
        dst[i].x = xt + xx*x + xy*y;
        dst[i].y = yt + yx*x + yy*y;
    }
}

static void rotation(HPGL_Mat *const forward,
                     HPGL_Mat *const backward,
                     const HPGL_Pt2& center,
                     const HPGL_Pt2& move,
                     double    const angle,
                     double    const scale)
{
    const double radians = angle * 0.0174532925199432957692369; /* Pi/180 */
    const double si = sin(radians),
                 co = cos(radians);

    if (forward) {
        const double s = si * scale,
                     c = co * scale;
        forward->xx =  c;
        forward->xy = -s;
        forward->xt = -c * center.x + s * center.y + move.x * scale;
        forward->yx =  s;
        forward->yy =  c;
        forward->yt = -s * center.x - c * center.y + move.y * scale;
    }

    if (backward) {
        const double s = si / scale,
                     c = co / scale;
        backward->xx =  c;
        backward->xy =  s;
        backward->xt =  center.x - co * move.x - si * move.y;
        backward->yx = -s;
        backward->yy =  c;
        backward->yt =  center.y + si * move.x - co * move.y;
    }
}

/* octants:  +y
 *         |5 3 4|
 *       -x|2 0 1|+x
 *         |8 6 7|
*/

static constexpr int octant(const double x, const double y)
{
   const int x_contrib = (x > 0.0) ? 1 : ((x < 0.0) ? 2 : 0);
   const int y_contrib = (y > 0.0) ? 3 : ((y < 0.0) ? 6 : 0);
   return x_contrib + y_contrib;
   /*
    return (x > 0.0 ? 1 : 0)
         + (x < 0.0 ? 2 : 0)
         + (y > 0.0 ? 3 : 0)
         + (y < 0.0 ? 6 : 0);
   */
}

//lint -e835 -e778 // -0
static const int octant_winding[9][9] = {
    { -0, -0, -0, -0, -0, -0, -0, -0, -0 }, /* -0: Start or end point at vertex */
    { -0,  0, 00, -2, -1, -3,  2,  1,  3 }, /* 00: On boundary line */
    { -0, 00,  0,  2,  3,  1, -2, -3, -1 }, /* +0: Intersection check needed */
    { -0,  2, -2,  0,  1, -1, 00,  3, -3 },
    { -0,  1, -3, -1,  0, -2,  3,  2, +0 },
    { -0,  3, -1,  1,  2,  0, -3, +0, -2 },
    { -0, -2,  2, 00, -3,  3,  0, -1,  1 },
    { -0, -1,  3, -3, -2, +0,  1,  0,  2 },
    { -0, -3,  1,  3, +0,  2, -1, -2,  0 }
};

enum inout {
    INSIDE   = 0,
    BOUNDARY = 1,
    OUTSIDE  = 2
} ;

static inout inside(const HPGL_Pt2 point[],
                    const size_t points,
                    const double   x,
                    const double   y,
                    const int      /* nonzero */)
{
    int    winding = 0; /* +-8 per full turn */
    double curr_x = point[points-1].x - x;
    double curr_y = point[points-1].y - y;
    int curr_octant = octant(curr_x, curr_y);
    if (!curr_octant)
        return BOUNDARY;

    for (size_t i = 0; i < points; i++) {
		double distance;
        const double prev_x = curr_x;
        const double prev_y = curr_y;
        const int prev_octant = curr_octant;

        curr_x = point[i].x - x;
        curr_y = point[i].y - y;
        curr_octant = octant(curr_x, curr_y);
        if (!curr_octant)
            return BOUNDARY;

        if (prev_octant == curr_octant)
            continue;

        /* Polygon edge passes through the point? */
        if ((prev_octant == 2 && curr_octant == 1) ||
            (prev_octant == 1 && curr_octant == 2) ||
            (prev_octant == 3 && curr_octant == 6) ||
            (prev_octant == 6 && curr_octant == 3))
            return BOUNDARY;

        if (prev_octant == 4 && curr_octant == 8) {
            /* +x,+y to -x,-y */
            const double dx = prev_x - curr_x,
                         dy = prev_y - curr_y;

            if (dx >= dy) {
                if (prev_x >= -curr_x)
                    distance = (curr_x * dy) / dx - curr_y;
                else
                    distance = (prev_x * dy) / dx - prev_y;
            } else {
                if (prev_y >= -curr_y)
                    distance = curr_x - (curr_y * dx) / dy;
                else
                    distance = prev_x - (prev_y * dx) / dy;
            }

            if (distance > 0.0)
                winding += 4;
            else
            if (distance < 0)
                winding -= 4;
            else
                return BOUNDARY;

            continue;
        }

        if (prev_octant == 5 && curr_octant == 7) {
            /* -x,+y to +x,-y */
            const double dx = curr_x - prev_x,
                         dy = prev_y - curr_y;

            if (dx >= dy) {
                if (-prev_x >= curr_x)
                    distance = curr_y + (curr_x * dy) / dx;
                else
                    distance = prev_y + (prev_x * dy) / dx;
            } else {
                if (prev_y >= -curr_y)
                    distance = curr_x + (curr_y * dx) / dy;
                else
                    distance = prev_x + (prev_y * dx) / dy;
            }

            if (distance > 0.0)
                winding += 4;
            else
            if (distance < 0.0)
                winding -= 4;
            else
                return BOUNDARY;

            continue;
        }

        if (prev_octant == 7 && curr_octant == 5) {
            /* +x,-y to -x,+y */
            const double dx = prev_x - curr_x,
                         dy = curr_y - prev_y;
            if (dx >= dy) {
                if (prev_x >= -curr_x)
                    distance = (-curr_x * dy) / dx - curr_y;
                else
                    distance = (-prev_x * dy) / dx - prev_y;
            } else {
                if (-prev_y >= curr_y)
                    distance = (-curr_y * dx) / dy - curr_x;
                else
                    distance = (-prev_y * dx) / dy - prev_x;
            }

            if (distance > 0.0)
                winding += 4;
            else
            if (distance < 0.0)
                winding -= 4;
            else
                return BOUNDARY;

            continue;
        }

        if (prev_octant == 8 && curr_octant == 4) {
            /* -x,-y to +x,+y */
            const double dx = curr_x - prev_x,
                         dy = curr_y - prev_y;

            if (dx >= dy) {
                if (-prev_x >= curr_x)
                    distance = curr_y - (curr_x * dy) / dx;
                else
                    distance = prev_y - (prev_x * dy) / dx;
            } else {
                if (-prev_y >= curr_y)
                    distance = (curr_y * dx) / dy - curr_x;
                else
                    distance = (prev_y * dx) / dy - prev_x;
            }

            if (distance > 0.0)
                winding += 4;
            else
            if (distance < 0.0)
                winding -= 4;
            else
                return BOUNDARY;

            continue;
        }

        winding += octant_winding[prev_octant][curr_octant];
    }

    /* Adjust for polygon orientation. */
    if (winding < 0)
        winding = -winding;

    /* Not inside? */
    if (winding < 4)
        return OUTSIDE;

    /* Nonzero mode? */
    return INSIDE;
}

static inline int insert(double list[], unsigned int count, const double value)
{
	size_t i = 0;
	while (i < count && list[i] < value) {
		i++;
	}
        

    if (i >= count) {
        list[count] = value;
        return count + 1;
    }

    if (list[i] == value)
        return count;

    memmove(&(list[i+1]), &(list[i]), (size_t)(count - i) * sizeof list[0]);
    list[i] = value;

    return count + 1;
}

static inline int insert_intersection(double list[], int count, const HPGL_Pt2& p0, const HPGL_Pt2& p1, const double y)
{
    return insert(list, count, p0.x + (p1.x - p0.x) * (y - p0.y) / (p1.y - p0.y));
}


inline double my_round(double d)
{
  return floor(d + 0.5);
}

static void do_fill(const HPGL_Pt2 point[],
                    const size_t      points,
                    const HPGL_Mat& /* forward */ ,
                    const HPGL_Mat& backward,
                    const double   ymin,
                    const double   ymax,
                    const int      scale_flag,
                    const int      nzfill)
{
    const long  ny = (long)my_round(ymax - ymin);
    auto intersect = new double[points];
   // intersect = (double*) malloc((size_t)points * sizeof intersect[0]);

    if (!intersect)
        return;

    for (long n = 0L; n <= ny; n++) {
        const double y = ymin + (double)n;
        HPGL_Pt      p;

        int intersections = 0;
        for (size_t i = 0; i < points; i++)
            if (point[i].y == y)
                intersections = insert(intersect, intersections, point[i].x);
            else
            if (point[i].y <= y && point[i+1].y > y)
                intersections = insert_intersection(intersect, intersections, point[i+1], point[i], y);
            else
            if (point[i].y > y && point[i+1].y <= y)
                intersections = insert_intersection(intersect, intersections, point[i], point[i+1], y);

        intersections--;

        while (intersections > 0)
            if (inside(point, points, (intersect[intersections] + intersect[intersections-1])/2.0, y, nzfill) == INSIDE) {
                p = transform(backward, intersect[intersections--], y);
                Pen_action_to_tmpfile(MOVE_TO, &p, scale_flag);
                while (intersections > 0 && inside(point, points, (intersect[intersections] + intersect[intersections-1])/2.0, y, nzfill) != OUTSIDE)
                    intersections--;
                p = transform(backward, intersect[intersections], y);
                Pen_action_to_tmpfile(DRAW_TO, &p, scale_flag);
            } else
                intersections--;
    }

    delete [] intersect; // free(intersect);
}

static void strokedfill(const HPGL_Pt polygon[],
          int     numpoints,
          HPGL_Pt /*point1 */,
          HPGL_Pt /*point2 */,
          int     scale_flag,
          int     filltype,
          float   spacing,
          float   hatchangle,
          float   curwidth,
          int     /* curdpi */,
          int     nzfill)
{
    double    scale, ymin, ymax;


    if (numpoints < 3)
        return;

    if (filltype == 1 || filltype == 2)
        scale = 1.0 / (double)curwidth;
    else
    if (filltype == 3 && spacing > 0.0f)
        scale = 1.0 / (double)spacing;
    else
    if (filltype == 4 && spacing > 0.0f)
        scale = 1.0 / (double)spacing;
    else
        return;

    auto poly = new HPGL_Pt2[numpoints + 1]; // (HPGL_Pt2*) malloc((size_t)(numpoints + 1) * sizeof poly[0]);
    if (!poly) return;

    /* TODO: Center of rotation (center)? Fill startpoint (move)? */
    HPGL_Pt2  center, move;
    center.x = polygon[0].x;
    center.y = polygon[0].y;
    move.x = -polygon[0].x;
    move.y = -polygon[0].y;

	
    HPGL_Mat forward, backward;
    rotation(&forward, &backward, center, move, hatchangle, scale);

    transform_points(forward, poly, polygon, numpoints);

    int i = 0;
    int o = 0;
    while (i < numpoints) {
        poly[o] = poly[i++];
        while (i < numpoints && poly[i].x == poly[o].x && poly[i].y == poly[o].y)
            i++;
        o++;
    }
    numpoints = o;
    poly[numpoints] = poly[0];

    /* TODO:
     *	PlotCmd_to_tmpfile(DEF_PW);
	 *  Pen_Width_to_tmpfile(1, penwidth/10.);
     *  LlotCmd_to_tmpfile(DEF_LA);
	 *  Line_Attr_to_tmpfile(LineAttrEnd, LAE_round);
    */

    /* Obtain Y range for polygon. */
    ymin = ymax = poly[0].y;
    for (i = 1; i < numpoints; i++)
        if (poly[i].y < ymin)
            ymin = poly[i].y;
        else
        if (poly[i].y > ymax)
            ymax = poly[i].y;

    /* Constrict y range to integer coordinates (hatching has unit spacing) */
    if (ymax >= 0.0)
        ymax = floor(ymax);
    else
        ymax = ceil(ymax);
    if (ymin >= 0.0)
        ymin = ceil(ymin);
    else
        ymin = floor(ymin);

    /* Draw first hatching lines. */
    do_fill(poly, numpoints, forward, backward, ymin, ymax, scale_flag, nzfill);

    /* Crosshatch mode? */
    if (filltype == 4) {
        rotation(&forward, &backward, center, move, hatchangle + 1.57079632679489661923132, scale);

        transform_points(forward, poly, polygon, numpoints);
        poly[numpoints] = poly[0];

        /* Obtain Y range for polygon. */
        ymin = ymax = poly[0].y;
        for (i = 1; i < numpoints; i++)
            if (poly[i].y < ymin)
                ymin = poly[i].y;
            else
            if (poly[i].y > ymax)
                ymax = poly[i].y;

        /* Constrict y range to integer coordinates (hatching has unit spacing) */
        if (ymax >= 0.0)
            ymax = floor(ymax);
        else
            ymax = ceil(ymax);
        if (ymin >= 0.0)
            ymin = ceil(ymin);
        else
            ymin = floor(ymin);

        /* Draw crosshatch. */
        do_fill(poly, numpoints, forward, backward, ymin, ymax, scale_flag, nzfill);
    }

    delete [] poly; //free(poly);
}


void drvbase::simulate_fill() {
	// replace current path with a new one containing fill strokes.
	// ?? assumption? curves??  fill above seems to support just straight lines
	/// moveto == lineto for fill ??

 backend = this;
 auto  polygon = new HPGL_Pt[numberOfElementsInPath()];
 int p_i = 0;
 for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
      const basedrawingelement & elem = pathElement(n);
      switch (elem.getType()) {
      case moveto: {
		  const Point & p = elem.getPoint(0);
		  polygon[p_i].x = p.x();
		  polygon[p_i].y = p.y();
		  p_i++;
		  break;
	  }				   
      case lineto: {
		  const Point & p = elem.getPoint(0);
		  polygon[p_i].x = p.x();
		  polygon[p_i].y = p.y();
		  p_i++;
		  break ;
	  }
	  case closepath: {
/// TODO

		  break;			  
	  }
      case curveto:	{
	// TBD
		  break;
	  }
      default:
		errf << "\t\tFatal: unexpected path element in fillpoly" << endl;
		abort();
		break;
      }
  } // for

 // reset path and build new.
 outputPath->clear();
 PathInfo* savePath = currentPath;
 currentPath = outputPath; // because we write to currentPath subsequently but want to replace the current outputPath directly.
 backend->setCurrentShowType(drvbase::stroke);
 backend->setCurrentLineWidth(1); // TODO
   //pathnumber++;
 //backend->setIsPolygon(false);
  //backend->setPathNumber(pathnumber);

 const HPGL_Pt point1, point2;
  strokedfill(polygon,
       p_i, //   int     numpoints,
       point1, //   HPGL_Pt point1,
       point2, //   HPGL_Pt point2,
       1, //   int     scale_flag,
       1, //    int     filltype,
       10, //   float   spacing,
       1,//    float   hatchangle,
       10,//    float   curwidth, !!!!
       10,//   int     curdpi,
       1//   int     nzfill)
	   );

  delete [] polygon;
  currentPath = savePath; // now we are done with writing to "outputPath" so restore
  show_path();
}

static void Pen_action_to_tmpfile(action_type a, const struct HPGL_Pt2* p, int /* scale_flag */) {
	if (a == MOVE_TO) {
		backend->addtopath(new Moveto((float)p->x,(float)p->y));  
		printf("adding Moveto %d %d \n",(int)p->x,(int)p->y);
	} else {
		backend->addtopath(new Lineto((float)p->x,(float)p->y));  
		printf("adding lineto %d %d \n",(int)p->x,(int)p->y);
	}

}
