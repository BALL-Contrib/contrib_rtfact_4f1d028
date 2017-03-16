/*
 *  RTremotePrivateConfig.hpp
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
 *  Created on: 2009-09-30 14:15:34 +0200
 *  Author(s): Felix Klein, Lukas Marsalek, Ken Dahm, Vaclav Krajicek, Petr Kmoch, Ken Patrick Dahm
 */

#ifndef RTREMOTE_PRIVATE_CONFIG_HPP
#define RTREMOTE_PRIVATE_CONFIG_HPP

//-----------------------------------------------------------------------------
// Configuration
//-----------------------------------------------------------------------------

/* Use TBB for parallel Rendering
 */

#define USE_TBB

/* Build flat acceleration structures over the scene instead of two-level acceleration structure.
 * Traversal is faster, but build of acceleration structure MUCH slower
 * Should only be activated for STATIC SCENES in combination with ACC_ONCE
 */
//#define USE_FLAT_STRUCTURES

#define CLASSIC_SHADERS

/* Create the acceleration structure only once, when the scene is rendered the first time.
 * Recommended for static scenes.
 * The geometry of dynamic scenes won't be updated after the first frame.
 * Camera movement still works.
 */
//#define ACC_ONCE


/* Activates additional debug informations and options for the InstanceIntersector.
 * only works if USE_FLAT_STRUCTURES is DEACTIVATED.
 */
//#define RTFACT_INSTANT_INTERSECTOR_DEBUG


/* Output runtime evaluation after rendering each frame.
 * Might decrease overall performance of the renderer
 */
//#define EVALUATION_OUTPUT


/* Render colors according to number of instance intersections per packet.
 * Not recommended if rendering of realistic images is intended
 */
//#define INTERSECT_HEAT


/* Packet Size used by RTfact. Changing at own risk!
 */
#define PRIMARY_RAY_PACKET_SIZE RTFACT_REMOTE_RAY_PACKET_SIZE


/* Use IntMap for Primitive Storage. Vectors don't work on 64bit Machines
 */
#define RTFACT_USE_PRIMITIVE_INTMAP

#define RTFACT__DEBUG_IMPORTONS
#define RTFACT__DEBUG_PARTICLES

//-----------------------------------------------------------------------------
// External Includes
//-----------------------------------------------------------------------------

#include <RTfact/Config/ExternalIncludes.hpp>
#include <RTfact/Utils/IO/Configuration.hpp>
#include <iostream>
#include <fstream>
#include <boost/bind.hpp>
#include <boost/lambda/bind.hpp>

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_attribute.hpp>
#include <boost/spirit/include/classic_push_back_actor.hpp>
#include <boost/spirit/include/classic_assign_actor.hpp>
#include <assert.h>
#include <cstdlib>
#include <sstream>
#include <fstream>

#if defined(USE_TBB)
#    include <tbb/task_scheduler_init.h>
#    include <tbb/parallel_for.h>
#    include <tbb/blocked_range2d.h>
#    include <tbb/mutex.h>
#endif

//-----------------------------------------------------------------------------
// RTfact Includes
//-----------------------------------------------------------------------------

// Scene

#include <RTfact/Model/Primitive/Triangle.hpp>
#include <RTfact/Utils/Transform.hpp>
#include <RTfact/Model/Primitive/AccelerationInstance.hpp>
#include <RTfact/Model/Scene/GraphScene.hpp>
#include <RTfact/Model/Scene/NewGraphScene.hpp>
#include <RTfact/Model/Light/PointLight.hpp>
#include <RTfact/Model/Material/LambertianMaterial.hpp>
#include <RTfact/Model/Texture/Float3ConstantTexture.hpp>
#include <RTfact/Model/EnvironmentTexture/Image2DEnvironmentTexture.hpp>

// Renderer

#include <RTfact/Model/Renderer/BasicSurfaceRayTracer.hpp>
#include <RTfact/Model/PixelSampler/BasicPixelSampler.hpp>
#include <RTfact/Model/Camera/AsymmetricPinholeCamera.hpp>
#include <RTfact/Model/Integrator/SimpleIntegrator.hpp>
#include <RTfact/Model/Integrator/TestColorIntegrator.hpp>
#include <RTfact/Model/Integrator/EyeLightIntegrator.hpp>
#include <RTfact/Model/Integrator/DirectIlluminationIntegrator.hpp>
#include <RTfact/Model/Integrator/SurfaceShaderIntegrator.hpp>
#include <RTfact/Model/Integrator/AmbientOcclusionSurfaceShaderIntegrator.hpp>
#include <RTfact/Utils/SurfaceShaderContext.hpp>
#include <RTfact/Concept/Appearance/SurfaceShader.hpp>
#include <RTfact/Utils/Sampler.h>

// Acceleration Structure

#include <RTfact/Model/Primitive/KdTree.hpp>
#include <RTfact/Model/Primitive/BVH.hpp>
#include <RTfact/Model/Primitive/GraphBVH.hpp>
#include <RTfact/Model/Primitive/OrderedBVH.hpp>
//#include <RTfact/Model/Primitive/LoDIndexKdTree.hpp>

// Builder

#include <RTfact/Model/Builder/SAHBVHBuilder.hpp>
//#include <RTfact/Model/Builder/LoDIndexKdTreePointBuilder.hpp>
//#include <RTfact/Model/Builder/SimpleTriangleIndexKdTreeBuilder.hpp>
#include <RTfact/Model/Builder/GraphBVHBuilder.hpp>

#include <RTfact/Model/Builder/GraphStructSynchronizer.hpp>
#include <RTfact/Model/Builder/Data/AccelBuildData.hpp>
#include <RTfact/Model/Builder/NewGraphBVHBuilder.hpp>

// Intersector

//#include <RTfact/Model/Intersector/KdTreeIntersector.hpp>
#include <RTfact/Model/Intersector/BVHIntersector.hpp>
#include <RTfact/Model/Intersector/BVHPartitionIntersector.hpp>
//#include <RTfact/Model/Intersector/OrderedBVHIntersector.hpp>
#include <RTfact/Model/Intersector/PlueckerTriangleIntersector.hpp>
#include <RTfact/Model/Intersector/InstanceIntersector.hpp>
#include <RTfact/Model/Intersector/TestDataIntersector.hpp>
#include <RTfact/Model/EnvironmentTexture/Float3ConstantEnvironmentTexture.hpp>
// Display

#include <RTfact/Model/Framebuffer/Image2DFramebuffer.hpp>
#include <RTfact/Model/Image/BasicImage2D.hpp>

// Textures

#include <RTfact/Model/ImageSampler/BilinearFilterImageSampler.hpp>
#include <RTfact/Model/ImageSampler/BasicImageSampler.hpp>
#include <RTfact/Model/EnvironmentTexture/Float3ConstantEnvironmentTexture.hpp>
#include <RTfact/Model/EnvironmentTexture/Image2DEnvironmentTexture.hpp>

// Timer

#include <RTfact/Utils/System/TimeMeasurer.hpp>

// Synchronization

#include <RTfact/Utils/Synchronization.hpp>

// =============================================================================
// Particles
// =============================================================================

#include <RTfact/Model/Particles/VectorVPLStorage.hpp>
#include <RTfact/Model/Particles/BasicParticleGenerator.hpp>
#include <RTfact/Model/Particles/GlobalImportanceParticleFilter.hpp>

RTFACT_REMOTE_NAMESPACE_BEGIN


//---------------------------------------------------------------------
// Type Definitions
//---------------------------------------------------------------------

typedef RTfact::Triangle    t_PrimitiveConfig;
typedef Material            t_AppearanceConfig;
typedef Light               t_LightConfig;

//typedef BasicImageSampler          t_ImageSampler;
typedef BilinearFilterImageSampler t_ImageSampler;

typedef BasicImage2D<float> t_EnvImage;

//typedef Float3ConstantEnvironmentTexture t_EnvTexture;
typedef Image2DEnvironmentTexture<t_EnvImage, t_ImageSampler> t_EnvTexture;

// Particle typedefs
typedef BasicParticleGenerator<VectorVPLStorage>            tParticleGenerator;

#if defined(USE_TBB)
  typedef GlobalImportanceParticleFilter<VectorVPLStorage, tbb::mutex>    tParticleFilter;
#else
  typedef GlobalImportanceParticleFilter<VectorVPLStorage, NullMutex>    tParticleFilter;
#endif



#ifdef CLASSIC_SHADERS

    struct SceneConfig
    {
        RTFACT_STATIC_CONSTANT(PROPERTIES,
            SceneProperties::INVERT_NORMAL |
            SceneProperties::HAS_TRANSPARENCY);

        typedef SceneConfig This;

		typedef NewGraphScene< This, Light, t_EnvTexture, PROPERTIES>		Scene;
		typedef RTfact::Mesh<This>											Mesh;
        typedef RTfact::Instance<This>										Instance;
		typedef RTfact::Group<This>											Group;
        typedef RTfact::Triangle											Primitive;

		typedef OrderedBVH<Primitive>										InnerTree;
		typedef AccelerationInstance< InnerTree, Instance, Group >					AccelInstance;
        typedef OrderedBVH<AccelInstance>									OuterTree;
#ifndef USE_FLAT_STRUCTURES
        typedef BVHPartitionIntersector<InstanceIntersector<AccelInstance,
                BVHPartitionIntersector<PlueckerTriangleIntersector> > >	Intersector;
        typedef SurfaceShaderContext<
                    Scene,
                    OuterTree,
                    Intersector,
                    tParticleGenerator,
                    tParticleFilter>                                       Context;
#else
        typedef BVHPartitionIntersector<PlueckerTriangleIntersector> 		Intersector;
        typedef SurfaceShaderContext<
                    Scene,
                    InnerTree,
                    Intersector,
                    tParticleGenerator,
                    tParticleFilter>                                       Context;
#endif

        //typedef GraphScene<t_PrimitiveConfig, This, t_LightConfig, t_EnvTexture, PROPERTIES> Scene;
        //typedef RTfact::HandledScene<This, t_LightConfig, t_EnvTexture> HandledScene;


        typedef SurfaceShader<Context>										AppearanceBase;
    };

    typedef SceneConfig::Scene							t_Scene;
    typedef SceneConfig::Primitive						t_Primitive;
	typedef SceneConfig::Group							t_InternGroup;
    typedef Traits<SceneConfig::Group>::Ptr				t_InternGroupHandle;
    typedef SceneConfig::Mesh							t_InternMesh;
    typedef Traits<SceneConfig::Mesh>::Ptr				t_InternMeshHandle;
    typedef SceneConfig::Instance						t_InternInstance;
    typedef Traits<SceneConfig::Instance>::Ptr			t_InternInstanceHandle;
    typedef SceneConfig::AccelInstance					t_AccelInstance;
    typedef SceneConfig::Context						t_Context;
	typedef Sampler<RTFACT_AO_SAMPLE_TYPE,
					RTFACT_AO_HEMI_SAMPLE_TYPE,
					RTFACT_AO_PRECOMPUTED_SAMPLES>		t_AmbientRaySampler;
    typedef SurfaceShaderIntegrator<t_Context>			t_Integrator;
	typedef AmbientOcclusionSurfaceShaderIntegrator<
				t_Context,
				t_AmbientRaySampler>					t_AmbientOcclusionIntegrator;




	typedef SceneConfig::Intersector					t_Intersector;




#else

    typedef GraphScene<t_PrimitiveConfig, t_AppearanceConfig, t_LightConfig, t_EnvTexture> t_Scene;
    typedef HandledScene<t_AppearanceConfig, t_LightConfig> t_HandledScene;

    typedef EyeLightIntegrator<t_Scene, t_Tree, t_Intersector> t_Integrator;

#endif


#ifndef USE_FLAT_STRUCTURES

	typedef SceneConfig::InnerTree						t_InnerTree;
	typedef SceneConfig::OuterTree						t_Tree;
	typedef AccelBuildData<t_InnerTree, t_AccelInstance>	t_AccelData;

	typedef SAHBVHBuilder								t_InnerBuilder;
    typedef NewGraphBVHBuilder                          t_Builder;
	typedef GraphStructSynchronizer<t_InnerBuilder>		t_Synchronizer;


    #ifndef INTERSECT_HEAT

        //typedef EyeLightIntegrator<Image2DEnvironmentTexture<t_EnvImage> > t_Integrator;

    #else

        typedef BVHPartitionIntersector<InstanceIntersector<t_Tree::Instance,
            TestDataIntersector< BVHPartitionIntersector<PlueckerTriangleIntersector> > > > t_Intersector;
        //typedef TestColorIntegrator t_Integrator;

    #endif // INTERSECT_HEAT

#else

	typedef SceneConfig::InnerTree						t_Tree;

    typedef SAHBVHBuilder t_Builder;

#endif

typedef BasicPixelSampler		t_PixelSampler;
typedef BasicSurfaceRayTracer	t_Renderer;
typedef AsymmetricPinholeCamera	t_Camera;

typedef BasicImage2D<t_FrameBufferComponent>                t_RTRemoteFrameBuffer;
typedef BasicImage2D<t_DistanceBufferComponent>             t_RTRemoteDistanceBuffer;
typedef BasicImage2D<t_NormalBufferComponent>               t_RTRemoteNormalBuffer;
typedef BasicImage2D<t_AmbientOcclusionBufferComponent>     t_RTRemoteAmbientOcclusionBuffer;
typedef BasicImage2D<t_ColorBleedingBufferComponent>        t_RTRemoteColorBleedingBuffer;
typedef Image2DFramebuffer<
    t_RTRemoteFrameBuffer,
    t_RTRemoteDistanceBuffer,
    t_RTRemoteNormalBuffer,
    t_RTRemoteAmbientOcclusionBuffer,
    t_RTRemoteColorBleedingBuffer> t_RTRemoteFramebuffer;

//---------------------------------------------------------------------
// RendererPimpl Declaration
//---------------------------------------------------------------------

class RendererPimpl{
public:

	RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    //---------------------------------------------------------------------
    // Class Fields
	//---------------------------------------------------------------------

    // Sampler
	t_AmbientRaySampler mAmbientRaySampler;

	// Render mode
	uint mRenderMode;
	bool mUseOldSamples;
    int32 mRenderThreads;

    // Scene
    t_Scene scene;

    // Acceleration Structure
    t_Tree tree;

    //Particle tracing classes
    tParticleGenerator          particleGenerator;
    tParticleFilter             particleFilter;

    // Context for shaders
    t_Context context;

    // Building and intersection
#ifndef USE_FLAT_STRUCTURES
    t_AccelData builddata;
	t_Synchronizer synchronizer;
#endif
    t_Builder builder;
    t_Intersector intersector;

    // Integrators
    t_Integrator integrator;
	t_AmbientOcclusionIntegrator aoIntegrator;

    // Rendering
    t_PixelSampler pixelsampler;
    t_Renderer renderer;

    // Camera
    t_Camera camera;
    t_Camera::Params o_params;			// original parameters of the camera
    Vec3f<1> camera_offset_dir;			// offset direction of the camera
    RTfact::Transform camera_offset_rot;// offset rotation of the camera
    t_InternGroupHandle camera_group;	// group node containing the camera.

    // Framebuffer
    t_RTRemoteFramebuffer buffer;
    ImageClipRegion clip;

    // Parallel Rendering
    int32 mGranularity;

#ifdef USE_TBB
private:
    typedef boost::shared_ptr<tbb::task_scheduler_init> tTbbSchedulerInitPtr;
    typedef boost::shared_ptr<tbb::affinity_partitioner> tTbbAutomaticPartitionerPtr;
    typedef boost::shared_ptr<tbb::simple_partitioner> tTbbManualPartitionerPtr;

    tTbbSchedulerInitPtr mTbbSchedulerInit;
    tTbbAutomaticPartitionerPtr mTbbAutomaticPartitioner;
    tTbbManualPartitionerPtr mTbbManualPartitioner;

public:
    struct RenderParallel
    {
        t_Renderer& rend;
        t_Camera& cam;
        t_Scene& sc;
        t_Tree& tree;
        t_Intersector& intersector;
        t_PixelSampler& pixelSampler;
        t_Integrator& integrator;
		t_AmbientOcclusionIntegrator& aoIntegrator;
        t_RTRemoteFramebuffer& wind;
		uint mRenderMode;
		bool mUseOldSamples;

    public:

        void operator()(
            const tbb::blocked_range2d<RTfact::uint> &r) const
        {
            ImageClipRegion clip;
            clip.left = r.cols().begin();
            clip.right = r.cols().end();
            clip.top = r.rows().begin();
            clip.bottom = r.rows().end();

            if ((mRenderMode >> 1) == 1)
			{
				aoIntegrator.setUseOldSamples(mUseOldSamples);
				rend.render<PRIMARY_RAY_PACKET_SIZE>(
					cam, sc, tree, intersector, pixelSampler, aoIntegrator, wind, clip);
			}
			else
				rend.render<PRIMARY_RAY_PACKET_SIZE>(
					cam, sc, tree, intersector, pixelSampler, integrator, wind, clip);
        }

        RenderParallel(
            t_PixelSampler& apixelSampler,
            t_Integrator& aintegrator,
			t_AmbientOcclusionIntegrator& aAOIntegrator,
            t_Renderer& arend,
            t_Camera& acam,
            t_Scene& asc,
            t_Tree& atree,
            t_Intersector& atrav,
            t_RTRemoteFramebuffer& awind,
			uint renderMode,
			bool aUseOldSamples
        ) :
            rend(arend),
            cam(acam),
            sc(asc),
            tree(atree),
            intersector(atrav),
            pixelSampler(apixelSampler),
            integrator(aintegrator),
            aoIntegrator(aAOIntegrator),
            wind(awind),
			mRenderMode(renderMode),
			mUseOldSamples(aUseOldSamples)
        {}
    };

#endif // USE_TBB

private:

    void loadStartupConfiguration(IO::Configuration& aConfig);
    void printConfig();

public:

	// Runtime Measurement / Output
    std::ostringstream sconf, stime;
    TimeMeasurer time;
    double averageBuildTime;
    int builded;

	std::vector<RTAppearanceHandle>	appearances;
    std::vector<RTLightHandle>		lightsources;

    //---------------------------------------------------------------------

	//---------------------------------------------------------------------
	// Functions
	//---------------------------------------------------------------------

	RendererPimpl(const char* aConfigurationString = NULL);

	~RendererPimpl();

	void performBuild();

	void performRendering(const uint stillCameraIteration);

	void updateAverageBuildTime(double time);
	void setTestParameter(int id, float value);
	void setTestParameter(int id, int value);
  void setTime(float milliseconds);

};

//---------------------------------------------------------------------
// GeoPimpl Declaration
//---------------------------------------------------------------------

class GeoPimpl{
public:

	t_InternMeshHandle mMesh;

	GeoPimpl(t_InternMeshHandle aMesh) : mMesh(aMesh) {}

};

//---------------------------------------------------------------------
// GroupPimpl Declaration
//---------------------------------------------------------------------

class GroupPimpl{
public:

	t_InternGroupHandle mGroup;

	GroupPimpl(t_InternGroupHandle aGroup) : mGroup(aGroup) {}

};

RTFACT_REMOTE_NAMESPACE_END

#endif // RTREMOTE_PRIVATE_CONFIG_HPP
