/*
 *  InternalCommon.hpp
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
 *  Created on: 2010-09-26 00:35:31 +0200
 *  Author(s): Felix Klein
 */

 #ifndef RTFACT_RTPIE_INTERNALCOMMON_HPP
#define RTFACT_RTPIE_INTERNALCOMMON_HPP

#include <RTpie/Config/Common.hpp>
#include "Config/RTpiePrivateConfig.hpp"
#include <boost/intrusive_ptr.hpp>

#define RTFACT_RTPIE_FORWARD(className)     \
RTFACT_RTPIE_NAMESPACE_BEGIN                \
class className ;                           \
typedef boost::intrusive_ptr< className >    className##Ptr;    \
RTFACT_RTPIE_NAMESPACE_END


RTFACT_RTPIE_NAMESPACE_BEGIN
/**
 * A very simple type to describe vector data.
 * Used internally only.
 */
struct float3
{

    union {
        struct { float x, y, z; };
        struct { float data[3]; };
    };

    float3() {}

    float3(
        const float aX,
        const float aY,
        const float aZ
    ) :
        x(aX),
        y(aY),
        z(aZ)
    {}

    explicit float3(
        const float* aData
    ) :
        x(aData[0]),
        y(aData[1]),
        z(aData[2])
    {}

    float& operator[](
        const unsigned int aIndex)
    {
        return data[aIndex];
    }

    const float& operator[](
        const unsigned int aIndex) const
    {
        return data[aIndex];
    }

    bool operator==(const float3 &aOther)const
    {
        return (x == aOther.x && y == aOther.y && z == aOther.z);
    }
};
RTFACT_RTPIE_NAMESPACE_END


#endif // RTFACT_RTPIE_INTERNALCOMMON_HPP
