/* 
 *  Assert.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2009  Saarland University
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
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__ASSERT_HPP
#define RTFACT__ASSERT_HPP

#include <iostream>
#include <cassert>

#if defined(RTFACT_DEBUG) || defined(RTFACT_RELEASE_ASSERTIONS)
#   define RTFACT_ASSERTIONS_ENABLED
#endif

#ifndef RTFACT_ASSERTIONS_ENABLED
#   define BOOST_DISABLE_ASSERTS
#endif

#if defined(RTFACT_COMPILER_MSVC) || defined(RTFACT_COMPILER_INTEL)
#   define RTFACT_BREAK() __debugbreak()
#else
#   define RTFACT_BREAK() // maybe asm("int 3") for GCC?
#endif

#if defined(RTFACT_ASSERTIONS_ENABLED)
#   define RTFACT_BREAK_DEBUG() RTFACT_BREAK()
#   define RTFACT_BREAK_DEBUG_IF(x) if(x) { RTFACT_BREAK_DEBUG(); }
#else
#   define RTFACT_BREAK_DEBUG()
#   define RTFACT_BREAK_DEBUG_IF(x)
#endif

#if defined(RTFACT_DEBUG)
#   if defined(RTFACT_OS_WIN) &&                                               \
       (defined(RTFACT_COMPILER_MSVC) || defined(RTFACT_COMPILER_INTEL))
#       define RTFACT_ASSERT(x) _ASSERT(x)
#   else
#       define RTFACT_ASSERT(x) assert(x)
#   endif
#elif defined(RTFACT_RELEASE_ASSERTIONS)
#   define RTFACT_ASSERT(x)                                                    \
        if(!(x)) { RTFACT_PROGRAM_ERROR_F("Assertion failed: %1%", #x); }
#else
#   define RTFACT_ASSERT(x)
#endif

#define RTFACT_STATIC_ASSERT(x) BOOST_STATIC_ASSERT(x)
#define RTFACT_STATIC_FALSE_ASSERTION RTFACT_STATIC_ASSERT(sizeof(int) < 0)

#pragma region Ray tracing and rendering components

#define RTFACT_PRIMITIVE_ASSERT(x)                  RTFACT_ASSERT(x)
#define RTFACT_PRIMITIVE_STATIC_ASSERT(x)           RTFACT_STATIC_ASSERT(x)

#define RTFACT_CAMERA_ASSERT(x)                     RTFACT_ASSERT(x)
#define RTFACT_CAMERA_STATIC_ASSERT(x)              RTFACT_STATIC_ASSERT(x)

#define RTFACT_CONTAINER_ASSERT(x)                  RTFACT_ASSERT(x)
#define RTFACT_CONTAINER_STATIC_ASSERT(x)           RTFACT_STATIC_ASSERT(x)

#define RTFACT_FRAMEBUFFER_ASSERT(x)                RTFACT_ASSERT(x)
#define RTFACT_FRAMEBUFFER_STATIC_ASSERT(x)         RTFACT_STATIC_ASSERT(x)

#define RTFACT_INTEGRATOR_ASSERT(x)                 RTFACT_ASSERT(x)
#define RTFACT_INTEGRATOR_STATIC_ASSERT(x)          RTFACT_STATIC_ASSERT(x)

#define RTFACT_INTERSECTOR_ASSERT(x)                RTFACT_ASSERT(x)
#define RTFACT_INTERSECTOR_STATIC_ASSERT(x)         RTFACT_STATIC_ASSERT(x)

#define RTFACT_LIGHT_ASSERT(x)                      RTFACT_ASSERT(x)
#define RTFACT_LIGHT_STATIC_ASSERT(x)               RTFACT_STATIC_ASSERT(x)

#define RTFACT_APPEARANCE_ASSERT(x)                 RTFACT_ASSERT(x)
#define RTFACT_APPEARANCE_STATIC_ASSERT(x)          RTFACT_STATIC_ASSERT(x)

#define RTFACT_PIXEL_SAMPLER_ASSERT(x)              RTFACT_ASSERT(x)
#define RTFACT_PIXEL_SAMPLER_STATIC_ASSERT(x)       RTFACT_STATIC_ASSERT(x)

#define RTFACT_TEXTURE_ASSERT(x)                    RTFACT_ASSERT(x)
#define RTFACT_TEXTURE_STATIC_ASSERT(x)             RTFACT_STATIC_ASSERT(x)

#define RTFACT_IMAGE_SAMPLER_ASSERT(x)              RTFACT_ASSERT(x)
#define RTFACT_IMAGE_SAMPLER_STATIC_ASSERT(x)       RTFACT_STATIC_ASSERT(x)

#define RTFACT_IMAGE_ASSERT(x)                      RTFACT_ASSERT(x)
#define RTFACT_IMAGE_STATIC_ASSERT(x)               RTFACT_STATIC_ASSERT(x)

#define RTFACT_PARTICLE_GENERATOR_ASSERT(x)         RTFACT_ASSERT(x)
#define RTFACT_PARTICLE_GENERATOR_STATIC_ASSERT(x)  RTFACT_STATIC_ASSERT(x)

#define RTFACT_PARTICLE_FILTER_ASSERT(x)            RTFACT_ASSERT(x)
#define RTFACT_PARTICLE_FILTER_STATIC_ASSERT(x)     RTFACT_STATIC_ASSERT(x)

#define RTFACT_VEC2F_ASSERT(x)                      RTFACT_PACKET_ASSERT(x)
#define RTFACT_VEC2F_STATIC_ASSERT(x)               RTFACT_PACKET_STATIC_ASSERT(x)

#define RTFACT_MATH_ASSERT(x)                       RTFACT_PACKET_ASSERT(x)
#define RTFACT_MATH_STATIC_ASSERT(x)                RTFACT_PACKET_STATIC_ASSERT(x)

#pragma endregion // Ray tracing and rendering components

#endif // RTFACT__ASSERT_HPP
