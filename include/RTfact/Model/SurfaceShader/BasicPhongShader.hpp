/*
 *  PhongShader.hpp
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
 *  Created on: 2008-08-07 18:24:48 +0200
 *  Author(s): Iliyan Georgiev, Petr Kmoch, Lukas Marsalek
 */

#ifndef RTFACT__BASICPHONGSHADER_HPP
#define RTFACT__BASICPHONGSHADER_HPP

#include <RTfact/Config/Common.hpp>

#include <utility>

#include <RTfact/Concept/Appearance/SurfaceShader.hpp>
#include <RTfact/Utils/ShadingUtils.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tContext,
    class tDiffuseTexture>
class BasicPhongShader :
    public SurfaceShader<tContext>
{
    tDiffuseTexture mDiffuseTexture;
    Vec3f1 mDiffuseColor;
    Vec3f1 mSpecularColor;
    Vec3f1 mAmbientIntensity;
    float mShininess;
    bool mUseVertexColor;
	bool mReceiveIllumination;

    uint mNumShadowSamples;

public:

    BasicPhongShader() :
        mDiffuseColor(0.5f, 0.5f, 0.5f),
        mSpecularColor(0.f, 0.f, 0.f),
        mAmbientIntensity(0.7f, 0.7f, 0.7f),
        mShininess(0.f),
        mUseVertexColor(false),
		mReceiveIllumination(true),
        mNumShadowSamples(1)
    {}

    RTFACT_DEFINE_PROPERTY(tDiffuseTexture&, DiffuseTexture);
    RTFACT_DEFINE_PROPERTY(Vec3f1&,          DiffuseColor);
    RTFACT_DEFINE_PROPERTY(Vec3f1&,          SpecularColor);
    RTFACT_DEFINE_PROPERTY(Vec3f1&,          AmbientIntensity);
    RTFACT_DEFINE_PROPERTY(float,            Shininess);
	RTFACT_DEFINE_PROPERTY(bool,             UseVertexColor);
	RTFACT_DEFINE_PROPERTY(bool,             ReceiveIllumination);
    RTFACT_DEFINE_PROPERTY(uint,             NumShadowSamples);

    tDiffuseTexture& getDiffuseTexture()
    {
        return mDiffuseTexture;
    }

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
        RTFACT_APPEARANCE_ASSERT(!taStoreAlpha || oResultAlpha != RTFACT_NULL);

        typedef Vec3f<taSize>                 t_Vec3f;
        typedef Packet<taSize, float>         t_FPacket;
        typedef typename t_FPacket::Container t_FPacketC;
        typedef Packet<taSize, int>           t_IPacket;
        typedef typename t_Vec3f::Container   t_Vec3fC;
        typedef Mask<taSize>                  t_Mask;
        typedef typename t_Mask::Container    t_MaskC;
        typedef ActiveMask<taSize>            t_ActiveMask;

        t_Vec3f diffuseColor;

        mDiffuseTexture.template sample<true, false>(
            aIntersection, aRayMask, &diffuseColor);

        const t_Vec3fC diffColC = t_Vec3fC::replicate(mDiffuseColor);

        t_Vec3fC ambientIntC;
        ambientIntC = t_Vec3fC::replicate(mAmbientIntensity);

        t_Vec3f hitPoint, reflView;
        ActiveMask<taSize> viewDotNormalMask;
        viewDotNormalMask.setTrue();

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

            //set to true if shading should happen, e.g. N dot Dir <= 0
            viewDotNormalMask(i) = Math::dot(aIntersection.normal(i), aRayPacket.dir(i)) <= t_FPacketC::C_0();

            if(mUseVertexColor)
            {
                diffuseColor(i) *= aIntersection.vertexColor(i);
            }
            else
            {
                diffuseColor(i) *= diffColC;
            }


 			oResult(i) = aRayMask(i).iif(
                 ambientIntC * diffuseColor(i),
                 oResult(i));

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

			reflView(i) =
				aRayPacket.dir(i) - 2.f * aIntersection.normal(i) * Math::dot(
				aRayPacket.dir(i), aIntersection.normal(i));
		}
		
		if(mReceiveIllumination) {
			for(uint l = 0; l < aContext.scene.lights.size(); ++l)
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

				//const t_Vec3fC diffColC = t_Vec3fC::replicate(mDiffuseColor);
				const t_Vec3fC specColC = t_Vec3fC::replicate(mSpecularColor);
				const t_FPacketC shininessC = t_FPacketC::replicate(mShininess*128.f);

				RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, lightContributionMask)
				{
					const t_FPacketC dotReflNormal = Math::max(
						Math::dot(directionToLight(i), reflView(i)), t_FPacketC::C_0());

					oResult(i) = lightContributionMask(i).iif(
						oResult(i) + intensity(i) *
						(diffuseColor(i) * dotLightNormal(i) + specColC * Math::pow(dotReflNormal, shininessC)),
						oResult(i));
				}
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

#endif // RTFACT__BASICPHONGSHADER_HPP
