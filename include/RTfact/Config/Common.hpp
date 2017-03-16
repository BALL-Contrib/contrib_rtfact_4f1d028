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
 *  Created on: 2008-11-03 01:12:24 +0100
 *  Author(s): Iliyan Georgiev, Petr Kmoch
 */
 
 #ifndef RTFACT__COMMON_HPP
#define RTFACT__COMMON_HPP

#define RTFACT__STRINGIZE_HELPER(aParam) #aParam
#define RTFACT_STRINGIZE(aParam)         RTFACT__STRINGIZE_HELPER(aParam)

// The user can specify a custom configuration file to be included
#ifdef RTFACT_USER_CONFIG_FILE
#   include RTFACT_STRINGIZE(RTFACT_USER_CONFIG_FILE)
#endif

#include <RTfact/Config/Detail/System.hpp>

#define RTFACT_NULL 0

#pragma region External includes

#if defined(RTFACT_OS_WIN) && (defined(RTFACT_COMPILER_MSVC) || defined(RTFACT_COMPILER_INTEL))
#   include <intrin.h>                      // for __debugbreak()
#endif

#include <cassert>                          // for assertion macros
#ifdef RTFACT_COMPILER_MSVC
    #pragma warning(push)
    #pragma warning(disable:4985)
#endif
#include <boost/cstdint.hpp>                // for integer typedefs
#ifdef RTFACT_COMPILER_MSVC
    #pragma warning(pop)
#endif
#include <boost/intrusive_ptr.hpp>          // for integer typedefs

#include <boost/preprocessor/seq/elem.hpp>  // for packet size sequences
#include <boost/preprocessor/seq/size.hpp>

#include <stddef.h>                         // for size_t

#include <float.h>                          // for math constants
#include <climits>

#include <cstdlib>                          // for size_t

#pragma endregion // External includes

#if defined(RTFACT_OS_WIN) && defined(_DEBUG) || !defined(NDEBUG)
#   define RTFACT_DEBUG
#endif

#define RTFACT_NAMESPACE_BEGIN namespace RTfact {
#define RTFACT_NAMESPACE_END   }

#include <RTfact/Config/Detail/Inlining.hpp>
#include <RTfact/Config/Detail/Utils.hpp>
#include <RTfact/Config/Detail/Constants.hpp>

#pragma region Typedefs

RTFACT_NAMESPACE_BEGIN

typedef boost::int8_t   int8;
typedef boost::int16_t  int16;
typedef boost::int32_t  int32;
typedef boost::int64_t  int64;

typedef boost::uint8_t  uint8;
typedef boost::uint16_t uint16;
typedef boost::uint32_t uint32;
typedef boost::uint64_t uint64;

#ifdef RTFACT_ARCH_64_BIT
    typedef int64 nint;
#else
    typedef int32 nint;
#endif

#ifdef RTFACT_UINT_DEFINED
    using ::uint;
#else
    typedef unsigned int uint;
#endif

typedef uint            umask;
typedef unsigned char   uchar;

struct Empty{};

RTFACT_NAMESPACE_END

#pragma endregion // Typedefs

#include <RTfact/Utils/IO/ErrorLogUtils.hpp>
#include <RTfact/Config/Detail/Assert.hpp>
#include <RTfact/Utils/System/SystemUtils.hpp>
#include <RTfact/Utils/System/AllocationUtils.hpp>

#endif // RTFACT__COMMON_HPP
