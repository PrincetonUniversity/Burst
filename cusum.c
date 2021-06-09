/***************************************************************************
 *   cumsum.c                                                              *
 ***************************************************************************/

/***************************************************************************
 * cusum(traj,i,f,h,Sa,Sb)                                                 *
 *   traj: arrayed data structure containing photon informaiton            *
 *      i: beginning photon index for the CUSUM procedure                  *
 *      f: ending photon index for the CUSUM procedure                     *
 *      h: threshold                                                       *
 *     Sa: parameter to calculate the cusum function                       *
 *     Sb: parameter to calculate the cusum function                       *
 *                                                                         *
 * S_n = S_{n-1}+Sa-traj[n].dt*Sb                                          *
 ***************************************************************************/
//
// Change log:
// 20210610-HY Migrate from devel to dist.
//
 
#include "burst.h"
 
size_t cusum( traj, i, f, h, Sa, Sb )
struct data *traj;                   // trajectory  
size_t i,f;
double h, Sa, Sb;
{
  size_t j; // dummy index
  size_t k;
  int    dj=1; // increment/decrement of j
  double Sn=0.0; // sum over n photons
  
  if ( i < f ) dj = 1; // forward CUSUM
  else if ( i > f ) dj = -1; // backward CUSUM
  j = i;
  k = f;
  do {
    Sn += Sa-traj[j].dt*Sb;
    if ( Sn < 0 ) {
      // this is a background photon bunch, reset the tester
      Sn = 0.0;
      }
    else if ( Sn >= h ) {
      // this is a burst!
      k = j;
      break;
      }
    j += dj;
    } while ( j != f );
  
  return k;
  }
  
