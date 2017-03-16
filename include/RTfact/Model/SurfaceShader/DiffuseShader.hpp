/* 
 *  DiffuseShader.hpp
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
 *  Author(s): Iliyan Georgiev, Petr Kmoch, Dmitri Rubinstein
 */
 
 #ifndef RTFACT__DIFFUSESHADER_HPP
#define RTFACT__DIFFUSESHADER_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Appearance/SurfaceShader.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tContext,
    class tTexture>
class DiffuseShader :
    public SurfaceShader<tContext>
{
    tTexture mTexture;

public:

    DiffuseShader(
        const tTexture& aTexture
    ) :
        mTexture(aTexture)
    {}

    const tTexture &getTexture() const { return mTexture; }

    tTexture &getTexture() { return mTexture; }

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
        typedef typename t_Vec3f::Container   t_Vec3fC;

        //oResult = aRayMask.iif(t_Vec3f::C_1_1_1, t_Vec3f::C_0_0_0()); 
        //return;

        t_Vec3f color;

        mTexture.sample(aIntersection, aRayMask, color);

        //oResult = aRayMask.iif(t_Vec3f::C_1_0_0, oResult);

        const t_FPacketC invPiC = t_FPacketC::replicate(RTFACT_INV_PI);
        t_Vec3f hitPoint;

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
        {
            aIntersection.normal(i) = 
                (Math::dot(aIntersection.normal(i), aRayPacket.dir(i)) >
                    t_FPacketC::C_0()).iif(
                        aIntersection.normal(i),
                        -aIntersection.normal(i));

            color(i) = aRayMask(i).iif(
                color(i) * invPiC * Math::dot(aIntersection.normal(i), aRayPacket.dir(i)),
                t_Vec3fC::C_0_0_0());

            if(taCommonOrg)
            {
                hitPoint(i) = aRayPacket.org(0) + (aIntersection.dist(i) - t_FPacketC::C_RAY_EPS) * aRayPacket.dir(i);
            }
            else
            {
                hitPoint(i) = aRayPacket.org(i) + (aIntersection.dist(i) - t_FPacketC::C_RAY_EPS) * aRayPacket.dir(i);
            }
        }

        for(uint l = 0; l < aContext.scene.lights.size(); ++l)
        {
            RayPacket<taSize> shadowRayPacket;
            t_Vec3f directionToLight;
            t_FPacket invProb;
            t_Vec3f intensity;
            
            aContext.scene.lights[l]->sample(
                hitPoint, aRayMask, shadowRayPacket, directionToLight, intensity);

            typename tContext::Intersector::template Result<taSize> shadowInters;

            aContext.template intersect<IsectData::NONE, true>(
                shadowRayPacket, aRayMask, shadowInters);

            RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
            {
                color(i) = shadowInters.mask(i).iif(t_Vec3fC::C_0_0_0(), color(i) * intensity(i));

                oResult(i) = aRayMask(i).iif(color(i), oResult(i));

                if (
                    taStoreAlpha)
                {
                    (*oResultAlpha)(i) = aRayMask(i).iif(t_FPacketC::C_1, (*oResultAlpha)(i));
                }

                //oResult(i) *= aRayMask(i).iif(t_Vec3fC::C_0_0_1, t_Vec3fC::C_0_0_0());
            }
        }
    }

    template<
        bool taCommonOrg,
        bool taStoreAlpha>
    RTFACT_APPEARANCE_INLINE void shade(
        const RayPacket<1>& aRayPacket,
        const ActiveMask<1>& aRayMask,
        SurfaceIntersection<1>& aIntersection,
        tContext& aContext,
        Vec3f<1>& oResult,
        Packet<1, float>* oResultAlpha = RTFACT_NULL) const
    {}

    #include RTFACT_SURFACE_SHADER_VIRTUAL_METHODS
};

RTFACT_NAMESPACE_END

#endif // RTFACT__DIFFUSESHADER_HPP
