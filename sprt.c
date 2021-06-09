/***************************************************************************
 *   sprt.c                                                                *
 ***************************************************************************/

/***************************************************************************
* sprt    returns the photon index at which a burst occurs.                *
*   traj: data trajectory                                                  *
*      i: the beginning of the photon string to be analyzed.               *
*      N: total number of photons                                          *
*     hA: scaled A factor in Wald's SPRT                                   *
*     hB: scaled B factor in Wald's SPRT                                   *
*     hC: a constant                                                       *
*                                                                          *
* H1 is true if sum(photon durations) <= n*hC-hA                           *
* H0 is true if sum(photon durations) <= n*hC-hA                           *
* take another photon is the sum is in between.                            *
****************************************************************************/
//
// Change log:
// 20210610-HY Migrate from devel to dist.
//

#include "burst.h"
 
size_t sprt( traj, i, N, hA, hB, hC )
struct data *traj;                   // trajectory  
size_t i,N;
double hA, hB, hC;
{
  size_t f; // the photon index denoting the beginning of the burst
  size_t n=0; // number of photons under consideration
  size_t j; // dummy index
  double Sn=0.0; // sum over n photons
  
  j = i;
  f = N;
  do {
    //printf( "%u\n", j );
    Sn += traj[j].dt;
    n++;
    if ( Sn <= ((double)n*hC-hA) ) {
      // hitting the H1 ceiling: still inside the burst, reset the tester
      Sn = 0.0;
      n = 0;
      }
    else if ( Sn > ((double)n*hC-hB) ) {
      // hit the background, exit
      f = j;
      break;
      }
    j++;
    } while ( j < N );
  
  return f; 
  }
