/* 
 *  Picking.cpp
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
 *  Created on: 2010-02-22 11:22:10 +0100
 *  Author(s): Vaclav Krajicek, Lukas Marsalek, Petr Kmoch, Felix Klein
 */
 
 #include "Precompiled.hpp"

#define RTFACT_REMOTE_LIB

#include <RTfactRemoteConfig.hpp>
#include <RTremote/Picking.hpp>

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Picking.hpp>

#include "RTremotePrivateConfig.hpp"

RTFACT_REMOTE_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------
// Public Class Implementation
//-----------------------------------------------------------------------------

Picking::Picking(Renderer* renderer)
  : mRenderer(renderer)
{
}

void Picking::getResultTransformation(Result& result, Transform& transform)
{
    RTfact::Transform rtfactTransform = RTfact::Transform::identity();

    for(tGroupHandleVector::iterator it = result.mGroupsPath.begin();
        it != result.mGroupsPath.end();
        it++)
    {
        rtfactTransform *= (*it)->pimpl->mGroup->mXfm;
    }

    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
          transform.matrix[i][j] = rtfactTransform.matrix.columns[i][j];
}

bool Picking::pick(const ScreenCoordinates &input,
                   Result& oResult)
{

#ifndef USE_FLAT_STRUCTURES

    typedef RTfact::Picking<SceneConfig> tPicking;
    typedef t_PixelSampler::Sample<1> tRayPacketSample;

    //
    tRayPacketSample rayPacketSample;
    rayPacketSample.imageX = input.mPickX;
    rayPacketSample.imageY = input.mPickY;

    //
    tPicking picking;
    tPicking::Result pickResult;
    picking.pick<t_Tree, tRayPacketSample,t_Camera>(
        rayPacketSample,
        mRenderer->pimpl->camera,
        mRenderer->pimpl->intersector,
        mRenderer->pimpl->tree,
        pickResult);

    typedef tPicking::Result::t_GroupHandleVector tGroupHandleVector;

    //clean result
    oResult.mGroupsPath.clear();

    if(pickResult.mPath.size() > 0)
    {
        //find group
        tGroupHandleVector::iterator rtfact_groups_it = pickResult.mPath.begin();
        GroupHandle current_rtfact_group = mRenderer->getRoot();
        oResult.mGroupsPath.push_back(current_rtfact_group);
        rtfact_groups_it++;

        while(rtfact_groups_it != pickResult.mPath.end())
        {
            for(uint i = 0; i < current_rtfact_group->getGroupCount(); i++)
            {
                GroupHandle current_group_child = current_rtfact_group->getGroup(i);
                if((*current_group_child).pimpl->mGroup == *rtfact_groups_it)
                {
                    current_rtfact_group = current_group_child;
                    oResult.mGroupsPath.push_back(current_rtfact_group);
                    break;
                }
            }

            if((*current_rtfact_group).pimpl->mGroup != *rtfact_groups_it)
            {
              std::cerr << "Picking::pick - GROUP CORESPONDENCE ERROR";
              return false;
            }

            rtfact_groups_it++;
        }

        //find geometry
        if(oResult.mGroupsPath.size() > 0 &&
           oResult.mGroupsPath.back()->getGeometryCount() > 0)
        {
            GroupHandle lastGroup = oResult.mGroupsPath.back();
            for(uint i = 0; i < lastGroup->getGeometryCount(); i++)
            {
                GeoHandle geometry = lastGroup->getGeometry(i);
                if(geometry->pimpl->mMesh == pickResult.mMesh)
                {
                    oResult.mGeometry = geometry;
                    oResult.mPrimitiveNo =
                      pickResult.mPrimitiveBase +
                      pickResult.mPrimitiveOffset -
                      &(pickResult.mMesh->getPrimitive(0));
                    break;
                }
            }

            if(oResult.mGeometry->pimpl->mMesh != pickResult.mMesh)
            {
                std::cerr << "Picking::pick - WRONG MESH" << std::endl;
                return false;
            }
        }
        else
        {
            std::cerr << "Picking::pick - NO GEOMETRY" << std::endl;
            return false;
        }
    }

    return (oResult.mGroupsPath.size() > 0) &&
           (oResult.mGeometry.get() != NULL);
#else
    
    //Picking currently causes problems when flat structures are used, so switch it off
    return false;

#endif
}

RTFACT_REMOTE_NAMESPACE_END
