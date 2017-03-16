#ifndef RTFACT__PHONGSHADERSL_HPP
#define RTFACT__PHONGSHADERSL_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Appearance/SurfaceShader.hpp>

//#define RTFACT__PHONG
#define RTFACT__CHECKER

RTFACT_NAMESPACE_BEGIN

RTFACT_INLINE const Packet<4, float> mod(
    const Packet<4, float>& a,
    const Packet<4, float>& b)
{
    Packet<4, float> result;
    Packet<4, float> temp;

    temp.data.data = _mm_cvtepi32_ps(_mm_cvtps_epi32((a.data / b.data).data));
    result = a - temp * b;

    const Mask<4> mask = (result < Packet<4, float>::C_0());

    return mask.iif(result + b, result);
}

RTFACT_INLINE const Packet<1, float> mod(
    const Packet<1, float>& a,
    const Packet<1, float>& b)
{
    Packet<1, float> result;
    Packet<1, int> temp;

    temp.data = (int) (a.data / b.data);
    result.data = a.data - temp.data * b.data;

    if(result.data < 0.f)
    {
        result.data += b.data;
    }

    return result;
}

template<
    class tContext>
class TestShader :
    public SurfaceShader<tContext>
{
    Vec3f1 mAmbientIntensity, mDiffuseColor, mSpecularColor;
    Vec3f1 mWhiteColor, mBlackColor;

    float mShininess;
    float mFrequency, mSScale, mTScale;

public:

    TestShader() :
        mAmbientIntensity(1.f, 1.f, 1.f),
        mDiffuseColor(1.f, .2f, .2f),
        mSpecularColor(0.f, 0.f, 0.f),
        mWhiteColor(1.f, 1.f, 1.f),
        mBlackColor(0.f, 0.f, 0.f),
        mShininess(0.1f),
        mFrequency(50.f),
        mSScale(1.f),
        mTScale(1.f)
    {}

    //RTFACT_DEFINE_PROPERTY(Vec3f1&, AmbientIntensity);
    //RTFACT_DEFINE_PROPERTY(Vec3f1&, DiffuseColor);
    //RTFACT_DEFINE_PROPERTY(Vec3f1&, SpecularColor);
    //RTFACT_DEFINE_PROPERTY(float,  Shininess);

    template<
        bool taCommonOrg,
        bool taStoreAlpha,
        uint taSize>
    RTFACT_APPEARANCE_INLINE void shade(
        const RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        SurfaceIntersection<taSize>& aIntersection,
        tContext& aContext,
        Vec3f<taSize>& oResult,
        Packet<taSize, float>* oResultAlpha = RTFACT_NULL) const
    {
        typedef Vec3f<taSize>                 t_Vec3f;
        typedef Packet<taSize, float>         t_FPacket;
        typedef typename t_FPacket::Container t_FPacketC;
        typedef typename t_Vec3f::Container   t_Vec3fC;
        typedef Mask<taSize>                  t_Mask;
        typedef typename t_Mask::Container    t_MaskC;
        typedef ActiveMask<taSize>            t_ActiveMask;

        t_Vec3f hitPoint, reflView;
        const t_Vec3fC ambient = t_Vec3fC::replicate(mAmbientIntensity * mDiffuseColor);
        const t_Vec3fC white = t_Vec3fC::replicate(mWhiteColor);
        const t_Vec3fC black = t_Vec3fC::replicate(mBlackColor);
        const t_FPacketC half = t_FPacketC::replicate(0.5f);
        t_Vec3fC color;

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
        {
            if(RTFACT_CHECK_UMASK(tContext::Scene::PROPERTIES, SceneProperties::INVERT_NORMAL))
            {
                aIntersection.normal(i) =
                    (Math::dot(aIntersection.normal(i), aRayPacket.dir(i)) >
                        t_FPacketC::C_0()).iif(
                            -aIntersection.normal(i),
                            aIntersection.normal(i));
            }

            if(taCommonOrg)
            {
                hitPoint(i) =
                    aRayPacket.org(0) + aIntersection.dist(i) * aRayPacket.dir(i);
            }
            else
            {
                hitPoint(i) =
                    aRayPacket.org(i) + aIntersection.dist(i) * aRayPacket.dir(i);
            }

            #ifdef RTFACT__PHONG

                reflView(i) =
                    aRayPacket.dir(i) - 2.f * aIntersection.normal(i) * Math::dot(
                        aRayPacket.dir(i), aIntersection.normal(i));

                oResult(i) = aRayMask(i).iif(ambient, oResult(i));

            #endif

            #ifdef RTFACT__CHECKER

                const t_FPacketC repeatCount = t_FPacketC::replicate(mFrequency);
                t_FPacketC ss = aIntersection.texCoord1(i) * repeatCount + aIntersection.texCoord2(i);
                t_FPacketC tt = aIntersection.texCoord2(i) * repeatCount + aIntersection.texCoord1(i);

                t_FPacketC filterWidthSS =
                    Math::abs((aIntersection.dt1du(i) * repeatCount + aIntersection.dt2du(i)) * aIntersection.du(i)) +
                    Math::abs((aIntersection.dt1dv(i) * repeatCount + aIntersection.dt2dv(i)) * aIntersection.dv(i));

                t_FPacketC filterWidthTT =
                    Math::abs((aIntersection.dt2du(i) * repeatCount + aIntersection.dt1du(i)) * aIntersection.du(i)) +
                    Math::abs((aIntersection.dt2dv(i) * repeatCount + aIntersection.dt1dv(i)) * aIntersection.dv(i));

                ss = mod(ss, t_FPacketC::replicate(2.f));
                tt = mod(tt, t_FPacketC::replicate(2.f));

                t_FPacketC smag = Math::smoothStep(ss, 0.5f - filterWidthSS, 0.5f + filterWidthSS);
                smag -= Math::smoothStep(ss, 1.5f - filterWidthSS, 1.5f + filterWidthSS);

                smag = 2.f * smag - 1.f;
                smag *= 1.f - Math::smoothStep(filterWidthSS, t_FPacketC::replicate(.4f), t_FPacketC::replicate(1.2f));

                t_FPacketC tmag = Math::smoothStep(tt, 0.5f - filterWidthTT, 0.5f + filterWidthTT);
                tmag -= Math::smoothStep(tt, 1.5f - filterWidthTT, 1.5f + filterWidthTT);
                tmag = 2.f * tmag - 1.f;
                tmag *= 1.f - Math::smoothStep(filterWidthTT, t_FPacketC::replicate(.4f), t_FPacketC::replicate(1.2f));

                const t_FPacketC col = (smag * tmag) * 0.5f + 0.5f;

                color(i).x = col;
                color(i).y = col;
                color(i).z = col;

                oResult(i) = aRayMask(i).iif(ambient * color(i), oResult(i));

            #endif
        }

        for(uint l = 10; l < aContext.scene.lights.size(); ++l)
        {
            t_Vec3f directionToLight;
            t_Vec3f intensity;
            t_FPacket dotLightNormal;
            t_ActiveMask lightContributionMask;

            if(!getLightIntensity(
                hitPoint, aIntersection.normal, aRayMask,
                *aContext.scene.lights[l], aContext, directionToLight, intensity,
                dotLightNormal, lightContributionMask))
            {
                continue;
            }

            const t_Vec3fC diffColC = t_Vec3fC::replicate(mDiffuseColor);
            const t_FPacketC zero = t_FPacketC::C_0();

            #ifdef RTFACT__PHONG

                const t_Vec3fC specColC = t_Vec3fC::replicate(mSpecularColor);
                const t_FPacketC shininessC = t_FPacketC::replicate(mShininess);

            #endif

            RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, lightContributionMask)
            {
                #ifdef RTFACT__PHONG

                    const t_FPacketC dotReflNormal = Math::max(
                    Math::dot(directionToLight(i), reflView(i)), zero);

                    oResult(i) = lightContributionMask(i).iif(
                        oResult(i) + intensity(i) * dotLightNormal(i) *
                            (diffColC + specColC * Math::pow(dotReflNormal, shininessC)),
                        oResult(i));

                #endif

                #ifdef RTFACT__CHECKER

                    oResult(i) = lightContributionMask(i).iif(
                        oResult(i) + intensity(i) * dotLightNormal(i) * diffColC * color(i),
                        oResult(i));

                #endif
            }
        }
    }

    template<
        uint taSize>
    RTFACT_APPEARANCE_INLINE void attenuateLight(
        const ActiveMask<taSize>& aMask,
        SurfaceIntersection<taSize>& aIntersection,
        Mask<taSize>& oMask,
        Vec3f<taSize>& oIntensity) const
    {
        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            oMask(i) &= ~aMask(i);
        }
    }

    #include RTFACT_SURFACE_SHADER_VIRTUAL_METHODS
};

RTFACT_NAMESPACE_END

#endif // RTFACT__PHONGSHADERSL_HPP
