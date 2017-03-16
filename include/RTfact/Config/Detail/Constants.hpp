/* 
 *  Constants.hpp
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
 *  Created on: 2009-01-29 19:31:16 +0100
 *  Author(s): Iliyan Georgiev, Ken Dahm
 */
 
#ifndef RTFACT__CONFIG_CONSTANTS_HPP
#define RTFACT__CONFIG_CONSTANTS_HPP

#define RTFACT_LUMINANCE_FACTOR_X       0.212671f
#define RTFACT_LUMINANCE_FACTOR_Y       0.715160f
#define RTFACT_LUMINANCE_FACTOR_Z       0.072169f

#define RTFACT_PI            3.14159265358979323846f
#define RTFACT_2_PI          6.28318530717958647692f
#define RTFACT_4_PI          12.566370614359172f
#define RTFACT_2_PI_PI       19.739208802178717f
#define RTFACT_INV_PI        0.31830988618379067154f
#define RTFACT_INV_2_PI      0.15915494309189533577f
#define RTFACT_INV_4_PI      0.079577471545947673f
#define RTFACT_PI_OVER_2     1.5707963267948966f
#define RTFACT_PI_OVER_3     1.0471975511965976f
#define RTFACT_PI_OVER_4     0.78539816339744828f
#define RTFACT_PI_OVER_6     0.52359877559829882f
#define RTFACT_PI_OVER_12    0.26179938779914941f
#define RTFACT_PI_OVER_180   0.017453292519943295f
#define RTFACT_180_OVER_PI   57.295779513082323f
#define RTFACT_LOG2          0.69314718055994529f
#define RTFACT_INV_LOG2      1.4426950408889634f

#define RTFACT_EPSILON       0.00001f
#define RTFACT_RAY_EPSILON   0.01f
#define RTFACT_FLOAT_EPSILON FLT_MIN
#define RTFACT_FLOAT_MIN     -FLT_MAX
#define RTFACT_FLOAT_MAX     FLT_MAX
#define RTFACT_INT_MIN       INT_MIN
#define RTFACT_INT_MAX       INT_MAX
#define RTFACT_UINT_MIN      0
#define RTFACT_UINT_MAX      UINT_MAX
#define RTFACT_INFINITY      FLT_MAX

#define RTFACT_AO_ITER					2
#define RTFACT_AO_EPSILON				0.0001f
#define RTFACT_AO_SHADOW_RAY_LEN		0.15f
#define RTFACT_AO_VALUE_NO_HIT			0.6f
#define RTFACT_AO_VALUE_HIT			1.0f
#define RTFACT_AO_COS_POW_DIST			1.0f
#define RTFACT_AO_SAMPLE_TYPE			float2
#define RTFACT_AO_HEMI_SAMPLE_TYPE		float3
#define RTFACT_AO_PRECOMPUTED_SAMPLES	1000000
#define RTFACT_AO_OLDSAMPLES_ARRAY_X    256
#define RTFACT_AO_OLDSAMPLES_ARRAY_Y    256

#define RTFACT_ACCUMULATE_FRAMES

#ifdef RTFACT_RAND_USE_MTWISTER
#   define RTFACT_RAND_MAX              4294967295 // 0xFFFFFFFF
#   define RTFACT_RAND_MAX_FLOAT        0.99999994f
#   define RTFACT_RAND_NORMALIZATION    2.3283063e-010f
#else
#   define RTFACT_RAND_MAX              32767      // 0x7FFF
#   define RTFACT_RAND_MAX_FLOAT        0.99996948f
#   define RTFACT_RAND_NORMALIZATION    3.0517578e-005f
#   
#endif

#define RTFACT_CACHE_LINE_SIZE  128
#define RTFACT_SIMD_WIDTH       4
#define RTFACT_SIMD_WIDTH_BYTES 16

#endif // RTFACT__CONSTANTS_HPP
