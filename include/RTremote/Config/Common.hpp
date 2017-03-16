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
 *  Created on: 2008-11-06 02:52:20 +0100
 *  Author(s): Felix Klein, Iliyan Georgiev, Lukas Marsalek, Ken Dahm
 */
 
 #ifndef RTFACT_REMOTE__COMMON_HPP
#define RTFACT_REMOTE__COMMON_HPP

//#include <RTfact/Config/Common.hpp>

// Determine Operating System
#ifdef _WIN32
#   define RTFACT_REMOTE_OS_WIN

//Are we using MSVC?
#   ifdef _MSC_VER
#      define RTREMOTE_COMPILER_MSVC
#      define RTREMOTE_COMPILER_MSVC_VERSION _MSC_VER
#   endif

#elif defined(__CYGWIN__) ||                                                   \
      defined(__linux__) ||                                                    \
      defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) ||   \
      defined(__hpux) ||                                                       \
      defined(__sgi) ||                                                        \
      defined(__sun) ||                                                        \
      defined(_POSIX_SOURCE) || defined(_XOPEN_SOURCE)
#   define RTFACT_REMOTE_OS_POSIX
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#   define RTFACT_REMOTE_OS_MACOS
#else
#   pragma message("Warning: Cannot determine OS. Using default: POSIX")
#   define RTFACT_REMOTE_OS_POSIX
#endif

#define RTFACT_REMOTE_NAMESPACE_BEGIN namespace RTfact { namespace Remote {
#define RTFACT_REMOTE_NAMESPACE_END   } }

#define RTFACT_POSTPROCESSING_NAMESPACE_BEGIN namespace RTfact { namespace Remote {
#define RTFACT_POSTPROCESSING_NAMESPACE_END } }


#if defined(RTFACT_REMOTE_OS_WIN)
#  if defined(RTFACT_REMOTE_LIB)
#    define RTFACT_REMOTE_API __declspec(dllexport)
#  else
#    define RTFACT_REMOTE_API __declspec(dllimport)
#  endif
#else
#  define RTFACT_REMOTE_API
#endif

#include <boost/cstdint.hpp>                // for integer typedefs

RTFACT_REMOTE_NAMESPACE_BEGIN

typedef boost::int8_t   int8;
typedef boost::int16_t  int16;
typedef boost::int32_t  int32;
typedef boost::int64_t  int64;

typedef boost::uint8_t  uint8;
typedef boost::uint16_t uint16;
typedef boost::uint32_t uint32;
typedef boost::uint64_t uint64;
typedef uint32          uint;

/* 
 * Defines the types of the components of the framebuffer buffers for RTremote
*/
typedef float t_FrameBufferComponent;
//typedef uint8 t_FrameBufferComponent;
typedef float t_DistanceBufferComponent;
typedef float t_NormalBufferComponent;
typedef float t_AmbientOcclusionBufferComponent;
typedef float t_ColorBleedingBufferComponent;

RTFACT_REMOTE_NAMESPACE_END

#endif // RTFACT_REMOTE__COMMON_HPP
