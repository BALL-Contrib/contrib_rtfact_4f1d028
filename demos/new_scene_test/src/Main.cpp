/*
 *  Main.cpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2009  Saarland University
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
 *  Created on: 2009-02-24 16:01:56 +0100
 *  Author(s): Felix Klein
 */

 #include "Precompiled.hpp"

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

#define USE_BVH
//#define POINTS
//#define MEASURE
//#define TBB
#define CLASSIC_SHADERS

#if defined(USE_BVH)
#   define PRIMARY_RAY_PACKET_SIZE 64
#elif defined(POINTS)
#   define PRIMARY_RAY_PACKET_SIZE 4
#else
#   define PRIMARY_RAY_PACKET_SIZE 16
#endif

#define DEFAULT_WINDOW_CAPTION   "RTfact demo"
#define DEFAULT_IMAGE_RESOLUTION 512
#define CONFIG_FILE              "RTfact.ini"

#pragma region Includes




// =============================================================================
// SCENES
// =============================================================================

#include <RTfact/Model/Scene/BasicScene.hpp>


// =============================================================================
// NEW SCENE
// =============================================================================

#include <RTfact/Model/Scene/NewGraphScene.hpp>

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
//#include <RTfact/Model/Renderer/BasicPointRayTracer.hpp>

// =============================================================================
// CAMERAS
// =============================================================================

#include <RTfact/Model/Camera/PinholeCamera.hpp>

// =============================================================================
// PIXEL SAMPLERS
// =============================================================================

//#include <RTfact/Model/PixelSampler/AccumulationSampler.hpp>
#include <RTfact/Model/PixelSampler/BasicPixelSampler.hpp>
//#include <RTfact/Model/PixelSampler/PixelCornerSampler.hpp>

// =============================================================================
// MATERIALS
// =============================================================================

#include <RTfact/Model/Material/LambertianMaterial.hpp>

//#include <RTfact/Concept/SurfaceShaderContext.hpp>
#include <RTfact/Model/Integrator/SurfaceShaderIntegrator.hpp>
#include <RTfact/Model/SurfaceShader/PhongShader.hpp>

#include <RTfact/Model/ImageSampler/BasicImageSampler.hpp>

// =============================================================================
// LIGHTS
// =============================================================================

#include <RTfact/Model/Light/PointLight.hpp>
#include <RTfact/Model/Light/DirectionalLight.hpp>
//#include <RTfact/Model/Light/FakePointLight.hpp>

#include <RTfact/Model/LightAttenuation/NoLightAttenuation.hpp>

// =============================================================================
// PRIMITIVES
// =============================================================================

#include <RTfact/Model/Primitive/KdTree.hpp>
#include <RTfact/Model/Primitive/BVH.hpp>
#include <RTfact/Model/Primitive/OrderedBVH.hpp>
#include <RTfact/Model/Primitive/AccelerationInstance.hpp>
//#include <RTfact/Model/Primitive/LoDIndexKdTree.hpp>

// =============================================================================
// BUILDERS
// =============================================================================

#include <RTfact/Model/Builder/SAHBVHBuilder.hpp>
#include <RTfact/Model/Builder/SAHBVHBuilder.hpp>
#include <RTfact/Model/Builder/SAHKdTreeBuilder.hpp>
#include <RTfact/Model/Builder/SimpleKdTreeBuilder.hpp>
//#include <RTfact/Model/Builder/LoDIndexKdTreePointBuilder.hpp>

#include <RTfact/Model/Builder/GraphStructSynchronizer.hpp>
#include <RTfact/Model/Builder/Data/AccelBuildData.hpp>
#include <RTfact/Model/Builder/NewGraphBVHBuilder.hpp>


// =============================================================================
// INTERSECTORS
// =============================================================================

#include <RTfact/Model/Intersector/KdTreeIntersector.hpp>
#include <RTfact/Model/Intersector/BVHIntersector.hpp>
//#include <RTfact/Model/Intersector/OrderedBVHIntersector.hpp>
#include <RTfact/Model/Intersector/BVHPartitionIntersector.hpp>
#include <RTfact/Model/Intersector/PlueckerTriangleIntersector.hpp>
#include <RTfact/Model/Intersector/ReshetovTriangleIntersector.hpp>
#include <RTfact/Model/Intersector/RaysterizerTriangleIntersector.hpp>
#include <RTfact/Model/Intersector/InstanceIntersector.hpp>
//#include <RTfact/Model/Intersector/ProjectionTriangleIntersector.hpp>
//#include <RTfact/Model/Intersector/LODIndexKdTreePixelFrustumIntersector.hpp>
//#include <RTfact/Model/Intersector/PixelFrustumPointIntersector.hpp>

// =============================================================================
// INTEGRATORS
// =============================================================================

#include <RTfact/Model/Integrator/SimpleIntegrator.hpp>
#include <RTfact/Model/Integrator/EyeLightIntegrator.hpp>
#include <RTfact/Model/Integrator/DirectIlluminationIntegrator.hpp>

// =============================================================================
// CONTROLLERS
// =============================================================================

#include <RTfact/Utils/UI/Controllers/CameraController.hpp>
#include <RTfact/Utils/UI/Controllers/ApplicationController.hpp>
#include <RTfact/Utils/UI/Controllers/BasicLightController.hpp>

// =============================================================================
// WINDOWS & FRAMEBUFFERS
// =============================================================================

#include <RTfact/Utils/UI/Windows/SDLGLSLWindow.hpp>
#include <RTfact/Utils/UI/Windows/SDLGLWindow.hpp>

#include <RTfact/Model/Framebuffer/BasicComponent3Framebuffer.hpp>

// =============================================================================
// INPUT/OUTPUT
// =============================================================================

#include <RTfact/Utils/IO/Loader.hpp>
#include <RTfact/Utils/IO/Loaders/TriangleMeshOBJLoader.hpp>
#include <RTfact/Utils/IO/Loaders/BasicTriangleSceneOBJXLoader.hpp>
#include <RTfact/Utils/IO/Loaders/Image2DTGALoader.hpp>

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

#include <RTfact/Config/Init.inc.cpp>

// =============================================================================

#if defined(TBB)
#   define USE_TBB
#endif

#include <tbb/task_scheduler_init.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range2d.h>

#if defined(USE_TBB) && defined(RTFACT_OS_WIN)
#   pragma comment(lib, "tbb.lib")
#endif

#pragma endregion // Includes

using namespace RTfact;
using namespace RTfact::UI;

#ifdef POINTS

    typedef BasicPointRayTracer<PixelCornerSampler> t_Renderer;
    typedef BasicScene<Point> t_Scene;

#else

    typedef AccumulationSampler t_PixelSampler;
    typedef BasicSurfaceRayTracer t_Renderer;

#endif

#if defined(USE_BVH)

    //typedef BVH<Triangle> t_Tree;
    typedef OrderedBVH<Triangle> t_Tree;
    typedef SAHBVHBuilder t_Builder;
    //typedef BVHIntersector<RaysterizerTriangleIntersector> t_Intersector;
    typedef BVHPartitionIntersector<PlueckerTriangleIntersector> t_Intersector;
    //typedef OrderedBVHIntersector<PlueckerTriangleIntersector> t_Intersector;

#elif defined(POINTS)

    typedef LoDIndexKdTree t_Tree;
    typedef LoDIndexKdTreePointBuilder<> t_Builder;
    typedef LODIndexKdTreePixelFrustumIntersector<PixelFrustumPointIntersector> t_Intersector;

#else

    typedef KdTree<Triangle> t_Tree;
    typedef SAHKdTreeBuilder<t_Tree> t_Builder;
    //typedef SimpleKdTreeBuilder t_Builder;
    typedef KdTreeIntersector<PlueckerTriangleIntersector> t_Intersector;

#endif

typedef GraphStructSynchronizer<t_Builder> t_Synchronizer;


#ifndef POINTS

    //typedef Float3ConstantEnvironmentTexture t_EnvTexture;
    typedef Component3ImageEnvironmentTexture<BasicImage2D<3, uint8>, BilinearFilterImageSampler> t_EnvTexture;

    struct TestSceneConfig
    {
        typedef TestSceneConfig                                         This;
        typedef NewGraphScene< This, Light, t_EnvTexture>               Scene;
        typedef Mesh<This>                                              Mesh;
        typedef Instance<This>                                          Instance;
        typedef Group<This>                                             Group;
        typedef Triangle                                                Primitive;

        typedef AccelerationInstance< t_Tree, Instance >                AccelInstance;
        typedef OrderedBVH<AccelInstance>                               TestTree;
        typedef BVHPartitionIntersector<InstanceIntersector<AccelInstance,
                BVHPartitionIntersector<PlueckerTriangleIntersector> > >    TestIntersector;

        typedef SurfaceShaderContext<Scene, TestTree, TestIntersector>  Context;
        typedef SurfaceShader<Context>                                  AppearanceBase;
    };

    typedef TestSceneConfig::Scene                      t_TestScene;
    typedef TestSceneConfig::Group                      t_Group;
    typedef Traits<TestSceneConfig::Group>::Ptr         t_GroupHandle;
    typedef TestSceneConfig::Mesh                       t_Mesh;
    typedef Traits<TestSceneConfig::Mesh>::Ptr          t_MeshHandle;
    typedef TestSceneConfig::Instance                   t_Instance;
    typedef Traits<TestSceneConfig::Instance>::Ptr      t_InstanceHandle;
    typedef TestSceneConfig::AccelInstance              t_AccelInstance;
    typedef TestSceneConfig::TestTree                   t_TestTree;
    typedef TestSceneConfig::TestIntersector            t_TestIntersector;
    typedef TestSceneConfig::Context                    t_TestContext;
    typedef SurfaceShaderIntegrator<t_TestContext>      t_TestIntegrator;

    typedef AccelBuildData<t_Tree, t_AccelInstance>     t_AccelData;
    typedef NewGraphBVHBuilder                          t_TestBuilder;





    #ifdef CLASSIC_SHADERS

        struct SceneConfig
        {
            typedef SceneConfig This;

            typedef BasicScene<This, Light, Triangle, t_EnvTexture> Scene;
            typedef SurfaceShaderContext<Scene, t_Tree, t_Intersector> Context;
            typedef SurfaceShader<Context> AppearanceBase;
        };

        typedef SceneConfig::Scene t_Scene;
        typedef SceneConfig::Context t_Context;
        typedef SurfaceShaderIntegrator<t_Context> t_Integrator;


    #else

        typedef BasicScene<Material, Light, Triangle, t_EnvTexture> t_Scene;
        typedef EyeLightIntegrator<t_EnvTexture> t_Integrator;
        //typedef DirectIlluminationIntegrator<t_EnvTexture> t_Integrator;
        //typedef SimpleIntegrator t_Integrator;

    #endif

#endif

typedef PinholeCamera t_Camera;
typedef BasicImage2D<3, float> t_FramebufferImage;
typedef BasicComponent3Framebuffer<t_FramebufferImage> t_Framebuffer;
//typedef SDLGLWindow t_Window;
typedef SDLGLSLWindow t_Window;

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


    void sceneGraphTests(t_TestScene *aScene, t_TestTree &tree, t_AccelData &data)
    {
        // Primitives:
        Triangle t1(Vec3f<1>(1,0,0),Vec3f<1>(0,0,1),Vec3f<1>(0,0,0));
        t1.appearanceID = 1;
        t1.updateNormal();
        Triangle t2(Vec3f<1>(1,0,0),Vec3f<1>(0,1,0),Vec3f<1>(0,0,0));
        t2.appearanceID = 1;
        t2.updateNormal();
        Triangle t3(Vec3f<1>(0,0,1),Vec3f<1>(0,1,0),Vec3f<1>(0,0,0));
        t3.appearanceID = 1;
        t3.updateNormal();
        Triangle t4(Vec3f<1>(0,0,1),Vec3f<1>(0,1,0),Vec3f<1>(0,1,0));
        t4.appearanceID = 1;
        t4.updateNormal();

        // Create Meshes
        t_MeshHandle mesh1 = new t_Mesh( aScene );
        mesh1->addPrimitive(t1);
        mesh1->addPrimitive(t2);
        mesh1->addPrimitive(t3);
        mesh1->addPrimitive(t4);

        // Create Root
        t_GroupHandle root = new t_Group( aScene, Transform::identity() );
        aScene->setRoot( root );

        //--------------------------------------------------------
        // Test 1
        // Create scaled group with two meshes and connect to root
        //--------------------------------------------------------
        t_GroupHandle group1 = new t_Group(aScene, Transform::identity() );
        t_InstanceHandle inst1_1 = group1->add(mesh1);
        root->add(group1);

        std::cout<<"\nTest 1-----------------------------------\n";
        aScene->print_content();
        //--------------------------------------------------------
        // Test 2
        // Create local Mesh and Group. Add to graph
        //--------------------------------------------------------
        {
            t_GroupHandle group2 = new t_Group(aScene, Transform::translation(3.0, 0.0, 0.0)
                * Transform::rotationX(0.5) * Transform::scale(Vec3f1(1.0, 3.0, 0.5)));
            t_InstanceHandle inst3_1 = group2->add(mesh1);
            root->add(group2);
        }
        std::cout<<"\nTest 2-----------------------------------\n";
        aScene->print_content();
        //-------------------------------------------------------
        // Test 3
        // Update Bounds o_ô
        //--------------------------------------------------------
        aScene->update();
        std::cout<<"\nTest 3-----------------------------------\n";
        aScene->print_content();

        //--------------------------------------------------------
        // Test 4
        // Remove all Groups from root.
        //--------------------------------------------------------
        //while(root->mGroups.size()>0)
        //    root->remove(root->mGroups[0]);
        std::cout<<"\nTest 4-----------------------------------\n";
        aScene->print_content();

        //--------------------------------------------------------
        // Test 5
        // Add Group 1 again + updateBounds
        //--------------------------------------------------------
        //root->add(group1);
        aScene->update();

        std::cout<<"\nTest 5-----------------------------------\n";
        aScene->print_content();

        // --- Builder inner Structures


        t_Synchronizer sync;
        t_TestBuilder builder;

        sync.build(*aScene, data);
        builder.build(tree, *aScene, data);
    }


    int run(
        IO::Configuration& aConfig)
    {
        #ifdef USE_TBB

            tbb::task_scheduler_init init;

        #endif

        boost::intrusive_ptr<t_Scene> scene;
        boost::intrusive_ptr<t_TestScene> testScene;

        testScene = new t_TestScene();


        if(aConfig.exists("sceneDumpInput"))
        {
            RTFACT_LOG("Loading scene data... ");

            TimeMeasurer m;
            m.start();

            scene = new t_Scene;

            const std::string filePath = aConfig["sceneDumpInput"];

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

            scene = IO::Loader<t_Scene>::load(aConfig["scene"], aConfig);

            m.stop();

            RTFACT_LOG_LINE_F("done (%1% seconds).", (m.getTotalTime() / 1000.0));
        }

        if(aConfig.exists("sceneEx"))
        {
            IO::BasicTriangleSceneOBJXLoader<
                OBJXLightCreator<t_Scene::LightBase>,
                OBJXEnvironmentTextureCreator<t_EnvTexture> >::load(
                    *scene, aConfig["sceneEx"], aConfig);
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

        if(scene->lights.size() == 0)
        {
            RTFACT_USER_WARNING("No lights loaded.");
        }

        /*
        if(aConfig.exists("fakeLightPos"))
        {
            scene->lights.push_back(
                new FakePointLight<NoLightAttenuation>(
                    aConfig["fakeLightPos"].as<Vec3f1>(),
                    aConfig["fakeLightInt"].as<Vec3f1>()));
        }
        */
        typedef PhongShader<t_TestContext, Component3ImageTexture2D<BasicImage2D<3, float>, BasicImageSampler > > t_PhongShader;
        t_PhongShader* shader = new t_PhongShader();
        shader->setDiffuseColor(Vec3f1(1.0f, 1.0f, 1.0f));
        shader->setAmbientIntensity(Vec3f1(0.3f, 0.3f, 0.3f));
        testScene->appearances.push_back(shader);
        testScene->appearances.push_back(shader);

        //scene->lights.push_back(new DirectionalLight(Vec3f1(-0.2f, -1.f, -0.2f), Vec3f1(0.5f, 0.5f, 0.5f)));

        RTFACT_LOG_LINE("done.");

        if(aConfig.exists("sceneDumpOutput"))
        {
            RTFACT_LOG("Dumping scene data... ");

            TimeMeasurer m;
            m.start();

            const std::string filePath = aConfig["sceneDumpOutput"];

            std::ofstream out(filePath.c_str(), std::ios_base::binary);

            RTFACT_USER_ERROR_IF_F(!out, "Could not open file: %s", filePath);

            IO::Serializer serializer(out);

            serializer << *scene;

            m.stop();

            RTFACT_LOG_LINE_F("done (%1% seconds).", (m.getTotalTime() / 1000.0));
        }

        t_Camera camera;
        t_Renderer renderer;
        t_TestTree tree;
        t_AccelData data;

        t_Builder builder;
        t_TestIntersector intersector;
        t_PixelSampler pixelSampler;

        sceneGraphTests(&*testScene, tree, data);

        #ifdef CLASSIC_SHADERS

            t_TestContext context(*testScene, tree, intersector);
            t_TestIntegrator integrator(context);

        #else

            t_Integrator integrator;//(t_EnvTexture(0.5, 0.5, 0));

        #endif

        #if defined(POINTS)
            builder.init(aConfig["levelsOfDetail"].as<int>(),
                         aConfig["maxLoDPoints"].as<int>(),
                         aConfig["maxTreeDepth"].as<int>(),
                         aConfig["maxLeafPoints"].as<int>(),
                         aConfig["maxPointDist"].as<float>());
            intersector.init(aConfig["frustumExtentFactor"].as<float>(), *scene);
        #else
            intersector.init(*testScene);
        #endif

        renderer.init(*testScene, tree, intersector, camera);
        /*
        if(aConfig.exists("accelStructDumpInput"))
        {
            RTFACT_LOG("Loading acceleration structure data... ");

            TimeMeasurer m;
            m.start();

            const std::string filePath = aConfig["accelStructDumpInput"];

            std::ifstream in(filePath.c_str(), std::ios_base::binary);

            RTFACT_USER_ERROR_IF_F(!in, "Could not open file: %s", filePath);

            IO::Serializer serializer(in);

            serializer >> tree;

            #ifndef POINTS
                tree.setElementBase(*scene->primitives.begin());
            #endif

            m.stop();

            RTFACT_LOG_LINE_F("done (%1% seconds).", (m.getTotalTime() / 1000.0));
        }
        else
        {
            RTFACT_LOG("Building acceleration structure... ");

            TimeMeasurer m;
            m.start();

            #if defined(POINTS)
                builder.build(tree, *scene);
            #else
                builder.build(tree, scene->primitives.begin(),
                              scene->primitives.end(),
                              boost::lambda::bind(&Triangle::getBounds, boost::lambda::_1));
            #endif

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

            const std::string filePath = aConfig["accelStructDumpOutput"];

            std::ofstream out(filePath.c_str(), std::ios_base::binary);

            RTFACT_USER_ERROR_IF_F(!out, "Could not open file: %s", filePath);

            IO::Serializer serializer(out);

            serializer << tree;

            m.stop();

            RTFACT_LOG_LINE_F("done (%1% seconds).", (m.getTotalTime() / 1000.0));
        }
        */

        RTFACT_LOG_LINE("Starting rendering...");

        const RTfact::uint resx = aConfig["resX"].as<RTfact::uint>();
        const RTfact::uint resy = aConfig["resY"].as<RTfact::uint>();

        Traits<t_FramebufferImage>::Ptr framebufferImage = new t_FramebufferImage(resx, resy);
        t_Framebuffer framebuffer(framebufferImage);

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

            ApplicationController appController(window);
            //BasicLightController<t_Scene> sceneController(window, *scene, aConfig);

        #endif

        FPSMeter fpsMeter;

        RTfact::uint stillCameraIterations = 0;
        pixelSampler.setSampleSeed(0.5f, 0.5f, 1.f);

        RTfact::uint currentFrameNumber = 1;

        Vector<t_Camera::Params> camParamsAnim;

        bool playbackMode = aConfig.exists("play");
        bool outputVideoSequenceMode = aConfig.exists("outputVideoSequence");
        RTfact::uint currentParamIndex = 0;

        if(playbackMode)
        {
            const std::string filePath = aConfig["animationFile"];

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

            #ifdef USE_TBB

                tbb::parallel_for(tbb::blocked_range2d<RTfact::uint>(
                    clip.top, clip.bottom, 64,
                    clip.left, clip.right, 64),
                    RenderParallel(renderer, camera, *scene, testAccelData.mInnerStructs[0], intersector,
                                   pixelSampler, integrator, framebuffer));

            #else

                renderer.render<PRIMARY_RAY_PACKET_SIZE>(
                    camera, *testScene, tree, intersector,
                    pixelSampler, integrator, framebuffer, clip);

                #ifdef COUNTER_STATS

                    RTFACT_LOG("Counters: ");

                    RTFACT_LOG_F("0: %1%; ", GET_COUNTER(0));
                    RTFACT_LOG_F("1: %1%; ", GET_COUNTER(1));

                    RTFACT_LOG("                            \r");

                    RESET_COUNTER(1);
                    RESET_COUNTER(2);

                #endif

                #ifdef TIME_STATS

                    RTFACT_LOG("Timers: ");

                    RTFACT_LOG_F("0: %1%ms; ", GET_TIMER(0));
                    RTFACT_LOG_F("1: %1%ms; ", GET_TIMER(1));

                    RTFACT_LOG("                            \r");

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

            framebuffer.postPaint();
            fpsMeter.endFrame();

            //RTFACT_LOG_F("FPS: %3.1f           \r", fpsMeter.getFPS());
            //RTFACT_LOG_F("FPS: %1%           \r", fpsMeter.getFPS());

            #ifdef DISPLAY_WINDOW

                window.update(*framebufferImage);
                window.display();

                window.handleEvents();

                if(!playbackMode)
                {
                    cameraController.applyChanges();

                    cameraController.getParams(camParams);

                    if(!appController.accumulationEnabled() ||
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
                            1.f / stillCameraIterations);
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
                    framebufferImage = new t_FramebufferImage(newResX, newResY);
                    framebuffer = t_Framebuffer(framebufferImage);

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
                s << aConfig["videoSequenceDir"] << "/" << setw(4) << setfill('0') << currentParamIndex << ".tga";
                IO::Loader<t_FramebufferImage>::save(*framebuffer.getImage(), s.str());
            }

            ++currentParamIndex;
        }

        return 0;
    }
};

int main(int argc, char** argv)
{
    typedef BasicImage2D<3, float> t_Float3Image;
    typedef BasicImage2D<3, uint8> t_Byte3Image;

    IO::Loader<t_Float3Image>::registerLoader(".tga", new IO::Image2DTGALoader<t_Float3Image>);
    IO::Loader<t_Byte3Image>::registerLoader(".tga", new IO::Image2DTGALoader<t_Byte3Image>);

    #ifdef CLASSIC_SHADERS

        typedef BasicTriangleMeshPopulator<
                t_Scene,
                OBJSurfaceShaderCreator<
                    t_Scene::AppearanceBase,
                    IO::Image2DTGALoader<t_Float3Image> > >
            t_ScenePopulator;

    #else

        typedef BasicTriangleMeshPopulator<
                t_Scene,
                OBJMaterialCreator<
                    t_Scene::AppearanceBase,
                    IO::Image2DTGALoader<t_Float3Image> > >
            t_ScenePopulator;

    #endif

    IO::Loader<t_Scene>::registerLoader(
        ".obj", new IO::TriangleMeshOBJLoader<t_Scene, t_ScenePopulator>);

    IO::Configuration config;

    try
    {
        config.loadFromFile(CONFIG_FILE);
    }
    catch(RTfact::ProgramException& e)
    {
        RTFACT_LOG_LINE(e.getMessage());
        RTFACT_BREAK_CONSOLE();
    }

    MainApp app;
    int i = app.run(config);
    return i;
}
