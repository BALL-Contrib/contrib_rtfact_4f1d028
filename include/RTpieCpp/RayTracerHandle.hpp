/*
 *  RayTracerHandle.hpp
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
 *  Created on: 2010-09-26 00:35:31 +0200
 *  Author(s): Felix Klein
 */

#ifndef RTFACT_HANDLE_RAYTRACER_HPP
#define RTFACT_HANDLE_RAYTRACER_HPP

#include <RTpieCpp/Config/Common.hpp>

#include <RTpie/IRayTracer.hpp>

#define IMPL_INTERFACE RTfact::RTpie::IRayTracer
WRAPPER_FWD(RayTracerHandle)
#undef IMPL_INTERFACE

#include <RTpieCpp/Base/BaseHandle.hpp>
#include <RTpieCpp/Scene/SceneHandle.hpp>
#include <RTpieCpp/Tasks/RenderTaskHandle.hpp>
#include <RTpieCpp/Tasks/PickTaskHandle.hpp>
#include <RTpieCpp/Tasks/RayGenerationTaskHandle.hpp>
#include <RTpieCpp/Tasks/RayPickTaskHandle.hpp>
#include <RTpieCpp/Tasks/IntersectTaskHandle.hpp>

#define IMPL_INTERFACE RTfact::RTpie::IRayTracer
WRAPPER_START(RayTracerHandle, BaseHandle)

    void attachScene(SceneHandle aScene)
    IMPLEMENT_WRAPPER_NO_RETURN( AttachScene(aScene.get()) )

    void syncStructures()
    IMPLEMENT_WRAPPER_NO_RETURN( SyncStructures() )

    RenderTaskHandle createRenderTask()
    IMPLEMENT_WRAPPER_ARG0( RTfact::RTpie::IRenderTask* , CreateRenderTask )

    PickTaskHandle createPickTask()
    IMPLEMENT_WRAPPER_ARG0( RTfact::RTpie::IPickTask* , CreatePickTask )

    RayGenerationTaskHandle createRayGenerationTask()
    IMPLEMENT_WRAPPER_ARG0( RTfact::RTpie::IRayGenerationTask* , CreateRayGenerationTask )


    RayPickTaskHandle createRayPickTask()
    IMPLEMENT_WRAPPER_ARG0( RTfact::RTpie::IRayPickTask* , CreateRayPickTask )

    IntersectTaskHandle createIntersectTask()
    IMPLEMENT_WRAPPER_ARG0( RTfact::RTpie::IIntersectTask* , CreateIntersectTask )

WRAPPER_END
#undef IMPL_INTERFACE


RTFACT_HANDLE_NAMESPACE_BEGIN

inline RayTracerHandle CreateRayTracerHandle()
{
    RTfact::RTpie::IRayTracer *result;
    RTfact::RTpie::CreateRayTracer(&result);
    return result;
}

RTFACT_HANDLE_NAMESPACE_END



#endif // RTFACT_HANDLE_RENDERER_HPP
