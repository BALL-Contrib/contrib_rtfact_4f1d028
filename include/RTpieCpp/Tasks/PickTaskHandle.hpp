/* 
 *  PickTaskHandle.hpp
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
 *  Created on: 2010-09-26 18:55:36 +0200
 *  Author(s): Felix Klein
 */
 
 #ifndef RTFACT_HANDLE_PICKTASK_HPP
#define RTFACT_HANDLE_PICKTASK_HPP

#include <RTpieCpp/Config/Common.hpp>

#include <RTpie/Tasks/IPickTask.hpp>

#define IMPL_INTERFACE RTfact::RTpie::IPickTask
WRAPPER_FWD(PickTaskHandle)
#undef IMPL_INTERFACE

#include <RTpieCpp/Base/BaseHandle.hpp>
#include <RTpieCpp/Scene/SceneContentHandles.hpp>
#include <RTpieCpp/Data/CameraHandle.hpp>
#include <RTpieCpp/Data/Float3.hpp>

#define IMPL_INTERFACE RTfact::RTpie::IPickTask
WRAPPER_START(PickTaskHandle, BaseHandle)

    // -----------------------------------------------------
    // Configure PickTask
    // -----------------------------------------------------

    void setCamera(CameraHandle aCamera)
    IMPLEMENT_WRAPPER_NO_RETURN( SetCamera(aCamera.get()) )

    CameraHandle getCamera()
    IMPLEMENT_WRAPPER_ARG0(RTfact::RTpie::ICamera*, GetCamera)

    void pick(const int aX, const int aY, const int aResX, const int aResY)
    IMPLEMENT_WRAPPER_NO_RETURN( Pick(aX, aY, aResX, aResY) )

    // -----------------------------------------------------
    // Results of PickTask
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



#endif // RTFACT_HANDLE_PICKTASK_HPP
