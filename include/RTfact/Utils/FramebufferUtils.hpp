/* 
 *  FramebufferUtils.hpp
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
 *  Created on: 2009-03-10 12:51:57 +0100
 *  Author(s): Iliyan Georgiev, Ken Dahm
 */
 
 #ifndef RTFACT__FRAMEBUFFERUTILS_HPP
#define RTFACT__FRAMEBUFFERUTILS_HPP

#include <RTfact/Config/Common.hpp>

RTFACT_NAMESPACE_BEGIN

struct FBSupportedData
{
    enum
    {
        COLOR    =  1,
        DISTANCE = (1 << 1),
        ALPHA    = (1 << 2),
        NORMAL   = (1 << 3),
		AMBIENTOCCLUSION = (1 << 4),
        COLORBLEEDING = (1 << 5)
    };
};

template<
    uint taSize>
struct FBData
{
    RTFACT_STATIC_CONSTANT(SIZE, taSize);

    umask               supportedData;

    Vec3f<SIZE>         color;
    Packet<SIZE, float> distance;
    Packet<SIZE, float> alpha;
    Vec3f<SIZE>         normal;
	Packet<SIZE, float> ambientOcclusion;
    Vec3f<SIZE>         colorBleeding;

    FBData() :
        supportedData(0)
    {}

    explicit FBData(
        const umask aSupportedData
    ) :
        supportedData(aSupportedData)
    {}
};

RTFACT_NAMESPACE_END

#endif // RTFACT__FRAMEBUFFERUTILS_HPP
