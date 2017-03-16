/*
 *  TLRenderTask.cpp
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

 #define RTFACT_RTPIE_LIB
#include <boost/intrusive_ptr.hpp>

#include <TwoLevelTracer/TLRenderTask.hpp>

#include <RTfact/Utils/FPSMeter.hpp>
#include <boost/pointer_cast.hpp>
#include <boost/cast.hpp>




//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Implementation
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

RTFACT_RTPIE_NAMESPACE_BEGIN

//RTFACT_RTPIE_QI_START(TLRenderTask)
HRESULT TLRenderTask ::QueryInterface(REFIID aId, void **_retval) \
  {                                                          \
      *_retval = 0;                                          \
      if(aId == IUnknown::GetIID())                          \
          *_retval = static_cast<IUnknown*>(static_cast<IRenderTask*>(this));
RTFACT_RTPIE_QI_ADD(IRenderTask)
RTFACT_RTPIE_QI_ADD(IParameterizable)
RTFACT_RTPIE_QI_END


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

TLRenderTask::TLRenderTask(TwoLevelTracer *aTracer) :
    TLTask(aTracer),
    mIntegrator(0),
    mAccumulatePixels(false),
    mGlobalIllimination(false),
    mClip(0, 0, RTFACT_RTPIE__DEFAULT_RESX,RTFACT_RTPIE__DEFAULT_RESY),
    mReset(false)
{
    mParticleGenerator.setLightPathCount(30);
    mParticleGenerator.setMaxPathLength(10);
    mParticleGenerator.setClampThreshold(0.f);
    mParticleGenerator.setClampFactor(0.2f);
    mParticleGenerator.setClampStartValue(0.05f);
    mParticleFilter.setImportonCount(100);
    mParticleFilter.setGBias(0.f);
    mParticleFilter.setAcceptanceEpsilon(0.08f);

    granularity = 8;
    mStillCameraIterations = 0;
    sceneChanged();
}

TLRenderTask::~TLRenderTask()
{
    if(mIntegrator){
        delete mIntegrator;
    }
}

void TLRenderTask::sceneChanged(){
    if(mIntegrator){
        delete mIntegrator;
    }
    mStillCameraIterations = 0;
    mIntegrator = new t_Integrator(
        t_Context(mScene->scene, mTracer->tree, mIntersector,
                  mParticleGenerator, mParticleFilter));
}

//-----------------------------------------------------------------------------
// IRenderer Implementation
//-----------------------------------------------------------------------------

HRESULT TLRenderTask::SetFrameBuffer(IFrameBuffer *aBuffer)
{
    mBuffer = static_cast<FrameBuffer*>(aBuffer);
    return S_OK;
}

HRESULT TLRenderTask::GetFrameBuffer(IFrameBuffer **_retval)
{
    *_retval = mBuffer.get();
    mBuffer->AddRef();
    return S_OK;
}

HRESULT TLRenderTask::SetCamera(ICamera *aCamera)
{
    mCamera = static_cast<Camera*>(aCamera);
    mStillCameraIterations = 0;
    return S_OK;
}

HRESULT TLRenderTask::GetCamera(ICamera **_retval)
{
    *_retval = mCamera.get();
    mCamera->AddRef();
    return S_OK;
}

HRESULT TLRenderTask::GetOverSamplingRate(unsigned int *_retval){
    *_retval = mPixelsampler.getOverSamplingRate();
    return S_OK;
}

HRESULT TLRenderTask::SetOverSamplingRate(unsigned int rate)
{
    mPixelsampler.setOverSamplingRate(rate);
    return S_OK;
}

HRESULT TLRenderTask::SetAccumulatePixels(bool active)
{
    mAccumulatePixels = active;
    return S_OK;
}

HRESULT TLRenderTask::GetAccumulatePixels(bool *_retval)
{
    *_retval = mAccumulatePixels;
    return S_OK;
}

HRESULT TLRenderTask::SetGlobalIllumination(bool active)
{
    mGlobalIllimination = active;
    return S_OK;
}

HRESULT TLRenderTask::GetGlobalIllumination(bool *_retval)
{
    *_retval = mGlobalIllimination;
    return S_OK;
}

HRESULT TLRenderTask::SetMaxDepth(unsigned int maxDepth)
{
    mRenderer.setMaxDepth(maxDepth);
    return S_OK;
}

HRESULT TLRenderTask::GetMaxDepth(unsigned int *_retval)
{
    *_retval = mRenderer.getMaxDepth();
    return S_OK;
}

HRESULT TLRenderTask::Run()
{
    if(!preRun())
        return E_FAIL;

    if(!mCamera || !mBuffer)
        return E_FAIL;

    bool cameraChanged = false;
    if(mCameraVersion != mCamera->getVersion() ||
       mSceneVersion != mScene->getVersion())
    {
        mCameraVersion = mCamera->getVersion();
        mSceneVersion = mScene->getVersion();
        cameraChanged = true;
    }

    if(mAccumulatePixels && !cameraChanged && !mReset)
    {
        ++mStillCameraIterations;

        mPixelsampler.setSampleSeed(
            Math::haltonBase(mStillCameraIterations, 2),
            Math::haltonBase(mStillCameraIterations, 3),
            1.f / mStillCameraIterations);
    }
    else
    {
        mStillCameraIterations = 0;
        mProgress = 0;
        mReset = false;

        mPixelsampler.setSampleSeed(0.5f, 0.5f, 1.0f);
    }

    t_Camera camera =
            mCamera->getRTfactCamera(mBuffer->getWidth(), mBuffer->getHeight());

    mClip.top = 0;
    mClip.left = 0;
    mClip.right = mBuffer->getWidth();
    mClip.bottom = mBuffer->getHeight();

    if(mBuffer->hasByteRenderBuffer())
    {
        t_RTpieByteFramebuffer buffer;
        mBuffer->getRTfactByteRenderBuffer(buffer);

        renderCore(camera, buffer);
    }
    else
    {
        t_RTpieFloatFramebuffer buffer;
        mBuffer->getRTfactFloatRenderBuffer(buffer);

        renderCore(camera, buffer);
    }
    return S_OK;
}

bool TLRenderTask::setParam(const std::string &name, const ParamValue &value)
{
    if(name == RTFACT_RTPIE__TL_RENDER_TASK_MAX_DEPTH)
    {
        const unsigned int *u = boost::get<unsigned int>(&value);
        if(u)
            mRenderer.setMaxDepth(*u);
    }
    else if(name == RTFACT_RTPIE__TL_RENDER_TASK_GLOBAL_ILLUMINATION)
    {
        const bool *b = boost::get<bool>(&value);
        if(b)
            mGlobalIllimination = *b;
    }
    else if(name == RTFACT_RTPIE__TL_RENDER_TASK_ACCUMULATE_PIXELS)
    {
        const bool *b = boost::get<bool>(&value);

        if(b)
            mAccumulatePixels = *b;
    }
    else if(name == RTFACT_RTPIE__TL_RENDER_TASK_OVERSAMPLING_RATE)
    {
        const unsigned int *u = boost::get<unsigned int>(&value);
        if(u)
            mPixelsampler.setOverSamplingRate(*u);
    }
    else if(name == RTFACT_RTPIE__TL_RENDER_TASK_PARTICLE_MAX_DEPTH)
    {
        const unsigned int *u = boost::get<unsigned int>(&value);
        if(u)
          mParticleGenerator.setMaxPathLength(*u);
    }
    else if(name == RTFACT_RTPIE__TL_RENDER_TASK_LIGHT_PATH_COUNT)
    {
        const unsigned int *u = boost::get<unsigned int>(&value);
        if(u)
          mParticleGenerator.setLightPathCount(*u);
    }
    else if(name == RTFACT_RTPIE__TL_RENDER_TASK_CLAMP_THRESHOLD)
    {
        const float *f = boost::get<float>(&value);
        if(f)
          mParticleGenerator.setClampThreshold(*f);
    }
    else if(name == RTFACT_RTPIE__TL_RENDER_TASK_CLAMP_FACTOR)
    {
        const float *f = boost::get<float>(&value);
        if(f)
          mParticleGenerator.setClampFactor(*f);
    }
    else if(name == RTFACT_RTPIE__TL_RENDER_TASK_CLAMP_START_VALUE)
    {
        const float *f = boost::get<float>(&value);
        if(f)
          mParticleGenerator.setClampStartValue(*f);
    }
    else if(name == RTFACT_RTPIE__TL_RENDER_TASK_IMPORTON_COUNT)
    {
        const unsigned int *u = boost::get<unsigned int>(&value);
        if(u)
          mParticleFilter.setImportonCount(*u);
    }
    else if(name == RTFACT_RTPIE__TL_RENDER_TASK_GBIAS)
    {
        const float *f = boost::get<float>(&value);
        if(f)
          mParticleFilter.setGBias(*f);
    }
    else if(name == RTFACT_RTPIE__TL_RENDER_TASK_ACCEPTANCE_EPSILON)
    {
        const float *f = boost::get<float>(&value);
        if(f)
          mParticleFilter.setAcceptanceEpsilon(*f);
    }
    else if(name == RTFACT_RTPIE__TL_RENDER_TASK_RESET)
    {
        const bool *b = boost::get<bool>(&value);

        if(b)
            mReset = *b;
    }
    else
    {
        return false;
    }

    return true;
}

bool TLRenderTask::getParam(const std::string &name, ParamValue &value)
{
    if(name == RTFACT_RTPIE__TL_RENDER_TASK_TOTAL_PARTICLE_COUNT)
    {
        value = (uint)mParticleGenerator.getTotalParticleCount();
    }
    else if(name == RTFACT_RTPIE__TL_RENDER_TASK_PROGRESS)
    {
        value = (float)mProgress;
    }
    else if(name == RTFACT_RTPIE__TL_RENDER_TASK_ITERATIONS)
    {
        value = (int)mStillCameraIterations;
    }
    else if(name == RTFACT_RTPIE__TL_RENDER_TASK_RESET)
    {
        value = (bool)mReset;
    }
    else
    {
        return false;
    }

    return true;
}

bool TLRenderTask::hasParam(const std::string &name)
{
    return
        name == RTFACT_RTPIE__TL_RENDER_TASK_MAX_DEPTH ||
        name == RTFACT_RTPIE__TL_RENDER_TASK_GLOBAL_ILLUMINATION ||
        name == RTFACT_RTPIE__TL_RENDER_TASK_ACCUMULATE_PIXELS ||
        name == RTFACT_RTPIE__TL_RENDER_TASK_OVERSAMPLING_RATE ||
        name == RTFACT_RTPIE__TL_RENDER_TASK_PARTICLE_MAX_DEPTH ||
        name == RTFACT_RTPIE__TL_RENDER_TASK_LIGHT_PATH_COUNT ||
        name == RTFACT_RTPIE__TL_RENDER_TASK_CLAMP_THRESHOLD ||
        name == RTFACT_RTPIE__TL_RENDER_TASK_CLAMP_FACTOR ||
        name == RTFACT_RTPIE__TL_RENDER_TASK_CLAMP_START_VALUE ||
        name == RTFACT_RTPIE__TL_RENDER_TASK_IMPORTON_COUNT ||
        name == RTFACT_RTPIE__TL_RENDER_TASK_GBIAS ||
        name == RTFACT_RTPIE__TL_RENDER_TASK_ACCEPTANCE_EPSILON ||
        name == RTFACT_RTPIE__TL_RENDER_TASK_RESET ||
        name == RTFACT_RTPIE__TL_RENDER_TASK_ITERATIONS;

}

//-----------------------------------------------------------------------------

RTFACT_RTPIE_NAMESPACE_END
