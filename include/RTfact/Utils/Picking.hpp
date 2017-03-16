/* 
 *  Picking.hpp
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
 *  Created on: 2010-02-22 12:03:07 +0100
 *  Author(s): Vaclav Krajicek
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
class Picking
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

        typedef typename tSceneConfig::Group            t_Group;
        typedef typename Traits<t_Group>::Ptr           t_GroupHandle;

        typedef typename RTfact::Vector<t_GroupHandle>          t_GroupHandleVector;

        typedef typename tSceneConfig::Instance         t_Instance;
        typedef typename Traits<t_Instance>::Ptr        t_InstanceHandle;

        typedef typename tSceneConfig::Primitive        t_Primitive;

        t_MeshHandle        mMesh;
        t_InstanceHandle    mInstance;
        t_GroupHandleVector mPath;
        t_Primitive*        mPrimitiveBase;
        int                 mPrimitiveOffset;

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

        RTfact::ActiveMask<1> rayMask;
        rayMask.setTrueAndUpdateActiveContainers();

        typename tIntersector::template Result<1> intersection;

        RTfact::Intersector::intersect<INTERSECTION_DATA, true>(
            intersector, rayPacket, rayMask,
            primitive, intersection);

        if(intersection.mask(0))
        {
            typedef typename tSceneConfig::AccelInstance tAccelInstance;
            typedef typename tAccelInstance::t_GroupHandleVector tGroupVector;

            //Causes problems with USE_FLAT_STRUCTURES - needs fixing
            tAccelInstance* instance = (intersection.instanceBase + intersection.instanceOffsets(0));

            std::cout << "path: ";
            for(typename tGroupVector::iterator it = instance->mPath.begin();
                it != instance->mPath.end();
                it++)
            {
                oresult.mPath.push_back(*it);
                //std::cout << &(*(*it)) << " ";
            }

            oresult.mMesh = instance->mInstance->mMesh;
            oresult.mInstance = instance->mInstance;
            oresult.mPrimitiveBase = &(intersection.getPrimitive(0));
            oresult.mPrimitiveOffset = intersection.primitiveOffset.data;

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

};

RTFACT_NAMESPACE_END

#endif // RTFACT__SIMPLEINTEGRATOR_HPP
