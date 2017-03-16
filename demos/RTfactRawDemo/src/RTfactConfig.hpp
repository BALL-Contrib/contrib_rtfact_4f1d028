/* 
 *  RTfactConfig.hpp
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
 *  Created on: 2010-08-21 21:49:13 +0200
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT_DEMO__CONFIG_HPP_INCLUDED
#define RTFACT_DEMO__CONFIG_HPP_INCLUDED

//#define RTFACT_HAVE_SSE3
//#define RTFACT_HAVE_SSE41
//#define RTFACT_HAVE_SSE42

//#define RTFACT_RELEASE_ASSERTIONS

//#define RTFACT_VECTOR_USE_FAST_VECTOR

#define RTFACT_PACKET_SIZES (1)(4)(16)(64)(256)

//#define RTFACT_NO_USER_WARNINGS
#define RTFACT_FULL_LOGGING

#define RTFACT_PRECISE_INT

#pragma region Inlining settings

//#define RTFACT_NO_FORCE_INLINE

//#define RTFACT_SSE_NO_FORCE_INLINE
//#define RTFACT_SSE_NO_INLINE

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

#define RTFACT_IMAGE_SAMPLER_NO_FORCE_INLINE
//#define RTFACT_IMAGE_SAMPLER_NO_INLINE

//#define RTFACT_PIXEL_SAMPLER_NO_FORCE_INLINE
//#define RTFACT_PIXEL_SAMPLER_NO_INLINE

//#define RTFACT_RAY_PACKET_NO_FORCE_INLINE
//#define RTFACT_RAY_PACKET_NO_INLINE

//#define RTFACT_MATH_NO_FORCE_INLINE
//#define RTFACT_MATH_NO_INLINE

#pragma endregion

#endif // RTFACT_DEMO__CONFIG_HPP_INCLUDED
