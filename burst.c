/***************************************************************************
 *   burst.c  -  the main driver program for "burst"                       *
 ***************************************************************************/
//
// Change log:
// 20210610-HY Migrate from devel to dist.
//

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "burst.h"

#define BIN_SIZE 30

int main(int argc, char *argv[])
{
  FILE              *fpin, *fpout;
  int               i, iter, j, k, hbin;
  double            T0, TB;
  size_t            N;                            // total number of photons
  size_t            Nk=0, Nk1=0;                  // number of bursts detected
  size_t            kl, kl1, kr, krp;
  double            T, T1;                        // total length in time, s
  double            A, B;                         // decision boundaries for Wald's SPRT
  double            hA, hB, hC;                   // parameters for calculating Wald's SPRT
  double            Sa, Sb;                       // parameters for calculating Page's CUSUM
  double            h;                            // threshold for Page's CUSUM
  double            KL_disc;                      // Kullback-Liebler discrimination information
  size_t            nd;                           // expected detection delay for CUSUM
  size_t            nw;                           // expected detection delay for SPRT
  double            dt;                           // delta t, s
  double            tmp, tmp1, tmp2, tmp3, tmp4;  // temporary variables
  double            tmp5, tmp6;                   // temporary variables
  double            I0;                           // average detected emission intensity
  double            I1;                           // the thresholding intensity, calculated from I0
  double            IB,IB1;                       // background level
  double            alpha, beta;                  // type-I and type-II error rates in Wald SPRT
  double            bin_t;
  struct data       *traj=NULL;                   // trajectory
  struct burst      *bst=NULL;                    // bursts
  struct background *bkg=NULL;                    // background (off-state) segments
  double            *harvest_dt=NULL;             // collection of all harvested inter-photon duration
  double            SB=0;
  char              out_name[255], in_name[255], *endptr[255], tmpstr[255];
  
  /*******************************
  *  Get command line arguments  *
  *******************************/
  
  if (argc < 4 || argc > 6) {
    // No file name entered in the command line
    printf( "\nburst  - A photon-by-photon fluorescence burst analysis program.\n" );
    // printf( "Version: %s build [%s] (compiled with GSL version %s)\n\n", 
    //         BURST_VERSION, __DATE__, GSL_VERSION );
    printf( "Version: %s build [%s]\n\n", BURST_VERSION, __DATE__ );
    printf( "Syntax : burst filename alpha [beta background_count(cps)] [S/B_ratio]\n\n" );
    printf( "Example: burst MYFILE 0.001 0.01 2000 30\n" );
    printf( "         analyzes MYFILE with type-I error of 0.1%% and type-II error of 1%%.\n" );
    printf( "         using background count 2000 photons/s and a S/B = 30.\n" );
    printf( "         It produces output files MYFILE.bst and MYFILE.bkg.\n\n" );
    printf( "         If S/B_ratio is omitted, it will be determined from the time series,\n" );
    printf( "         e.g., burst MYFILE 0.001 0.01 2000\n\n" );
    printf( "         Similarly, background_count will be determined from data if omitted,\n" );
    printf( "         e.g., burst MYFILE 0.001 0.01\n\n" );
    printf( "Bug    : Send bug reports to hawyang@princeton.edu\n\n" );
    exit(1);
    }
  else if (argc == 4) { // the user did not supply neither S/B nor IB
    // Set the output filename
    strcpy( in_name, argv[1] );
    strcpy( out_name, argv[1] );
    strcat( out_name, ".bst" );
    alpha = strtod( argv[2], endptr );
    beta = strtod( argv[3], endptr );
    }
  else if (argc == 5) { // the user supplied IB but not S/B
    // Set the output filename
    strcpy( in_name, argv[1] );
    strcpy( out_name, argv[1] );
    strcat( out_name, ".bst" );
    alpha = strtod( argv[2], endptr );
    beta = strtod( argv[3], endptr );
    IB = strtod( argv[4], endptr ) / 1000.0; // convert to counts per ms
    }
  else if (argc == 6) {
    // Set the output filename
    strcpy( in_name, argv[1] );
    strcpy( out_name, argv[1] );
    strcat( out_name, ".bst" );
    alpha = strtod( argv[2], endptr );
    beta = strtod( argv[3], endptr );
    IB = strtod( argv[4], endptr ) / 1000.0; // convert to counts per ms
    SB = strtod( argv[5], endptr ); // signal-to-background ratio
    }
    
  /*******************************
  *          Print Title         *
  *******************************/
  printf( "\n" );
  printf( "*******************************************************************\n");
  printf( "*       Photon by photon burst detection and collection           *\n");
  printf( "*                               by                                *\n");
  printf( "*                            Haw Yang                             *\n");
  printf( "*        Department of Chemistry, Princeton University            *\n");
  printf( "*******************************************************************\n");
  printf( "Reference: K. Zhang & H. Yang, J. Phys. Chem. B, 109, 21930 (2005).\n");
  // printf( "v%s: build [%s] (compiled with GSL version %s)\n\n", 
  //         BURST_VERSION, __DATE__, GSL_VERSION );
  printf( "v%s: build [%s]\n\n", BURST_VERSION, __DATE__ );
  
  /*******************************
  *          Read in data        *
  *******************************/
  if ( (fpin=fopen(in_name,"r")) == NULL ) {
    // file does not exist, exit with error
    printf( "File [%s] does not exist. Exiting ...\n", in_name );
	exit(1);
    }
  else {
    // file exists. start reading in data and correlate
    printf( "] Reading in data file ...\n");
    bin_t = 0.0;
    N = 0;
    T1 = 0.0;
    T = 0.0;
    I0 = 0.0;
    traj = (struct data *) realloc(traj,sizeof(struct data));
    traj[0].dt = 0.0;
    traj[0].time = 0.0;
    traj[0].value = 0.0;
    while ( !feof(fpin) ) {
      
      /******************************
       * MODIFY INPUT FORMAT        *
       * ACCORDING TO APPLICATION   *
       ******************************/

      /* data from simulations, sim_diff.exe */
      fscanf(fpin, "%lf %lf %lf %lf %lf\n", \
                   &tmp1, &tmp2, &tmp3, &tmp4, &tmp5);

      N++;                                      // keep track of # data points
	  
	  traj = (struct data *) realloc(traj,(N+1)*sizeof(struct data));
      traj[N].dt = (double) tmp2 * 1000.0;     // change time unit from s to ms
      T += traj[N].dt;      
      traj[N].time = T; // wall-clock time
      traj[N].value = (double) tmp4;           // true signal strength
      
      // form initial guess for I0 by binning every 10 photons (BIN_SIZE)
      if ( fmod((double) N, BIN_SIZE) == 0 ) {
        tmp = BIN_SIZE / bin_t;
        if ( tmp > I0 ) I0 = tmp; // in situ estimate of I0
        bin_t = 0.0;
        }
      else bin_t += traj[N].dt;
      }
    fclose( fpin );
    N--;                                        // take care of overshot data
    printf( "  Total of %zu photons, %.3lf seconds read in.\n\n", N, T/1000.0);
    }
  if (N > 0) {
    traj[0].dt = 0.0;
    traj[0].time = 0.0;
    traj[0].value = 0.0;
    }
  
  /*****************************************
  *    Form initial guess for I0 and Ib    *
  *****************************************/

  if ( argc == 6 ) { // 20210608-HY, both IB and SB supplied by user
    I0 = (SB-1.0)*IB;
    I1 = I0/exp(2.0)+IB;
    }
  else if ( argc == 5) { // 20210608-HY, IB from user, in-situ estimate for I0
    I1 = I0/exp(2.0)+IB; // scale instantaneous intensity to intensity amplitude
    SB = (I0+IB)/IB;
    }
  else { // 20210608-HY do in situ estimate for both SB and IB
    IB = (double) N/T;   // assuming sparse bursts, background = average count
    I1 = I0/exp(2.0)+IB; // scale instantaneous intensity to intensity amplitude
    SB = (I0+IB)/IB;
    }
  KL_disc = (IB-I1)/I1 + log(I1/IB);
  
  /********** Wald's SPRT **********/
  A = (1.0 - beta) / alpha;
  B = beta / (1.0 - alpha);
  hA = log(A)/(I1-IB);
  hB = log(B)/(I1-IB);
  hC = log(I1/IB)/(I1-IB);
  tmp = 1.0-I1/IB+log(I1/IB);
#if defined(_WIN32) || defined(_WIN64) // 20210608-HY
  nw = (size_t) floor(((1.0-alpha)*log(B)+alpha*log(A))/tmp);
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
  nw = (size_t) round(((1.0-alpha)*log(B)+alpha*log(A))/tmp);
#endif
  /********** Page's CUSUM **********/
  tmp = alpha/3.0/(KL_disc+1.0)/(KL_disc+1.0) * log(1.0/alpha);
  h = -log(tmp);
  Sa = log(I1/IB);
  Sb = (I1-IB);
#if defined(_WIN32) || defined(_WIN64) // 20210608-HY
  nd = (size_t) floor(log(1.0/alpha)/KL_disc);
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
  nd = (size_t) round(log(1.0/alpha)/KL_disc);
#endif
  
  printf( "] Initial guesses are:\n  SB = %.2f, I0 = %e, I1 = %e.\n", \
          SB, I0, I1 );
  printf( "  IB = %e, A = %e, B = %e\n", IB, A, B );
  printf( "  hA = %e, hB = %e, hC = %e\n", hA, hB, hC );
  printf( "  Kullback-Liebler distance D(I1||IB) = %e\n", KL_disc );
  printf( "  Optimal Page's CUSUM threshold h = %e\n", h );
  printf( "  Expected minimum SPRT detection delay = %zu photons\n", nw );
  printf( "  Expected minimum CUSUM detection delay = %zu photons\n", nd );
    
  /*****************************************
  *        Start burst finding loop        *
  *****************************************/
  printf( "\n] Starting iteration ...\n" );
  
  /********** reset storage variables **********/
  free(bst);
  free(bkg);
  free(harvest_dt);
  bst = NULL;
  bkg = NULL;
  harvest_dt = NULL;
  /********** initialize variables **********/    
  Nk1 = Nk;
  Nk = 0;
  /***** the f-CUSUM step, identify the beginning of a burst *****/
  kl = cusum(traj,1,N,h,Sa,Sb); // here, kl is the start of a burst, inclusive
  while ( kl < N ) { // burst selection ends when krp or kl1 hits N
    /***** the SPRT step, for the duration of a burst, which overshoots *****/
    krp = sprt(traj,kl,N,hA,hB,hC); // Krp is the end of a burst, exclusive
    /***** the b-CUSUM step, to bracket the burst, conservatively *****/
    if (krp < N) kl1 = cusum(traj,krp,N,h,Sa,Sb); else break;
      // kl1 is the beginning of the next burst, inclusive
    if (kl1 < N) kr = cusum(traj,kl1-nd,kl,h,Sa,Sb); else break;
      // kr is the end of the burst, conservatively inclusive
    if ( kr >= kl ) { // A new burst is successfully detected
      Nk++;
      bst = (struct burst *) realloc(bst,(Nk+1)*sizeof(struct burst));
      bkg = (struct background *) realloc(bkg,(Nk+1)*sizeof(struct background));
      if (Nk == 1) {
        bst[0].l = bst[0].r = 0;
        bkg[0].l = bkg[0].r = 0;
        }
      bst[Nk].l = kl;
      bst[Nk].r = kr;
      bkg[Nk].l = bst[Nk-1].r+1;
      bkg[Nk].r = kl-1;
      } /* end of if kr >= kl */
    kl = kl1;      
    } /* end of while kl < N */

  printf( "  Done. Total of %zu bursts found.\n\n", Nk );
  
  /*****************************************
  *   Output results for collected bursts  *
  *****************************************/
  fpout = fopen( out_name, "w" );
  for (i=1; i<=Nk; i++) {
    fprintf( fpout, "%e", (bst[i].r-bst[i].l+1)/(traj[bst[i].r].time - \
             traj[bst[i].l].time + traj[bst[i].l].dt) );
    fprintf( fpout, " %e", traj[bst[i].r].time - traj[bst[i].l].time + \
             traj[bst[i].l].dt );
    fprintf( fpout, " %zu %zu\n", bst[i].l, bst[i].r );
    }
  fclose(fpout);
  
  strcpy( out_name, argv[1] );
  strcat( out_name, ".bkg" );
  fpout = fopen( out_name, "w" );
  for (i=1; i<=Nk; i++) {
    fprintf( fpout, "%e", (bkg[i].r-bkg[i].l+1)/(traj[bkg[i].r].time - \
             traj[bkg[i].l].time+traj[bkg[i].l].dt) );
    fprintf( fpout, " %e", traj[bkg[i].r].time - traj[bkg[i].l].time + \
             traj[bkg[i].l].dt );
    fprintf( fpout, " %zu %zu\n",bkg[i].l, bkg[i].r );
    }
  fclose(fpout);

goto EXIT_POINT;  
    
  /*****************************************
  *        Additional Analysis Insert      *
  *****************************************/
  // 20210608-HY

EXIT_POINT:

  /*****************************************
  *              Exit Point                *
  *****************************************/  
  
  return EXIT_SUCCESS;
}
