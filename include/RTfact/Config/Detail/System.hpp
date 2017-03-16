/*
 *  System.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2010  Saarland University
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Created on: 2008-11-02 21:38:37 +0100
 *  Author(s): Iliyan Georgiev, Petr Kmoch, Felix Klein
 */

 #ifndef RTFACT__SYSTEM_HPP
#define RTFACT__SYSTEM_HPP

#pragma region OS configuration

#ifdef _WIN32
#   define RTFACT_OS_WIN
#elif defined(__CYGWIN__) ||                                                   \
      defined(__linux__) ||                                                    \
      defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) ||   \
      defined(__hpux) ||                                                       \
      defined(__sgi) ||                                                        \
      defined(__sun) ||                                                        \
      defined(_POSIX_SOURCE) || defined(_XOPEN_SOURCE)
#   define RTFACT_OS_POSIX
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#   define RTFACT_OS_MACOS
#else
#   pragma message("Warning: Cannot determine OS. Using default: POSIX")
#   define RTFACT_OS_POSIX
#endif

#ifdef _MSC_VER
#   define RTFACT_COMPILER_MSVC
#   define RTFACT_COMPILER_MSVC_VERSION _MSC_VER
#endif

#ifdef __INTEL_COMPILER
#   define RTFACT_COMPILER_INTEL
#   ifdef __ECL
#       define RTFACT_COMPILER_INTEL_VERSION __ECL
#   else
#       define RTFACT_COMPILER_INTEL_VERSION __ICL
#endif
#endif

#ifdef __GNUC__
#   define RTFACT_COMPILER_GCC
#   define RTFACT_COMPILER_GCC_VERSION                                         \
        (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

#ifdef __clang__
#   define RTFACT_COMPILER_CLANG
#   define RTFACT_COMPILER_CLANG_VERSION                                         \
        (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
#endif

#pragma endregion // OS configuration

#pragma region Architecture configuration

#if defined(__x86_64__) || defined(_M_IA64) ||                                 \
    defined(__ia64__)   || defined(__IA64__)  || defined(_M_X64)
#   define RTFACT_ARCH_64_BIT
#elif defined(__i386__) || defined(_M_IX86)
#   define RTFACT_ARCH_32_BIT
#else
#   error Could not determine architecture
#endif

#pragma endregion // Architecture configuration

#pragma region Compiler settings

// Disable some security checks in the standard libraries that come with MSVC
/*
#if defined(RTFACT_OS_WIN) && defined(RTFACT_STANDALONE)
#
#   ifdef _SECURE_SCL
#       undef _SECURE_SCL
#   endif
#   define _SECURE_SCL 0
#
#   ifdef _CRT_SECURE_NO_DEPRECATE
#      undef _CRT_SECURE_NO_DEPRECATE
#   endif
#   define _CRT_SECURE_NO_DEPRECATE 1
#
#   ifdef _CRT_SECURE_NO_WARNINGS
#       undef _CRT_SECURE_NO_WARNINGS
#   endif
#   define _CRT_SECURE_NO_WARNINGS 1
#
#   ifdef _SCL_SECURE_NO_WARNINGS
#       undef _SCL_SECURE_NO_WARNINGS
#   endif
#   define _SCL_SECURE_NO_WARNINGS 1
#
#   ifdef _CRT_NONSTDC_NO_DEPRECATE
#       undef _CRT_NONSTDC_NO_DEPRECATE
#   endif
#   define _CRT_NONSTDC_NO_DEPRECATE 1
#
#   ifdef RTFACT_COMPILER_INTEL
#       pragma warning(disable:1885) // unclosed region warnings (strange warning)
#   endif
#
#endif
*/

#ifdef RTFACT_COMPILER_INTEL
    #if __INTEL_COMPILER_BUILD_DATE < 20090314
        //disable "wrong number of arguments to intrinsic function" warning
        //this warning is a known bug in ICC ( http://software.intel.com/en-us/articles/diagnostic-980-wrong-number-of-actual-arguments-to-intrinsic-function/ )
        #pragma warning (disable:980)
    #endif
#endif

#pragma endregion // Compiler settings

#endif // RTFACT__SYSTEM_HPP
