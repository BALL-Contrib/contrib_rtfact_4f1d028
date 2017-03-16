/* 
 *  RTfactConfig.default.hpp
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
 *  Created on: 2010-10-11 20:54:13 +0200
 *  Author(s): Felix Klein
 */
 
 #ifndef RTFACT_CONFIG_DEFAULT_HPP
#define RTFACT_CONFIG_DEFAULT_HPP

#define RTFACT_VECTOR_USE_FAST_VECTOR

#pragma region Inlining settings

//#define RTFACT_NO_FORCE_INLINE

//#define RTFACT_UTILS_SSE_NO_FORCE_INLINE
//#define RTFACT_UTILS_SSE_NO_INLINE

//#define RTFACT_CORE_NO_FORCE_INLINE
//#define RTFACT_CORE_NO_INLINE

//#define RTFACT_FRAMEBUFFER_NO_FORCE_INLINE
//#define RTFACT_FRAMEBUFFER_NO_FORCE_INLINE

//#define RTFACT_PRIMITIVE_NO_FORCE_INLINE
//#define RTFACT_PRIMITIVE_NO_INLINE

#define RTFACT_INTERSECTOR_NO_FORCE_INLINE
//#define RTFACT_INTERSECTOR_NO_INLINE

//#define RTFACT_CAMERA_NO_FORCE_INLINE
//#define RTFACT_CAMERA_NO_INLINE

#define RTFACT_INTEGRATOR_NO_FORCE_INLINE
//#define RTFACT_INTEGRATOR_NO_INLINE

#define RTFACT_LIGHT_NO_FORCE_INLINE
//#define RTFACT_LIGHT_NO_INLINE

#define RTFACT_APPEARANCE_NO_FORCE_INLINE
//#define RTFACT_APPEARANCE_NO_INLINE

#define RTFACT_TEXTURE_NO_FORCE_INLINE
//#define RTFACT_TEXTURE_NO_INLINE

//#define RTFACT_PIXEL_SAMPLER_NO_FORCE_INLINE
//#define RTFACT_PIXEL_SAMPLER_NO_INLINE

//#define RTFACT_RAY_PACKET_NO_FORCE_INLINE
//#define RTFACT_RAY_PACKET_NO_INLINE

//#define RTFACT_MATH_NO_FORCE_INLINE
//#define RTFACT_MATH_NO_INLINE

#pragma endregion

//#define RTFACT_NO_USER_WARNINGS
#define RTFACT_FULL_LOGGING

#define BOOST_VARIANT_MINIMIZE_SIZE

#define RTFACT_PACKET_SIZES (1)(4)(16)(64)(256)

#define RTFACT_RAND_USE_MTWISTER

#endif // RTFACT_CONFIG_DEFAULT_HPP
