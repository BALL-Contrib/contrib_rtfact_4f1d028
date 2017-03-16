/* 
 *  AmbientOcclusionSurfaceShaderIntegrator.hpp
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
 *  Created on: 2010-06-19 23:00:43 +0200
 *  Author(s): Ken Dahm, Lukas Marsalek
 */
 
 #ifndef RTFACT__AMBIENTOCCLUSIONSURFACESHADERINTEGRATOR_HPP
#define RTFACT__AMBIENTOCCLUSIONSURFACESHADERINTEGRATOR_HPP

#include <RTremote/Parameterizable.hpp>

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Integrator.hpp>

#include <RTfact/Utils/Sampler.h>
#include <map>
using namespace RTfact::Remote;

RTFACT_NAMESPACE_BEGIN

template<
    class tContext,
  class tAmbientRaySampler>
class AmbientOcclusionSurfaceShaderIntegrator :
    public Integrator
{
    tContext mContext;
    tAmbientRaySampler& mAmbientRaySampler;
    bool useOldSamples;

    enum
    {
        INTERSECTION_DATA =
        IsectData::DISTANCE      |
        IsectData::NORMAL        |
        IsectData::APPEARANCE_ID |
        IsectData::TEX_COORDS    |
        IsectData::VERTEX_COLOR
    };

    struct HemispherePacket
    {
        bool initialized;
        RTFACT_AO_HEMI_SAMPLE_TYPE hemiSample;

        HemispherePacket() : initialized(false){}

        HemispherePacket(const HemispherePacket& hemiPacket)
        {
            initialized = hemiPacket.initialized;
            hemiSample = hemiPacket.hemiSample;
        }

        HemispherePacket(const RTFACT_AO_HEMI_SAMPLE_TYPE& sample)
        {
            hemiSample = sample;
            initialized = true;
        }
    };

    //std::map<uint, std::map<uint, std::vector<HemispherePacket> > > prevSamples[RTFACT_AO_ITER];
    HemispherePacket prevSamples[RTFACT_AO_ITER][RTFACT_AO_OLDSAMPLES_ARRAY_X][RTFACT_AO_OLDSAMPLES_ARRAY_X][PRIMARY_RAY_PACKET_SIZE];


    template<
        bool taCommonOrg,
        uint taSize>
        RTFACT_INTEGRATOR_INLINE void evaluateAmbientOcclusion(
        const PixelSampler::Sample<taSize>& aRayPacketSample,
        RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        FBData<taSize>& oResult,
        Packet<taSize, float>& dist,
        Vec3f<taSize>& normal)
    {
        const uint xPacketIdx = static_cast<uint>(aRayPacketSample.imageX[0]) / RayPacket<taSize>::SIZE_SQRT;
        const uint yPacketIdx = static_cast<uint>(aRayPacketSample.imageY[0]) / RayPacket<taSize>::SIZE_SQRT;

        oResult.ambientOcclusion = Packet<taSize, float>::replicate(RTFACT_AO_VALUE_HIT);
        for (int i=0; i < RTFACT_AO_ITER; ++i)
        {
            RayPacket<taSize> ambientRayPacket;
            ambientRayPacket.org = aRayPacket.template evaluate<taCommonOrg>(dist, aRayMask);

            RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(k, aRayMask)
            {
                Vec3f<4> w = normal(k);
                Vec3f<4> v = Math::cross(w, Vec3f<4>::replicate(0.0072f, 1.0f, 0.0034f));
                v.setNormalized();
                Vec3f<4> u = Math::cross(v, w);

                Packet<4, float> xpacket;
                Packet<4, float> ypacket;
                Packet<4, float> zpacket;
                if (useOldSamples)
                {
                    for (uint j=0; j < 4; ++j)
                    {
                        if (prevSamples[i][xPacketIdx][yPacketIdx][k * 4 + j].initialized)
                        {
                            RTFACT_AO_HEMI_SAMPLE_TYPE hemiSample = prevSamples[i][xPacketIdx][yPacketIdx][k * 4 + j].hemiSample;
                            xpacket[j] = hemiSample.x;
                            ypacket[j] = hemiSample.y;
                            zpacket[j] = hemiSample.z;
                        }
                        else
                        {
                            RTFACT_AO_HEMI_SAMPLE_TYPE hemiSample = mAmbientRaySampler.sampleHemisphere();
                            xpacket[j] = hemiSample.x;
                            ypacket[j] = hemiSample.y;
                            zpacket[j] = hemiSample.z;
                            prevSamples[i][xPacketIdx][yPacketIdx][k * 4 + j] = HemispherePacket(hemiSample);
                        }
                    }
                }
                else
                {
                    for (uint j=0; j < 4; ++j)
                    {
                        RTFACT_AO_HEMI_SAMPLE_TYPE hemiSample = mAmbientRaySampler.sampleHemisphere();
                        xpacket[j] = hemiSample.x;
                        ypacket[j] = hemiSample.y;
                        zpacket[j] = hemiSample.z;
                    }
                }
                ambientRayPacket.dir(k)  = u * xpacket + v * ypacket + w * zpacket;
                ambientRayPacket.org(k) += Packet<4, float>::replicate(RTFACT_AO_EPSILON) * ambientRayPacket.dir(k);
            }
            ambientRayPacket.dir.setNormalized();
            ambientRayPacket.invDir = ambientRayPacket.dir.reciprocal();
            ambientRayPacket.tMin = Packet<taSize, float>::C_EPS;
            ambientRayPacket.tMax = Packet<taSize, float>::replicate(RTFACT_AO_SHADOW_RAY_LEN);

            ActiveMask<taSize> ambientRayMask;
            ambientRayMask.setTrueAndUpdateActiveContainers();
            ambientRayMask = aRayMask;

            if (RTFACT_CHECK_UMASK(oResult.supportedData, FBSupportedData::COLORBLEEDING))
            {
                typename tContext::Intersector::template Result<taSize> shadowInters;
                oResult.colorBleeding = Vec3f<taSize>::replicate(1.0f, 1.0f, 1.0f);

                RTfact::Intersector::intersect<INTERSECTION_DATA, false>(
                mContext.intersector, ambientRayPacket, ambientRayMask,
                mContext.primitive, shadowInters);

                Packet<4, float> white = Packet<4, float>::replicate(RTFACT_AO_VALUE_HIT);
                RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(k, aRayMask)
                {
                    oResult.ambientOcclusion(k) *= shadowInters.mask(k).iif(Math::sqrt<4>(shadowInters.dist(k)), white);
                }

                mContext.template shade<false, false>(ambientRayPacket, shadowInters.mask, shadowInters, oResult.colorBleeding);
            }
            else
            {
                typename tContext::Intersector::template Result<taSize> shadowInters;
                RTfact::Intersector::intersect<IsectData::DISTANCE, false>(
                mContext.intersector, ambientRayPacket, ambientRayMask,
                mContext.primitive, shadowInters);

                Packet<4, float> white = Packet<4, float>::replicate(RTFACT_AO_VALUE_HIT);
                RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(k, aRayMask)
                oResult.ambientOcclusion(k) *= shadowInters.mask(k).iif(Math::sqrt<4>(shadowInters.dist(k)), white);
            }
        }
    }

public:

    AmbientOcclusionSurfaceShaderIntegrator(
        const tContext& aContext,
    tAmbientRaySampler& aAmbientRaySampler
    ) :
        mContext(aContext),
    mAmbientRaySampler(aAmbientRaySampler),
    useOldSamples(false)
    {
  }

  void setUseOldSamples(bool uos)
  {
    useOldSamples = uos;
  }

    template<
        bool taCommonOrg,
        uint taSize>
    RTFACT_INTEGRATOR_INLINE void evaluate(
        const PixelSampler::Sample<taSize>& aRayPacketSample,
        RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        const Scene&,
        const Primitive&,
        const Intersector&,
        FBData<taSize>& oResult)
    {
    oResult.color = Vec3f<taSize>::C_0_0_0();

        typename tContext::Intersector::template Result<taSize> intersection;

        RTfact::Intersector::intersect<INTERSECTION_DATA, taCommonOrg>(
            mContext.intersector, aRayPacket, aRayMask,
            mContext.primitive, intersection);

        if (RTFACT_CHECK_UMASK(oResult.supportedData, FBSupportedData::DISTANCE))
        {
            oResult.distance = intersection.dist;
        }

    if (RTFACT_CHECK_UMASK(oResult.supportedData, FBSupportedData::NORMAL))
    {

      for (uint i=0; i < Vec3f<taSize>::CONTAINER_COUNT; ++i)
      {
        Vec3f<4> normal = (Math::dot(intersection.normal(i), -aRayPacket.dir(i)) > Packet<taSize, float>::Container::C_0()).iif(intersection.normal(i), -intersection.normal(i));
        oResult.normal(i) = aRayMask(i).iif(
          intersection.mask(i).iif(normal, Vec3f<taSize>::Container::C_0_0_0()),
          oResult.normal(i));
      }
    }

        if (RTFACT_CHECK_UMASK(oResult.supportedData, FBSupportedData::ALPHA))
        {
            mContext.template shade<taCommonOrg, true>(
                aRayPacket, aRayMask, intersection, oResult.color, &oResult.alpha);
        }
        else
        {
            mContext.template shade<taCommonOrg, false>(
                aRayPacket, aRayMask, intersection, oResult.color);
        }

    if (RTFACT_CHECK_UMASK(oResult.supportedData, FBSupportedData::AMBIENTOCCLUSION))
    {
      intersection.normal = (Math::dot(intersection.normal, -aRayPacket.dir) > Packet<taSize, float>::Container::C_0()).iif(intersection.normal, -intersection.normal);
      evaluateAmbientOcclusion<taCommonOrg, taSize>(
                aRayPacketSample,
        aRayPacket,
        aRayMask,
        oResult,
        intersection.dist,
        intersection.normal);
    }
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__SURFACESHADERINTEGRATOR_HPP
