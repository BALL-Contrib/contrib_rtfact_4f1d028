/* 
 *  Float3ConstantTexture.hpp
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
 *  Created on: 2008-07-16 17:43:42 +0200
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__FLOAT3CONSTANTTEXTURE_HPP
#define RTFACT__FLOAT3CONSTANTTEXTURE_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Packets/Vec3f.hpp>

#include <RTfact/Concept/Texture.hpp>

RTFACT_NAMESPACE_BEGIN

class Float3ConstantTexture :
    public Texture
{
protected:

    Vec3f1 mColor;

public:

    RTFACT_SIMD_ALIGNED_NEW_OPERATORS;
    
    RTFACT_DEFINE_PROPERTY(Vec3f1&, Color);

    RTFACT_FORCE_INLINE bool hasTransparency() const
    {
        return false;
    }

    Float3ConstantTexture()
    {}

    Float3ConstantTexture(
        const Vec3f1& aColor
    ) :
        mColor(aColor)
    {}

    Float3ConstantTexture(
        const float aRed,
        const float aGreen,
        const float aBlue
    ) :
        mColor(aRed, aGreen, aBlue)
    {}

    template<
        bool taGetColor,
        bool taGetTransparency,
        uint taSize>
    RTFACT_TEXTURE_INLINE void sample(
        SurfaceIntersection<taSize>&,
        const ActiveMask<taSize>& aMask,
        Vec3f<taSize>* oColor,
        Packet<taSize, float>* = RTFACT_NULL) const
    {
        // cannot be made a compile-time assertion
        RTFACT_TEXTURE_ASSERT(taGetColor && !taGetTransparency);

        RTFACT_TEXTURE_ASSERT(oColor != RTFACT_NULL);

        const typename Vec3f<taSize>::Container colorC =
            Vec3f<taSize>::Container::replicate(mColor);

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            (*oColor)(i) = aMask(i).iif(colorC, (*oColor)(i));
        }
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__FLOAT3CONSTANTTEXTURE_HPP
