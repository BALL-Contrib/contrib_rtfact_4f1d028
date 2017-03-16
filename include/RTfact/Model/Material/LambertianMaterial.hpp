/* 
 *  LambertianMaterial.hpp
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
 *  Author(s): Iliyan Georgiev, Dmitri Rubinstein
 */
 
 #ifndef RTFACT__LAMBERTIANMATERIAL_HPP
#define RTFACT__LAMBERTIANMATERIAL_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Appearance/Material.hpp>
#include <RTfact/Utils/Math/MathUtils.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tReflectionTexture,
    class tEmissionTexture>
class LambertianMaterial :
    public Material
{
public:

    typedef tReflectionTexture ReflectionTexture;
    typedef tEmissionTexture   EmissionTexture;

protected:

    ReflectionTexture mReflectionTexture;
    EmissionTexture   mEmissionTexture;

public:

    LambertianMaterial() :
        Material(false)
    {}

    LambertianMaterial(
        const ReflectionTexture& aReflectionTexture,
        const EmissionTexture& aEmissionTexture
    ) :
        Material(false),
        mReflectionTexture(aReflectionTexture),
        mEmissionTexture(aEmissionTexture)
    {}

    const ReflectionTexture& getReflectionTexture() const
    {
        return mReflectionTexture;
    }

    void setReflectionTexture(
        const ReflectionTexture& aTexture)
    {
        mReflectionTexture = aTexture;
    }

    const EmissionTexture& getEmissionTexture() const
    {
        return mEmissionTexture;
    }

    void setEmissionTexture(
        const EmissionTexture& aTexture)
    {
        mEmissionTexture = aTexture;
    }

    template<
        uint taSize>
    RTFACT_APPEARANCE_INLINE void evaluate(
        const Vec3f<taSize>&,
        const Vec3f<taSize>&,
        SurfaceIntersection<taSize>& aIntersection,
        const ActiveMask<taSize>& aMask,
        Vec3f<taSize>& oResult,
        const BxDFType = BSDF_ALL) const
    {
        mReflectionTexture.template sample<true, false>(aIntersection, aMask, &oResult);

        const typename Packet<taSize, float>::Container invPiC =
            Packet<taSize, float>::Container::replicate(RTFACT_INV_PI);

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            oResult(i) = aMask(i).iif(invPiC * oResult(i), oResult(i));
        }
    }

    template<
        uint taSize>
    RTFACT_APPEARANCE_INLINE void emittance(
        const Vec3f<taSize>&,
        SurfaceIntersection<taSize>& aIntersection,
        const ActiveMask<taSize>& aMask,
        Vec3f<taSize>& oResult) const
    {
        mEmissionTexture.template sample<true, false>(aIntersection, aMask, &oResult);

        /*const typename Packet<taSize, float>::Container invPiC =
            Packet<taSize, float>::Container::replicate(RTFACT_INV_PI);

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            oResult(i) = aMask(i).iif(invPiC * oResult(i), oResult(i));
        }*/
    }

    /*RTFACT_APPEARANCE_INLINE const Vec3f<1> sample(
        const Vec3f<1>&,
        const float aX,
        const float aY,
        SurfaceIntersection<1>& aIntersection,
        Vec3f<1>& oIncomingDir,
        float& oInvProb) const
    {
        Math::cosineHemisphere(aX, aY, aIntersection.normal, oIncomingDir, oInvProb);

        return mReflectionTexture.evaluate(aIntersection) * RTFACT_INV_PI;
    }

    RTFACT_APPEARANCE_INLINE const Vec3f<1> reflectance(
        const Vec3f<1>&,
        SurfaceIntersection<1>& aIntersection) const
    {
        return mReflectionTexture.evaluate(aIntersection);
    }

    RTFACT_APPEARANCE_INLINE const Vec3f<1> reflectance(
        SurfaceIntersection<1>& aIntersection) const
    {
        return mReflectionTexture.evaluate(aIntersection) * RTFACT_INV_PI;
    }*/

    #include RTFACT_MATERIAL_VIRTUAL_METHODS
};

RTFACT_NAMESPACE_END

#endif // RTFACT__LAMBERTIANMATERIAL_HPP
