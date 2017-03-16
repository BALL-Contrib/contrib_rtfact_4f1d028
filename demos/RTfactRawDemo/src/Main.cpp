/* 
 *  Main.cpp
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
 *  Created on: 2010-08-21 21:49:13 +0200
 *  Author(s): Iliyan Georgiev, Petr Kmoch, Rossen Dimov, Lukas Marsalek
 */
 
#include "Precompiled.hpp"
#ifndef EXCLUDE_FROM_BUILD

#include <RTfact/Config/Common.hpp>

// =============================================================================
// BASIC CONFIGURATION
// =============================================================================

#define DISPLAY_WINDOW

#include <iomanip>
#include <RTfact/Utils/System/TimeMeasurer.hpp>

RTfact::TimeMeasurer t0, t1, t2, t3;
RTfact::uint         c0 = 0, c1 = 0, c2 = 0;

//#define TIME_STATS
//#define COUNTER_STATS

#ifdef TIME_STATS
#   define START_TIMER(aNumber) t##aNumber.start()
#   define STOP_TIMER(aNumber)   t##aNumber.stop()
#   define RESET_TIMER(aNumber) t##aNumber.reset()
#   define GET_TIMER(aNumber)   t##aNumber.getTotalTime()
#else
#   define START_TIMER(aNumber)
#   define STOP_TIMER(aNumber)
#   define RESET_TIMER(aNumber)
#   define GET_TIMER(aNumber) 0
#endif

#ifdef COUNTER_STATS
#   define INCREMENT_COUNTER(aNumber) ++c##aNumber
#   define RESET_COUNTER(aNumber) c##aNumber = 0
#   define GET_COUNTER(aNumber) c##aNumber
#else
#   define INCREMENT_COUNTER(aNumber)
#   define RESET_COUNTER(aNumber)
#   define GET_COUNTER(aNumber) 0
#endif

#define DEFAULT_WINDOW_CAPTION   "RTfact demo"
#define DEFAULT_IMAGE_RESOLUTION 512
#define CONFIG_FILE              "RTfact.ini"

#define RTFACT__DEBUG_IMPORTONS
#define RTFACT__DEBUG_PARTICLES
//#define RTFACT_VISUALIZE_IMPORTONS
//#define RTFACT_VISUALIZE_PARTICLES

#pragma region Includes

// =============================================================================
// SCENES
// =============================================================================

#include <RTfact/Model/Scene/BasicScene.hpp>

// =============================================================================
// TEXTURES
// =============================================================================

#include <RTfact/Model/Texture/Float3ConstantTexture.hpp>

#include <RTfact/Model/EnvironmentTexture/Float3ConstantEnvironmentTexture.hpp>
#include <RTfact/Model/EnvironmentTexture/Image2DEnvironmentTexture.hpp>

// =============================================================================
// RENDERERS
// =============================================================================

#include <RTfact/Model/Renderer/BasicSurfaceRayTracer.hpp>

// =============================================================================
// CAMERAS
// =============================================================================

#include <RTfact/Model/Camera/PinholeCamera.hpp>

// =============================================================================
// PIXEL SAMPLERS
// =============================================================================

#include <RTfact/Model/PixelSampler/BasicPixelSampler.hpp>

// =============================================================================
// MATERIALS
// =============================================================================

#include <RTfact/Model/Material/LambertianMaterial.hpp>

#include <RTfact/Utils/SurfaceShaderContext.hpp>
#include <RTfact/Model/Integrator/SurfaceShaderIntegrator.hpp>
#include <RTfact/Model/SurfaceShader/PhongShader.hpp>

// =============================================================================
// LIGHTS
// =============================================================================

#include <RTfact/Model/Light/PointLight.hpp>
#include <RTfact/Model/Light/DirectionalLight.hpp>
#include <RTfact/Model/Light/IESLight.hpp>
#include <RTfact/Model/Light/EnvironmentLight.hpp>

#include <RTfact/Model/LightAttenuation/NoLightAttenuation.hpp>

// =============================================================================
// PRIMITIVES
// =============================================================================

#include <RTfact/Model/Primitive/KdTree.hpp>
#include <RTfact/Model/Primitive/BVH.hpp>
#include <RTfact/Model/Primitive/OrderedBVH.hpp>

// =============================================================================
// BUILDERS
// =============================================================================

#include <RTfact/Model/Builder/SAHBVHBuilder.hpp>
#include <RTfact/Model/Builder/SAHKdTreeBuilder.hpp>

// =============================================================================
// INTERSECTORS
// =============================================================================

#include <RTfact/Model/Intersector/KdTreeIntersector.hpp>
#include <RTfact/Model/Intersector/BVHIntersector.hpp>
#include <RTfact/Model/Intersector/OrderedBVHIntersector.hpp>
#include <RTfact/Model/Intersector/BVHPartitionIntersector.hpp>
#include <RTfact/Model/Intersector/PlueckerTriangleIntersector.hpp>
#include <RTfact/Model/Intersector/ReshetovTriangleIntersector.hpp>
#include <RTfact/Model/Intersector/RaysterizerTriangleIntersector.hpp>
//#include <RTfact/Model/Intersector/ProjectionTriangleIntersector.hpp>

// =============================================================================
// INTEGRATORS
// =============================================================================

#include <RTfact/Model/Integrator/SimpleIntegrator.hpp>
#include <RTfact/Model/Integrator/EyeLightIntegrator.hpp>
#include <RTfact/Model/Integrator/DirectIlluminationIntegrator.hpp>

// =============================================================================
// Particles
// =============================================================================

#include <RTfact/Model/Particles/VectorVPLStorage.hpp>
#include <RTfact/Model/Particles/BasicParticleGenerator.hpp>
#include <RTfact/Model/Particles/GlobalImportanceParticleFilter.hpp>

// =============================================================================
// CONTROLLERS
// =============================================================================

#include <RTfact/Utils/UI/Controllers/CameraController.hpp>
#include <RTfact/Utils/UI/Controllers/ApplicationController.hpp>

// =============================================================================
// WINDOWS & FRAMEBUFFERS
// =============================================================================

#ifdef DISPLAY_WINDOW
#   include <RTfact/Utils/UI/Windows/SDLGLSLWindow.hpp>
#   include <RTfact/Utils/UI/Windows/SDLGLWindow.hpp>
#endif

#include <RTfact/Model/Framebuffer/Image2DFramebuffer.hpp>

// =============================================================================
// INPUT/OUTPUT
// =============================================================================

#include <RTfact/Utils/IO/Loader.hpp>
#include <RTfact/Utils/IO/Loaders/TriangleMeshOBJLoader.hpp>
#include <RTfact/Utils/IO/Loaders/BasicTriangleSceneOBJXLoader.hpp>
#include <RTfact/Utils/IO/Loaders/Image2DLoaderTGA.hpp>
#include <RTfact/Utils/IO/Loaders/Image2DLoaderPNG.hpp>
#include <RTfact/Utils/IO/Loaders/Image2DLoaderJPEG.hpp>

#include <RTfact/Utils/Populators/BasicTriangleMeshPopulator.hpp>
#include <RTfact/Utils/Populators/OBJSurfaceShaderCreator.hpp>
#include <RTfact/Utils/Populators/OBJMaterialCreator.hpp>
#include <RTfact/Utils/Populators/OBJXEnvironmentTextureCreator.hpp>
#include <RTfact/Utils/Populators/OBJXLightCreator.hpp>

#include <RTfact/Utils/IO/SerializationUtils.hpp>
#include <RTfact/Utils/IO/Configuration.hpp>

// =============================================================================
// MISC
// =============================================================================

#include <RTfact/Utils/FPSMeter.hpp>
#include <RTfact/Utils/StaticInitializer.hpp>
#include <RTfact/Utils/IO/PathUtils.hpp>
#include <RTfact/Utils/Synchronization.hpp>

#include <RTfact/Config/Init.hpp>

// =============================================================================

#pragma endregion // Includes

#define USE_BVH
//#define MEASURE
#define TBB
#define CLASSIC_SHADERS

#if defined(USE_BVH)
#   define PRIMARY_RAY_PACKET_SIZE 256
#else
#   define PRIMARY_RAY_PACKET_SIZE 16
#endif 

#if defined(TBB)
#   define USE_TBB
#endif

#include <tbb/task_scheduler_init.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range2d.h>
#include <tbb/mutex.h>

#if defined(USE_TBB) && defined(RTFACT_OS_WIN)
#   pragma comment(lib, "tbb.lib")
#endif

using namespace RTfact;
using namespace RTfact::UI;

typedef BasicPixelSampler t_PixelSampler;
typedef BasicSurfaceRayTracer t_Renderer;
typedef VectorVPLStorage    t_ParticleStorage;
typedef VectorVPLStorage    t_ImportonStorage;
typedef BasicParticleGenerator<t_ParticleStorage> t_ParticleGenerator;
#if defined(USE_TBB)
  typedef GlobalImportanceParticleFilter<VectorVPLStorage, tbb::mutex>    t_ParticleFilter;
#else
  typedef GlobalImportanceParticleFilter<VectorVPLStorage, NullMutex>    t_ParticleFilter;
#endif

#if defined(USE_BVH)

    //typedef BVH<Triangle> t_Tree;
    typedef OrderedBVH<Triangle> t_Tree;
    typedef SAHBVHBuilder t_Builder;  
    //typedef BVHIntersector<PlueckerTriangleIntersector> t_Intersector;
    typedef BVHPartitionIntersector<PlueckerTriangleIntersector> t_Intersector;
    //typedef OrderedBVHIntersector<PlueckerTriangleIntersector> t_Intersector;

#else

    typedef KdTree<Triangle> t_Tree;
    typedef SAHKdTreeBuilder<t_Tree> t_Builder;
    //typedef SimpleKdTreeBuilder t_Builder;
    typedef KdTreeIntersector<PlueckerTriangleIntersector> t_Intersector;

#endif

//typedef Float3ConstantEnvironmentTexture t_EnvTexture;
typedef Image2DEnvironmentTexture<BasicImage2D<uint8>, BilinearFilterImageSampler> t_EnvTexture;

#ifdef CLASSIC_SHADERS

    struct SceneConfig
    {
        RTFACT_STATIC_CONSTANT(PROPERTIES,
            /*SceneProperties::INVERT_NORMAL |*/
            SceneProperties::HAS_TRANSPARENCY);

        typedef SceneConfig This;

        typedef BasicScene<This, Light, Triangle, t_EnvTexture, PROPERTIES> Scene;
        typedef SurfaceShaderContext<Scene, t_Tree, t_Intersector, t_ParticleGenerator, t_ParticleFilter> Context;
        typedef SurfaceShader<Context> AppearanceBase;
    };

    typedef SceneConfig::Scene t_Scene;
    typedef SceneConfig::Context t_Context;
    typedef SurfaceShaderIntegrator<t_Context> t_Integrator;

#else

    typedef BasicScene<Material, Light, Triangle, t_EnvTexture> t_Scene;
    typedef EyeLightIntegrator t_Integrator;
    //typedef DirectIlluminationIntegrator t_Integrator;
    //typedef SimpleIntegrator t_Integrator;

#endif

typedef PinholeCamera t_Camera;
typedef BasicImage2D<float> t_FramebufferImage;
typedef Image2DFramebuffer<t_FramebufferImage> t_Framebuffer;
typedef SDLGLWindow t_Window;
//typedef SDLGLSLWindow t_Window;

#ifdef USE_TBB

    struct RenderParallel
    {
        t_Renderer& rend;
        t_Camera& cam;
        t_Scene& sc;
        t_Tree& tree;
        t_Intersector& intersector;
        t_PixelSampler& pixelSampler;
        t_Integrator& integrator;
        t_Framebuffer& framebuffer;

    public:
        void operator() (const tbb::blocked_range2d<RTfact::uint> &r) const
        {
            ImageClipRegion clip;
            clip.left = r.cols().begin();
            clip.right = r.cols().end();
            clip.top = r.rows().begin();
            clip.bottom = r.rows().end();

            rend.render<PRIMARY_RAY_PACKET_SIZE>(
                cam, sc, tree, intersector, pixelSampler,
                integrator, framebuffer, clip);
        }

        RenderParallel(          
            t_Renderer& arend,
            t_Camera& acam,
            t_Scene& asc,
            t_Tree& atree,
            t_Intersector& atrav,
            t_PixelSampler& apixelSampler,
            t_Integrator& aintegrator,
            t_Framebuffer& aframebuffer
        ) :
            rend(arend),
            cam(acam),
            sc(asc),
            tree(atree),
            intersector(atrav),
            pixelSampler(apixelSampler),
            integrator(aintegrator),
            framebuffer(aframebuffer)
        {}
    };

#endif

class MainApp
{
public:

    int run(
        IO::Configuration& aConfig,
        const std::string& aConfigDir="")
    {
        #ifdef USE_TBB

            tbb::task_scheduler_init init;

        #endif

        boost::intrusive_ptr<t_Scene> scene;

        if(aConfig.exists("sceneDumpInput"))
        {
            RTFACT_LOG("Loading scene data... ");

            TimeMeasurer m;
            m.start();

            scene = new t_Scene;

            const std::string filePath = configPath(aConfigDir,aConfig["sceneDumpInput"]);

            std::ifstream in(filePath.c_str(), std::ios_base::binary);

            RTFACT_USER_ERROR_IF_F(!in, "Could not open file: %s", filePath);

            IO::Serializer serializer(in);

            serializer >> *scene;

            m.stop();

            RTFACT_LOG_LINE_F("done (%1% seconds).", (m.getTotalTime() / 1000.0));
        }
        else
        {
            RTFACT_LOG("Loading data... ");

            TimeMeasurer m;
            m.start();

            scene = IO::Loader<t_Scene>::load(configPath(aConfigDir,aConfig["scene"]), aConfig);

            m.stop();

            RTFACT_LOG_LINE_F("done (%1% seconds).", (m.getTotalTime() / 1000.0));
        }

        if(aConfig.exists("sceneEx"))
        {
            IO::BasicTriangleSceneOBJXLoader<
                OBJXLightCreator<t_Scene::LightBase>,
                OBJXEnvironmentTextureCreator<t_EnvTexture> >::load(
                    *scene, configPath(aConfigDir,aConfig["sceneEx"]), aConfig);
        }

        if(scene->appearances.size() == 0)
        {
            RTFACT_USER_WARNING("No appearances loaded. Setting default.");

            #ifdef CLASSIC_SHADERS

                scene->appearances.push_back(
                    new PhongShader<t_Context, Float3ConstantTexture>());
            #else
                
                Float3ConstantTexture diffTex(1, 1, 1);
                Float3ConstantTexture ambTex(0.3, 0.3, 0.3);

                scene->appearances.push_back(
                    new LambertianMaterial<Float3ConstantTexture, Float3ConstantTexture>(
                        diffTex, ambTex));

            #endif

            for(RTfact::uint i = 0; i < scene->primitives.size(); ++i)
            {
                scene->primitives[i].appearanceID = 0;
            }
        }

//#define SPONZA_LIGHT__
//#define POINT_LIGHT_
//#define QUAD_LIGHT_
//#define DIRECTIONAL_LIGHT_
//#define SPOT_LIGHT_
//#ifdef SPONZA_LIGHT__
//        scene->lights.clear();
//        QuadLight<QuadraticLightAttenuation> *light =
//            new QuadLight<QuadraticLightAttenuation>(            
//            Vec3f<1>(-31.0f, 22.0f, -5.0f),
//            Vec3f<1>( 31.0f, 22.0f, -5.0f),            
//            Vec3f<1>(-31.0f, 22.0f,  5.0f),
//            Vec3f<1>(4.0f, 4.0f, 4.0f));
//        scene->lights.push_back(light);
//#else
//        scene->lights.clear();
//#   if defined(POINT_LIGHT_)
//        PointLight<QuadraticLightAttenuation> *light = 
//            new PointLight<QuadraticLightAttenuation>(
//            Vec3f<1>(0.0f, 3.995f, 0.0f),
//            Vec3f<1>(10.0f, 10.0f, 10.0f));
//#   elif defined(QUAD_LIGHT_)
//        float hlen = 0.4f;
//        QuadLight<QuadraticLightAttenuation> *light =
//            new QuadLight<QuadraticLightAttenuation>(
//            Vec3f<1>(-hlen, 3.995f, -hlen),
//            Vec3f<1>( hlen, 3.995f, -hlen),
//            Vec3f<1>(-hlen, 3.995f,  hlen),
//            Vec3f<1>(30.0f, 30.0f, 30.0f));
//#   elif defined(DIRECTIONAL_LIGHT_)
//        DirectionalLight *light = new DirectionalLight(
//            Vec3f<1>(0.0f, -1.0f, -0.25f).normalize(),
//            Vec3f<1>(3.0f, 3.0f, 3.0f));
//#   elif defined(SPOT_LIGHT_)
//        BBox worldBounds(Vec3f<1>(0.0f, 0.0f, 0.0f), Vec3f<1>(0.0f, 0.0f, 0.0f));
//        for (uint i=0; i < scene->primitives.size(); ++i)
//            worldBounds.extend(scene->primitives[i].getBounds());
//
//        Vec3f<1> spotPos(0.0f, 3.995f, 0.0f);
//        Vec3f<1> spotTar(0.0f, 0.0f, 0.0f);
//        SpotLight<QuadraticLightAttenuation> *light = 
//            new SpotLight<QuadraticLightAttenuation>(
//            spotPos,
//            Vec3f<1>(10.0f, 10.0f, 10.0f),
//            (spotTar - spotPos).normalize(),
//            45.0f,
//            60.0f,
//            context.primitive.getBounds()
//            );
//#   endif
//        scene->lights.push_back(light);
//
//#endif
        
        //add ies light
        //scene->lights.clear();

        //IESLight<OpenGLLightAttenuation>* iesLight = 
        //    new IESLight<OpenGLLightAttenuation>();
        //iesLight->setPosition(Vec3f1(0,1,0));
        //iesLight->setDirection(Vec3f1(0,-1,0));
        //iesLight->setIntensity(0.2f);
        //iesLight->setColor(Vec3f1(0.8f, 0.7f, 0.17f));        
        //iesLight->setBaseVector(Vec3f1(1,0,0));
        //if(aConfig.exists("iesLight"))
        //{
        //    std::string iesFilePath = aConfig["iesLight"];
        //    if(!IO::isPathAbsolute(iesFilePath))
        //    {
        //        iesFilePath = IO::appendPath(aConfigDir, iesFilePath);
        //    }
        //    iesLight->setFile(iesFilePath);
        //}
        //scene->lights.push_back(iesLight);

        if(scene->lights.size() == 0)
        {
            RTFACT_USER_WARNING("No lights loaded.");
        }

        if(aConfig.exists("sceneDumpOutput"))
        {
            RTFACT_LOG("Dumping scene data... ");

            TimeMeasurer m;
            m.start();

            const std::string filePath = configPath(aConfigDir,aConfig["sceneDumpOutput"]);

            std::ofstream out(filePath.c_str(), std::ios_base::binary);

            RTFACT_USER_ERROR_IF_F(!out, "Could not open file: %s", filePath);

            IO::Serializer serializer(out);

            serializer << *scene;

            m.stop();

            RTFACT_LOG_LINE_F("done (%1% seconds).", (m.getTotalTime() / 1000.0));
        }

        t_Camera camera;
        t_Renderer renderer;
        t_Tree tree;
        t_Builder builder;
        t_Intersector intersector;
        t_PixelSampler pixelSampler;        

        t_ParticleGenerator particleGenerator;
        t_ParticleFilter particleFilter;

        particleGenerator.setLightPathCount(30);
        particleGenerator.setMaxPathLength(10);
        particleGenerator.setClampThreshold(0.f);
        particleGenerator.setClampFactor(0.2f);
        particleGenerator.setClampStartValue(0.05f);
        particleFilter.setImportonCount(100);
        particleFilter.setGBias(0.f);
        particleFilter.setAcceptanceEpsilon(0.08f);
        //particleFilter.setAcceptanceEpsilon(1.08f);
                

        #ifdef CLASSIC_SHADERS

            t_Context context(*scene, tree, intersector, particleGenerator, particleFilter);
            t_Integrator integrator(context);

        #else

            t_Integrator integrator;//(t_EnvTexture(0.5, 0.5, 0));

        #endif

        intersector.init(*scene);

        renderer.init(*scene, tree, intersector, camera);

        if(aConfig.exists("accelStructDumpInput"))
        {
            RTFACT_LOG("Loading acceleration structure data... ");

            TimeMeasurer m;
            m.start();

            const std::string filePath = configPath(aConfigDir,aConfig["accelStructDumpInput"]);

            std::ifstream in(filePath.c_str(), std::ios_base::binary);

            RTFACT_USER_ERROR_IF_F(!in, "Could not open file: %s", filePath);

            IO::Serializer serializer(in);

            serializer >> tree;

            tree.setElementBase(*scene->primitives.begin());

            m.stop();

            RTFACT_LOG_LINE_F("done (%1% seconds).", (m.getTotalTime() / 1000.0));
        }
        else
        {
            RTFACT_LOG("Building acceleration structure... ");

            TimeMeasurer m;
            m.start();

            builder.build(tree, scene->primitives.begin(),
                          scene->primitives.end(),
                          boost::lambda::bind(&Triangle::getBounds, boost::lambda::_1));

            m.stop();

            RTFACT_LOG_LINE_F("done (%1% seconds).", (m.getTotalTime() / 1000.0));

            //int sn, sl, st;
            //tree.getStats(sn, sl, st);
            //RTFACT_LOG_LINE_F("n: %1%, l: %2%, t: %3%.", sn % sl % st);
        }

        if(aConfig.exists("accelStructDumpOutput"))
        {
            RTFACT_LOG("Dumping acceleration structure data... ");

            TimeMeasurer m;
            m.start();

            const std::string filePath = configPath(aConfigDir,aConfig["accelStructDumpOutput"]);

            std::ofstream out(filePath.c_str(), std::ios_base::binary);

            RTFACT_USER_ERROR_IF_F(!out, "Could not open file: %s", filePath);

            IO::Serializer serializer(out);

            serializer << tree;

            m.stop();

            RTFACT_LOG_LINE_F("done (%1% seconds).", (m.getTotalTime() / 1000.0));
        }
        
        for(RTfact::uint i = 0; i < scene->lights.size(); ++i)
        {
            scene->lights[i]->setBoundingSphere(tree.getBounds().getBoundingSphere());
        }
        
        RTFACT_LOG_LINE("Starting rendering...");

        const RTfact::uint resx = aConfig["resX"].as<RTfact::uint>();
        const RTfact::uint resy = aConfig["resY"].as<RTfact::uint>();

        Traits<t_FramebufferImage>::Ptr framebufferImage =
            new t_FramebufferImage(3, resx, resy);

        t_Framebuffer framebuffer;
        framebuffer.setColorImage(framebufferImage);
        //framebuffer.setDistanceImage(new t_FramebufferImage(1, resx, resy));

        t_Camera::Params camParams;

        camParams.position     = aConfig["cameraPos"].as<Vec3f1>();
        camParams.direction    = aConfig["cameraDir"].as<Vec3f1>().normalize();
        camParams.up           = aConfig["cameraUp"].as<Vec3f1>().normalize();
        camParams.openingAngle = aConfig["cameraOpeningAngle"].as<float>();
        camParams.resX         = resx;
        camParams.resY         = resy;

        camera.setParams(camParams);

        #ifdef DISPLAY_WINDOW

            const bool fullScreen = aConfig.exists("fullScreen") ?
                aConfig["fullScreen"].as<bool>() :
                false;

            const bool windowResizable = aConfig.exists("windowResizable") ?
                aConfig["windowResizable"].as<bool>() :
                true;

            bool windowResized = false;

            t_Window window;
            
            window.init(resx, resy, fullScreen, windowResizable);
            window.setCaption(DEFAULT_WINDOW_CAPTION);

            CameraController cameraController(window, tree.getBounds());

            cameraController.setParams(camParams);
            cameraController.applyChanges();
            cameraController.getParams(camParams);

            camera.setParams(camParams);

            ApplicationController appController(window, 2);
            //BasicLightController<t_Scene> sceneController(window, *scene, aConfig);

        #endif

        FPSMeter fpsMeter;

        RTfact::uint stillCameraIterations = 0;
        pixelSampler.setSampleSeed(0.5f, 0.5f, 1.f);

        Vector<t_Camera::Params> camParamsAnim;

        bool playbackMode = aConfig.exists("play");
        bool outputVideoSequenceMode = aConfig.exists("outputVideoSequence");
        RTfact::uint currentParamIndex = 0;

        if(playbackMode)
        {
            const std::string filePath = configPath(aConfigDir,aConfig["animationFile"]);

            std::ifstream in(filePath.c_str());

            RTFACT_USER_ERROR_IF_F(!in, "Could not open file: %s", filePath);

            while(!in.eof())
            {
                t_Camera::Params p = camParams;

                in >> p.position.x >> p.position.y >> p.position.z
                    >> p.direction.x >> p.direction.y >> p.direction.z
                    >> p.up.x >> p.up.y >> p.up.z;

                camParamsAnim.push_back(p);
            }
        }

        for(;;)
        {
            ImageClipRegion clip(
                0, 0, framebufferImage->getResX(), framebufferImage->getResY());

            framebuffer.prePaint();

            fpsMeter.startFrame();

            if(playbackMode)
            {
                if(currentParamIndex == camParamsAnim.size())
                {
                    break;
                }

                camParams = camParamsAnim[currentParamIndex];

                camera.setParams(camParams);

                RTFACT_LOG_LINE_F("Rendering frame: %1%        \r", currentParamIndex);
            }
            
            //particleGenerator.setClampStartValue(10.0f);
            particleFilter.startFrame(camera, context, stillCameraIterations);
            particleGenerator.generateParticles(camera, context, stillCameraIterations);

            #ifdef USE_TBB

                tbb::parallel_for(tbb::blocked_range2d<RTfact::uint>(
                    clip.top, clip.bottom, 64,
                    clip.left, clip.right, 64),
                    RenderParallel(renderer, camera, *scene, tree, intersector,
                                   pixelSampler, integrator, framebuffer));

            #else

                renderer.render<PRIMARY_RAY_PACKET_SIZE>(
                    camera, *scene, tree, intersector,
                    pixelSampler, integrator, framebuffer, clip);

                #ifdef COUNTER_STATS

                    RTFACT_LOG("Counters: ");

                    RTFACT_LOG_F("0: %1%; ", GET_COUNTER(0));
                    RTFACT_LOG_F("1: %1%; ", GET_COUNTER(1));
                    RTFACT_LOG_F("2: %1%; ", GET_COUNTER(2));
                    
                    //RTFACT_LOG("                            \r");

                    RESET_COUNTER(0);
                    RESET_COUNTER(1);
                    RESET_COUNTER(2);

                #endif

                #ifdef TIME_STATS

                    RTFACT_LOG("Timers: ");

                    RTFACT_LOG_F("0: %1%ms; ", GET_TIMER(0));
                    RTFACT_LOG_F("1: %1%ms; ", GET_TIMER(1));
                    
                    //RTFACT_LOG("                            \r");

                    RESET_TIMER(0);
                    RESET_TIMER(1);
                    RESET_TIMER(2);
                    RESET_TIMER(3);

                #endif

                /*RTFACT_LOG_LINE_F("Counter 1: %1%        \r", GET_COUNTER(1));
                RTFACT_LOG_LINE_F("Counter 2: %1%        \r", GET_COUNTER(2));
                RTFACT_LOG_LINE_F("Counter 3: %1%        \r", GET_COUNTER(3));*/

                //RESET_COUNTER(1);
                //RESET_COUNTER(2);
                //RESET_COUNTER(3);

            #endif
            
            particleFilter.endFrame(camera, context, stillCameraIterations);

            framebuffer.postPaint();
            fpsMeter.endFrame();

            RTFACT_LOG_F("FPS: %3.1f           \r", fpsMeter.getFPS());

            #ifdef DISPLAY_WINDOW

                window.update(*framebufferImage);
                window.display();

                window.handleEvents();

                if(!playbackMode)
                {
                    cameraController.applyChanges();

                    cameraController.getParams(camParams);

                    if(appController.getSetting() == 0 ||
                       cameraController.cameraChanging() ||
                       windowResized)
                    {
                        stillCameraIterations = 0;

                        pixelSampler.setSampleSeed(0.5f, 0.5f, 1.0f);
                    }
                    else
                    {
                        ++stillCameraIterations;

                        pixelSampler.setSampleSeed(
                            Math::haltonBase(stillCameraIterations, 2),
                            Math::haltonBase(stillCameraIterations, 3),
                            1.f / (stillCameraIterations + 1));
                    }

                    camera.setParams(camParams);
                }

                cameraController.acceptChanges();

                if(appController.shouldTerminate())
                {
                    break;
                }

                RTfact::uint newResX, newResY;
                windowResized = appController.shouldResize(newResX, newResY);

                if(windowResized)
                {
                    framebufferImage = new t_FramebufferImage(3, newResX, newResY);
                    framebuffer.setColorImage(framebufferImage);

                    window.init(newResX, newResY, fullScreen, windowResizable);

                    camParams.resX = newResX;
                    camParams.resY = newResY;

                    camera.setParams(camParams);
                    cameraController.setParams(camParams);
                }

                appController.acceptChanges();

            #endif

            if(outputVideoSequenceMode)
            {
                std::stringstream s;
                s << configPath(aConfigDir,aConfig["videoSequenceDir"]) << "/" << setw(4) << setfill('0') << currentParamIndex << ".tga";
                IO::Loader<t_FramebufferImage>::save(*framebuffer.getColorImage(), s.str());
            }

            ++currentParamIndex;
        }

        return 0;
    }

protected:
    static std::string configPath(
        const std::string &aConfigDir,
        const IO::Configuration::Entry &aPathEntry)
    {
        if (
            IO::isPathAbsolute(aPathEntry))
        {
            return aPathEntry;
        }
        else
        {
            return IO::appendPath(aConfigDir,aPathEntry);
        }
    }
};

int main(int argc, char** argv)
{
    typedef BasicImage2D<float> t_FloatImage;
    typedef BasicImage2D<uint8> t_ByteImage;

    IO::Loader<t_FloatImage>::registerLoader(".jpg", new IO::Image2DLoaderJPEG<t_FloatImage>);
    IO::Loader<t_ByteImage >::registerLoader(".jpg", new IO::Image2DLoaderJPEG<t_ByteImage>);
    IO::Loader<t_FloatImage>::registerLoader(".png", new IO::Image2DLoaderPNG<t_FloatImage>);
    IO::Loader<t_ByteImage >::registerLoader(".png", new IO::Image2DLoaderPNG<t_ByteImage>);
    IO::Loader<t_FloatImage>::registerLoader(".tga", new IO::Image2DLoaderTGA<t_FloatImage>);
    IO::Loader<t_ByteImage >::registerLoader(".tga", new IO::Image2DLoaderTGA<t_ByteImage>);

    #ifdef CLASSIC_SHADERS

        typedef BasicTriangleMeshPopulator<
                t_Scene, OBJSurfaceShaderCreator<t_Scene::AppearanceBase> >
            t_ScenePopulator;

    #else

        typedef BasicTriangleMeshPopulator<
                t_Scene, OBJMaterialCreator<t_Scene::AppearanceBase> >
            t_ScenePopulator;

    #endif

    IO::Loader<t_Scene>::registerLoader(
        ".obj", new IO::TriangleMeshOBJLoader<t_Scene, t_ScenePopulator>);

    std::string configDir = (argc == 2) ? argv[1] : CONFIG_FILE;
    IO::Configuration *config=NULL;
    try {
        config=new IO::Configuration(configDir);
        RTFACT_LOG_LINE_F("Using configuration file %1%", configDir);
        configDir = IO::getParentDirectory(configDir);
    }
    catch(RTfact::ProgramException& e)
    {
        RTFACT_LOG_LINE(e.getMessage());
        RTFACT_BREAK_CONSOLE();
    }

    if (
        config!=NULL)
    {
        MainApp app;
        int res=app.run(*config,configDir);
        delete config;
        return res;
    }
    else
    {
        return 1;
    }
}
#endif
