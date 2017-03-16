/*
 *  TLRenderTask.hpp
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

 #ifndef RTFACT_RTPIE__TLRENDERTASK_HPP
#define RTFACT_RTPIE__TLRENDERTASK_HPP

#include "Config/InternalCommon.hpp"


#include <RTpie/Base/IParameterizable.hpp>
#include <RTpie/Tasks/IRenderTask.hpp>

RTFACT_RTPIE_FORWARD(TLRenderTask)

#include "TwoLevelTracer/TLTask.hpp"
#include "Data/Camera.hpp"
#include "Data/FrameBuffer.hpp"


#define RTFACT_RTPIE__DEFAULT_RESX 512
#define RTFACT_RTPIE__DEFAULT_RESY 512

#define RTFACT_RTPIE__TL_RENDER_TASK_MAX_DEPTH            "max_depth"
#define RTFACT_RTPIE__TL_RENDER_TASK_GLOBAL_ILLUMINATION  "global_illumination"
#define RTFACT_RTPIE__TL_RENDER_TASK_ACCUMULATE_PIXELS    "accumulate_pixels"
#define RTFACT_RTPIE__TL_RENDER_TASK_OVERSAMPLING_RATE    "oversampling_rate"

#define RTFACT_RTPIE__TL_RENDER_TASK_LIGHT_PATH_COUNT     "light_path_count"
#define RTFACT_RTPIE__TL_RENDER_TASK_PARTICLE_MAX_DEPTH   "max_particle_depth"
#define RTFACT_RTPIE__TL_RENDER_TASK_CLAMP_THRESHOLD      "clamp_threshold"
#define RTFACT_RTPIE__TL_RENDER_TASK_CLAMP_FACTOR         "clamp_factor"
#define RTFACT_RTPIE__TL_RENDER_TASK_CLAMP_START_VALUE    "clamp_start_value"
#define RTFACT_RTPIE__TL_RENDER_TASK_IMPORTON_COUNT       "importon_count"
#define RTFACT_RTPIE__TL_RENDER_TASK_GBIAS                "gbias"
#define RTFACT_RTPIE__TL_RENDER_TASK_ACCEPTANCE_EPSILON   "acceptance_epsilon"

#define RTFACT_RTPIE__TL_RENDER_TASK_TOTAL_PARTICLE_COUNT "total_particle_count"
#define RTFACT_RTPIE__TL_RENDER_TASK_PROGRESS             "progress"
#define RTFACT_RTPIE__TL_RENDER_TASK_RESET                "reset"
#define RTFACT_RTPIE__TL_RENDER_TASK_ITERATIONS           "iterations"

#define RTFACT_RTPIE__FRAME_ILLUM_ARRAY 10

RTFACT_RTPIE_NAMESPACE_BEGIN

class TLRenderTask :
    public Parameterizable, public TLTask, public IRenderTask, public IParameterizable
{

public:

    // IBase

    RTFACT_RTPIE_DECLARE_QI

    RTFACT_RTPIE_FORWARD_BASE

    // IParameterizable

    RTFACT_RTPIE_FORWARD_PARAMETERIZABLE

    virtual HRESULT BeginParams(){ return S_OK;}

    virtual HRESULT EndParams(){ return S_OK;}

    // IRenderTask

    virtual HRESULT SetFrameBuffer(IFrameBuffer *aBuffer);

    virtual HRESULT GetFrameBuffer(IFrameBuffer **_retval);

    virtual HRESULT SetCamera(ICamera *aCamera);

    virtual HRESULT GetCamera(ICamera **_retval);

    virtual HRESULT SetOverSamplingRate(unsigned int rate);

    virtual HRESULT GetOverSamplingRate(unsigned int *_retval);

    virtual HRESULT SetAccumulatePixels(bool active);

    virtual HRESULT GetAccumulatePixels(bool *_retval);

    virtual HRESULT SetGlobalIllumination(bool active);

    virtual HRESULT GetGlobalIllumination(bool *_retval);

    virtual HRESULT SetMaxDepth(unsigned int maxDepth);

    virtual HRESULT GetMaxDepth(unsigned int *_retval);

    virtual HRESULT Run();

    // Internal

    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    // Constructor.
    TLRenderTask(TwoLevelTracer *aTracer);

    // Parameterizable implementation

    virtual bool setParam(
        const std::string &name,
        const ParamValue &value);

    virtual bool getParam(
        const std::string &name,
        ParamValue &value);

    virtual bool hasParam(
        const std::string &name);

    // Destructor
    virtual ~TLRenderTask();

    // TLTask:

protected:

    virtual void sceneChanged();


private:

    void performRendering();

private:

    //---------------------------------------------------------------------
    // API Data
    //---------------------------------------------------------------------

    CameraPtr mCamera;
    FrameBufferPtr mBuffer;

    // Version checkers
    unsigned int mSceneVersion, mCameraVersion;

    //---------------------------------------------------------------------
    // RTfact Data
    //---------------------------------------------------------------------

    t_ParticleGenerator mParticleGenerator;
    t_ParticleFilter mParticleFilter;
    t_Intersector mIntersector;
    t_Integrator *mIntegrator;
    t_PixelSampler mPixelsampler;
    t_Renderer mRenderer;
    ImageClipRegion mClip;

    // Parallel Rendering
    int granularity;

    // Progressive rendering
    int mStillCameraIterations;
    bool mAccumulatePixels;
    bool mGlobalIllimination;
    float mTotalFrameIntensity[RTFACT_RTPIE__FRAME_ILLUM_ARRAY];
    float mMinimumTotalFrameIntensity;
    float mProgress;
    bool mReset;

#ifdef USE_TBB

    tbb::task_scheduler_init init;

    template<class tFrameBuffer>
    struct RenderParallel
    {
        t_Renderer& rend;
        t_Camera& cam;
        t_Scene& sc;
        t_Tree& tree;
        t_Intersector& intersector;
        t_PixelSampler& pixelSampler;
        t_Integrator& integrator;
        tFrameBuffer& wind;

    public:

        void operator()(
            const tbb::blocked_range2d<RTfact::uint> &r) const
        {
            ImageClipRegion clip;
            clip.left = r.cols().begin();
            clip.right = r.cols().end();
            clip.top = r.rows().begin();
            clip.bottom = r.rows().end();

            rend.render<PRIMARY_RAY_PACKET_SIZE>(
                cam, sc, tree, intersector, pixelSampler, integrator, wind, clip);
        }

        RenderParallel(
            t_PixelSampler& apixelSampler,
            t_Integrator& aintegrator,
            t_Renderer& arend,
            t_Camera& acam,
            t_Scene& asc,
            t_Tree& atree,
            t_Intersector& atrav,
            tFrameBuffer& awind
        ) :
            rend(arend),
            cam(acam),
            sc(asc),
            tree(atree),
            intersector(atrav),
            pixelSampler(apixelSampler),
            integrator(aintegrator),
            wind(awind)
        {}
    };

#endif // USE_TBB

    template<class tFramebuffer>
    void renderCore(t_Camera &camera, tFramebuffer &buffer)
    {
        buffer.prePaint();

        //
        t_Context context(mScene->scene, mTracer->tree, mIntersector,
                          mParticleGenerator, mParticleFilter);
        if(mGlobalIllimination)
        {
            mParticleFilter.startFrame(camera, context, mStillCameraIterations);
            mParticleGenerator.generateParticles(camera, context, mStillCameraIterations);
        }
    #ifdef USE_TBB
        tbb::parallel_for(tbb::blocked_range2d<RTfact::uint>(
            mClip.top, mClip.bottom, granularity, mClip.left, mClip.right, granularity),
            RenderParallel<tFramebuffer>(mPixelsampler, *mIntegrator, mRenderer,
                camera, mScene->scene, mTracer->tree, mIntersector,
                buffer));

    #else
        mRenderer.render<PRIMARY_RAY_PACKET_SIZE>(
            camera, mScene->scene, mTracer->tree, mIntersector,
            mPixelsampler, *mIntegrator, buffer, mClip);

    #endif

        //
        if(mGlobalIllimination)
        {
            mParticleGenerator.releaseParticles();
            mParticleFilter.endFrame(camera, context, mStillCameraIterations);

            // convergence criterion

            // total frame intensity
            typename tFramebuffer::ColorImagePtr img = buffer.getColorImage();
            typename tFramebuffer::ColorImage::Component* f = img->getFirstComponent(0, 0);
            const uint bufferSize = (img->getPitch() * img->getResY()) / sizeof(float);

            typename tFramebuffer::ColorImage::Component sum = 0;
            for(uint i = 0; i < bufferSize; ++i)
              sum += f[i];

            // save direct illumination
            if(mStillCameraIterations == 0 || mMinimumTotalFrameIntensity > sum)
            {
                mMinimumTotalFrameIntensity = sum;
            }

            mTotalFrameIntensity[mStillCameraIterations%RTFACT_RTPIE__FRAME_ILLUM_ARRAY] = sum - mMinimumTotalFrameIntensity;

            if(mStillCameraIterations >= RTFACT_RTPIE__FRAME_ILLUM_ARRAY)
            {
                double mean = 0;
                for(int i = 0; i < RTFACT_RTPIE__FRAME_ILLUM_ARRAY; i++)
                  mean += mTotalFrameIntensity[i];
                mean /= RTFACT_RTPIE__FRAME_ILLUM_ARRAY;

                double var = 0;
                for(int i = 0; i < RTFACT_RTPIE__FRAME_ILLUM_ARRAY; i++)
                  var += (mTotalFrameIntensity[i] - mean) * (mTotalFrameIntensity[i] - mean);
                var /= RTFACT_RTPIE__FRAME_ILLUM_ARRAY;

                mProgress = (float)(1.0 - std::sqrt(var) / mean);
            }
        }

        buffer.postPaint();
    }

};

RTFACT_RTPIE_NAMESPACE_END

#endif // RTFACT_RTPIE__RENDERER_HPP

