/* 
 *  Float3ConstantEnvironmentTexture.hpp
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
 *  Created on: 2008-11-07 18:41:48 +0100
 *  Author(s): Iliyan Georgiev, Petr Kmoch
 */
 
 #ifndef RTFACT__FLOAT3CONSTANTENVIRONMENTTEXTURE_HPP
#define RTFACT__FLOAT3CONSTANTENVIRONMENTTEXTURE_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/EnvironmentTexture.hpp>

RTFACT_NAMESPACE_BEGIN

class Float3ConstantEnvironmentTexture :
    public EnvironmentTexture
{
    Vec3f1 mValue;

public:

    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    typedef float Component;

    Float3ConstantEnvironmentTexture() :
        mValue(0.f, 0.f, 0.f)
    {}

    Float3ConstantEnvironmentTexture(
        const float aV1,
        const float aV2,
        const float aV3
    ) :
        mValue(aV1, aV2, aV3)
    {}

    Float3ConstantEnvironmentTexture(
        const Vec3f1& aValue
    ) :
        mValue(aValue)
    {}

    RTFACT_TEXTURE_INLINE void getValues(
        float& oV1,
        float& oV2,
        float& oV3) const
    {
        oV1 = mValue.x;
        oV2 = mValue.y;
        oV3 = mValue.z;
    }

    RTFACT_TEXTURE_INLINE const Vec3f1& getValues() const
    {
        return mValue;
    }

    RTFACT_TEXTURE_INLINE void setValues(
        const float aV1,
        const float aV2,
        const float aV3)
    {
        mValue = Vec3f1(aV1, aV2, aV3);
    }

    RTFACT_TEXTURE_INLINE void setValues(
        const Vec3f1& aVec)
    {
        mValue = aVec;
    }

    template<
        bool taGetTransparency,
        bool taCommonOrg,
        uint taSize>
    void sample(
        const RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aMask,
        Vec3f<taSize>& oResult,
        Packet<taSize, float>* oAlpha = RTFACT_NULL) const
    {
        RTFACT_ASSERT(!taGetTransparency || oAlpha != RTFACT_NULL);

        const typename Vec3f<taSize>::Container colorC =
            Vec3f<taSize>::Container::replicate(mValue);

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            oResult(i) = aMask(i).iif(colorC, oResult(i));
            if (
                taGetTransparency)
            {
                (*oAlpha)(i) = aMask(i).iif( Packet<taSize, float>::Container::C_1 , (*oAlpha)(i));
            }
        }
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__FLOAT3CONSTANTENVIRONMENTTEXTURE_HPP
