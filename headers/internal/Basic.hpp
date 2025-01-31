/* Some basic definitions. */

#ifndef TK4CPP_INTERNAL_BASIC
#define TK4CPP_INTERNAL_BASIC

#include <limits>

// I don't know the usage of this.
#define USE_TCL_UNICODE 1

#include <tcl.h>
#include <tk.h>

#define TK_HEX_VERSION ((TK_MAJOR_VERSION << 24) | \
                        (TK_MINOR_VERSION << 16) | \
                        (TK_RELEASE_LEVEL << 8) | \
                        (TK_RELEASE_SERIAL << 0))
#if TK_HEX_VERSION != 0x0806020f
#error "Please use Tk 8.6.15, \
(I don't want to consider other compatible versions.) "
#endif

#ifndef TCL_WITH_EXTERNAL_TOMMATH
#define TCL_NO_TOMMATH_H
#endif
#if defined(TCL_WITH_EXTERNAL_TOMMATH) || (TK_HEX_VERSION >= 0x08070000)
#define USE_DEPRECATED_TOMMATH_API 0
#else
#define USE_DEPRECATED_TOMMATH_API 1
#endif
#include <tclTomMath.h>

#ifndef TCL_SIZE_MAX
typedef int Tcl_Size;
#define TCL_SIZE_MAX INT_MAX
#endif

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <functional>
#include <stdexcept>
#include <malloc.h>

#define MIN(x, y) (((x) > (y)) ? (y) : (x))

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#define ABS(x) ((x) < 0 ? -(x) : (x))

#define THROW_ERROR(EXC, MSG) { printf("%s[%s]", std::string(EXC).c_str(), std::string(MSG).c_str()); throw std::exception(); }

namespace tk4cpp {

    typedef int sint;
    typedef unsigned int uint;
    typedef long long slll;
    typedef unsigned long long ulll;

    struct TkApp;

}

#endif