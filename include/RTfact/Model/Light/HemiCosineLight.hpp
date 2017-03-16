/* 
 *  HemiCosineLight.hpp
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
 *  Created on: 2010-12-18 21:30:08 +0200
 *  Author(s): Lukas Marsalek
 */
 
#ifndef RTFACT__HEMI_COSINE_LIGHT_HPP
#define RTFACT__HEMI_COSINE_LIGHT_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Light.hpp>
#include <RTfact/Model/LightAttenuation/QuadraticLightAttenuation.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tLightAttenuation = QuadraticLightAttenuation>
class HemiCosineLight :
    public Light
{
    Vec3f1 mPosition;
    Vec3f1 mNormal;
    Vec3f1 mIntensity;
    tLightAttenuation mAttenuation;

public:

    RTFACT_DEFINE_PROPERTY(Vec3f1&,            Normal);
    RTFACT_DEFINE_PROPERTY(Vec3f1&,            Position);
    RTFACT_DEFINE_PROPERTY(Vec3f1&,            Intensity);
    RTFACT_DEFINE_PROPERTY(tLightAttenuation&, Attenuation);

    RTFACT_LIGHT_INLINE HemiCosineLight() :
        Light(Light::HAS_COMMON_SAMPLE_POS |
              Light::IS_DELTA |
              Light::CASTS_SHADOWS),
        mPosition(0.f, 0.f, 0.f),
        mIntensity(0.f, 0.f, 0.f),
        mNormal(0.f, 0.f, 0.f)
    {}
    
    RTFACT_LIGHT_INLINE HemiCosineLight(
        const Vec3f1& aPosition,
        const Vec3f1& aIntensity,
        const Vec3f1& aNormal
    ) :
        Light(Light::HAS_COMMON_SAMPLE_POS |
              Light::IS_DELTA |
              Light::CASTS_SHADOWS),
        mPosition(aPosition),
        mIntensity(aIntensity),
        mNormal(aNormal.normalize())
    {}

    RTFACT_LIGHT_INLINE HemiCosineLight(
        const Vec3f1& aPosition,
        const Vec3f1& aIntensity,
        const Vec3f1& aNormal,
        const tLightAttenuation& aAttenuation
    ) :
        Light(Light::HAS_COMMON_SAMPLE_POS |
              Light::IS_DELTA |
              Light::CASTS_SHADOWS),
        mPosition(aPosition),
        mIntensity(aIntensity),
        mNormal(aNormal.normalize()),
        mAttenuation(aAttenuation)
    {}

    template<
        uint taSize>
    RTFACT_LIGHT_INLINE void illuminatePoint(
        const Vec2f<taSize>& aPositionSamples,
        const Vec3f<taSize>& aPoint,
        const ActiveMask<taSize>& aMask,
        Vec3f<taSize>& oDirection,
        Packet<taSize, float>& oDistance,
        Vec3f<taSize>& oRadiance,
        Vec3f<1> *oCommonSamplePosition) const   
    {
        typedef Packet<taSize, float>           t_FPacket;
        typedef typename t_FPacket::Container   t_FPacketC;
        typedef Vec3f<taSize>                   t_Vec3f;
        typedef typename t_Vec3f::Container     t_Vec3fC;

        RTFACT_ASSERT(oCommonSamplePosition != NULL);
        *oCommonSamplePosition = mPosition;

        const t_Vec3fC intensityC = t_Vec3fC::replicate(mIntensity);        
        const t_Vec3fC lightNormalC = t_Vec3fC::replicate(mNormal);

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            oDirection(i) = mPosition - aPoint(i);
            oDistance(i) = oDirection(i).length();
            oDirection(i) *= Math::reciprocal(oDistance(i));
            const t_FPacketC dirDotNormalC = Math::max(-Math::dot(lightNormalC, oDirection(i)), t_FPacketC::C_0());
            oRadiance(i) = intensityC * dirDotNormalC * mAttenuation.getFactor(oDistance(i));
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
        oPosition = Vec3f<taSize>::replicate(mPosition);
        
        typedef Packet<taSize, float>           t_FPacket;
        typedef typename t_FPacket::Container   t_FPacketC;
        typedef Vec3f<taSize>                   t_Vec3f;
        typedef typename t_Vec3f::Container     t_Vec3fC;
        typedef Vec2f<taSize>                   t_Vec2f;
        typedef typename t_Vec2f::Container     t_Vec2fC;

        const t_Vec3fC intensityC = t_Vec3fC::replicate(mIntensity);
        const t_Vec3fC lightNormalC = t_Vec3fC::replicate(mNormal);

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            t_FPacketC pdf;
            const t_Vec2fC uvSamples = aPositionSamples(i);//(aU(i), aV(i));
            t_Vec3fC direction;
            direction = Math::cosineHemisphere(uvSamples, &pdf);
            oDirection(i) = direction;
            oPdf(i) = pdf;
            const t_FPacketC dirDotNormalC = Math::max(-Math::dot(lightNormalC, oDirection(i)), t_FPacketC::C_0());
            oRadiance(i) = intensityC * dirDotNormalC;
        }
    }

    RTFACT_LIGHT_INLINE const Vec3f1 getTotalPower() const
    {
        return Vec3f1::C_0_0_0();
    }

    #include RTFACT_LIGHT_VIRTUAL_METHODS
};

RTFACT_NAMESPACE_END

#endif // RTFACT__HEMI_COSINE_LIGHT_HPP
