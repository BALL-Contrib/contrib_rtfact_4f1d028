/*
 *  RTpiePrivateConfig.hpp
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
 *  Created on: 2010-09-15 17:59:35 +0200
 *  Author(s): Felix Klein
 */

 #ifndef RTRTPIE_PRIVATE_CONFIG_HPP
#define RTRTPIE_PRIVATE_CONFIG_HPP

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
// #define USE_FLAT_STRUCTURES

#define CLASSIC_SHADERS

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
#define RTFACT_RTPIE_RAY_PACKET_SIZE 64
#define PRIMARY_RAY_PACKET_SIZE RTFACT_RTPIE_RAY_PACKET_SIZE

/* Use IntMap for Primitive Storage. Vectors don't work on 64bit Machines
 */
#define RTFACT_USE_PRIMITIVE_INTMAP



//-----------------------------------------------------------------------------
// RTfact Includes
//-----------------------------------------------------------------------------

#include <RTfact/Config/ExternalIncludes.hpp>
#include <iostream>
#include <fstream>
#include <boost/bind.hpp>
#include <boost/lambda/bind.hpp>

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
#include <RTfact/Model/Primitive/TwoLevelAccelInstance.hpp>

//#include <RTfact/Model/Scene/NewGraphScene.hpp>

#include <RTfact/Model/Scene/TwoLevelScene.hpp>
#include <RTfact/Model/Scene/TwoLevelScene/TwoLevelSceneInstance.hpp>
#include <RTfact/Model/Scene/TwoLevelScene/TwoLevelSceneGeometry.hpp>
#include <RTfact/Model/Scene/TwoLevelScene/TwoLevelSceneMesh.hpp>

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
#include <RTfact/Utils/SurfaceShaderContext.hpp>
#include <RTfact/Concept/Appearance/SurfaceShader.hpp>

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

#include <RTfact/Model/Builder/TwoLevelStructSynchronizer.hpp>
#include <RTfact/Model/Builder/Data/AccelBuildData.hpp>
//#include <RTfact/Model/Builder/NewGraphBVHBuilder.hpp>

// Intersector

//#include <RTfact/Model/Intersector/KdTreeIntersector.hpp>
#include <RTfact/Model/Intersector/BVHIntersector.hpp>
#include <RTfact/Model/Intersector/BVHPartitionIntersector.hpp>
//#include <RTfact/Model/Intersector/OrderedBVHIntersector.hpp>
#include <RTfact/Model/Intersector/PlueckerTriangleIntersector.hpp>
#include <RTfact/Model/Intersector/MultiShaderInstanceIntersector.hpp>
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


RTFACT_RTPIE_NAMESPACE_BEGIN


//---------------------------------------------------------------------
// Type Definitions
//---------------------------------------------------------------------

typedef float t_DistanceBufferComponent;

typedef RTfact::Triangle    t_PrimitiveConfig;
typedef Material            t_AppearanceConfig;
typedef Light               t_LightConfig;

//typedef BasicImageSampler          t_ImageSampler;
typedef BilinearFilterImageSampler t_ImageSampler;

typedef BasicImage2D<float> t_EnvImage;

//typedef Float3ConstantEnvironmentTexture t_EnvTexture;
typedef Image2DEnvironmentTexture<t_EnvImage, t_ImageSampler> t_EnvTexture;

// Particle typedefs
typedef BasicParticleGenerator<VectorVPLStorage>            t_ParticleGenerator;
#if defined(USE_TBB)
  typedef GlobalImportanceParticleFilter<VectorVPLStorage, tbb::mutex>    t_ParticleFilter;
#else
  typedef GlobalImportanceParticleFilter<VectorVPLStorage, NullMutex>    t_ParticleFilter;
#endif


#ifdef CLASSIC_SHADERS

    struct SceneConfig
    {
        RTFACT_STATIC_CONSTANT(PROPERTIES,
            SceneProperties::INVERT_NORMAL |
            SceneProperties::HAS_TRANSPARENCY);

        typedef SceneConfig This;

		typedef TwoLevelScene< This, Light, t_EnvTexture, PROPERTIES>		Scene;
		typedef RTfact::TwoLevelSceneInstance<This>							Instance;
        typedef RTfact::TwoLevelSceneGeometry<This>							Geometry;
		typedef RTfact::TwoLevelSceneMesh<This>								Mesh;
        typedef RTfact::Triangle											Primitive;

		typedef OrderedBVH<Primitive>										InnerTree;
		typedef TwoLevelAccelInstance< InnerTree, Instance >				AccelInstance;
        typedef OrderedBVH<AccelInstance>									OuterTree;
#ifndef USE_FLAT_STRUCTURES
        typedef BVHPartitionIntersector<InstanceIntersector<AccelInstance,
                BVHPartitionIntersector<PlueckerTriangleIntersector> > >	Intersector;
        typedef SurfaceShaderContext<
                    Scene,
                    OuterTree,
                    Intersector,
                    t_ParticleGenerator,
                    t_ParticleFilter>                                       Context;
#else
        typedef BVHPartitionIntersector<PlueckerTriangleIntersector> 		Intersector;
        typedef SurfaceShaderContext<
                    Scene,
                    InnerTree,
                    Intersector,
                    t_ParticleGenerator,
                    t_ParticleFilter>                                       Context;
#endif

        //typedef GraphScene<t_PrimitiveConfig, This, t_LightConfig, t_EnvTexture, PROPERTIES> Scene;
        //typedef RTfact::HandledScene<This, t_LightConfig, t_EnvTexture> HandledScene;


        typedef SurfaceShader<Context>										AppearanceBase;
    };

    typedef SceneConfig::Scene							t_Scene;
    typedef SceneConfig::Primitive						t_Primitive;
	typedef SceneConfig::Instance						t_InternInstance;
    typedef Traits<SceneConfig::Instance>::Ptr		    t_InternInstanceHandle;
    typedef SceneConfig::Geometry							t_InternGeometry;
    typedef Traits<SceneConfig::Geometry>::Ptr		    t_InternGeometryHandle;
    typedef SceneConfig::Mesh						    t_InternMesh;
    typedef Traits<SceneConfig::Mesh>::Ptr			    t_InternMeshHandle;

    typedef SceneConfig::AccelInstance					t_AccelInstance;
    typedef SceneConfig::Context						t_Context;
    typedef SurfaceShaderIntegrator<t_Context>			t_Integrator;





	typedef SceneConfig::Intersector					t_Intersector;




#else

    typedef GraphScene<t_PrimitiveConfig, t_AppearanceConfig, t_LightConfig, t_EnvTexture> t_Scene;
    typedef HandledScene<t_AppearanceConfig, t_LightConfig> t_HandledScene;

    typedef EyeLightIntegrator<t_Scene, t_Tree, t_Intersector> t_Integrator;

#endif


#ifndef USE_FLAT_STRUCTURES

	typedef SceneConfig::InnerTree						    t_InnerTree;
	typedef SceneConfig::OuterTree						    t_Tree;
	typedef AccelBuildData<t_InnerTree, t_AccelInstance>    t_AccelData;

	typedef SAHBVHBuilder								    t_InnerBuilder;
    typedef SAHBVHBuilder                                   t_OuterBuilder;
	typedef TwoLevelStructSynchronizer<t_InnerBuilder,
        t_OuterBuilder>		                                t_Synchronizer;


    #ifndef INTERSECT_HEAT

        //typedef EyeLightIntegrator<Image2DEnvironmentTexture<t_EnvImage> > t_Integrator;

    #else

        typedef BVHPartitionIntersector<MultiShaderInstanceIntersector<t_Tree::Instance,
            TestDataIntersector< BVHPartitionIntersector<PlueckerTriangleIntersector> > > > t_Intersector;
        //typedef TestColorIntegrator t_Integrator;

    #endif // INTERSECT_HEAT

#else

	typedef SceneConfig::InnerTree						t_Tree;

    typedef SAHBVHBuilder t_Builder;

#endif

typedef BasicPixelSampler		t_PixelSampler;
typedef BasicSurfaceRayTracer	t_Renderer;
typedef AsymmetricPinholeCamera			t_Camera;

typedef BasicImage2D<unsigned char>		       t_RTpieByteBuffer;
typedef BasicImage2D<float>                    t_RTpieFloatBuffer;

typedef t_RTpieFloatBuffer                     t_RTpieDistanceBuffer;

typedef Image2DFramebuffer<t_RTpieByteBuffer,
    t_RTpieDistanceBuffer>                     t_RTpieByteFramebuffer;
typedef Image2DFramebuffer<t_RTpieFloatBuffer,
    t_RTpieDistanceBuffer>                     t_RTpieFloatFramebuffer;

RTFACT_RTPIE_NAMESPACE_END

#endif // RTRTPIE_PRIVATE_CONFIG_HPP
