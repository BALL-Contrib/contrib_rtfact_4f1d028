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
 *  Created on: 2008-11-16 20:24:13 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #include "Precompiled.hpp"

// =============================================================================
// SCENES
// =============================================================================

#include <RTfact/Model/Scene/BasicScene.hpp>

// =============================================================================
// RENDERERS
// =============================================================================

#include <RTfact/Model/Renderer/BasicSurfaceRayTracer.hpp>
#include <RTfact/Model/Renderer/RasterizationRenderer.hpp>
#include <RTfact/Model/Renderer/OpenGLRenderer.hpp>

// =============================================================================
// CAMERAS
// =============================================================================

#include <RTfact/Model/Camera/PinholeCamera.hpp>
#include <RTfact/Model/Camera/RasterizationCamera.hpp>

// =============================================================================
// PIXEL SAMPLERS
// =============================================================================

#include <RTfact/Model/PixelSampler/BasicPixelSampler.hpp>
#include <RTfact/Model/PixelSampler/AccumulationSampler.hpp>

// =============================================================================
// PRIMITIVES
// =============================================================================

#include <RTfact/Model/Primitive/BVH.hpp>
#include <RTfact/Model/Primitive/Triangle.hpp>

// =============================================================================
// BUILDERS
// =============================================================================

#include <RTfact/Model/Builder/SAHBVHBuilder.hpp>

// =============================================================================
// INTERSECTORS
// =============================================================================

#include <RTfact/Model/Intersector/BVHIntersector.hpp>
#include <RTfact/Model/Intersector/PlueckerTriangleIntersector.hpp>

// =============================================================================
// SHADING
// =============================================================================

#include <RTfact/Concept/Light.hpp>
#include <RTfact/Model/Integrator/SimpleIntegrator.hpp>

#include <RTfact/Model/Image/BasicImage2D.hpp>
#include <RTfact/Model/EnvironmentTexture/Float3ConstantEnvironmentTexture.hpp>

// =============================================================================
// CONTROLLERS
// =============================================================================

#include <RTfact/Utils/UI/Controllers/CameraController.hpp>
#include <RTfact/Utils/UI/Controllers/ApplicationController.hpp>

// =============================================================================
// WINDOWS & FRAMEBUFFERS
// =============================================================================

#include <RTfact/Utils/UI/Windows/SDLGLWindow.hpp>

#include <RTfact/Model/Framebuffer/BasicComponent3Framebuffer.hpp>

// =============================================================================
// INPUT/OUTPUT
// =============================================================================

#include <RTfact/Utils/IO/Loader.hpp>
#include <RTfact/Utils/IO/Loaders/TriangleMeshOBJLoader.hpp>
#include <RTfact/Utils/IO/Loaders/Image2DLoaderTGA.hpp>

#include <RTfact/Utils/Populators/BasicTriangleMeshPopulator.hpp>
#include <RTfact/Utils/Populators/OBJMaterialCreator.hpp>

#include <RTfact/Utils/IO/SerializationUtils.hpp>
#include <RTfact/Utils/IO/Configuration.hpp>

// =============================================================================
// MISC
// =============================================================================

#include <RTfact/Utils/FPSMeter.hpp>
#include <RTfact/Utils/StaticInitializer.hpp>
#include <RTfact/Utils/System/TimeMeasurer.hpp>

// For RTfact initialization - should be included only once and in the CPP that
// defines the main() function
#include <RTfact/Config/Init.inc.cpp>

// =============================================================================

using namespace RTfact;

// Scene
typedef BasicScene<Material, Light, Triangle, Float3ConstantEnvironmentTexture>
    t_Scene;

// Renderers
typedef BasicSurfaceRayTracer                           t_RTRenderer;
typedef RasterizationRenderer                           t_RasterizationRenderer;
typedef OpenGLRenderer                                  t_HardwareRenderer;

// Cameras
typedef PinholeCamera                                   t_RTCamera;
typedef RasterizationCamera                             t_RasterizationCamera;

// Ray tracing components
typedef AccumulationSampler                             t_PixelSampler;
typedef BVH<Triangle>                                   t_Tree;
typedef SAHBVHBuilder                                   t_Builder;  
typedef BVHIntersector<PlueckerTriangleIntersector>     t_Intersector;
typedef SimpleIntegrator                                t_Integrator;

// Framebuffer and window
typedef BasicImage2D<3, float>                          t_FramebufferImage;
typedef BasicComponent3Framebuffer<t_FramebufferImage>  t_Framebuffer;
typedef UI::SDLGLWindow                                 t_Window;

class MainApp
{
public:

    int run(
        IO::Configuration& aConfig)
    {
        const RTfact::uint resx = aConfig["resX"].as<RTfact::uint>();
        const RTfact::uint resy = aConfig["resY"].as<RTfact::uint>();

        const bool fullScreen = aConfig.exists("fullScreen") ?
            aConfig["fullScreen"].as<bool>() :
            false;

        // =====================================================================
        // Create basic components
        // =====================================================================

        Traits<t_Scene>::Ptr scene;

        t_RTCamera              rtCamera;
        t_RTRenderer            rtRenderer;

        t_RasterizationCamera   rasterCamera;
        t_RasterizationRenderer rasterRenderer;
        t_HardwareRenderer      hardwareRenderer;

        t_Tree                  rtTree;
        t_Builder               rtBuilder;
        t_Intersector           rtIntersector;
        t_PixelSampler          rtPixelSampler;
        t_Integrator            integrator;

        t_Window                window;
        t_Framebuffer           framebuffer(new t_FramebufferImage(resx, resy));

        // =====================================================================
        // Load the scene
        // =====================================================================

        RTFACT_LOG("Loading scene data... ");

        scene = IO::Loader<t_Scene>::load(aConfig["scene"], aConfig);

        RTFACT_LOG_LINE("done.");

        // =====================================================================
        // Build acceleration structure for ray tracing
        // =====================================================================

        RTFACT_LOG("Building ray tracing acceleration structure... ");

        TimeMeasurer buildTimeMeasurer;
        buildTimeMeasurer.start();

        rtBuilder.build(rtTree, scene->primitives.begin(),
            scene->primitives.end(),
            boost::lambda::bind(&Triangle::getBounds, boost::lambda::_1));

        buildTimeMeasurer.stop();

        RTFACT_LOG_LINE_F("done (%1% seconds).",
                          (buildTimeMeasurer.getTotalTime() / 1000.0));

        // =====================================================================
        // Initialize window, cameras, and controllers
        // =====================================================================
        
        window.init(resx, resy, fullScreen);
        window.setCaption("RTfact rasterization demo");

        UI::CameraController cameraController(window, rtTree.getBounds());

        RasterizationCamera::Params camParams;

        camParams.position     = aConfig["cameraPos"].as<Vec3f1>();
        camParams.direction    = aConfig["cameraDir"].as<Vec3f1>().normalize();
        camParams.up           = aConfig["cameraUp"].as<Vec3f1>().normalize();
        camParams.openingAngle = aConfig["cameraOpeningAngle"].as<float>();
        camParams.resX         = aConfig["resX"].as<uint>();
        camParams.resY         = aConfig["resY"].as<uint>();

        camParams.nearPlane    = aConfig.exists("cameraNearPlane") ?
                                    aConfig["cameraNearPlane"].as<float>() :
                                    RTFACT_EPSILON;

        camParams.farPlane     = aConfig.exists("cameraFarPlane") ?
                                    aConfig["cameraFarPlane"].as<float>() :
                                    RTFACT_FLOAT_MAX;

        cameraController.setParams(camParams);

        rasterCamera.setParams(camParams);
        rtCamera.setParams(camParams);

        UI::ApplicationController appController(window, 3);

        FPSMeter fpsMeter;

        // =====================================================================
        // Initialize rendering components
        // =====================================================================

        rtIntersector.init(*scene);
        rtRenderer.init(*scene, rtTree, rtIntersector, rtCamera);

        rasterRenderer.init(rasterCamera, *scene);
        hardwareRenderer.init(rasterCamera, *scene);

        // =====================================================================
        // Start rendering loop
        // =====================================================================

        RTFACT_LOG_LINE("Starting rendering...");
        
        for(;;)
        {
            ImageClipRegion clip(
                0, 0,
                framebuffer.getImage()->getResX(),
                framebuffer.getImage()->getResY());

            framebuffer.prePaint();
            fpsMeter.startFrame();

            switch(appController.getSetting())
            {
            case 0:

                rasterRenderer.render(rasterCamera, *scene, framebuffer, clip);
                break;

            case 1:

                rtRenderer.render<256>(
                    rtCamera, *scene, rtTree, rtIntersector,
                    rtPixelSampler, integrator, framebuffer, clip);
                break;

            case 2:

                hardwareRenderer.render(rasterCamera, *scene);
                break;
            }

            framebuffer.postPaint();
            fpsMeter.endFrame();

            RTFACT_LOG_F("FPS: %3.1f           \r", fpsMeter.getFPS());

            if(appController.getSetting() != 2)
            {
                window.update(*framebuffer.getImage());
            }

            window.display();
            
            window.handleEvents();

            cameraController.applyChanges();
            cameraController.getParams(camParams);
            cameraController.acceptChanges();

            rtCamera.setParams(camParams);
            rasterCamera.setParams(camParams);

            if(appController.shouldTerminate())
            {
                break;
            }

            RTfact::uint newResX, newResY;
            
            if(appController.shouldResize(newResX, newResY))
            {
                framebuffer.setImage(new t_FramebufferImage(newResX, newResY));

                window.init(newResX, newResY, fullScreen);

                camParams.resX = newResX;
                camParams.resY = newResY;

                rtCamera.setParams(camParams);
                rasterCamera.setParams(camParams);
                cameraController.setParams(camParams);
            }

            appController.acceptChanges();
        }

        return 0;
    }
};

int main(int argc, char** argv)
{
    typedef BasicImage2D<3, float> t_Float3Image;

    typedef BasicTriangleMeshPopulator<
                t_Scene,
                OBJMaterialCreator<
                    t_Scene::AppearanceBase,
                    IO::Image2DLoaderTGA<t_Float3Image> > >
            t_ScenePopulator;

    IO::Loader<t_Scene>::registerLoader(
        ".obj", new IO::TriangleMeshOBJLoader<t_Scene, t_ScenePopulator>);

    IO::Loader<BasicImage2D<3, float> >::registerLoader(
        ".tga", new IO::Image2DLoaderTGA<BasicImage2D<3, float> >);

    IO::Configuration config;

    try
    {
        config.loadFromFile("RTfact.ini");
    }
    catch(RTfact::ProgramException& e)
    {
        RTFACT_LOG_LINE(e.getMessage());

        RTFACT_BREAK_CONSOLE();
    }

    MainApp app;
    return app.run(config);
}
