/*
 *  TwoLevelPicking.hpp
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
 *  Created on: 2010-09-15 17:54:13 +0200
 *  Author(s): Felix Klein, Byelozyorov Sergiy
 */

 #ifndef RTFACT__PICKING_HPP
#define RTFACT__PICKING_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Containers/Vector.hpp>
#include <RTfact/Utils/SurfaceIntersection.hpp>
#include <RTfact/Utils/RayPacket.hpp>
#include <RTfact/Concept/Intersector.hpp>

RTFACT_NAMESPACE_BEGIN

template<class tSceneConfig>
class TwoLevelPicking
{
    enum
    {
        INTERSECTION_DATA =
        IsectData::DISTANCE |
        IsectData::NORMAL |
        IsectData::APPEARANCE_ID |
        IsectData::TEX_COORDS |
        IsectData::VERTEX_COLOR
    };

public:

    typedef typename tSceneConfig::OuterTree   tPrimitive;
    typedef typename tSceneConfig::Intersector tIntersector;

    class Result
    {
    public:

        typedef typename tSceneConfig::Mesh             t_Mesh;
        typedef typename Traits<t_Mesh>::Ptr            t_MeshHandle;

        typedef typename tSceneConfig::Geometry           t_Geometry;
        typedef typename Traits<t_Geometry>::Ptr          t_GeometryHandle;

        typedef typename tSceneConfig::Instance         t_Instance;
        typedef typename Traits<t_Instance>::Ptr        t_InstanceHandle;

        typedef typename tSceneConfig::Primitive        t_Primitive;

        t_MeshHandle        mMesh;
        t_InstanceHandle    mInstance;
        t_Primitive*        mPrimitiveBase;
        Vec3f<1>            mPosition;
        Vec3f<1>            mNormal;
        int                 mPrimitiveOffset;

        bool isEmpty()
        {
            return mMesh == 0;
        }

    };

    template<
      class t_Primitive,
      class t_RayPacketSample,
      class t_Camera>
    bool pick(
      t_RayPacketSample& rayPacketSample,
      const t_Camera& camera,
      typename tSceneConfig::Intersector& intersector,
      t_Primitive& primitive,
      Result& oresult)
    {
        RTfact::RayPacket<1> rayPacket = camera.generateRay(rayPacketSample);

        return rayPick<t_Primitive>(rayPacket, intersector, primitive, oresult);
    }

    template<class t_Primitive>
    bool rayPick(
      RTfact::RayPacket<1>& rayPacket,
      typename tSceneConfig::Intersector& intersector,
      t_Primitive& primitive,
      Result& oresult)
    {
        RTfact::ActiveMask<1> rayMask;
        rayMask.setTrueAndUpdateActiveContainers();

        typename tIntersector::template Result<1> intersection;

        RTfact::Intersector::intersect<INTERSECTION_DATA, true>(
            intersector, rayPacket, rayMask,
            primitive, intersection);

        if(intersection.mask(0))
        {
            typedef typename tSceneConfig::AccelInstance tAccelInstance;

            tAccelInstance* instance = (intersection.instanceBase + intersection.instanceOffsets(0));
            typename Result::t_Primitive *prim = (&intersection.getPrimitive(0) + intersection.primitiveOffset.data);


            oresult.mInstance = instance->mInstance;
            oresult.mMesh = oresult.mInstance->getGeometry()->getMesh(prim->appearanceID);

            oresult.mPrimitiveBase = &(intersection.getPrimitive(0));
            oresult.mPrimitiveOffset = intersection.primitiveOffset.data;

            oresult.mPosition = rayPacket.org + rayPacket.dir * intersection.dist[0];
            oresult.mNormal = intersection.normal;

            /*
            std::cout << std::endl;
            std::cout << std::endl;
            std::cout << input.mPickX << "," << input.mPickY
                      << " - primitiveBase: " << intersection.primitiveBase
                      << " - primitiveOffset: " << intersection.primitiveOffset.data
                      << " - mMesh: " << oresult.mMesh << std::endl;
            */
            return true;
        }
        return false;
    }

    template<class t_Primitive>
    bool intersectRays(
      typename tSceneConfig::Intersector& intersector,
      t_Primitive& primitive,
      const float* aOrigins,
      const float* aDirections,
      const unsigned int aRayCount,
      float* oDistances)
    {
        ActiveMask<1> mask;
        mask.setTrueAndUpdateActiveContainers();

        for(uint i = 0; i < aRayCount; ++i)
        {
            typename tIntersector::template Result<1> intersection;

            const uint i3 = 3 * i;

            RayPacket<1> ray;
            ray.org = Vec3f1(aOrigins[i3], aOrigins[i3 + 1], aOrigins[i3 + 2]);
            ray.dir = Vec3f1(aDirections[i3], aDirections[i3 + 1], aDirections[i3 + 2]);
            ray.tMin = 0.f;
            ray.tMax = RTFACT_INFINITY;

            RTfact::Intersector::intersect<INTERSECTION_DATA, true>(
                intersector, ray, mask,
                primitive, intersection);

            oDistances[i] = intersection.dist;
        }

        return true;
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__PICKING_HPP
