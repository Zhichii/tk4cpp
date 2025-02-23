#define TKI_FLAGBUILD_TCL_ERR
#undef _DEBUG

/* Some basic definitions. */

#ifndef TK4CPP_INTERNAL_BASIC
#define TK4CPP_INTERNAL_BASIC

#include <limits>

#include <tcl.h>
#include <tk.h>

#define TK_HEX_VERSION ((TK_MAJOR_VERSION << 24) | \
                        (TK_MINOR_VERSION << 16) | \
                        (TK_RELEASE_LEVEL << 8) | \
                        (TK_RELEASE_SERIAL << 0))

#if TK_HEX_VERSION < 0x0805020c
#error "Tk older than 8.5.12 not supported"
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

#define TKI_MIN(x, y) (((x) > (y)) ? (y) : (x))

#define TKI_MAX(x, y) (((x) > (y)) ? (x) : (y))

#define TKI_ABS(x) ((x) < 0 ? -(x) : (x))

#define TKI_ERR(EXC, MSG) { printf("%s[%s]\n", std::string(EXC).c_str(), std::string(MSG).c_str()); throw std::exception(); }

#define TKI_MEM_ERR() TKI_ERR("MemoryError", "no memory")

namespace tki {

    typedef int si32;
    typedef unsigned int ui32;
    typedef long long sint;
    typedef unsigned long long uint;

    struct TkApp;

}

#endif