/* 
 *  EmissiveMaterial.hpp
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
 *  Created on: 2008-06-09 17:30:26 +0200
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__EMISSIVEMATERIAL_HPP
#define RTFACT__EMISSIVEMATERIAL_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Appearance/Material.hpp>
#include <RTfact/Utils/Math/MathUtils.hpp>

RTFACT_NAMESPACE_BEGIN

class EmissiveMaterial :
    public Material
{
    Vec3f<1> mColorOverPi;

public:

    EmissiveMaterial(
        const Vec3f<1>& aColor
    ) :
        mColorOverPi(RTFACT_INV_PI * aColor)
    {}

    template<
        uint taSize>
    RTFACT_APPEARANCE_INLINE const Vec3f<taSize> evaluate(
        const Vec3f<taSize>& aOutgoingDir,
        const Vec3f<taSize>& aIncomingDir,
        SurfaceIntersection<taSize>& aIntersection,
        const BxDFType aBxDFType = BSDF_ALL) const
    {
        return Vec3f<taSize>::C_0_0_0();
    }

    template<
        uint taSize>
    RTFACT_APPEARANCE_INLINE const Vec3f<taSize> emittance(
        const Vec3f<taSize>& aOutgoingDir,
        SurfaceIntersection<taSize>& aIntersection) const
    {
        return Vec3f<taSize>::replicate(mColorOverPi);
    }

    RTFACT_APPEARANCE_INLINE const Vec3f<1> sample(
        const Vec3f<1>& aOutgoingDir,
        const float aX,
        const float aY,
        SurfaceIntersection<1>& aIntersection,
        Vec3f<1>& oIncomingDir,
        float& oInvProb) const
    {
        return Vec3f<1>::C_0_0_0();
    }

    RTFACT_APPEARANCE_INLINE const Vec3f<1> reflectance(
        const Vec3f<1>& aOutgoingDir,
        SurfaceIntersection<1>& aIntersection) const
    {
        return Vec3f<1>::C_0_0_0();
    }

    RTFACT_APPEARANCE_INLINE const Vec3f<1> reflectance(
        SurfaceIntersection<1>& aIntersection) const
    {
        return Vec3f<1>::C_0_0_0();
    }

    #include RTFACT_MATERIAL_VIRTUAL_METHODS
};

RTFACT_NAMESPACE_END

#endif // RTFACT__EMISSIVEMATERIAL_HPP
