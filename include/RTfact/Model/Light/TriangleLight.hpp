/*
 *  QuadLight.hpp
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
 *  Created on: 2011-01-05 17:30:26 +0200
 *  Author(s): Vaclav Krajicek
 */

#ifndef RTFACT__TRIANGLELIGHT_HPP
#define RTFACT__TRIANGLELIGHT_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Light.hpp>
#include <RTfact/Model/LightAttenuation/QuadraticLightAttenuation.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tLightAttenuation = QuadraticLightAttenuation>
class TriangleLight :
    public Light
{
    Vec3f1 mNormal;
    Vec3f1 mBinormal;
    Vec3f1 mTangent;
    Vec3f1 mIntensity;
    tLightAttenuation mAttenuation;
    Vec3f1 mVertex0;
    Vec3f1 mVertex1;
    Vec3f1 mVertex2;
    bool mInverse;
    float mInvArea;

    void computeLocalFrame()
    {
        Vec3f1 e0 = mVertex0 - mVertex1;
        Vec3f1 e1 = mVertex2 - mVertex1;
        Vec3f1 e2 = mVertex0 - mVertex2;
        mNormal = Math::cross(e0.normalize(), e1.normalize());
        mNormal *= (mInverse? 1.0f : -1.0f);
        mNormal.setNormalized();

        float a = e0.length();
        float b = e1.length();
        float c = e2.length();
        float s = (a + b + c) / 2.0f;
        float area = Math::sqrt(s*(s-a)*(s-b)*(s-c));

        mInvArea = 1.0f / area;
        Math::getFrame(mNormal, mBinormal, mTangent);
    }

public:

    void setVertex0(const Vec3f1& aVertex0)
    {
      mVertex0 = aVertex0;
      computeLocalFrame();
    }

    const Vec3f1& getVertex0() const
    {
        return mVertex0;
    }

    void setVertex1(const Vec3f1& aVertex1)
    {
        mVertex1 = aVertex1;
        computeLocalFrame();
    }

    const Vec3f1& getVertex1() const
    {
        return mVertex1;
    }

    void setVertex2(const Vec3f1& aVertex2)
    {
        mVertex2 = aVertex2;
        computeLocalFrame();
    }

    const Vec3f1& getVertex2() const
    {
        return mVertex2;
    }

    void setInverse(const bool aInverse)
    {
        mInverse = aInverse;
        computeLocalFrame();
    }

    const bool& getInverse() const
    {
        return mInverse;
    }

    RTFACT_DEFINE_PROPERTY(Vec3f1&,            Intensity);
    RTFACT_DEFINE_PROPERTY(tLightAttenuation&, Attenuation);

    RTFACT_LIGHT_INLINE TriangleLight() :
        Light(Light::CASTS_SHADOWS),
        mIntensity(0.f, 0.f, 0.f),
        mVertex0(0.f, 1.f, 0.f),
        mVertex1(0.f, 1.f, 1.f),
        mVertex2(1.f, 1.f, 1.f),
        mInverse(false)
    {
        computeLocalFrame();
        Math::seedRandom();
    }

    RTFACT_LIGHT_INLINE TriangleLight(
        const Vec3f<1>& aVertex0,
        const Vec3f<1>& aVertex1,
        const Vec3f<1>& aVertex2,
        const Vec3f<1>& aIntensity
    ) :
        Light(Light::CASTS_SHADOWS),
        mVertex0(aVertex0),
        mVertex1(aVertex1),
        mVertex2(aVertex2),
        mIntensity(aIntensity),
        mInverse(false)
    {
        computeLocalFrame();
        Math::seedRandom();
    }

    RTFACT_LIGHT_INLINE TriangleLight(
        const Vec3f<1>& aVertex0,
        const Vec3f<1>& aVertex1,
        const Vec3f<1>& aVertex2,
        const Vec3f<1>& aIntensity,
        const tLightAttenuation& aAttenuation
    ) :
        Light(Light::CASTS_SHADOWS),
        mIntensity(aIntensity),
        mVertex0(aVertex0),
        mVertex1(aVertex1),
        mVertex2(aVertex2),
        mAttenuation(aAttenuation),
        mInverse(false)
    {
        computeLocalFrame();
        Math::seedRandom();
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

        const t_Vec3fC intensityC = t_Vec3fC::replicate(mIntensity);
        const t_Vec3fC normalC = t_Vec3fC::replicate(-mNormal);
        const t_Vec3fC vertex0C = t_Vec3fC::replicate(mVertex0);
        const t_Vec3fC vertex1C = t_Vec3fC::replicate(mVertex1);
        const t_Vec3fC vertex2C = t_Vec3fC::replicate(mVertex2);

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            const t_PacketC r = Math::sqrt(t_PacketC::replicate(aPositionSamples(i).x));
            const t_PacketC u = t_PacketC::replicate(1) - r;
            const t_PacketC v = t_PacketC::replicate(aPositionSamples(i).y) * r;
            const t_PacketC w = t_PacketC::replicate(1) - u - v;

            oDirection(i) = u * vertex0C + v * vertex1C + w * vertex2C;
            oDirection(i) -= aPoint(i);
            oDistance(i) = oDirection(i).length();
            oDirection(i) *= Math::reciprocal(oDistance(i));
            oRadiance(i) = intensityC * Math::zeroClampDot(normalC, oDirection(i)) * mAttenuation.getFactor(oDistance(i)) / mInvArea;
        }
    }

    template<
        uint taSize>
    RTFACT_LIGHT_INLINE void sample(
        const Vec2f<taSize>& aPositionSamples,
        const Vec2f<taSize>& aDirectionSamples,
        const ActiveMask<taSize>& aMask,
        Vec3f<taSize>& oPosition,
        Vec3f<taSize>& oDirection,
        Vec3f<taSize>& oRadiance,
        Packet<taSize, float>& oPdf) const
    {
        typedef Packet<taSize, float>         t_Packet;
        typedef typename t_Packet::Container  t_PacketC;
        typedef Vec3f<taSize>                 t_Vec3f;
        typedef typename t_Vec3f::Container   t_Vec3fC;

        const t_Vec3fC vertex0C = t_Vec3fC::replicate(mVertex0);
        const t_Vec3fC vertex1C = t_Vec3fC::replicate(mVertex1);
        const t_Vec3fC vertex2C = t_Vec3fC::replicate(mVertex2);
        const t_Vec3fC normalC = t_Vec3fC::replicate(mNormal);
        const t_Vec3fC binormalC = t_Vec3fC::replicate(mBinormal);
        const t_Vec3fC tangentC = t_Vec3fC::replicate(mTangent);

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            const t_PacketC r = Math::sqrt(t_PacketC::replicate(aPositionSamples(i).x));
            const t_PacketC u = t_PacketC::replicate(1) - r;
            const t_PacketC v = t_PacketC::replicate(aPositionSamples(i).y) * r;
            const t_PacketC w = t_PacketC::replicate(1) - u - v;

            oPosition(i) = u * vertex0C + v * vertex1C + w * vertex2C;
            oDirection(i) = Math::cosineHemisphere(aDirectionSamples(i), &oPdf(i));
            oPdf(i) *= mInvArea;
            oRadiance(i) = oDirection(i).z * mIntensity;
            oDirection(i) = Math::transformToWorld(oDirection(i), normalC, binormalC, tangentC);
        }
    }

    RTFACT_LIGHT_INLINE const Vec3f<1> getTotalPower() const
    {
        return RTFACT_4_PI * mIntensity / mInvArea;
    }

    #include RTFACT_LIGHT_VIRTUAL_METHODS
};

RTFACT_NAMESPACE_END

#endif // RTFACT__TRIANGLELIGHT_HPP
