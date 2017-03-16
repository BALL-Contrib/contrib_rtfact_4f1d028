/* 
 *  Common.hpp
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
 *  Created on: 2010-09-15 17:59:35 +0200
 *  Author(s): Felix Klein
 */
 
#ifndef RTFACT_RTPIE__HCOMMON_HPP
#define RTFACT_RTPIE__HCOMMON_HPP

#include <RTpie/Config/RTCOM.hpp>

#define RTFACT_RTPIE_NAMESPACE_BEGIN namespace RTfact { namespace RTpie {
#define RTFACT_RTPIE_NAMESPACE_END   } }

#include <limits.h>

// Determine Operating System
#ifdef _WIN32
#   define RTFACT_RTPIE_OS_WIN
#elif defined(__CYGWIN__) ||                                                   \
      defined(__linux__) ||                                                    \
      defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) ||   \
      defined(__hpux) ||                                                       \
      defined(__sgi) ||                                                        \
      defined(__sun) ||                                                        \
      defined(_POSIX_SOURCE) || defined(_XOPEN_SOURCE)
#   define RTFACT_RTPIE_OS_POSIX
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#   define RTFACT_RTPIE_OS_MACOS
#else
#   pragma message("Warning: Cannot determine OS. Using default: POSIX")
#   define RTFACT_RTPIE_OS_POSIX
#endif





#if defined(RTFACT_RTPIE_OS_WIN)
#  if defined(RTFACT_RTPIE_LIB)
#    define RTFACT_RTPIE_API __declspec(dllexport)
#  else
#    define RTFACT_RTPIE_API __declspec(dllimport)
#  endif
#else
#  define RTFACT_RTPIE_API
#endif



RTFACT_RTPIE_NAMESPACE_BEGIN

// This logic comes from boost
#  if ULONG_MAX == 0xffffffff
typedef long             int32;
typedef unsigned long    uint32;
# else
typedef int              int32;
typedef unsigned int     uint32;
#  endif


/*
 * Defines the type of the framebuffer for RTpie
*/
//typedef float t_FrameBufferComponent;

RTFACT_RTPIE_NAMESPACE_END


#endif // RTFACT_RTPIE__COMMON_HPP
