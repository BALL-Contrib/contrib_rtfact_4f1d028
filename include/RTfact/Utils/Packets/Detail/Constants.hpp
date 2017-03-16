/*
 *  Constants.hpp
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

#ifndef RTFACT__CONSTANTS_HPP
#define RTFACT__CONSTANTS_HPP

#pragma region Mask constants

#define RTFACT_MASK_DECLARE_CONSTS                                             \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_TRUE)

#define RTFACT_MASK_INIT_CONSTS(aSize)                                         \
    RTFACT_MASK_INIT_CONST(aSize, C_TRUE, 1)

#define RTFACT_MASK_4_DECLARE_CONSTS                                           \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_0_0_0_T)                     \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_0_0_T_0)                     \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_0_0_T_T)                     \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_0_T_0_0)                     \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_0_T_0_T)                     \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_0_T_T_0)                     \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_0_T_T_T)                     \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_T_0_0_0)                     \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_T_0_0_T)                     \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_T_0_T_0)                     \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_T_0_T_T)                     \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_T_T_0_0)                     \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_T_T_0_T)                     \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_T_T_T_0)                     \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_T_T_T_T)                     \

#pragma endregion // Mask constants

#pragma region Packet constants

#define RTFACT_PACKET_DECLARE_CONSTS                                           \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_1)                           \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_2)                           \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_3)                           \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_ONE_HALF)                    \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_ONE_THIRD)                   \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_INFINITY)                    \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_EPS)                         \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_1_MINUS_EPS)                 \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_RAY_EPS)                     \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_RAY_EPS_NEG)                 \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_DIST_EPS)                    \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_DIST_EPS_L)                  \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_DIST_EPS_U)                  \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_1_L)                         \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_1_H)                         \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_MIN)                         \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_MAX)                         \
    RTFACT_PACKET_DECLARE_SPECIAL_CONST_AND_INITIALIZER(C_SIZE, SIZE)          \
    RTFACT_PACKET_DECLARE_SPECIAL_CONST_AND_INITIALIZER(C_SIZE_SQRT, SIZE_SQRT)\

#define RTFACT_FLOAT_PACKET_INIT_CONSTS(aSize)                                 \
    RTFACT_PACKET_INIT_CONST(aSize, float, C_1, 1.f)                           \
    RTFACT_PACKET_INIT_CONST(aSize, float, C_2, 2.f)                           \
    RTFACT_PACKET_INIT_CONST(aSize, float, C_3, 3.f)                           \
    RTFACT_PACKET_INIT_CONST(aSize, float, C_ONE_HALF, 0.5f)                   \
    RTFACT_PACKET_INIT_CONST(aSize, float, C_ONE_THIRD, 0.333333f)             \
    RTFACT_PACKET_INIT_CONST(aSize, float, C_INFINITY, RTFACT_INFINITY)        \
    RTFACT_PACKET_INIT_CONST(aSize, float, C_EPS, RTFACT_EPSILON)              \
    RTFACT_PACKET_INIT_CONST(aSize, float, C_1_MINUS_EPS, 1.f - RTFACT_EPSILON)\
    RTFACT_PACKET_INIT_CONST(aSize, float, C_RAY_EPS, RTFACT_RAY_EPSILON)      \
    RTFACT_PACKET_INIT_CONST(aSize, float, C_RAY_EPS_NEG, -RTFACT_RAY_EPSILON) \
    RTFACT_PACKET_INIT_CONST(aSize, float, C_1_L, 1.f - RTFACT_RAY_EPSILON)    \
    RTFACT_PACKET_INIT_CONST(aSize, float, C_1_H, 1.f + RTFACT_RAY_EPSILON)    \
    RTFACT_PACKET_INIT_CONST(aSize, float, C_MIN, RTFACT_FLOAT_MIN)            \
    RTFACT_PACKET_INIT_CONST(aSize, float, C_MAX, RTFACT_FLOAT_MAX)            \
    RTFACT_PACKET_INIT_SPECIAL_CONST(aSize, float, C_SIZE)                     \
    RTFACT_PACKET_INIT_SPECIAL_CONST(aSize, float, C_SIZE_SQRT)                \

#define RTFACT_INT_PACKET_INIT_CONSTS(aSize)                                   \
    RTFACT_PACKET_INIT_CONST(aSize, int, C_1, 1)                               \
    RTFACT_PACKET_INIT_CONST(aSize, int, C_2, 2)                               \
    RTFACT_PACKET_INIT_CONST(aSize, int, C_3, 3)                               \
    RTFACT_PACKET_INIT_CONST(aSize, int, C_INFINITY, -1)                       \
    RTFACT_PACKET_INIT_SPECIAL_CONST(aSize, int, C_SIZE)                       \
    RTFACT_PACKET_INIT_SPECIAL_CONST(aSize, int, C_SIZE_SQRT)                  \

#define RTFACT_PACKET_4_DECLARE_CONSTS                                         \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_1_1_1_0)                     \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_1_1_1_1)                     \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_2_2_2_2)                     \
    RTFACT_PACKET_DECLARE_CONST_AND_INITIALIZER(C_3_3_3_3)                     \

#define RTFACT_QUAD_FLOAT_PACKET_INIT_CONSTS                                   \

#define RTFACT_QUAD_INT_PACKET_INIT_CONSTS                                     \

#pragma endregion // Packet constants

#pragma region Vec3f constants

#define RTFACT_VEC3F_DECLARE_CONSTS                                            \
    RTFACT_VEC3F_DECLARE_CONST_AND_INITIALIZER(C_1_0_0)                        \
    RTFACT_VEC3F_DECLARE_CONST_AND_INITIALIZER(C_0_1_0)                        \
    RTFACT_VEC3F_DECLARE_CONST_AND_INITIALIZER(C_0_0_1)                        \
    RTFACT_VEC3F_DECLARE_CONST_AND_INITIALIZER(C_1_1_1)                        \
    RTFACT_VEC3F_DECLARE_CONST_AND_INITIALIZER(C_1_1_0)                        \
    RTFACT_VEC3F_DECLARE_CONST_AND_INITIALIZER(C_1_0_1)                        \
    RTFACT_VEC3F_DECLARE_CONST_AND_INITIALIZER(C_LUMINANCE_FACTORS)            \
    RTFACT_VEC3F_DECLARE_CONST_AND_INITIALIZER(C_MIN_MIN_MIN)                  \
    RTFACT_VEC3F_DECLARE_CONST_AND_INITIALIZER(C_MAX_MAX_MAX)

#define RTFACT_VEC3F_INIT_CONSTS(aSize)                                        \
    RTFACT_VEC3F_INIT_CONST(aSize, C_1_0_0, 1.f, 0.f, 0.f)                     \
    RTFACT_VEC3F_INIT_CONST(aSize, C_0_1_0, 0.f, 1.f, 0.f)                     \
    RTFACT_VEC3F_INIT_CONST(aSize, C_0_0_1, 0.f, 0.f, 1.f)                     \
    RTFACT_VEC3F_INIT_CONST(aSize, C_1_1_1, 1.f, 1.f, 1.f)                     \
    RTFACT_VEC3F_INIT_CONST(aSize, C_1_1_0, 1.f, 1.f, 0.f)                     \
    RTFACT_VEC3F_INIT_CONST(aSize, C_1_0_1, 1.f, 0.f, 1.f)                     \
    RTFACT_VEC3F_INIT_CONST(aSize, C_LUMINANCE_FACTORS, 0.212671f, 0.715160f, 0.072169f)    \
    RTFACT_VEC3F_INIT_CONST(aSize, C_MIN_MIN_MIN, RTFACT_FLOAT_MIN, RTFACT_FLOAT_MIN, RTFACT_FLOAT_MIN)\
    RTFACT_VEC3F_INIT_CONST(aSize, C_MAX_MAX_MAX, RTFACT_FLOAT_MAX, RTFACT_FLOAT_MAX, RTFACT_FLOAT_MAX)\

#pragma endregion // Vec3f constants

#endif // RTFACT__CONSTANTS_HPP
