/***************************************************************************
 *   burst.h                                                               *
 ***************************************************************************/
//
// Change log:
// 20210610-HY Migrate from devel to dist.
//

/*******************************
*          Head Files          *
*******************************/
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>

/***** Determine OS *****/

#if defined(_WIN32)
    #define PLATFORM_NAME "windows" // Windows
#elif defined(_WIN64)
    #define PLATFORM_NAME "windows" // Windows
#elif defined(__CYGWIN__) && !defined(_WIN32)
    #define PLATFORM_NAME "windows" // Windows (Cygwin POSIX under Microsoft Window)
#elif defined(__ANDROID__)
    #define PLATFORM_NAME "android" // Android (implies Linux, so it must come first)
#elif defined(__linux__)
    #define PLATFORM_NAME "linux" // Debian, Ubuntu, Gentoo, Fedora, openSUSE, RedHat, Centos and other
#elif defined(__unix__) || !defined(__APPLE__) && defined(__MACH__)
    #include <sys/param.h>
    #if defined(BSD)
        #define PLATFORM_NAME "bsd" // FreeBSD, NetBSD, OpenBSD, DragonFly BSD
    #endif
#elif defined(__hpux)
    #define PLATFORM_NAME "hp-ux" // HP-UX
#elif defined(_AIX)
    #define PLATFORM_NAME "aix" // IBM AIX
#elif defined(__APPLE__) && defined(__MACH__) // Apple OSX and iOS (Darwin)
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR == 1
        #define PLATFORM_NAME "ios" // Apple iOS
    #elif TARGET_OS_IPHONE == 1
        #define PLATFORM_NAME "ios" // Apple iOS
    #elif TARGET_OS_MAC == 1
        #define PLATFORM_NAME "osx" // Apple OSX
    #endif
#elif defined(__sun) && defined(__SVR4)
    #define PLATFORM_NAME "solaris" // Oracle Solaris, Open Indiana
#else
    #define PLATFORM_NAME NULL
#endif

#define VER "0.92-"
#define BURST_VERSION VER PLATFORM_NAME

/*******************************
*         Data Structures      *
*******************************/
enum bool {false=0, true=1};
struct data {
  double        time;       // chronological time stamp
  double        dt;         // inter-photon duration
  double        value;      // numeric value of the data
  };

struct burst {
  size_t        l;
  size_t        r;
  };

struct background {
  size_t        l;
  size_t        r;
  };
  
/*******************************
*      Function Declaration    *
*******************************/
size_t cusum();
size_t sprt();
