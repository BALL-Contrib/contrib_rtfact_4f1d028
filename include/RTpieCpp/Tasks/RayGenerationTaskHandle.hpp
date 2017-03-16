#ifndef RTFACT_HANDLE_RAYGENERATIONTASK_HPP
#define RTFACT_HANDLE_RAYGENERATIONTASK_HPP

#include <RTpieCpp/Config/Common.hpp>

#include <RTpie/Tasks/IRayGenerationTask.hpp>

#define IMPL_INTERFACE RTfact::RTpie::IRayGenerationTask
WRAPPER_FWD(RayGenerationTaskHandle)
#undef IMPL_INTERFACE

#include <RTpieCpp/Base/BaseHandle.hpp>
#include <RTpieCpp/Scene/SceneContentHandles.hpp>
#include <RTpieCpp/Data/CameraHandle.hpp>

#define IMPL_INTERFACE RTfact::RTpie::IRayGenerationTask
WRAPPER_START(RayGenerationTaskHandle, BaseHandle)

    // -----------------------------------------------------
    // Configure RayGenerationTask
    // -----------------------------------------------------

    void setCamera(CameraHandle aCamera)
    IMPLEMENT_WRAPPER_NO_RETURN( SetCamera(aCamera.get()) )

    CameraHandle getCamera()
    IMPLEMENT_WRAPPER_ARG0(RTfact::RTpie::ICamera*, GetCamera)

    void generateRay(const int aX, const int aY, const int aResX, const int aResY)
    IMPLEMENT_WRAPPER_NO_RETURN( GenerateRay(aX, aY, aResX, aResY) )

    // -----------------------------------------------------
    // Results of RayGenerationTask
    // -----------------------------------------------------

    bool hasResult()
    IMPLEMENT_WRAPPER_ARG0(bool, HasResult)

    float3 getRayOrigin()
    IMPLEMENT_WRAPPER_FLOAT3(GetRayOrigin)

    float3 getRayDirection()
    IMPLEMENT_WRAPPER_FLOAT3(GetRayDirection)


WRAPPER_END
#undef IMPL_INTERFACE



#endif // RTFACT_HANDLE_RAYGENERATIONTASK_HPP
