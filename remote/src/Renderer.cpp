/*
 *  Renderer.cpp
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
 *  Created on: 2008-09-08 14:39:03 +0200
 *  Author(s): Felix Klein, Iliyan Georgiev, Lukas Marsalek, Ken Patrick Dahm, Dmitri Rubinstein, Petr Kmoch, Ken Dahm, Mike Phillips
 */

 #include "Precompiled.hpp"

#define RTFACT_REMOTE_LIB

//-----------------------------------------------------------------------------
// RTfact Includes
//-----------------------------------------------------------------------------

#include <RTfact/Config/Init.hpp>

#include <RTfactRemoteConfig.hpp>
#include <RTfact/Config/Common.hpp>
#include <RTfact/Utils/FPSMeter.hpp>
#include <RTfact/Utils/IO/Configuration.hpp>
#include <RTfact/Utils/EnvLightGen.hpp>

#include <RTremote/Renderer.hpp>

#include "RTremotePrivateConfig.hpp"



#include <boost/pointer_cast.hpp>
#include <boost/cast.hpp>
//#include <RTremote/RTLambMaterial.hpp>

//-----------------------------------------------------------------------------
// RTremote Display Includes
//-----------------------------------------------------------------------------

#ifdef RTFACT_REMOTE_HAVE_ANYSL
#define RTANYSL_LIB
#include "RTAnySLShader.hpp"
#include <RTAnySL/LibraryInit.hpp>
#endif

#include "RTPhongShader.hpp"
#include "RTTestShader.hpp"
#include "RTPlasticShader.hpp"
#include "RTNormalShader.hpp"
#include "RTBasicPhongShader.hpp"
#include "RTGlassShader.hpp"
#include "RTPointLight.hpp"
#include "RTSpotLight.hpp"
#include "RTDirectionalLight.hpp"
#include "RTIESLight.hpp"
#include "RTIESAreaLight.hpp"
#include "RTQuadLight.hpp"
#include "RTTriangleLight.hpp"
#include "RTImageImpl.hpp"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Implementation
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

RTFACT_REMOTE_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------
// Static inits
//-----------------------------------------------------------------------------

const char* Renderer::StartupParams::PARAM_NAME__NUMBER_OF_RENDER_THREADS = "numRenderThreads";
const char* Renderer::StartupParams::PARAM_NAME__GRANULARITY = "granularity";


//---------------------------------------------------------------------

void Renderer::paramTestFunction(const ParamValue &p)
{
    std::cout << p.which();
}

void Renderer::paramTestFunction2(const float3 &p)
{
    std::cout << p.x;
}



//-----------------------------------------------------------------------------
// RendererPimpl
//-----------------------------------------------------------------------------

RendererPimpl::RendererPimpl(const char* aConfigurationString) :
    #ifdef CLASSIC_SHADERS
        context(scene, tree, intersector, particleGenerator, particleFilter),
        integrator(context),
		aoIntegrator(context, mAmbientRaySampler),
    #endif
    clip(0, 0, RTFACT_REMOTE__DEFAULT_RESX,RTFACT_REMOTE__DEFAULT_RESY),
    mRenderThreads(Renderer::StartupParams::Values::PARAM_AUTOMATIC),
    mGranularity(Renderer::StartupParams::Values::PARAM_AUTOMATIC)
{
    particleGenerator.setLightPathCount(30);
    particleGenerator.setMaxPathLength(10);
    particleGenerator.setClampThreshold(0.f);
    particleGenerator.setClampFactor(0.2f);
    particleGenerator.setClampStartValue(0.05f);
    particleFilter.setImportonCount(100);
    particleFilter.setGBias(0.f);
    particleFilter.setAcceptanceEpsilon(0.08f);
    //particleFilter.setAcceptanceEpsilon(1.08f);

    //Load initial configuration, if any
    if(aConfigurationString != NULL)
    {
        std::stringstream configStream(aConfigurationString);
        if(configStream.good())
        {
            IO::Configuration startupConfig;
            startupConfig.loadFromStream(configStream);
            loadStartupConfiguration(startupConfig);
        }
    }

    //Initialize TBB TaskScheduler
#if defined(USE_TBB)
    mTbbSchedulerInit = tTbbSchedulerInitPtr(new tTbbSchedulerInitPtr::element_type(mRenderThreads));
    if(mGranularity == Renderer::StartupParams::Values::PARAM_AUTOMATIC)
    {
        mTbbAutomaticPartitioner = tTbbAutomaticPartitionerPtr(new tTbbAutomaticPartitionerPtr::element_type());
    }
    else
    {
        int pSizeSqrt = Packet<PRIMARY_RAY_PACKET_SIZE, float>::SIZE_SQRT;
        if(mGranularity < pSizeSqrt)
        {
            mGranularity = pSizeSqrt;
            RTFACT_LOG_WARNING_F("Adjusted task granularity to at least primary packet size (%1%x%1%)", pSizeSqrt);
        }
        mTbbManualPartitioner = tTbbManualPartitionerPtr(new tTbbManualPartitionerPtr::element_type());
    }
#else
    //to notify the user that TBB is switched off
    mRenderThreads = 1;
#endif

    o_params.position     = Vec3f<1>(-2.0, 2.0, 0);
    o_params.direction    = Vec3f<1>(1.0, -0.5, 0).normalize();
    o_params.up           = Vec3f<1>(0, 1, 0).normalize();
    o_params.openingAngle = 60;
    o_params.resX         = RTFACT_REMOTE__DEFAULT_RESX;
    o_params.resY         = RTFACT_REMOTE__DEFAULT_RESY;
    camera.setParams(o_params);

    camera_offset_dir= Vec3f<1>(0,0,0);
    camera_offset_rot= RTfact::Transform::identity();

    // Init Environment Map
    Traits<t_EnvImage>::Ptr img = new t_EnvImage;
    float color[6];
    color[0]=color[1]=color[2]=255;
    color[3]=color[4]=color[5]=255;
    img->init(color, 3, 2, 1);
    //integrator.getEnvironmentTexture().setImage(img);

	// Create Root Group
	t_InternGroupHandle root =
        new t_InternGroup( &scene, RTfact::Transform::identity() );
	scene.setRoot(root);

	camera_group = root;

#ifdef RTFACT_REMOTE_HAVE_ANYSL
    // init AnySL system
    RTAnySL::LibraryInit::initialize();
#endif

	// Init sampler
	mAmbientRaySampler.generateMultiJittered();
	mAmbientRaySampler.mapSamplesToHemisphere(RTFACT_AO_COS_POW_DIST);
	mUseOldSamples = true;

    printConfig();
}

RendererPimpl::~RendererPimpl()
{
#ifdef RTFACT_REMOTE_HAVE_ANYSL
    // shutdown AnySL system
    std::cout<<"Shutdown AnySL"<<std::endl;
    RTAnySL::LibraryInit::shutdown();
#endif
}

//---------------------------------------------------------------------
void RendererPimpl::loadStartupConfiguration(IO::Configuration& aConfig)
{
#if defined(USE_TBB)
    if(aConfig.exists(Renderer::StartupParams::PARAM_NAME__NUMBER_OF_RENDER_THREADS))
    {
        mRenderThreads = aConfig[Renderer::StartupParams::PARAM_NAME__NUMBER_OF_RENDER_THREADS].as<int32>();
    }

    if(aConfig.exists(Renderer::StartupParams::PARAM_NAME__GRANULARITY))
    {
        mGranularity = aConfig[Renderer::StartupParams::PARAM_NAME__GRANULARITY].as<int32>();
    }
#endif
}

//---------------------------------------------------------------------
void RendererPimpl::printConfig()
{
    RTFACT_LOG_LINE("===== RTremote Startup Configuration ======");
    if(mRenderThreads == Renderer::StartupParams::Values::PARAM_AUTOMATIC)
    {
        RTFACT_LOG_LINE("   Using as many as available render threads");
    }
    else
    {
        RTFACT_LOG_LINE_F("   Using %1% render thread(s)", mRenderThreads);
    }

    if(mGranularity == Renderer::StartupParams::Values::PARAM_AUTOMATIC)
    {
        RTFACT_LOG_LINE("   Using automatic task granularity");
    }
    else
    {
        RTFACT_LOG_LINE_F("   Using %1%x%1% task granularity", mGranularity);
    }
}

//---------------------------------------------------------------------

void RendererPimpl::performBuild()
{
    #ifdef USE_FLAT_STRUCTURES
        sconf <<"\n||";
        time.reset(); time.start();
        RTFACT_LOG_LINE_F("Number of primitives: %1%", scene.mFlattenPrimitives.size());
        builder.build(tree, scene.mFlattenPrimitives.begin(),
            scene.mFlattenPrimitives.end(),
            boost::lambda::bind(&t_Primitive::getBounds,
                             boost::lambda::_1));
        time.stop();
        RTFACT_LOG_LINE_F("BVH build time: %1%s", (time.getTotalTime() / 1000.));
    #else
		synchronizer.build(scene, builddata);
        time.reset(); time.start();
        builder.build(tree, scene, builddata);
        time.stop();

    #endif
}

//---------------------------------------------------------------------

void RendererPimpl::performRendering(const uint stillCameraIteration)
{
    // reset timers for detailed evaluation
    #ifndef USE_FLAT_STRUCTURES
        intersector.intersectTime->reset();
        intersector.partATime->reset();
        intersector.partBTime->reset();
        intersector.partCTime->reset();
    #endif

        time.reset();
        bool genParticles = (mRenderMode == Renderer::RENDERMODE_PROGRESSIVE_GI);
        if(genParticles)
        {
            particleFilter.startFrame(camera, context, stillCameraIteration);
            particleGenerator.generateParticles(camera, context, stillCameraIteration);
        }
        //else
        //{
        //    particleFilter.importonStorage.clear();
        //    particleFilter.stats.clear();
        //    particleGenerator.particleStorage.clear();
        //    particleGenerator.statistics.clear();
        //}

        // Start Rendering
        time.start();
    #ifdef USE_TBB

        if(mGranularity == Renderer::StartupParams::Values::PARAM_AUTOMATIC)
        {
            tbb::parallel_for(tbb::blocked_range2d<RTfact::uint>(
                clip.top, clip.bottom, clip.left, clip.right),
                RenderParallel(pixelsampler, integrator, aoIntegrator, renderer, camera, scene, tree, intersector,
			    buffer, mRenderMode, mUseOldSamples), *mTbbAutomaticPartitioner);
        }
        else
        {
            tbb::parallel_for(tbb::blocked_range2d<RTfact::uint>(
                clip.top, clip.bottom, mGranularity, clip.left, clip.right, mGranularity),
                RenderParallel(pixelsampler, integrator, aoIntegrator, renderer, camera, scene, tree, intersector,
			    buffer, mRenderMode, mUseOldSamples), *mTbbManualPartitioner);
        }

    #else

        renderer.render<PRIMARY_RAY_PACKET_SIZE>(
            camera, scene, tree, intersector,
            pixelsampler, integrator, buffer, clip);

    #endif

            time.stop();

            if(genParticles)
            {
                particleGenerator.releaseParticles();
                particleFilter.endFrame(camera, context, stillCameraIteration);
            }

            // Evaluate
            updateAverageBuildTime(time.getTotalTime());
            stime <<" + (Render: " << averageBuildTime;

    #ifndef USE_FLAT_STRUCTURES

        #ifdef RTFACT_INSTANT_INTERSECTOR_DEBUG

            if(intersector.filterInstance)
                sconf <<" || I-Filter: " << intersector.filterInstance;
            sconf <<" || I-HitCount: " << intersector.instanceHitCount;
            intersector.instanceHitCount = 0;

        #endif

            //sconf <<" || #Instance: " << tree.getInstanceCount();
            //sconf <<" || #Primitive: " << tree.getPrimitiveCount();
            stime<<" (I-Conv:"<<
            (intersector.intersectTime->getTotalTime()*100/time.getTotalTime()) <<
                "%) )";

    #endif
}

//---------------------------------------------------------------------

void RendererPimpl::updateAverageBuildTime(double time)
{
    ++builded;
    averageBuildTime= ((builded-1)*averageBuildTime + time)/builded;
}

//---------------------------------------------------------------------

void RendererPimpl::setTestParameter(int id, float value)
{
    switch (id) {
        //List ids which expect int values here:
        case 1:
        case 2:
        case 3:
            setTestParameter(id,int(value));
            return;
    }
    //Work with ids which accept float values here:
}

void RendererPimpl::setTestParameter(int id, int value)
{
    #ifndef USE_FLAT_STRUCTURES

        if(id== 1)
        {
            averageBuildTime=0.0;
            builded=0;

            builder.setMaxExpand(value);
        }

        #ifdef RTFACT_INSTANT_INTERSECTOR_DEBUG

            else if(id == 2)
            {

                intersector.filterInstance= value;
            }

        #endif

    #endif
}

void RendererPimpl::setTime(float milliseconds)
{
    integrator.mContext.time = milliseconds;
}

//-----------------------------------------------------------------------------
// Renderer Implementation PUBLIC
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

Renderer::Renderer(const char* aStartupConfiguration) :
    pimpl(new RendererPimpl(aStartupConfiguration)),
    mStillCameraIterations(0),
    mProgressiveRefinement(false),
    acc_built(false)
{
    Float3ConstantTexture diffTex(1, 1, 1);
    Float3ConstantTexture ambTex(0.5, 0.5, 0.5);
    //pimpl->scene.appearances.push_back(
    //    new LambertianMaterial<Float3ConstantTexture, Float3ConstantTexture>(diffTex, ambTex));
    pimpl->averageBuildTime=0;
    pimpl->builded=0;
    pimpl->renderer.init(pimpl->scene, pimpl->tree, pimpl->intersector, pimpl->camera);

    RTAppearanceHandle mat = createAppearance("PhongShader");
    mat->setParam("diffuseColor", float3(1.0f, 1.0f, 1.0f));
    mat->setParam("ambientIntensity", float3(.7f, .7f, .0f));

    //addAppearance(lmat);
    defaultAppearance = mat;
}

Renderer::~Renderer()
{
    delete pimpl;
}

//-----------------------------------------------------------------------------
// Window & Framebuffer
//-----------------------------------------------------------------------------

void Renderer::attachFrameBuffer(
    float* data,
    RTfact::uint componentCount,
    RTfact::uint width,
    RTfact::uint height)
{
    pimpl->clip.top = 0;
    pimpl->clip.left = 0;
    pimpl->clip.right = width;
    pimpl->clip.bottom = height;
    pimpl->o_params.resX = width;
    pimpl->o_params.resY = height;

    //HACK: The conversion is unsafe!
    pimpl->buffer.setColorImage(
        new t_RTRemoteFrameBuffer(
            reinterpret_cast<t_FrameBufferComponent*>(data),
            componentCount, width, height, false));
}

void Renderer::attachFrameBuffer(
    RTfact::uint8* data,
    RTfact::uint componentCount,
    RTfact::uint width,
    RTfact::uint height,
		RTfact::uint lineWidth)
{
    pimpl->clip.top = 0;
    pimpl->clip.left = 0;
    pimpl->clip.right = width;
    pimpl->clip.bottom = height;
    pimpl->o_params.resX = width;
    pimpl->o_params.resY = height;

    //HACK: The conversion is unsafe!
    pimpl->buffer.setColorImage(
        new t_RTRemoteFrameBuffer(
            reinterpret_cast<t_FrameBufferComponent*>(data),
            componentCount, width, height, lineWidth, false));
}

void Renderer::attachFrameBuffer(
    float* data,
    RTfact::uint componentCount,
    RTfact::uint width,
    RTfact::uint height,
		RTfact::uint lineWidth)
{
    pimpl->clip.top = 0;
    pimpl->clip.left = 0;
    pimpl->clip.right = width;
    pimpl->clip.bottom = height;
    pimpl->o_params.resX = width;
    pimpl->o_params.resY = height;

    //HACK: The conversion is unsafe!
    pimpl->buffer.setColorImage(
        new t_RTRemoteFrameBuffer(
            reinterpret_cast<t_FrameBufferComponent*>(data),
            componentCount, width, height, lineWidth, false));
}

void Renderer::attachFrameBuffer(
    RTfact::uint8* data,
    RTfact::uint componentCount,
    RTfact::uint width,
    RTfact::uint height)
{
    pimpl->clip.top = 0;
    pimpl->clip.left = 0;
    pimpl->clip.right = width;
    pimpl->clip.bottom = height;
    pimpl->o_params.resX = width;
    pimpl->o_params.resY = height;

    //HACK: The conversion is unsafe!
    pimpl->buffer.setColorImage(
        new t_RTRemoteFrameBuffer(
            reinterpret_cast<t_FrameBufferComponent*>(data),
            componentCount, width, height, false));
}

void Renderer::attachFrameBuffer(
    float* data,
    RTfact::uint componentCount,
    RTfact::uint width,
    RTfact::uint height,
    ClipData& clip)
{
    pimpl->clip.top = clip.top;
    pimpl->clip.left = clip.left;
    pimpl->clip.right = clip.right;
    pimpl->clip.bottom = clip.bottom;
    //pimpl->o_params.resX = width;
    //pimpl->o_params.resY = height;

    //check whether framebuffer size matches clip region size
    if( (clip.bottom - clip.top) == height && (clip.right - clip.left) == width )
    {
	    //HACK: The conversion is unsafe!
	    pimpl->buffer.setColorImage(
		new t_RTRemoteFrameBuffer(
		    reinterpret_cast<t_FrameBufferComponent*>(data),
		    componentCount, width, height, false),
		ImageClipRegion(clip.left, clip.top, clip.right, clip.bottom));
    }
    else
    {
	    //HACK: The conversion is unsafe!
	    pimpl->buffer.setColorImage(
		new t_RTRemoteFrameBuffer(
		    reinterpret_cast<t_FrameBufferComponent*>(data),
		    componentCount, width, height, false));
    }
}

void Renderer::attachFrameBuffer(
    RTfact::uint8* data,
    RTfact::uint componentCount,
    RTfact::uint width,
    RTfact::uint height,
    ClipData& clip)
{
    pimpl->clip.top = clip.top;
    pimpl->clip.left = clip.left;
    pimpl->clip.right = clip.right;
    pimpl->clip.bottom = clip.bottom;
    //pimpl->o_params.resX = width;
    //pimpl->o_params.resY = height;

    //check whether framebuffer size matches clip region size
    if( (clip.bottom - clip.top) == height && (clip.right - clip.left) == width )
    {
	    //HACK: The conversion is unsafe!
	    pimpl->buffer.setColorImage(
		new t_RTRemoteFrameBuffer(
		    reinterpret_cast<t_FrameBufferComponent*>(data),
		    componentCount, width, height, false),
		ImageClipRegion(clip.left, clip.top, clip.right, clip.bottom));

    }
    else
    {
	    //HACK: The conversion is unsafe!
	    pimpl->buffer.setColorImage(
		new t_RTRemoteFrameBuffer(
		    reinterpret_cast<t_FrameBufferComponent*>(data),
		    componentCount, width, height, false));
    }
}

void Renderer::attachDistanceBuffer(
    float* data,
    RTfact::uint width,
    RTfact::uint height)
{
    pimpl->buffer.setDistanceImage(
        new t_RTRemoteDistanceBuffer(data, 1, width, height, false));
}


void Renderer::attachAmbientOcclusionBuffer(
    float* data,
    RTfact::uint width,
    RTfact::uint height)
{
	pimpl->buffer.setAmbientOcclusionImage(new t_RTRemoteAmbientOcclusionBuffer(data, 1, width, height, false));
}

void Renderer::attachNormalBuffer(
    float* data,
    RTfact::uint width,
    RTfact::uint height)
{
    pimpl->buffer.setNormalImage(
        new t_RTRemoteNormalBuffer(data, 3, width, height, false));
}

void Renderer::attachColorBleedingBuffer(
    float* data,
    RTfact::uint width,
    RTfact::uint height)
{
    pimpl->buffer.setColorBleedingImage(
        new t_RTRemoteColorBleedingBuffer(data, 3, width, height, false));
}

void Renderer::useProgressiveRefinement(
    bool state)
{
    mProgressiveRefinement = state;
    pimpl->mUseOldSamples = !mProgressiveRefinement;
}

bool Renderer::isProgressiveRefinementOn()
{
    return mProgressiveRefinement;
}

void Renderer::setOverSamplingRate(unsigned int rate)
{
    pimpl->pixelsampler.setOverSamplingRate(rate);
}

unsigned int Renderer::getOverSamplingRate() const
{
    return pimpl->pixelsampler.getOverSamplingRate();
}

void Renderer::setLightPathCount(const uint aCount)
{
    pimpl->particleGenerator.setLightPathCount(aCount);
}

uint Renderer::getLightPathCount()
{
    return pimpl->particleGenerator.getLightPathCount();
}

void Renderer::setMaxParticlePathLength(const uint aLength)
{
    pimpl->particleGenerator.setMaxPathLength(aLength);
}

uint Renderer::getMaxParticlePathLength()
{
    return pimpl->particleGenerator.getMaxPathLength();
}

void Renderer::setVPLClampThreshold(const float aThreshold)
{
    pimpl->particleGenerator.setClampThreshold(aThreshold);
}

float Renderer::getVPLClampThreshold()
{
    return pimpl->particleGenerator.getClampThreshold();
}

void Renderer::setVPLClampFactor(const float aFactor)
{
    pimpl->particleGenerator.setClampFactor(aFactor);
}

float Renderer::getVPLClampFactor()
{
    return pimpl->particleGenerator.getClampFactor();
}

void Renderer::setVPLClampStartValue(const float aValue)
{
    pimpl->particleGenerator.setClampStartValue(aValue);
}

float Renderer::getVPLClampStartValue()
{
    return pimpl->particleGenerator.getClampStartValue();
}

void Renderer::setImportonCount(const uint aCount)
{
    pimpl->particleFilter.setImportonCount(aCount);
}

uint Renderer::getImportonCount()
{
    return pimpl->particleFilter.getImportonCount();
}

void Renderer::setTotalFrameLuminance(const float aLuminance)
{
    pimpl->particleFilter.stats.setTotalFrameLuminance(aLuminance);
}

float Renderer::getTotalFrameLuminance()
{
    return pimpl->particleFilter.stats.getTotalFrameLuminance();
}

size_t Renderer::getTotalParticleCount()
{
    return pimpl->particleGenerator.getTotalParticleCount();
}

//-----------------------------------------------------------------------------
// Geometry Construction
//-----------------------------------------------------------------------------

void Renderer::startConstruction(uint geometryType)
{
}

void Renderer::finishConstruction(uint geometryType)
{
    if(geometryType == GEOMETRYTYPE_STATIC)
    {
#ifdef USE_FLAT_STRUCTURES
        RTFACT_LOG_LINE("Flattening hierarchy...");
        pimpl->scene.computeFlattenedPrimitives();
#endif
    }
}

GroupHandle Renderer::getRoot()
{
	return Group::getRootGroup(this);
}

//-----------------------------------------------------------------------------

GroupHandle Renderer::createGroup(const Transform &t)
{
	return new Group(this, t);
}

//-----------------------------------------------------------------------------

GeoHandle Renderer::createGeometry()
{
	return new Geometry(this);
}

//-----------------------------------------------------------------------------

GeoHandle Renderer::createGeometry(TriangleVector &aVector)
{
	GeoHandle h(new Geometry(this));
	for(uint i = 0; i < aVector.size(); i++)
		h->addPrimitive(aVector[i]);
	return h;
}

//-----------------------------------------------------------------------------

GeoHandle Renderer::createGeometry(
    const float* aVertices,
    const float* aNormals,
    const float* aVertexColors,
    const unsigned int* aTriangles,
    const unsigned int aTriangleCount,
    RTAppearanceHandle aAppearance)
{
    return new Geometry(this, aVertices, aNormals, aVertexColors,
		aTriangles, aTriangleCount, aAppearance);
}

GeoHandle Renderer::createGeometry(
    const float* aVertices,
    const float* aNormals,
    const unsigned int* aTriangles,
    const unsigned int aTriangleCount,
    RTAppearanceHandle aAppearance)
{
    return new Geometry(this, aVertices, aNormals, aTriangles,
		aTriangleCount, aAppearance);
}


void Renderer::clearScene()
{
    Group::setRootGroup(this, createGroup(Transform::identity()));
    pimpl->scene.clearMeshData();
}

void Renderer::intersectRays(
    const float* aOrigins,
    const float* aDirections,
    const unsigned int aRayCount,
    float* oDistances)
{
    ActiveMask<1> mask;
    mask.setTrueAndUpdateActiveContainers();

    for(uint i = 0; i < aRayCount; ++i)
    {
        t_Intersector::Result<1> intersection;

        const uint i3 = 3 * i;

        RayPacket<1> ray;
        ray.org = Vec3f1(aOrigins[i3], aOrigins[i3 + 1], aOrigins[i3 + 2]);
        ray.dir = Vec3f1(aDirections[i3], aDirections[i3 + 1], aDirections[i3 + 2]);
        ray.tMin = 0.f;
        ray.tMax = RTFACT_INFINITY;

        RTfact::Intersector::intersect<IsectData::DISTANCE, true>(
            pimpl->intersector, ray, mask, pimpl->tree, intersection);

        oDistances[i] = intersection.dist;
    }
}

//-----------------------------------------------------------------------------

RTLightHandle Renderer::createLight(
    const std::string& aType)
{
    RTLightHandle light = 0;

    if(aType == "PointLight")
    {
        light = new RTPointLight;

        light->setParam("attenuation", float3(1, 0, 0));
    }
	  else if(aType == "SpotLight")
    {
        light = new RTSpotLight;

        light->setParam("attenuation", float3(1, 0, 0));
    }
    else if(aType == "DirectionalLight")
    {
        light = new RTDirectionalLight;
    }
    else if(aType == "IESLight")
    {
        light = new RTIESLight;
    }
    else if(aType == "IESAreaLight")
    {
        light = new RTIESAreaLight;
    }
    else if(aType == "QuadLight")
    {
        light = new RTQuadLight;
    }
    else if(aType == "TriangleLight")
    {
        light = new RTTriangleLight;
    }

    if(light)
    {
        pimpl->lightsources.push_back(light);
        switchLightOn(light);
    }
    else
	{
        RTFACT_USER_WARNING_F("Unknown light source type: %1%", aType);
    }

    return light;
}

void Renderer::switchLightOn(
                   RTLightHandle aLight)
{
    Traits<t_Scene::LightBase>::Ptr rtLight =
        boost::dynamic_pointer_cast<t_Scene::LightBase>(
            boost::polymorphic_cast<RTPrivateLight*>(&*aLight)->getRTfactLight()
            );
    if(!rtLight)
    {
        RTFACT_PROGRAM_ERROR("Incorrect light source handle specified");
    }

    //If the light is already present in the RTfact list, do nothing
    //This can happen, if switchLightOn is called multiple times without switching it off first
    for(t_Scene::LightVector::iterator it = pimpl->scene.lights.begin();
        it != pimpl->scene.lights.end(); ++it)
    {
        if(*it == rtLight)
        {
            return;
        }
    }

    pimpl->scene.lights.push_back(rtLight);
}

void Renderer::switchLightOff(
                    RTLightHandle aLight)
{
    Traits<t_Scene::LightBase>::Ptr rtLight =
        boost::dynamic_pointer_cast<t_Scene::LightBase>(
            boost::polymorphic_cast<RTPrivateLight*>(&*aLight)->getRTfactLight()
            );

    for(t_Scene::LightVector::iterator it = pimpl->scene.lights.begin();
        it != pimpl->scene.lights.end(); ++it)
    {
        if(*it == rtLight)
        {
            pimpl->scene.lights.erase(it);

            break;
        }
    }
}

void Renderer::removeLight(
    RTLightHandle aLight)
{
    if(aLight)
    {
        switchLightOff(aLight);

        for(std::vector<RTLightHandle>::iterator it = pimpl->lightsources.begin();
            it != pimpl->lightsources.end(); ++it)
        {
            if(*it == aLight)
            {
                pimpl->lightsources.erase(it);

                break;
            }
        }
    }
}

//-----------------------------------------------------------------------------

void Renderer::createAccelStruct()
{

    pimpl->time.reset(); pimpl->time.start();
    pimpl->scene.update();
    pimpl->time.stop();
    double updating = pimpl->time.getTotalTime();
    pimpl->performBuild();

    pimpl->stime <<"\n(SG: "<<updating<<") + (AS: "<<pimpl->time.getTotalTime()<<") ";
}

//-----------------------------------------------------------------------------

void Renderer::setEnvironmentColor(
    const float aRed, const float aGreen, const float aBlue, const float aAlpha)
{
    pimpl->scene.environmentTexture.setColor( Packet<4,float>(aRed, aGreen, aBlue, aAlpha) );
}

//-----------------------------------------------------------------------------

void Renderer::setEnvironmentTexture(const RTImageHandle &handle, const uint upDimension, const bool flip)
{
    RTFACT_USER_ERROR_IF(handle->getComponentCount() != 3 && handle->getComponentCount() != 4, "Environment texture should be 3-float or 4-float format");

    pimpl->scene.environmentTexture.setImage(
        boost::static_pointer_cast<t_EnvImage>(
            boost::polymorphic_cast<RTImageImpl*>(&*handle)->getRTfactImage()));
    pimpl->scene.environmentTexture.setUpDimension(upDimension);
    pimpl->scene.environmentTexture.setUpDimensionFlip(flip);
}

//-----------------------------------------------------------------------------

void Renderer::setEnvironmentTexture(
    const float* data,
    const uint componentCount,
    const uint width,
    const uint height)
{
    RTFACT_USER_ERROR_IF(componentCount != 3 && componentCount != 4, "Environment texture should be 3-float or 4-float format");

    pimpl->scene.environmentTexture.setImage(
        new t_EnvImage(data, componentCount, width, height));
}

//-----------------------------------------------------------------------------

void Renderer::createEnvironmentLights(const unsigned int aPartitionCount)
{
    RTFACT_USER_ERROR_IF(!pimpl->scene.environmentTexture.getImage(), "Environment texture not set!");

    RTfact::Vector<RTfact::EnvLightGen::EnvironmentLight> lightContainer;
    RTfact::EnvLightGen lightGen;

    t_EnvImage envTex = *(pimpl->scene.environmentTexture.getImage());
    lightGen.extractLights(envTex, aPartitionCount, lightContainer);

    for (uint i=0; i < lightContainer.size(); ++i)
    {
        RTfact::EnvLightGen::EnvironmentLight& l = lightContainer[i];

        RTLightHandle light = createLight("DirectionalLight");
        light->setParam("direction", float3(l.direction.xf, l.direction.yf, l.direction.zf));
        light->setParam("intensity", float3(l.intensity.xf, l.intensity.yf, l.intensity.zf));
    }
}

//-----------------------------------------------------------------------------
// Camera Management
//-----------------------------------------------------------------------------

void Renderer::setCameraNode(GroupHandle g)
{
    pimpl->camera_group = g->pimpl->mGroup;
}

//-----------------------------------------------------------------------------

void Renderer::setCameraPosition(
    const float3 &pos,
    const float3 &view,
    const float3 &up)
{
    pimpl->o_params.position     = Vec3f1(pos.x, pos.y, pos.z);
    pimpl->o_params.direction    = Vec3f1(view.x, view.y, view.z);
    pimpl->o_params.up           = Vec3f1(up.x, up.y, up.z);
}

void Renderer::setCameraOpeningAngle(
    const float angle)
{
    pimpl->o_params.openingAngle = angle;
}

float Renderer::getCameraOpeningAngle() const
{
    return pimpl->o_params.openingAngle;
}

void Renderer::setFrustum(const FrustumParameters& aParams)
{
    pimpl->o_params.frustumLeft = aParams.left;
    pimpl->o_params.frustumRight = aParams.right;
    pimpl->o_params.frustumTop = aParams.top;
    pimpl->o_params.frustumBottom = aParams.bottom;
    pimpl->o_params.frustumNearPlane = aParams.nearPlane;
    pimpl->o_params.frustumFarPlane = aParams.farPlane;

    //disable the opening angle computation
    pimpl->o_params.openingAngle = 0.f;
}

FrustumParameters Renderer::getFrustum()
{
    FrustumParameters params;


    params.left = pimpl->o_params.frustumLeft;
    params.right = pimpl->o_params.frustumRight;
    params.top = pimpl->o_params.frustumTop;
    params.bottom = pimpl->o_params.frustumBottom;
    params.nearPlane = pimpl->o_params.frustumNearPlane;
    params.farPlane = pimpl->o_params.frustumFarPlane;

    return params;
}

void Renderer::useNonSquarePixels(bool aUse)
{
    pimpl->o_params.useNonSquarePixels = aUse;
}

bool Renderer::getNonSquarePixelsOn()
{
    return pimpl->o_params.useNonSquarePixels;
}

void Renderer::setCameraResolution(const int x_resolution, const int y_resolution)
{
    pimpl->o_params.resX = x_resolution;
    pimpl->o_params.resY = y_resolution;
}

//-----------------------------------------------------------------------------

void Renderer::getCameraPosition(
    float3 &pos,
    float3 &view,
    float3 &up)
{
    pos = float3(pimpl->o_params.position.x,
                 pimpl->o_params.position.y,
                 pimpl->o_params.position.z);

    view = float3(pimpl->o_params.direction.x,
                  pimpl->o_params.direction.y,
                  pimpl->o_params.direction.z);

    up = float3(pimpl->o_params.up.x,
                pimpl->o_params.up.y,
                pimpl->o_params.up.z);
}

//-----------------------------------------------------------------------------

//void Renderer::setCameraOffset(const Vec3f<1> &dir, const Transform &rot)
//{
//    pimpl->camera_offset_dir=dir;
//    pimpl->camera_offset_rot=rot;
//}

//-----------------------------------------------------------------------------
// Rendering
//-----------------------------------------------------------------------------

void Renderer::setRenderMode(uint renderFlag)
{
	pimpl->mRenderMode = renderFlag;
}
void Renderer::renderToBuffer()
{
    //prepare camera, update scene
    prepareRendering();
    pimpl->scene.update();

    pimpl->buffer.prePaint();

    if(mProgressiveRefinement)
    {
        ++mStillCameraIterations;

        pimpl->pixelsampler.setSampleSeed(
            Math::haltonBase(mStillCameraIterations, 2),
            Math::haltonBase(mStillCameraIterations, 3),
            1.f / mStillCameraIterations);
    }
    else
    {
        mStillCameraIterations = 0;

        pimpl->pixelsampler.setSampleSeed(0.5f, 0.5f, 1.0f);
    }

    pimpl->performRendering(mStillCameraIterations);

    pimpl->buffer.postPaint();


    #ifdef EVALUATION_OUTPUT

            std::cout<< pimpl->sconf.str() << pimpl->stime.str() << "\n-------------------------------";

    #endif

    pimpl->sconf.str(""); pimpl->stime.str("");
}

//-----------------------------------------------------------------------------
// Testing Options
//-----------------------------------------------------------------------------

void Renderer::setTestParameter(int id, float value)
{
    pimpl->setTestParameter(id, value);
}

void Renderer::setTestParameter(int id, int value)
{
    pimpl->setTestParameter(id, value);
}

void Renderer::setTime(float milliseconds)
{
    pimpl->setTime(milliseconds);
}

int32 Renderer::findBestGranularity()
{
    if(pimpl->mGranularity == Renderer::StartupParams::Values::PARAM_AUTOMATIC)
    {
        RTFACT_LOG_WARNING("Render task granularity is in automatic mode. First switch to manual mode (by using something else than -1) in Renderer constructor before calling findBestGranularity()");
        return -1;
    }
    RTFACT_LOG_LINE("Searching for best render task granularity...");
    RTfact::FPSMeter fpsMeter;
    int pSizeSqrt = Packet<PRIMARY_RAY_PACKET_SIZE, float>::SIZE_SQRT;
    int bestGranularity = pSizeSqrt;
    int currentGran = pSizeSqrt;
    //double minTime = std::numeric_limits<double>::max();
    double maxFPS = -1.0;
    int maxGran = (pimpl->o_params.resX > pimpl->o_params.resY) ? pimpl->o_params.resX : pimpl->o_params.resY;
    while(currentGran < maxGran)
    {
        pimpl->mGranularity = currentGran;
        //double startTime = rtsg::getCurrentTime();
        for(int frame = 0; frame < RTFACT__DEFAULT_FRAME_AVERAGE_COUNT; frame++)
        {
            fpsMeter.startFrame();
            renderToBuffer();
            fpsMeter.endFrame();
        }
        //double endTime = rtsg::getCurrentTime();
        double measuredFPS = fpsMeter.getFPS();
        //double measuredTime = endTime - startTime;
        if(measuredFPS > maxFPS)
        {
            bestGranularity = currentGran;
            maxFPS = measuredFPS;
        }
        RTFACT_LOG_LINE_F("Render task size %1%x%1%, FPS %2%", currentGran % measuredFPS);
        currentGran += PRIMARY_RAY_PACKET_SIZE;
    }
    pimpl->mGranularity = bestGranularity;
    RTFACT_LOG_LINE_F("done. Optimal render task granularity is %1%x%1%", bestGranularity);
    return bestGranularity;
}

//-----------------------------------------------------------------------------
// Appearance:

RTAppearanceHandle Renderer::createAppearance(
    const std::string& aType)
{
    RTAppearanceHandle app = 0;
    uint appID = RTFACT_DOWNSIZE_PACKET_CAST(uint,pimpl->scene.appearances.size());

    if (aType == "TestShader")
    {
        app = new RTTestShader<t_Context>(appID);
    }
    else if (aType == "PhongShader" || aType == "Phong")
    {
        app = new RTPhongShader<t_Context>(appID);
    }
	else if (aType == "PlasticShader")
	{
		app = new RTPlasticShader<t_Context>(appID);
	}
	else if (aType == "NormalShader")
	{
		app = new RTNormalShader<t_Context>(appID);
	}
	else if (aType == "BasicPhongShader")
	{
		app = new RTBasicPhongShader<t_Context>(appID);
	}
    else if (aType == "PhongGlass")
    {
        app = new RTGlassShader<t_Context>(appID);

        /*app->setParam("diffuseColor", float3(0.3f, 0.3f, 0.3f));
        app->setParam("ambientIntensity", float3(0.5f, 0.5f, 0.5f));
        app->setParam("reflective", float3(0.9f, 0.9f, 0.9f));*/
    }
    else if (aType == "EyeLightShader" || aType == "EyeLight")
    {
        app = new RTPhongShader<t_Context>(appID);
        app->setParam("receiveIllumination", false);
    }
#ifdef RTFACT_REMOTE_HAVE_ANYSL
    else
	{
        if (aType == "AnySL" ||
            aType.substr(0,6) == "AnySL:" ||
            aType.substr(0,4) == "JIT:")
        {
            std::string shaderFile;

            const size_t colonPos = aType.find(':');

            if (colonPos != std::string::npos && colonPos+1 < aType.size())
            {
                shaderFile = aType.substr(colonPos+1, std::string::npos);
            }

            RTAnySLShader * anySLShader = new RTAnySLShader(appID, shaderFile);
            assert (anySLShader && "Creation of AnySL shader failed!");
            app = anySLShader;
        }
	}
#endif

    if (app)
    {
        Traits<t_Scene::AppearanceBase>::Ptr rtApp =
            boost::dynamic_pointer_cast<t_Scene::AppearanceBase>(
                boost::dynamic_pointer_cast<RTPrivateAppearance>(&*app)->getRTfactAppearance());

        if (!rtApp)
        {
            RTFACT_PROGRAM_ERROR("Incorrect appearance specified");
        }

        pimpl->scene.appearances.push_back(rtApp);
        pimpl->appearances.push_back(app);
        boost::dynamic_pointer_cast<RTPrivateAppearance>(app.get())->setAppearanceUpdated();
    }
    else
    {
        RTFACT_LOG_WARNING(std::string("Unknown appearance with name: ")+aType);
#ifndef RTFACT_REMOTE_HAVE_ANYSL
        RTFACT_LOG_WARNING("RTremote compiled without AnySL-support!");
#endif
    }

    return app;
}

RTAppearanceHandle Renderer::getAppearance(uint aIndex)
{
	// Todo: Check for invalid bounds
	return pimpl->appearances[aIndex];
}


RTImageHandle Renderer::createImage(
    const float* aData,
    const uint aComponentCount,
    const uint aResX,
    const uint aResY)
{
    return new RTImageImpl(
        static_cast<const float*>(aData), RTImage::COMPONENT_FLOAT,
        aComponentCount, aResX, aResY);
}

RTImageHandle Renderer::createImage(
    RTImage::ComponentType aType,
    const uint aComponentCount,
    const uint aResX,
    const uint aResY)
{
    return new RTImageImpl(
        aType, aComponentCount, aResX, aResY);
}

//-----------------------------------------------------------------------------

/*
void Renderer::linkAppearanceToGeometry(
    Geometry* aGeo,
    const RTAppearanceHandle& aAppearance)
{
    pimpl->scene.setAppearanceID(aGeo->handle, aAppearance->getID());
    aGeo->appearance = aAppearance;
}
*/

//-----------------------------------------------------------------------------

void Renderer::prepareRendering()
{
    // Update Camera

    t_Camera::Params params = pimpl->o_params;

    Vec3f<1> left = Math::cross( params.direction, Vec3f<1>(0,1,0));
    Vec3f<1> up = Math::cross(params.direction, left);
    params.position    += -pimpl->camera_offset_dir.zf * params.direction
        + pimpl->camera_offset_dir.yf * params.up + pimpl->camera_offset_dir.xf * left;

    params.direction    = pimpl->camera_offset_rot.applyToVectorAffine( params.direction ).normalize();
    //params.up           = pimpl->camera_offset_rot.applyToVectorAffine( params.up ).normalize();
    RTfact::Transform t = pimpl->camera_group->getAccumTransform();

    params.position     = t.applyToPointAffine(params.position );
    params.direction    = t.applyToVectorAffine(params.direction).normalize();
    params.up           = t.applyToVectorAffine(params.up).normalize();

    // Update Lightsources
	for(uint i=0; i < pimpl->lightsources.size(); i++)
	{
		RTLightHandle l = pimpl->lightsources[i];
		GroupHandle g = l->getParentGroup();
		if(g)
            boost::polymorphic_cast<RTPrivateLight*>(
            &*l)->updateTransformed(g->pimpl->mGroup->getAccumTransform());
	}

    // Update Appearances
	for(uint i=0; i < pimpl->appearances.size(); i++)
	{
	    Traits<RTPrivateAppearance>::Ptr app =
	            boost::dynamic_pointer_cast<RTPrivateAppearance>(pimpl->appearances[i].get());
	    if(app->hasNewAppearance())
	    {
	        Traits<t_Scene::AppearanceBase>::Ptr rtApp =
	                    boost::dynamic_pointer_cast<t_Scene::AppearanceBase>(
	                            app->getRTfactAppearance());
	        if (rtApp)
            {
	            pimpl->scene.appearances[i] = rtApp;
	            app->setAppearanceUpdated();
            }
	    }
	}

    /*for(int i=0; i < pimpl->lightsources.size(); i++)
    {
        fprintf(f, "Check1");
        RTLightHandle l = pimpl->lightsources[i];
        Vec3f<1> lightPos;
        Vec3f<1> intensity;
        fprintf(f, "Check2");
        fprintf(f, "light %p", &(*l));
        std::cerr << "\nAdress: " << l;

        lightPos = l->mPos;
        intensity = l->mIntensity;
        fprintf(f, "Check3");

        if(l->mParent)
        {
            Transform t = pimpl->scene.getAccumTransform(l->mParent->handle);
            lightPos = t.applyToPoint(lightPos);
        }

        fprintf(f, "Check4");
        PointLight *pl = static_cast<PointLight*>(pimpl->scene.lights[l->mLightID]);
        fprintf(f, "Adress: %p", pl);
        std::cerr << "\nAdress: " << &(lightPos);
        std::cerr << " " << pl;
        pl->position = lightPos;
        pl->intensity = intensity;
    }*/

    pimpl->camera.setParams(params);

    // Update Acceleration Structure
    #ifdef ACC_ONCE

        if(!acc_built)
        {
            createAccelStruct();

            acc_built = true;
        }

    #else

        createAccelStruct();

    #endif

    // Update light sources with new bounding box, if appropriate
    RTPrivateLight *light;
    Light *rtfactLight;
    BoundingSphere bSphere = pimpl->tree.getBounds().getBoundingSphere();
    for (uint i=0; i < pimpl->lightsources.size(); i++)
    {
	    RTLightHandle l = pimpl->lightsources[i];
        if ((light = dynamic_cast<RTPrivateLight*>(&*l)) != NULL)
        {
            if((rtfactLight = reinterpret_cast<Light *>(light->getRTfactLight().get())) != NULL)
            {
                rtfactLight->setBoundingSphere(bSphere);
            }
        }
    }
}

//-----------------------------------------------------------------------------

RTFACT_REMOTE_NAMESPACE_END
