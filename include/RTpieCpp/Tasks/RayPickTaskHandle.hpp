/* 
 *  RayPickTaskHandle.hpp
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
 *  Created on: 2011-01-06 00:15:00 +0200
 *  Author(s): Felix Klein, Boris Broenner
 */
 
#ifndef RTFACT_HANDLE_RAYPICKTASK_HPP
#define RTFACT_HANDLE_RAYPICKTASK_HPP

#include <RTpieCpp/Config/Common.hpp>

#include <RTpie/Tasks/IRayPickTask.hpp>

#define IMPL_INTERFACE RTfact::RTpie::IRayPickTask
WRAPPER_FWD(RayPickTaskHandle)
#undef IMPL_INTERFACE

#include <RTpieCpp/Base/BaseHandle.hpp>
#include <RTpieCpp/Scene/SceneContentHandles.hpp>
#include <RTpieCpp/Data/Float3.hpp>

#define IMPL_INTERFACE RTfact::RTpie::IRayPickTask
WRAPPER_START(RayPickTaskHandle, BaseHandle)

    // -----------------------------------------------------
    // Configure RayPickTask
    // -----------------------------------------------------

    void rayPick(const float origX, const float origY, const float origZ,
              const float dirX, const float dirY, const float dirZ)
    IMPLEMENT_WRAPPER_NO_RETURN( RayPick(origX, origY, origZ, dirX, dirY, dirZ) )

    // -----------------------------------------------------
    // Results of RayPickTask
    // -----------------------------------------------------

    bool hasResult()
    IMPLEMENT_WRAPPER_ARG0(bool, HasResult)

    InstanceHandle getInstance()
    IMPLEMENT_WRAPPER_ARG0(RTfact::RTpie::IInstance*, GetInstance)

    MeshHandle getMesh()
    IMPLEMENT_WRAPPER_ARG0(RTfact::RTpie::IMesh*, GetMesh)

    float3 getPosition()
    IMPLEMENT_WRAPPER_FLOAT3(GetPosition)

    float3 getNormal()
    IMPLEMENT_WRAPPER_FLOAT3( GetNormal)


WRAPPER_END
#undef IMPL_INTERFACE



#endif // RTFACT_HANDLE_RAYPICKTASK_HPP
