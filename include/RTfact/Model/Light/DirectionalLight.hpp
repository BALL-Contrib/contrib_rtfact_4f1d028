/* 
 *  DirectionalLight.hpp
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
 
 #ifndef RTFACT__DIRECTIONALLIGHT_HPP
#define RTFACT__DIRECTIONALLIGHT_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Light.hpp>

RTFACT_NAMESPACE_BEGIN

class DirectionalLight :
    public Light
{
    Vec3f<1> mDirection;
    Vec3f<1> mIntensity;
    Vec3f<1> mDirBinormal;
    Vec3f<1> mDirTangent;
    BoundingSphere mBSphere;

public:
    DirectionalLight(
        const Vec3f<1>& aDirection,
        const Vec3f<1>& aIntensity,
        const BBox& aBBox = BBox()
    ) :
        Light(Light::IS_DELTA |
              Light::CASTS_SHADOWS),
        mIntensity(aIntensity)
    {
        setDirection(aDirection);
        setBoundingSphere(aBBox.getBoundingSphere());
    }

    DirectionalLight() :
        Light(Light::IS_DELTA |
              Light::CASTS_SHADOWS),
        mDirection(0.f, 1.f, 0.f),
        mIntensity(0.f, 0.f, 0.f)
    {}

    RTFACT_LIGHT_INLINE void setBoundingSphere(const BoundingSphere& aBSphere)
    {
        mBSphere = aBSphere;
    }

    RTFACT_LIGHT_INLINE BoundingSphere* getBoundingSphere()
    {
        return &mBSphere;
    }

    const Vec3f1 getDirection() const
    {
        return -mDirection;
    }

    void setDirection(
        const Vec3f1& aDirection)
    {
        mDirection = -aDirection.normalize();
        Math::getFrame(mDirection, mDirBinormal, mDirTangent);
    }

    const Vec3f1& getIntensity() const
    {
        return mIntensity;
    }

    void setIntensity(
        const Vec3f1& aIntensity)
    {
        mIntensity = aIntensity;
    }

    template<
        uint taSize>
    RTFACT_LIGHT_INLINE void illuminatePoint(
        const Vec2f<taSize>& aPositionSamples,
        const Vec3f<taSize>& aPoint,
        const ActiveMask<taSize>& aMask,
        Vec3f<taSize>& oDirection,
        Packet<taSize, float>& oDistance,
        Vec3f<taSize>& oRadiance,
        Vec3f<1> *) const
    {
        typedef Packet<taSize, float>        t_Packet;
        typedef typename t_Packet::Container t_PacketC;
        typedef Vec3f<taSize>                t_Vec3f;
        typedef typename t_Vec3f::Container  t_Vec3fC;

        const t_Vec3fC directionC = t_Vec3fC::replicate(mDirection);
        const t_Vec3fC intensityC = t_Vec3fC::replicate(mIntensity);

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            oDirection(i) = directionC;
            oDistance(i) = t_PacketC::C_INFINITY;
            oRadiance(i) = intensityC;
        }
    }

    template<
        uint taSize>
    RTFACT_LIGHT_INLINE void sample(
        const Vec2f<taSize>& aPositionSamples,
        const Vec2f<taSize>&,
        const ActiveMask<taSize>& aMask,
        Vec3f<taSize>& oPosition,
        Vec3f<taSize>& oDirection,
        Vec3f<taSize>& oRadiance,
        Packet<taSize, float>& oPdf) const
    {
        typedef Packet<taSize, float>        t_Packet;
        typedef typename t_Packet::Container t_PacketC;
        typedef Vec2f<taSize>                t_Vec2f;
        typedef typename t_Vec2f::Container  t_Vec2fC;
        typedef Vec3f<taSize>                t_Vec3f;
        typedef typename t_Vec3f::Container  t_Vec3fC;

        const t_Vec3fC dirC = t_Vec3fC::replicate(mDirection);
        const t_Vec3fC dirBinormalC = t_Vec3fC::replicate(mDirBinormal);
        const t_Vec3fC dirTangentC = t_Vec3fC::replicate(mDirTangent);
        const t_Vec3fC bsCenterC = t_Vec3fC::replicate(mBSphere.center);
        const t_PacketC bsRadius = t_PacketC::replicate(mBSphere.radius);
        const t_PacketC invRadSq = t_PacketC::replicate(1.0f / (mBSphere.radius * mBSphere.radius));

        oRadiance = t_Vec3f::replicate(mIntensity);
        oDirection = t_Vec3f::replicate(-mDirection);
        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            t_Vec2fC pos = Math::concentricDisk(aPositionSamples(i), &oPdf(i));
            oPdf(i) *= invRadSq;
            oPosition(i) = bsCenterC + bsRadius * (pos.x * dirBinormalC + pos.y * dirTangentC + dirC);
        }
    }

    RTFACT_LIGHT_INLINE const Vec3f<1> getTotalPower() const
    {
        //HACK:
        return Vec3f<1>(RTFACT_FLOAT_MAX, RTFACT_FLOAT_MAX, RTFACT_FLOAT_MAX);
    }

    #include RTFACT_LIGHT_VIRTUAL_METHODS
};

RTFACT_NAMESPACE_END

#endif // RTFACT__DIRECTIONALLIGHT_HPP
