/* 
 *  SpotLight.hpp
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
 *  Created on: 2009-05-07 17:21:50 +0200
 *  Author(s): Mike Phillips, Ken Dahm
 */
 
 #ifndef RTFACT__SPOTLIGHT_HPP
#define RTFACT__SPOTLIGHT_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Light.hpp>
#include <RTfact/Model/LightAttenuation/QuadraticLightAttenuation.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tLightAttenuation = QuadraticLightAttenuation>
class SpotLight :
    public Light
{
    Vec3f<1> mPosition;
    Vec3f<1> mIntensity;
    Vec3f<1> mDirection;
    Vec3f<1> mBinormal;
    Vec3f<1> mTangent;

    float mCosBeamAngle;
    float mCosCutoff;

    tLightAttenuation mAttenuation;

    template<
        uint taSize>
    const Packet<taSize, float> getFalloff(const Packet<taSize, float>& aCosTheta) const
    {
        typedef Packet<taSize, float> t_PacketC;

        t_PacketC falloff;
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
        {
            for (uint k=0; k < t_PacketC::SIZE; ++k)
            {
                if (aCosTheta(i)[k] < mCosCutoff)
                {
                    falloff(i)[k] = 0.0f;
                }
                else if (aCosTheta(i)[k] > mCosBeamAngle)
                {
                    falloff(i)[k] = 1.0f;
                }
                else
                {
                    float delta = (aCosTheta(i)[k] - mCosCutoff) /
                                  (mCosBeamAngle - mCosCutoff);
                    falloff(i)[k] = delta;
                }
            }
        }
        return falloff;
    }

public:
    RTFACT_LIGHT_INLINE SpotLight() :
        Light(Light::HAS_COMMON_SAMPLE_POS |
              Light::IS_DELTA |
              Light::CASTS_SHADOWS),
        mPosition(0.f, 0.f, 0.f),
        mIntensity(0.f, 0.f, 0.f),
        mDirection(0.f, 1.f, 0.f)
    {
        setAngle(0.385f);
        setCutoff(1.04f);
        setDirection(mDirection);
    }
    
    RTFACT_LIGHT_INLINE SpotLight(
        const Vec3f<1>& aPosition,
        const Vec3f<1>& aIntensity,
        const Vec3f<1>& aDirection,
        const float& aAngle,
        const float& aCutoff
    ) :
        Light(Light::HAS_COMMON_SAMPLE_POS |
              Light::IS_DELTA |
              Light::CASTS_SHADOWS),
        mPosition(aPosition),
        mIntensity(aIntensity),
        mDirection(aDirection)
    {
        setAngle(aAngle);
        setCutoff(aCutoff);
        setDirection(aDirection);
    }

    RTFACT_LIGHT_INLINE SpotLight(
        const Vec3f<1>& aPosition,
        const Vec3f<1>& aIntensity,
        const Vec3f<1>& aDirection,
        const float& aAngle,
        const float& aCutoff,
        const tLightAttenuation& aAttenuation
    ) :
        Light(Light::HAS_COMMON_SAMPLE_POS |
              Light::IS_DELTA |
              Light::CASTS_SHADOWS),
        mPosition(aPosition),
        mIntensity(aIntensity),
        mDirection(aDirection),
        mAttenuation(aAttenuation)
    {
        setAngle(aAngle);
        setCutoff(aCutoff);
        setDirection(aDirection);
    }

    RTFACT_DEFINE_PROPERTY(Vec3f1&,            Position);
    RTFACT_DEFINE_PROPERTY(Vec3f1&,            Intensity);
    RTFACT_DEFINE_GET_PROPERTY(Vec3f1&,        Direction);
    RTFACT_DEFINE_PROPERTY(tLightAttenuation&, Attenuation);

    Vec3f1& setDirection(const Vec3f1& aDirection)
    {
        mDirection = aDirection.normalize();
        Math::getFrame(mDirection, mBinormal, mTangent);
        return mDirection;
    }

    void setAngle(const float aAngle)
    {
        //the beam angle should not exceed the cutoff angle
        mCosBeamAngle = Math::cos(Math::clamp(aAngle, 0.0f, getCutoff()));
    }

    const float getAngle() const
    {
        return Math::degrees(Math::acos(mCosBeamAngle));
    }

    void setCutoff(const float aCutoff)
    {
        mCosCutoff = Math::cos(Math::clamp(aCutoff, 0.0f, RTFACT_PI_OVER_2));
    }

    const float getCutoff() const
    {
        return Math::acos(mCosCutoff);
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
        Vec3f<1> *oCommonSamplePosition) const   
    {
        typedef Packet<taSize, float>        t_Packet;
        typedef typename t_Packet::Container t_PacketC;
        typedef Vec3f<taSize>                t_Vec3f;
        typedef typename t_Vec3f::Container  t_Vec3fC;
        typedef Mask<taSize>                  t_Mask;
        typedef typename t_Mask::Container    t_MaskC;

        RTFACT_ASSERT(oCommonSamplePosition != NULL);
        *oCommonSamplePosition = mPosition;
 
        const t_Vec3fC intensityC = t_Vec3fC::replicate(mIntensity);
        const t_Vec3fC directionC = t_Vec3fC::replicate(mDirection);

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            oDirection(i) = mPosition - aPoint(i);
            oDistance(i) = oDirection(i).length();
            oDirection(i) *= Math::reciprocal(oDistance(i));

            const t_PacketC costheta = Math::dot(directionC, -oDirection(i));
            const t_PacketC falloff = getFalloff(costheta);
            oRadiance(i) = intensityC * falloff * mAttenuation.getFactor(oDistance(i));
        }
    }

    template<
        uint taSize>
    RTFACT_LIGHT_INLINE void sample(
        const Vec2f<taSize>&,
        const Vec2f<taSize>& aDirectionSamples,
        const ActiveMask<taSize>& aMask,
        Vec3f<taSize>& oPosition,
        Vec3f<taSize>& oDirection,
        Vec3f<taSize>& oRadiance,
        Packet<taSize, float>& oPdf) const
    {
        typedef Packet<taSize, float>        t_Packet;
        typedef typename t_Packet::Container t_PacketC;
        typedef Vec3f<taSize>                t_Vec3f;
        typedef typename t_Vec3f::Container  t_Vec3fC;
        typedef Mask<taSize>                  t_Mask;
        typedef typename t_Mask::Container    t_MaskC;
 
        const t_Vec3fC intensityC = t_Vec3fC::replicate(mIntensity);
        const t_Vec3fC directionC = t_Vec3fC::replicate(mDirection);
        const t_Vec3fC binormalC = t_Vec3fC::replicate(mBinormal);
        const t_Vec3fC tangentC = t_Vec3fC::replicate(mTangent);
        const t_PacketC cosCutoffC = t_PacketC::replicate(mCosCutoff);

        oPosition = t_Vec3f::replicate(mPosition);
        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            oDirection(i) = Math::uniformCone(aDirectionSamples(i), cosCutoffC, &oPdf(i));
            //oDirection(i) = Math::uniformSphere(aDirectionSamples(i), &oPdf(i));
            oDirection(i) = Math::transformToWorld(oDirection(i), directionC, binormalC, tangentC);

            const t_PacketC costheta = Math::dot(directionC, oDirection(i));
            const t_PacketC falloff = getFalloff(costheta);
            oRadiance(i) = intensityC * falloff;
        }
    }

    RTFACT_LIGHT_INLINE const Vec3f<1> getTotalPower() const
    {
        return mIntensity * RTFACT_2_PI * (1.0f - 0.5f * (mCosCutoff + mCosBeamAngle));
    }

    #include RTFACT_LIGHT_VIRTUAL_METHODS
};

RTFACT_NAMESPACE_END

#endif // RTFACT__SPOTLIGHT_HPP
