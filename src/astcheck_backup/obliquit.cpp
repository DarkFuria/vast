/* obliquit.cpp: function to compute earth's obliquity

Copyright (C) 2010, Project Pluto

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.    */

#include "watdefs.h"
#include "lunar.h"

#define PI 3.14159265358979323846264338327950288
#define CVT (PI / 180.)

          /* obliquity formula comes from p 135, Meeus,  Astro Algor */
          /* input is time in julian centuries from 2000. */
          /* rval is mean obliq. (epsilon sub 0) in radians */
          /* Valid range is the years -8000 to +12000 (t = -100 to 100) */

double DLL_FUNC mean_obliquity( const double t_cen)
{
   double u, u0;
   int i;
   static double t0 = 30000., rval;
   static long coeffs[10] = { -468093L, -155L, 199925L, -5138L,
            -24967L, -3905L, 712L, 2787L, 579L, 245L };

   if( t0 == t_cen)
      return( rval);
   t0 = t_cen;
   u = u0 = t_cen / 100.;     /* u is in julian 10000's of years */
   rval = 23. * 3600. + 26. * 60. + 21.448;

   for( i = 0; i < 10; i++, u *= u0)
      rval += u * (double)coeffs[i] / 100.;

   rval *= CVT / 3600.;    /* convert from seconds to radians */
   return( rval);
}

#ifdef TEST_PROGRAM

#include <stdio.h>
#include <stdlib.h>

void main( int argc, char **argv)
{
   double t0 = atof( argv[1]);
   double dt = atof( argv[2]);
   int n_steps = atoi( argv[3]);

   while( n_steps--)
      {
      printf( "%lf: %lf\n", t0,
                 mean_obliquity( (t0 - 2000.) / 100.) * (180. / PI));
      t0 += dt;
      }
}
#endif
