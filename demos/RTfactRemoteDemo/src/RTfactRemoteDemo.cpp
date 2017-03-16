/*
 *  RTfactRemoteDemo.cpp
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
 *  Author(s): Felix Klein, Ken Dahm, Lukas Marsalek, Iliyan Georgiev, Vaclav Krajicek, Petr Kmoch
 */


#ifndef EXCLUDE_FROM_BUILD

#include <RTremote/Renderer.hpp>
#include <RTremote/Picking.hpp>

#ifdef USE_CGFX_POSTPROCESSOR
    #include <RTremote/Postprocessor.hpp>
    #include <RTfact/Utils/UI/Windows/SDLGLPostprocessorWindow.hpp>
#else
    #include<RTfact/Utils/UI/Windows/SDLGLWindow.hpp>
#endif

#include <RTfact/Config/Init.hpp>

#include <RTfact/Utils/IO/Loaders/Image2DLoaderTGA.hpp>

#include <RTfact/Utils/Containers/IntMap.hpp>

#include <RTfact/Model/Image/BasicImage2D.hpp>
#include <RTfact/Model/Framebuffer/Image2DFramebuffer.hpp>
#include <RTfact/Model/Camera/PinholeCamera.hpp>

#include <RTfact/Utils/FPSMeter.hpp>
#include <RTfact/Utils/EnvLightGen.hpp>

#include <RTfact/Utils/UI/Controllers/PickingController.hpp>
#include <RTfact/Utils/UI/Controllers/CameraController.hpp>
#include <RTfact/Utils/UI/Controllers/ApplicationController.hpp>
#include <RTfact/Utils/IO/PathUtils.hpp>
#include <RTfact/Utils/IO/Loaders/TriangleMeshOBJLoader.hpp>
#include <RTfact/Utils/Populators/BasicTriangleMeshPopulator.hpp>
#include <RTfact/Utils/Populators/OBJMaterialCreator.hpp>
#include <RTfact/Model/Primitive/Triangle.hpp>
#include <RTfact/Model/Scene/BasicScene.hpp>

#include <RTfact/Concept/Appearance.hpp>
#include <RTfact/Concept/EnvironmentTexture.hpp>
#include <RTfact/Concept/Primitive.hpp>
#include <RTfact/Concept/Light.hpp>

#include <RTfact/Model/PixelSampler/BasicPixelSampler.hpp>

//using namespace RTfact;
using namespace RTfact::UI;
using namespace RTfact::IO;
using namespace RTfact::Remote;
using namespace boost;

typedef RTfact::PinholeCamera                                       t_Camera;
typedef RTfact::BasicImage2D<t_FrameBufferComponent>                t_FramebufferImage;
typedef RTfact::BasicImage2D<t_DistanceBufferComponent>             t_FramebufferDistanceImage;
typedef RTfact::BasicImage2D<t_NormalBufferComponent>               t_FramebufferNormalImage;
typedef RTfact::BasicImage2D<t_AmbientOcclusionBufferComponent>     t_AmbientOcclusionImage;
typedef RTfact::BasicImage2D<t_ColorBleedingBufferComponent>        t_ColorBleedingImage;
typedef RTfact::Image2DFramebuffer<
    t_FramebufferImage,
    t_FramebufferDistanceImage,
    t_FramebufferNormalImage,
    t_AmbientOcclusionImage,
    t_ColorBleedingImage>														    t_Framebuffer;

#ifdef USE_CGFX_POSTPROCESSOR
    typedef SDLGLPostprocessorWindow<Postprocessor>                                 t_Window;
#else
    typedef SDLGLWindow                                                             t_Window;
#endif

typedef PickingController															t_PickingController;

// -------------------------------------------
// Global Materials
// -------------------------------------------
std::vector<Triangle> triangleFaces;
std::vector<Triangle> groundFaces;
std::vector<Triangle> wallFaces;
std::vector<Triangle> cubeFaces;
std::vector<Triangle> cornerFaces;

GroupHandle cursorGroup;

// -------------------------------------------
// initialisation functions
// -------------------------------------------
void initRenderer(Configuration& aConfig, Renderer** aRenderer)
{
    std::stringstream configStream;
    if(aConfig.exists(Renderer::StartupParams::PARAM_NAME__NUMBER_OF_RENDER_THREADS))
    {
        configStream << Renderer::StartupParams::PARAM_NAME__NUMBER_OF_RENDER_THREADS << "=" << aConfig[Renderer::StartupParams::PARAM_NAME__NUMBER_OF_RENDER_THREADS] << std::endl;
    }
    if(aConfig.exists(Renderer::StartupParams::PARAM_NAME__GRANULARITY))
    {
        configStream << Renderer::StartupParams::PARAM_NAME__GRANULARITY << "=" << aConfig[Renderer::StartupParams::PARAM_NAME__GRANULARITY] << std::endl;
    }
    *aRenderer = new Renderer(configStream.str().c_str());
}

void initEnvTexture(Renderer* renderer,
                 std::string &configDir,
                 Configuration &config)
{
    // Create and attach Environment Texture
	if(config.exists("envTexture"))
	{
        std::string envTexPath = config["envTexture"];
        if(!isPathAbsolute(envTexPath))
        {
            envTexPath = appendPath(configDir, envTexPath);
        }

        RTFACT_LOG_LINE_F("Loading environment texture %1%", envTexPath);
        Image2DLoaderTGA<RTfact::BasicImage2D<float> > texLoader;
        RTfact::BasicImage2D<float> *envTex = texLoader.load(envTexPath, config);

        RTImageHandle envTexx;

        if(false)
        {
            envTexx = renderer->createImage(
            envTex->getFirstComponent(0,0),
            envTex->getComponentCount(),
            envTex->getResX(),
            envTex->getResY());
        }
        else
        {
            envTexx = renderer->createImage(
            RTImage::COMPONENT_FLOAT,
            envTex->getComponentCount(),
            envTex->getResX(),
            envTex->getResY());

            void *data = envTexx->getData();
            memcpy(data, envTex->getFirstComponent(0,0),
                sizeof(float)
                * envTex->getComponentCount()
                * envTex->getResX()
                * envTex->getResY()
                );
        }

        uint envUpDimension = 1;
        if (config.exists("envUpDimension"))
        {
            envUpDimension = config["envUpDimension"].as<uint>();
        }

        bool envUpDimensionFlip = false;
        if (config.exists("envUpDimensionFlip"))
        {
            envUpDimensionFlip = config["envUpDimensionFlip"].as<bool>();
        }

        renderer->setEnvironmentTexture(envTexx, envUpDimension, envUpDimensionFlip);

        // Handle environment lighting
        if (config["useEnvTextureAsLightSource"].as<bool>())
        {
            uint partitionCount = config["envLightingPartitionCount"].as<uint>();
            renderer->createEnvironmentLights(partitionCount);
        }

        delete envTex;
    }
    else
    {
        // if we have no environment texture, set background to transparent
        renderer->setEnvironmentColor(0, 0, 0, 0);
    }
}

#ifdef USE_CGFX_POSTPROCESSOR

void initPostprocessor(
                       t_Window& aWindow,
                       t_FramebufferImage& aColorImage,
                       t_FramebufferDistanceImage& aDistanceImage,
                       t_FramebufferNormalImage&   aNormalImage,
                       t_AmbientOcclusionImage& aAmbientOcclusionImage,
                       t_ColorBleedingImage& aColorBleedingImage,
                       Postprocessor& aPP,
                       std::string &configDir,
                       Configuration &config)
{
    if(config.exists("shaderPath"))
	{
        std::string shaderPath = config["shaderPath"];
        if(!isPathAbsolute(shaderPath))
        {
            shaderPath = appendPath(configDir, shaderPath);
        }
		aPP.appendShaderPath(shaderPath.c_str());
	}
	aPP.initEffects();
	aWindow.setPostprocessor(&aPP);
    aPP.attachBuffer(aColorImage.getFirstComponent(0,0),
                     aColorImage.getComponentCount(),
                     aColorImage.getResX(),
                     aColorImage.getResY(),
                     Postprocessor::BufferType::COLOR);

    aPP.attachBuffer(aDistanceImage.getFirstComponent(0,0),
                     aDistanceImage.getComponentCount(),
                     aDistanceImage.getResX(),
                     aDistanceImage.getResY(),
                     Postprocessor::BufferType::DISTANCE);

    aPP.attachBuffer(aNormalImage.getFirstComponent(0,0),
                     aNormalImage.getComponentCount(),
                     aNormalImage.getResX(),
                     aNormalImage.getResY(),
                     Postprocessor::BufferType::NORMAL);

    aPP.attachBuffer(aAmbientOcclusionImage.getFirstComponent(0,0),
                     aAmbientOcclusionImage.getComponentCount(),
                     aAmbientOcclusionImage.getResX(),
                     aAmbientOcclusionImage.getResY(),
                     Postprocessor::BufferType::AMBIENT_OCCLUSION);

    aPP.attachBuffer(aColorBleedingImage.getFirstComponent(0,0),
                     aColorBleedingImage.getComponentCount(),
                     aColorBleedingImage.getResX(),
                     aColorBleedingImage.getResY(),
                     Postprocessor::BufferType::COLOR_BLEEDING);

	//list out settable parameters of postprocessing effects
	RTFACT_LOG_LINE("Following postprocessing parameters are available:");
	PostprocessorParameterHandle hParam = aPP.getFirstParameter();
	while(hParam)
	{
		RTFACT_LOG_LINE_F("	Postprocessor param found: %1%", hParam->getName());
		hParam = hParam->getNextParameter();
	}

	//this is how you set postprocess parameters
	//hParam = aPP.getParameterByName("AORadius");
	//hParam->setValue(5.0f);

	//hParam = aPP.getParameterByName("BlurSharpness");
	//hParam->setValue(30.0f);

	//this is how you switch effects on and off
	//EffectHandle hEffect = aPP.getEffectByName("SSAO:Blur");
	//hEffect->setEnabled(false);
}

#endif

void initCamera(Renderer *&renderer,
               CameraController &cameraController,
               Configuration &config)
{
    // Get Camera Parameters from Configuration
    t_Camera::Params camParams;

    const RTfact::uint resX = config["resX"].as<RTfact::uint>();
    const RTfact::uint resY = config["resY"].as<RTfact::uint>();

    camParams.position     = config["cameraPos"].as<RTfact::Vec3f1>();
    camParams.direction    = config["cameraDir"].as<RTfact::Vec3f1>().normalize();
    camParams.up           = config["cameraUp"].as<RTfact::Vec3f1>().normalize();
    camParams.openingAngle = config["cameraOpeningAngle"].as<float>();
    camParams.resX         = resX;
    camParams.resY         = resY;

    // Configure Camera for Renderer
    renderer->setCameraPosition(
        float3(camParams.position.x, camParams.position.y, camParams.position.z),
        float3(camParams.direction.x, camParams.direction.y, camParams.direction.z),
        float3(camParams.up.x, camParams.up.y, camParams.up.z));

    // Configure Camera for Application
    cameraController.setParams(camParams);
    cameraController.applyChanges();
    cameraController.getParams(camParams);
}

void setupCamera(Renderer *&renderer,
				 CameraController& cameraController,
				 const RTfact::uint resX,
				 const RTfact::uint resY,
				 const float3 cameraPos,
				 const float3 direction,
				 const float3 up,
				 const float openingAngle)
{
	// Get Camera Parameters from Configuration
    t_Camera::Params camParams;
	camParams.position     = RTfact::Vec3f1(cameraPos.x, cameraPos.y, cameraPos.z);
	camParams.direction    = RTfact::Vec3f1(direction.x, direction.y, direction.z).normalize();
	camParams.up           = RTfact::Vec3f1(up.x, up.y, up.z).normalize();
    camParams.openingAngle = openingAngle;
    camParams.resX         = resX;
    camParams.resY         = resY;

    // Configure Camera for Renderer
    renderer->setCameraPosition(
        float3(camParams.position.x, camParams.position.y, camParams.position.z),
        float3(camParams.direction.x, camParams.direction.y, camParams.direction.z),
        float3(camParams.up.x, camParams.up.y, camParams.up.z));

    // Configure Camera for Application
    cameraController.setParams(camParams);
    cameraController.applyChanges();
    cameraController.getParams(camParams);
}

void initWindow(t_Window &window,
               Configuration &config)
{
    const RTfact::uint resX = config["resX"].as<RTfact::uint>();
    const RTfact::uint resY = config["resY"].as<RTfact::uint>();
    // Set Window size and caption
    window.init(resX, resY);
    window.setCaption("RTremote Demo");
}

void initFramebuffer(Renderer* renderer,
                     t_Framebuffer &framebuffer,
                     t_FramebufferImage **framebufferImage,
                     t_FramebufferDistanceImage **framebufferDistanceImage,
                     t_FramebufferNormalImage **framebufferNormalImage,
					 t_AmbientOcclusionImage **framebufferAOImage,
                     t_ColorBleedingImage **colorBleedingImage,
                     Configuration &config)
{
    const RTfact::uint resX = config["resX"].as<RTfact::uint>();
    const RTfact::uint resY = config["resY"].as<RTfact::uint>();

    *framebufferImage = new t_FramebufferImage(4, resX, resY);
    framebuffer.setColorImage(*framebufferImage);

    if (
        framebufferDistanceImage!=NULL)
    {
        *framebufferDistanceImage = new t_FramebufferDistanceImage(1,resX,resY);
        framebuffer.setDistanceImage(*framebufferDistanceImage);
    }

    if (
        framebufferNormalImage!=NULL)
    {
        *framebufferNormalImage = new t_FramebufferNormalImage(3,resX,resY);
        framebuffer.setNormalImage(*framebufferNormalImage);
    }

    if (
        colorBleedingImage != NULL)
    {
        *colorBleedingImage = new t_ColorBleedingImage(3, resX, resY);
        framebuffer.setColorBleedingImage(*colorBleedingImage);
    }

    renderer->attachFrameBuffer(
    (*framebufferImage)->getFirstComponent(0,0),
    (*framebufferImage)->getComponentCount(),
    (*framebufferImage)->getResX(),
    (*framebufferImage)->getResY());

    renderer->attachDistanceBuffer(
        (*framebufferDistanceImage)->getFirstComponent(0,0),
        (*framebufferDistanceImage)->getResX(),
        (*framebufferDistanceImage)->getResY());

    renderer->attachNormalBuffer(
        (*framebufferNormalImage)->getFirstComponent(0,0),
        (*framebufferNormalImage)->getResX(),
        (*framebufferNormalImage)->getResY());

	*framebufferAOImage = new t_AmbientOcclusionImage(1, resX, resY);
    framebuffer.setAmbientOcclusionImage(*framebufferAOImage);

	renderer->attachAmbientOcclusionBuffer(
    (*framebufferAOImage)->getFirstComponent(0,0),
    (*framebufferAOImage)->getResX(),
    (*framebufferAOImage)->getResY());

    renderer->attachColorBleedingBuffer(
    (*colorBleedingImage)->getFirstComponent(0,0),
    (*colorBleedingImage)->getResX(),
    (*colorBleedingImage)->getResY());
}


void initMaterials()
{
    // Face Vectors

    triangleFaces.push_back(
       Triangle(float3(0,0,0),float3(1,0,0),float3(1,1,0)));

    float cs = 3;
    cornerFaces.push_back(Triangle(
        float3(-cs, 0, -cs), float3(-cs, 0, cs),float3(cs,  0, -cs)));
    cornerFaces.push_back(Triangle(
        float3(cs,  0, -cs), float3(-cs, 0, cs),float3(cs,  0, cs)));
    cornerFaces.push_back(Triangle(
        float3(-cs, 0, -cs), float3(cs, 0, -cs),float3(cs,  2*cs, -cs)));
    cornerFaces.push_back(Triangle(
        float3(-cs,  2*cs, -cs), float3(cs, 2*cs, -cs),float3(-cs,  0, -cs)));

    float size = 50;
    groundFaces.push_back(Triangle(
        float3(-size, 0, -size),
        float3(-size, 0, size),
        float3(size,  0, -size)));

    groundFaces.push_back(Triangle(
        float3(size,  0, -size),
        float3(-size, 0, size),
        float3(size,  0, size)));

    wallFaces.push_back(Triangle(
        float3(-size, 0, -size),
        float3(size, 0, -size),
        float3(-size, size, -size)));

    wallFaces.push_back(Triangle(
        float3(size, 0, -size),
        float3(size, size, -size),
        float3(-size,  size, -size)));


    cubeFaces.push_back(
       Triangle(float3(0,0,0),float3(1,0,0),float3(1,1,0)));
    cubeFaces.push_back(
       Triangle(float3(0,1,0),float3(0,0,0),float3(1,1,0)));

    cubeFaces.push_back(
       Triangle(float3(0,0,0),float3(1,0,0),float3(1,0,1)));
    cubeFaces.push_back(
       Triangle(float3(0,0,1),float3(0,0,0),float3(1,0,1)));

    cubeFaces.push_back(
       Triangle(float3(0,0,0),float3(0,1,0),float3(0,1,1)));
    cubeFaces.push_back(
       Triangle(float3(0,0,1),float3(0,0,0),float3(0,1,1)));

    cubeFaces.push_back(
       Triangle(float3(0,0,1),float3(1,0,1),float3(1,1,1)));
    cubeFaces.push_back(
       Triangle(float3(0,1,1),float3(0,0,1),float3(1,1,1)));

    cubeFaces.push_back(
       Triangle(float3(1,1,0),float3(0,1,0),float3(1,1,1)));
    cubeFaces.push_back(
       Triangle(float3(0,1,0),float3(0,1,1),float3(1,1,1)));

    cubeFaces.push_back(
       Triangle(float3(1,0,0),float3(1,1,0),float3(1,1,1)));
    cubeFaces.push_back(
       Triangle(float3(1,0,1),float3(1,0,0),float3(1,1,1)));
}

// -------------------------------------------
// scene creation functions
// -------------------------------------------

void paramTestFunction(const ParamValue &p)
{
    std::cout << p.which();
}

void createSimpleScene(Renderer* renderer)
{
    // --------------------------------------------------------
    // Lightsources
    // --------------------------------------------------------

    RTLightHandle light = renderer->createLight("PointLight");
    light->setParam("position", float3(-4,4,0));
    light->setParam("intensity", float3(70,70,70));
    light->setParam("attenuation", float3(0,0,1));

    // --------------------------------------------------------
    // Materials
    // --------------------------------------------------------

    RTAppearanceHandle cyanShader =
        renderer->createAppearance("PhongShader");
    cyanShader->setParam("diffuseColor", float3(0.0f, 1.0f, 1.0f));
    cyanShader->setParam("ambientIntensity", float3(.0f, .0f, .0f));

    RTAppearanceHandle yellowShader =
        renderer->createAppearance("PhongShader");
    yellowShader->setParam("diffuseColor", float3(1.0f, 1.0f, 0.0f));
    yellowShader->setParam("ambientIntensity", float3(.0f, .0f, .0f));

    // --------------------------------------------------------
    // Geometries
    // --------------------------------------------------------

    GeoHandle corner = renderer->createGeometry(cornerFaces);
    corner->setAppearance(yellowShader);
    GeoHandle cube = renderer->createGeometry(cubeFaces);
    cube->setAppearance(cyanShader);

    // --------------------------------------------------------
    // Group Hierarchy
    // --------------------------------------------------------
    GroupHandle root= renderer->getRoot();

    GroupHandle subgroup=renderer->createGroup(Transform::translation(0, 0, 0));
    subgroup->add(cube);
    root->add(subgroup);

    subgroup=renderer->createGroup(Transform::translation(0, 0, -2.8f));
    subgroup->add(cube);
    root->add(subgroup);

    subgroup=renderer->createGroup(Transform::translation(2, 0, 0));
    subgroup->add(cube);
    root->add(subgroup);

    root->add(corner);
}

void createSceneDefault(Renderer* renderer,
                        std::string &configDir,
                        Configuration &config,
                        bool createLight = true)
{
    // --------------------------------------------------------
    // Lightsources
    // --------------------------------------------------------

    if (createLight)
    {
        RTLightHandle light = renderer->createLight("PointLight");

        //light->setParam("castShadow", false);
        light->setParam("position", float3(0,10,0));
        light->setParam("intensity", float3(70,70,70));
        light->setParam("attenuation", float3(0,0,1));
    }

    // --------------------------------------------------------
    // Materials
    // --------------------------------------------------------

    RTAppearanceHandle coloredTriangleShader =
        renderer->createAppearance("PhongShader");

    coloredTriangleShader->setParam("diffuseColor", float3(1.0f, 1.0f, 0));
    coloredTriangleShader->setParam("ambientIntensity", float3(.7f, .7f, .7f));
    coloredTriangleShader->setParam("useVertexColor", true);
    coloredTriangleShader->setParam("alpha", 0.8f);

    // --------------------------------------------------------
    // Geometries
    // --------------------------------------------------------

    GeoHandle coloredTriangle = renderer->createGeometry();
    coloredTriangle->addPrimitive(triangleFaces[0]);
    coloredTriangle->setAppearance(coloredTriangleShader);

    GeoHandle groundGeoPlane = renderer->createGeometry(groundFaces);

    // --------------------------------------------------------
    // Group Hierarchy
    // --------------------------------------------------------
    GroupHandle root= renderer->getRoot();

    GroupHandle lastgroup = renderer->createGroup(Transform::identity());
    lastgroup->add(coloredTriangle);

    for(int i=0; i<6; i++)
    {
        GroupHandle subgroup=renderer->createGroup(
			    Transform::translation(
				    float(2.5*pow((double)2,i/2)*((i+1)%2)),
				    0,
				    float(2.5*pow((double)2,i/2)*(i%2))));
		    GroupHandle group=renderer->createGroup(Transform::identity());

        subgroup->add(lastgroup);
        group->add(lastgroup);
        group->add(subgroup);
        lastgroup = group;
    }

    root->add(lastgroup);
    root->add(groundGeoPlane);

    GeoHandle cursor = renderer->createGeometry(cubeFaces);
    cursorGroup = renderer->createGroup(Transform::identity());
    cursorGroup->add(cursor);
    root->add(cursorGroup);

    #ifdef RTFACT_ASSERTIONS_ENABLED
    GeoHandle groundGeoPlane2 = root->getGeometry(0);
    #endif
    // --------------------------------------------------------
    // Check geometry comparisions
    // --------------------------------------------------------
    RTFACT_ASSERT(*groundGeoPlane == *groundGeoPlane);
    RTFACT_ASSERT(*groundGeoPlane == *groundGeoPlane2);
    RTFACT_ASSERT(Geometry::equal(groundGeoPlane, groundGeoPlane));
    RTFACT_ASSERT(Geometry::equal(groundGeoPlane, groundGeoPlane2));
    RTFACT_ASSERT(*coloredTriangle == *coloredTriangle);
    RTFACT_ASSERT(Geometry::equal(coloredTriangle, coloredTriangle));
    RTFACT_ASSERT(!(*groundGeoPlane == *coloredTriangle));
    RTFACT_ASSERT(!(*coloredTriangle == *groundGeoPlane));
    RTFACT_ASSERT(!(*coloredTriangle == *groundGeoPlane2));
    RTFACT_ASSERT(!Geometry::equal(coloredTriangle, groundGeoPlane));
    RTFACT_ASSERT(!Geometry::equal(0, groundGeoPlane));
    RTFACT_ASSERT(!Geometry::equal(groundGeoPlane, 0));
    RTFACT_ASSERT(Geometry::equal(0, 0));


    #ifdef RTFACT_ASSERTIONS_ENABLED
    GroupHandle root2= renderer->getRoot();
    #endif
    // --------------------------------------------------------
    // Check group comparisions
    // --------------------------------------------------------
    RTFACT_ASSERT(*root == *root);
    RTFACT_ASSERT(*root == *root2);
    RTFACT_ASSERT(Group::equal(root, root));
    RTFACT_ASSERT(Group::equal(root, root2));
    RTFACT_ASSERT(*lastgroup == *lastgroup);
    RTFACT_ASSERT(Group::equal(lastgroup, lastgroup));
    RTFACT_ASSERT(!(*root == *lastgroup));
    RTFACT_ASSERT(!(*lastgroup == *root));
    RTFACT_ASSERT(!(*lastgroup == *root2));
    RTFACT_ASSERT(!Group::equal(lastgroup, root));
    RTFACT_ASSERT(!Group::equal(0, root));
    RTFACT_ASSERT(!Group::equal(root, 0));
    RTFACT_ASSERT(Group::equal(0, 0));
}

void createDirectionalLightScene(Renderer* renderer)
{
    // --------------------------------------------------------
    // Lightsources
    // --------------------------------------------------------

    RTLightHandle light = renderer->createLight("DirectionalLight");

    light->setParam("direction", float3(0.0f,-1.0f, 5.0f));
    light->setParam("intensity", float3(0.4f,0.4f,0.4f));

    // --------------------------------------------------------
    // Materials
    // --------------------------------------------------------

    RTAppearanceHandle coloredTriangleShader =
        renderer->createAppearance("PhongShader");

    coloredTriangleShader->setParam("diffuseColor", float3(1.0f, 1.0f, 0));
    coloredTriangleShader->setParam("ambientIntensity", float3(.7f, .7f, .7f));
    coloredTriangleShader->setParam("useVertexColor", true);
    coloredTriangleShader->setParam("alpha", 0.8f);

    // --------------------------------------------------------
    // Geometries
    // --------------------------------------------------------

    GeoHandle coloredTriangle = renderer->createGeometry();
    coloredTriangle->addPrimitive(triangleFaces[0]);
    coloredTriangle->setAppearance(coloredTriangleShader);

    GeoHandle groundGeoPlane = renderer->createGeometry(groundFaces);

    // --------------------------------------------------------
    // Group Hierarchy
    // --------------------------------------------------------
    GroupHandle root= renderer->getRoot();

    GroupHandle lastgroup = renderer->createGroup(Transform::identity());
    lastgroup->add(coloredTriangle);

    for(int i=0; i<6; i++)
    {
        GroupHandle subgroup=renderer->createGroup(
			    Transform::translation(
				    float(2.5*pow((double)2,i/2)*((i+1)%2)),
				    0,
				    float(2.5*pow((double)2,i/2)*(i%2))));
		    GroupHandle group=renderer->createGroup(Transform::identity());

        subgroup->add(lastgroup);
        group->add(lastgroup);
        group->add(subgroup);
        lastgroup = group;
    }

    root->add(lastgroup);
    root->add(groundGeoPlane);

    GeoHandle cursor = renderer->createGeometry(cubeFaces);
    cursorGroup = renderer->createGroup(Transform::identity());
    cursorGroup->add(cursor);
    root->add(cursorGroup);
}

void createAreaLightScene(Renderer* renderer, Configuration &config)
{
    // --------------------------------------------------------
    // Lightsources
    // --------------------------------------------------------

    RTLightHandle light;

    int sceneID = config["sceneID"].as<RTfact::uint>();
    switch(sceneID){
      case 4:
        light = renderer->createLight("QuadLight");
        break;
      case 10:
        light = renderer->createLight("TriangleLight");
        break;
    }

    light->setParam("vertex0", float3(10,30.1f,1));
    light->setParam("vertex1", float3(0,30.1f,1));
    light->setParam("vertex2", float3(0,3,0));

    light->setParam("intensity", float3(1,1,1));
    light->setParam("attenuation", float3(0,0,1));

    // --------------------------------------------------------
    // Materials
    // --------------------------------------------------------

    RTAppearanceHandle coloredTriangleShader =
        renderer->createAppearance("PhongShader");

    coloredTriangleShader->setParam("diffuseColor", float3(1.0f, 1.0f, 0));
    coloredTriangleShader->setParam("ambientIntensity", float3(.7f, .7f, .7f));
    coloredTriangleShader->setParam("useVertexColor", true);
    coloredTriangleShader->setParam("alpha", 0.8f);
    coloredTriangleShader->setParam("numShadowSamples", 16);

    // --------------------------------------------------------
    // Geometries
    // --------------------------------------------------------

    GeoHandle coloredTriangle = renderer->createGeometry();
    coloredTriangle->addPrimitive(triangleFaces[0]);
    coloredTriangle->setAppearance(coloredTriangleShader);

    GeoHandle groundGeoPlane = renderer->createGeometry(groundFaces);

    // --------------------------------------------------------
    // Group Hierarchy
    // --------------------------------------------------------
    GroupHandle root= renderer->getRoot();

    GroupHandle lastgroup = renderer->createGroup(Transform::identity());
    lastgroup->add(coloredTriangle);

    for(int i=0; i<6; i++)
    {
        GroupHandle subgroup=renderer->createGroup(
			    Transform::translation(
				    float(2.5*pow((double)2,i/2)*((i+1)%2)),
				    0,
				    float(2.5*pow((double)2,i/2)*(i%2))));
		    GroupHandle group=renderer->createGroup(Transform::identity());

        subgroup->add(lastgroup);
        group->add(lastgroup);
        group->add(subgroup);
        lastgroup = group;
    }

    root->add(lastgroup);
    root->add(groundGeoPlane);

    GeoHandle cursor = renderer->createGeometry(cubeFaces);
    cursorGroup = renderer->createGroup(Transform::identity());
    cursorGroup->add(cursor);
    root->add(cursorGroup);
}

void createIESScene(Renderer* renderer,
                        std::string &configDir,
                        Configuration &config,
                        RTLightHandle& animLight)
{
    // --------------------------------------------------------
    // Lightsources
    // --------------------------------------------------------

    animLight = renderer->createLight(
      config.exists("iesLightType") ? config["iesLightType"] : "IESLight");
    animLight->setParam("position", float3(0,30,-20));
    animLight->setParam("direction", float3(0,-1,0));
    animLight->setParam("intensity", 0.1f);
    animLight->setParam("color", float3(1,0,1));
    animLight->setParam("attenuation", float3(0,0,1));
    animLight->setParam("baseVector", float3(0,0,1));
    if(config.exists("iesLight"))
    {
        std::string iesFilePath = config["iesLight"];
        if(!isPathAbsolute(iesFilePath))
        {
            iesFilePath = appendPath(configDir, iesFilePath);
        }
        animLight->setParam("file", iesFilePath);
    }

    // --------------------------------------------------------
    // Materials
    // --------------------------------------------------------

    RTAppearanceHandle coloredTriangleShader =
        renderer->createAppearance("PhongShader");

    coloredTriangleShader->setParam("diffuseColor", float3(1.0f, 1.0f, 0));
    coloredTriangleShader->setParam("ambientIntensity", float3(.7f, .7f, .7f));
    coloredTriangleShader->setParam("useVertexColor", true);
    coloredTriangleShader->setParam("alpha", 0.8f);

    // --------------------------------------------------------
    // Geometries
    // --------------------------------------------------------

    GeoHandle coloredTriangle = renderer->createGeometry();
    coloredTriangle->addPrimitive(triangleFaces[0]);
    coloredTriangle->setAppearance(coloredTriangleShader);

    GeoHandle groundGeoPlane = renderer->createGeometry(groundFaces);
    GeoHandle wallPlaneGeo = renderer->createGeometry(wallFaces);

    // --------------------------------------------------------
    // Group Hierarchy
    // --------------------------------------------------------
    GroupHandle root= renderer->getRoot();

    GroupHandle lastgroup = renderer->createGroup(Transform::identity());
    lastgroup->add(coloredTriangle);

    for(int i=0; i<6; i++)
    {
        GroupHandle subgroup=renderer->createGroup(
			    Transform::translation(
				    float(2.5*pow((double)2,i/2)*((i+1)%2)),
				    0,
				    float(2.5*pow((double)2,i/2)*(i%2))));
		    GroupHandle group=renderer->createGroup(Transform::identity());

        subgroup->add(lastgroup);
        group->add(lastgroup);
        group->add(subgroup);
        lastgroup = group;
    }

    root->add(lastgroup);
    root->add(groundGeoPlane);
    root->add(wallPlaneGeo);

    GeoHandle cursor = renderer->createGeometry(cubeFaces);
    cursorGroup = renderer->createGroup(Transform::identity());
    cursorGroup->add(cursor);
    root->add(cursorGroup);
}

void createAnySLScene(Renderer* renderer,
                      std::string &configDir,
                      Configuration &config)
{
    // --------------------------------------------------------
    // Lightsources
    // --------------------------------------------------------

    RTLightHandle light = renderer->createLight("SpotLight");

    //light->setParam("castShadow", false);
    light->setParam("position", float3(0,50,0));
    light->setParam("intensity", float3(15000,15000,15000));
    light->setParam("attenuation", float3(0,0,1));
    light->setParam("direction", float3(0,-1,0));
    light->setParam("cutOffAngle", 3.14f/5+3.14f/20);
    light->setParam("beamWidth", 3.14f/5);

    // --------------------------------------------------------
    // Materials
    // --------------------------------------------------------

    int sceneID = config["sceneID"].as<RTfact::uint>();

    RTAppearanceHandle wallShader;
    if(sceneID == 6)
    {
        wallShader = renderer->createAppearance("PhongShader");

        wallShader->setParam("diffuseColor", float3(1.0f, 1.0f, 0));
        wallShader->setParam("ambientIntensity", float3(.0f, .0f, .0f));
    }

    if(sceneID == 7)
    {
        wallShader = renderer->createAppearance("AnySL:PhongShaderGI.bc");
        wallShader->beginParams();
        wallShader->setParam("ambientIntensity", float3(.0f, .0f, .0f));
        wallShader->setParam("diffuseColor", float3(1.0f, 1.0f, 0));
        wallShader->setParam("isSpecular", 0);
        wallShader->setParam("packetize", false);
        wallShader->setParam("specialize", false);
        wallShader->setParam("shininess", 0.f);
        wallShader->setParam("specularColor", float3(0.f, 0.f, 0.f));
        wallShader->setParam("useVertexColor", false);
        wallShader->endParams();
    }

    // --------------------------------------------------------
    // Geometries
    // --------------------------------------------------------

    GeoHandle groundGeoPlane = renderer->createGeometry(groundFaces);
    GeoHandle wallPlaneGeo = renderer->createGeometry(wallFaces);

    groundGeoPlane->setAppearance(wallShader);
    wallPlaneGeo->setAppearance(wallShader);

    // --------------------------------------------------------
    // Group Hierarchy
    // --------------------------------------------------------
    GroupHandle root= renderer->getRoot();

	  root->add(groundGeoPlane);
    root->add(wallPlaneGeo);
}

void createCornellScene(Renderer *renderer)
{
    RTLightHandle light = renderer->createLight("SpotLight");
    light->setParam("castShadow", true);
    light->setParam("position", float3(0,3.995f,0));
    light->setParam("intensity", float3(700.0f, 700.0f, 700.0f));
    light->setParam("attenuation", float3(0,0,1));
    light->setParam("direction", float3(0,-1,0));
    light->setParam("cutOffAngle", 0.46f);
    light->setParam("beamWidth", 0.2f);

    RTAppearanceHandle rShader = renderer->createAppearance("AnySL:PhongShaderGI.bc");
    rShader->beginParams();
    rShader->setParam("ambientIntensity", float3(.0f, .0f, .0f));
    rShader->setParam("diffuseColor", float3(1.0f, 0.0f, 0));
    rShader->setParam("isSpecular", 0);
    rShader->setParam("packetize", false);
    rShader->setParam("specialize", false);
    rShader->setParam("shininess", 0.f);
    rShader->setParam("specularColor", float3(0.f, 0.f, 0.f));
    rShader->setParam("useVertexColor", false);
    rShader->setParam("VPLIllumination", true);
    rShader->endParams();

    RTAppearanceHandle gShader = renderer->createAppearance("AnySL:PhongShaderGI.bc");
    gShader->beginParams();
    gShader->setParam("ambientIntensity", float3(.0f, .0f, .0f));
    gShader->setParam("diffuseColor", float3(0.0f, 1.0f, 0));
    gShader->setParam("isSpecular", 0);
    gShader->setParam("packetize", false);
    gShader->setParam("specialize", false);
    gShader->setParam("shininess", 0.f);
    gShader->setParam("specularColor", float3(0.f, 0.f, 0.f));
    gShader->setParam("useVertexColor", false);
    gShader->setParam("VPLIllumination", true);
    gShader->endParams();

    RTAppearanceHandle oShader = renderer->createAppearance("AnySL:PhongShaderGI.bc");
    oShader->beginParams();
    oShader->setParam("ambientIntensity", float3(.0f, .0f, .0f));
    oShader->setParam("diffuseColor", float3(0.6f, 0.6f, 0.6f));
    oShader->setParam("isSpecular", 0);
    oShader->setParam("packetize", false);
    oShader->setParam("specialize", false);
    oShader->setParam("shininess", 0.f);
    oShader->setParam("specularColor", float3(0.f, 0.f, 0.f));
    oShader->setParam("useVertexColor", false);
    oShader->setParam("VPLIllumination", true);
    oShader->endParams();

    #if 1
    RTAppearanceHandle sCubeShader = renderer->createAppearance("AnySL:PhongShaderGI.bc");
    sCubeShader->beginParams();
    sCubeShader->setParam("ambientIntensity", float3(.0f, .0f, .0f));
    sCubeShader->setParam("diffuseColor", float3(0.6f, 0.6f, 0.6f));
    sCubeShader->setParam("isSpecular", 0);
    sCubeShader->setParam("packetize", false);
    sCubeShader->setParam("specialize", false);
    sCubeShader->setParam("shininess", 0.f);
    sCubeShader->setParam("specularColor", float3(0.f, 0.f, 0.f));
    sCubeShader->setParam("useVertexColor", false);
    sCubeShader->setParam("VPLIllumination", true);
    sCubeShader->endParams();
    #else
    RTAppearanceHandle sCubeShader = renderer->createAppearance("AnySL:MirrorShader.bc");
    #endif

    float s = 2;

    std::vector<Triangle> rFaces;
    std::vector<Triangle> gFaces;
    std::vector<Triangle> oFaces;

    rFaces.push_back(Triangle(
        float3(-s, 0,  s),
        float3(-s, 0, -s),
        float3(-s, 2*s, -s)));

    rFaces.push_back(Triangle(
        float3(-s, 0,  s),
        float3(-s, 2*s, -s),
        float3(-s, 2*s,  s)));

    gFaces.push_back(Triangle(
        float3(s, 0, -s),
        float3(s, 0,  s),
        float3(s, 2*s, -s)));

    gFaces.push_back(Triangle(
        float3(s, 0,    s),
        float3(s, 2*s,  s),
        float3(s, 2*s, -s)));

    oFaces.push_back(Triangle(
        float3(-s, 0,  s),
        float3(-s, 0, -s),
        float3( s, 0, -s)));

    oFaces.push_back(Triangle(
        float3(-s, 0,  s),
        float3( s, 0, -s),
        float3( s, 0,  s)));

    oFaces.push_back(Triangle(
        float3(-s, 0, -s),
        float3( s, 0, -s),
        float3(-s, 2*s, -s)));

    oFaces.push_back(Triangle(
        float3( s, 0, -s),
        float3( s, 2*s, -s),
        float3(-s, 2*s, -s)));

    oFaces.push_back(Triangle(
        float3(-s, 2*s, -s),
        float3( s, 2*s, -s),
        float3(-s, 2*s,  s)));

    oFaces.push_back(Triangle(
        float3(-s, 2*s,  s),
        float3( s, 2*s, -s),
        float3( s, 2*s,  s)));

    GeoHandle rPlane = renderer->createGeometry(rFaces);
    GeoHandle gPlane = renderer->createGeometry(gFaces);
    GeoHandle oPlane = renderer->createGeometry(oFaces);
    GeoHandle sCube = renderer->createGeometry(cubeFaces);

    rPlane->setAppearance(rShader);
    gPlane->setAppearance(gShader);
    oPlane->setAppearance(oShader);
    sCube->setAppearance(sCubeShader);

    renderer->getRoot()->add(rPlane);
    renderer->getRoot()->add(gPlane);
    renderer->getRoot()->add(oPlane);

    GroupHandle group=renderer->createGroup(Transform::identity());
    GroupHandle subgroup=renderer->createGroup(Transform::translation(-2, 0, -2));
    subgroup->add(sCube);
    group->add(sCube);
    group->add(subgroup);

    renderer->getRoot()->add(group);
}

void createSceneDeleteTest(Renderer* renderer)
{
    // --------------------------------------------------------
    // Lightsources
    // --------------------------------------------------------

    RTLightHandle light = renderer->createLight("PointLight");

    light->setParam("position", float3(0,10,0));
    light->setParam("intensity", float3(100,100,100));
    light->setParam("attenuation", float3(0,0,1));


    RTLightHandle light2 = renderer->createLight("PointLight");

    light2->setParam("position", float3(10,10,0));
    light2->setParam("intensity", float3(10,100,10));
    light2->setParam("attenuation", float3(0,0,1));


    // --------------------------------------------------------
    // Materials
    // --------------------------------------------------------

    RTAppearanceHandle coloredTriangleShader =
        renderer->createAppearance("PhongShader");

    coloredTriangleShader->setParam("diffuseColor", float3(1.0f, 1.0f, 0));
    coloredTriangleShader->setParam("ambientIntensity", float3(.7f, .7f, .7f));
    coloredTriangleShader->setParam("useVertexColor", true);
    coloredTriangleShader->setParam("alpha", 0.8f);

    // --------------------------------------------------------
    // Geometries
    // --------------------------------------------------------

    GeoHandle triangle = renderer->createGeometry(triangleFaces);
    triangle->setAppearance(coloredTriangleShader);

    GeoHandle groundGeoPlane = renderer->createGeometry(groundFaces);

    // --------------------------------------------------------
    // Group Hierarchy
    // --------------------------------------------------------
    // Create a scene without adding it to the root
    {
        GroupHandle planeGroup = renderer->createGroup(Transform::identity());
        planeGroup->add(groundGeoPlane);
        GroupHandle someGroup = planeGroup->createChildGroup(Transform::identity());
        someGroup->add(triangle);
        renderer->getRoot()->add(triangle);
        renderer->renderToBuffer();
    }
    // Create simple graph
    {
        GroupHandle planeGroup = renderer->createGroup(Transform::identity());
        {
            planeGroup->add(groundGeoPlane);
            GroupHandle someGroup = planeGroup->createChildGroup(Transform::identity());
            someGroup->add(triangle);
            renderer->getRoot()->add(planeGroup);
        }
        renderer->renderToBuffer();
        // Remove part of the graph
        {
            GroupHandle doNotWant = planeGroup->getGroup(0);
            renderer->getRoot()->getGroup(0)->remove(doNotWant);
        }
        renderer->renderToBuffer();
        {
            // Create graph with local geometrie handles
            GroupHandle localGeoGroup = renderer->getRoot()->createChildGroup(Transform::identity());
            {
                GeoHandle localTri = renderer->createGeometry(triangleFaces);
                GeoHandle localCube = renderer->createGeometry(cubeFaces);
                localGeoGroup->add(localTri);
                localGeoGroup->add(localCube);
            }
            renderer->renderToBuffer();
            // add second instanciation of localGeoGroup
            planeGroup->add(localGeoGroup);
            renderer->renderToBuffer();
            // remove first instanciation
            renderer->getRoot()->remove(localGeoGroup);
            renderer->renderToBuffer();
            // remove second instanciation
            planeGroup->remove(localGeoGroup);
            renderer->renderToBuffer();
        }
        // Out of Scope: Group and Geometries should be deleted
        renderer->renderToBuffer();
        // Clear the scene
        renderer->clearScene();
        renderer->renderToBuffer();
        // Create new geometries:
        triangle = renderer->createGeometry(triangleFaces);
        groundGeoPlane = renderer->createGeometry(groundFaces);
    }
    renderer->getRoot()->add(triangle);
    renderer->getRoot()->add(groundGeoPlane);
}

void createSceneEmptyTest(Renderer* renderer)
{
    // --------------------------------------------------------
    // Lightsources
    // --------------------------------------------------------

    RTLightHandle light = renderer->createLight("PointLight");

    light->setParam("position", float3(0,10,0));
    light->setParam("intensity", float3(100,100,100));
    light->setParam("attenuation", float3(0,0,1));


    RTLightHandle light2 = renderer->createLight("PointLight");

    light2->setParam("position", float3(10,10,0));
    light2->setParam("intensity", float3(10,100,10));
    light2->setParam("attenuation", float3(0,0,1));


    // --------------------------------------------------------
    // Materials
    // --------------------------------------------------------

    RTAppearanceHandle coloredTriangleShader =
        renderer->createAppearance("PhongShader");

    coloredTriangleShader->setParam("diffuseColor", float3(1.0f, 1.0f, 0));
    coloredTriangleShader->setParam("ambientIntensity", float3(.7f, .7f, .7f));
    coloredTriangleShader->setParam("useVertexColor", true);
    coloredTriangleShader->setParam("alpha", 0.8f);

    // --------------------------------------------------------
    // Geometries
    // --------------------------------------------------------

    GeoHandle triangle = renderer->createGeometry(triangleFaces);
    triangle->setAppearance(coloredTriangleShader);

    /* GeoHandle groundGeoPlane = */ renderer->createGeometry(groundFaces);

    // --------------------------------------------------------
    // Group Hierarchy
    // --------------------------------------------------------
    // Render empty scene
    renderer->renderToBuffer();
    // Add empty geometry to empty scene!
    GeoHandle emptyGeo = renderer->createGeometry();
    renderer->getRoot()->add(emptyGeo);
    // empty scene is empty. want to render anyway
    renderer->renderToBuffer();
    // now add a not empty geometry
    renderer->getRoot()->add(triangle);
    renderer->renderToBuffer();
    // make scene empty again
    renderer->clearScene();
    renderer->renderToBuffer();
}

// -------------------------------------------
// helper functions
// -------------------------------------------

void computeStereoFrustumAndOffset(
    const float aEyeSeparationAngleInDeg,
    const uint aResX,
    const uint aResY,
    const float aOpeningAngleInDeg,
    const uint aEye,
    const float3 aCameraDir,
    const float3 aCameraUp,
    FrustumParameters& aFrustum,
    float3& aCameraPos)
{
    float aspectRatio = (float)aResX / aResY;
    float offset = -RTfact::Math::tan(RTfact::Math::radians(aEyeSeparationAngleInDeg) * 0.5f) / aspectRatio;

    aFrustum.top = 1.f;
    aFrustum.bottom = -1.f;
    aFrustum.nearPlane = 1.f / RTfact::Math::tan(RTfact::Math::radians(aOpeningAngleInDeg)* 0.5f);
    aFrustum.farPlane = RTfact::Packet<1, float>::C_INFINITY;

    RTfact::Vec3f1 camDir(aCameraDir.x, aCameraDir.y, aCameraDir.z);
    RTfact::Vec3f1 camUp(aCameraUp.x, aCameraUp.y, aCameraUp.z);
    RTfact::Vec3f1 xAxis = RTfact::Math::cross(camDir, camUp);


    if(aEye == 0) //left eye
    {
        aFrustum.left = -1.f;
        aFrustum.right = 2.f + aFrustum.left + offset;
        aCameraPos.x = aCameraPos.x + offset * xAxis.xf;
        aCameraPos.y = aCameraPos.y + offset * xAxis.yf;
        aCameraPos.z = aCameraPos.z + offset * xAxis.zf;
    }
    else if (aEye == 1) //right eye
    {
        aFrustum.right = 1.f;
        aFrustum.left = -2.f + aFrustum.right - offset;
        aCameraPos.x = aCameraPos.x - offset * xAxis.xf;
        aCameraPos.y = aCameraPos.y - offset * xAxis.yf;
        aCameraPos.z = aCameraPos.z - offset * xAxis.zf;
    }
    else //wrong eye, put center
    {
        aFrustum.left = -1.f;
        aFrustum.right = 1.f;
    }
}

// -------------------------------------------
// main function
// -------------------------------------------

int main(int argc, char** argv)
{
    std::string configDir = (argc == 2) ? argv[1] : "RTfact.ini";
    Configuration config(configDir);
    RTFACT_LOG_LINE_F("Using configuration file %1%", configDir);
    configDir = getParentDirectory(configDir);

    // ----------------------------------------------------------
    // Main Components:
    // ----------------------------------------------------------

    // RTremote Renderer for scene construction and rendering
    Renderer*               renderer;

#ifdef USE_CGFX_POSTPROCESSOR
    // RTremote Postprocessor for scene display
    Postprocessor          postprocessor;
#endif

    // SDLGLCgWindow to display the scene
    t_Window                window;

    // FrameBuffer and FrameBufferImage are the connection between window and renderer
    t_FramebufferImage*         framebufferImage;
    t_FramebufferDistanceImage* framebufferDistanceImage;
    t_FramebufferNormalImage*   framebufferNormalImage;
	t_AmbientOcclusionImage *ambientOcclusionImage;
    t_ColorBleedingImage *colorBleedingImage;
    t_Framebuffer               framebuffer;

    // A little application, connected to the window, for camera control and other stuff
    ApplicationController appController(window);
    t_PickingController cameraController(
        window, RTfact::BBox(RTfact::Vec3f1(0, 0, 0), RTfact::Vec3f1(100, 100, 100)));
    RTfact::FPSMeter fpsMeter;

    // ----------------------------------------------------------
    // Initialisation
    // ----------------------------------------------------------
    initRenderer(config, &renderer);
    initEnvTexture(renderer, configDir, config);
    initWindow(window, config);
    initFramebuffer(
        renderer,
        framebuffer,
        &framebufferImage,
        &framebufferDistanceImage,
        &framebufferNormalImage,
        &ambientOcclusionImage,
        &colorBleedingImage,
        config);

#ifdef USE_CGFX_POSTPROCESSOR
    initPostprocessor(
        window,
        *framebufferImage,
        *framebufferDistanceImage,
        *framebufferNormalImage,
        *ambientOcclusionImage,
        *colorBleedingImage,
        postprocessor,
        configDir,
        config);
#endif
    renderer->setOverSamplingRate(1);

    // ----------------------------------------------------------
    // scene constructionVPLIllumination
    // ----------------------------------------------------------

    renderer->startConstruction(Renderer::GEOMETRYTYPE_STATIC);

#if 1
    initCamera(renderer, cameraController, config);
    initMaterials();
    int sceneID = 0;
    RTLightHandle animLight;

    if(config.exists("sceneID"))
        sceneID = config["sceneID"].as<RTfact::uint>();

    switch(sceneID)
    {
    case 0:
        createSceneDefault(renderer, configDir, config);
        break;
    case 1:
        createSceneDeleteTest(renderer);
        break;
    case 2:
        createSceneEmptyTest(renderer);
        break;
    case 3:
        createIESScene(renderer, configDir, config, animLight);
        break;
    case 4:
        createAreaLightScene(renderer, config);
        break;
    case 5:
        createDirectionalLightScene(renderer);
        break;
    case 6:
    case 7:
        createAnySLScene(renderer, configDir, config);
        break;
    case 8:
        createCornellScene(renderer);
        break;
    case 9:
        createSceneDefault(renderer, configDir, config, false);
        break;
    case 10:
        createAreaLightScene(renderer, config);
        break;
    case 11:
        createSimpleScene(renderer);
        break;
    default:
        createSceneDefault(renderer, configDir, config);
        break;
    }
#else
#if 1
    typedef TriangleMeshOBJLoader<RTfact::BasicScene<RTfact::Appearance, RTfact::Light, RTfact::Triangle, RTfact::EnvironmentTexture>,
								  RTfact::BasicTriangleMeshPopulator<RTfact::BasicScene<RTfact::Appearance, RTfact::Light, RTfact::Triangle, RTfact::EnvironmentTexture>,
								  RTfact::OBJMaterialCreator<RTfact::Material> > > t_MeshLoader;
	t_MeshLoader meshLoader;
	t_MeshLoader::Entity *entity = meshLoader.load("C:/Users/kdahm/rtfact-pp/rtfact/remote/cornell-box.obj");
	setupCamera(renderer,
				cameraController,
				config["resX"].as<RTfact::uint>(),
				config["resY"].as<RTfact::uint>(),
				float3(0.0f, 2.0f, 5.2f),
				float3(0.0f, 0.0f, -1.0f),
				float3(0.0f, 1.0f, 0.0f),
				70);

	RTfact::Vector<RTfact::Triangle> prims = entity->primitives;




	for (unsigned int i=0u; i < prims.size(); ++i)
	{
		RTfact::Vec3f1 v0 = prims[i].vertices[0];
		RTfact::Vec3f1 v1 = prims[i].vertices[1];
		RTfact::Vec3f1 v2 = prims[i].vertices[2];

		GeoHandle coloredTriangle = renderer->createGeometry();
        //coloredTriangle->setAppearance(renderer->getAppearance(prims[i].appearanceID));
		coloredTriangle->addPrimitive(RTfact::Remote::Triangle(float3(v0.xf, v0.yf, v0.zf),
															   float3(v1.xf, v1.yf, v1.zf),
															   float3(v2.xf, v2.yf, v2.zf)));
		renderer->getRoot()->add(coloredTriangle);
	}
#else
    LWObject lwObj;
    lwObj.read("C:/Users/kdahm/rtfact-pp/rtfact/remote/cornell-box.obj", false);



    for (unsigned int i=0u; i < lwObj.faces.size(); ++i)
	{
        LWObject::Face f = lwObj.faces[i];
		Point v0 = lwObj.vertices[f.vert1];
		Point v1 = lwObj.vertices[f.vert2];
		Point v2 = lwObj.vertices[f.vert3];
        LWObject::Material mat = lwObj.materials[f.material];

        RTAppearanceHandle coloredTriangleShader =
        renderer->createAppearance("PhongShader");

        coloredTriangleShader->setParam("diffuseColor", float3(mat.diffuseCoeff.x, mat.diffuseCoeff.y, mat.diffuseCoeff.z));

		GeoHandle coloredTriangle = renderer->createGeometry();
        //coloredTriangle->setAppearance(coloredTriangleShader);
		coloredTriangle->addPrimitive(RTfact::Remote::Triangle(float3(v0.x, v0.y, v0.z),
															   float3(v1.x, v1.y, v1.z),
															   float3(v2.x, v2.y, v2.z)));
		renderer->getRoot()->add(coloredTriangle);
	}

#endif
    printf("Done");
#endif

    renderer->finishConstruction(Renderer::GEOMETRYTYPE_STATIC);

    // Stereo test with anaglyphs
    bool renderAnaglyph = false;
    if(config.exists("renderAnaglyphStereo"))
    {
        renderAnaglyph = config["renderAnaglyphStereo"].as<bool>();
    }

    t_FramebufferImage *rightColorBuffer;
    const float camOpeningAngle = renderer->getCameraOpeningAngle();
    float eyeSeparationAngle = 0.65f;
    float3 camDir, camPos, camPosCenter, camUp;
    if(renderAnaglyph)
    {
        //create right-eye buffer with the same parameters as original color buffer
        rightColorBuffer = new t_FramebufferImage(
            framebufferImage->getComponentCount(),
            framebufferImage->getResX(),
            framebufferImage->getResY()
            );

        //eye separation factor
        if(config.exists("eyeSeparationFactor"))
        {
            eyeSeparationAngle = config["eyeSeparationFactor"].as<float>();
        }
    }

    // ----------------------------------------------------------
    // render loop
    // ----------------------------------------------------------

    //define to render also ambient occlusion buffer
#ifdef USE_CGFX_POSTPROCESSOR
    renderer->setRenderMode(Renderer::RENDERMODE_DIRECT_ILLUMINATION |
                            Renderer::RENDERMODE_AMBIENT_OCCLUSION);
#else
    int renderMode = 0;
    if(config.exists("renderMode"))
        renderMode = config["renderMode"].as<RTfact::uint>();

    switch(renderMode)
    {
    case 0:
        renderer->setRenderMode(Renderer::RENDERMODE_DIRECT_ILLUMINATION);
        break;
    case 1:
    default:
        renderer->setRenderMode(Renderer::RENDERMODE_PROGRESSIVE_GI);
        break;
    }
#endif

    //optimize render settings based on the scene, machine, etc.
    //renderer->findBestGranularity();

    bool allowProgressiveRefinement = true;
    if(config.exists("allowProgressiveRefinement"))
    {
        allowProgressiveRefinement = config["allowProgressiveRefinement"].as<bool>();
    }

    //scene with IES ligth animation
    float lightAnimSpeed = 0.05f;
    if(sceneID == 3)
    {
        allowProgressiveRefinement = false;
        if(config.exists("lightAnimationSpeed"))
        {
            lightAnimSpeed = config["lightAnimationSpeed"].as<float>();
        }
    }
    float iesLightAngle = 0.f;

    t_Camera::Params camParams;
    for(;;)
    {
#ifdef USE_CGFX_POSTPROCESSOR

        framebuffer.prePaint();

        fpsMeter.startFrame();

        renderer->renderToBuffer();

        fpsMeter.endFrame();

        framebuffer.postPaint();

        RTFACT_LOG_F("FPS: %3.1f            \r", fpsMeter.getFPS());

        {
            float3 pos,dir,up;
            renderer->getCameraPosition(pos,dir,up);
            postprocessor.setCameraPosition(pos.x, pos.y, pos.z);
            postprocessor.setCameraViewDirection(dir.x, dir.y, dir.z);
            postprocessor.setCameraUpVector(up.x, up.y, up.z);
            postprocessor.setCameraOpeningAngle(renderer->getCameraOpeningAngle());
        }
        postprocessor.process();

    #else

        if(renderAnaglyph)
        {
            FrustumParameters frustumParams;
            fpsMeter.startFrame();
            renderer->getCameraPosition(camPosCenter, camDir, camUp);

            //render left eye
            camPos = camPosCenter;
            computeStereoFrustumAndOffset(
                eyeSeparationAngle,
                window.getResX(),
                window.getResY(),
                camOpeningAngle,
                0,
                camDir,
                camUp,
                frustumParams,
                camPos);
            renderer->setCameraPosition(camPos, camDir, camUp);
            renderer->setFrustum(frustumParams);
            renderer->attachFrameBuffer(
                framebufferImage->getFirstComponent(0,0),
                framebufferImage->getComponentCount(),
                framebufferImage->getResX(),
                framebufferImage->getResY());
            renderer->renderToBuffer();

            //render right eye
            camPos = camPosCenter;
            computeStereoFrustumAndOffset(
                eyeSeparationAngle,
                window.getResX(),
                window.getResY(),
                camOpeningAngle,
                1,
                camDir,
                camUp,
                frustumParams,
                camPos);
            renderer->setCameraPosition(camPos, camDir, camUp);
            renderer->setFrustum(frustumParams);
            renderer->attachFrameBuffer(
                rightColorBuffer->getFirstComponent(0,0),
                rightColorBuffer->getComponentCount(),
                rightColorBuffer->getResX(),
                rightColorBuffer->getResY());
            renderer->renderToBuffer();

            fpsMeter.endFrame();

            //assemble right and left eye into the left eye buffer
            float leftVal,rightVal;
            for (uint y = 0; y < framebufferImage->getResY(); ++y)
            {
                for (uint x = 0; x < framebufferImage->getResX(); ++x)
                {
                    t_FrameBufferComponent *compLeft = framebufferImage->getFirstComponent(x,y);
                    t_FrameBufferComponent *compRight = rightColorBuffer->getFirstComponent(x,y);
                    leftVal  = 0.2125f * compLeft[0] + 0.7154f * compLeft[1] + 0.0721f * compLeft[2];
                    rightVal = 0.2125f * compRight[0] + 0.7154f * compRight[1] + 0.0721f * compRight[2];
                    compLeft[0] = (t_FrameBufferComponent)leftVal;
                    compLeft[1] = (t_FrameBufferComponent)rightVal;
                    compLeft[2] = (t_FrameBufferComponent)0.f;
                }
            }
        }
        else
        {
            fpsMeter.startFrame();

            renderer->renderToBuffer();

            fpsMeter.endFrame();
        }

        //paste the normal buffer onto the screen
        window.update(*framebufferImage);

        RTFACT_LOG_F("FPS: %3.1f            \r", fpsMeter.getFPS());

    #endif

        window.display();

        window.handleEvents();

        ///picking
        PickingController::tScreenSample pickCoords;
        //returns true if picking was requested by the user, together with the requested screen position
        if(cameraController.pick(pickCoords))
        {
            Picking picking(renderer);
            Picking::Result pickingResult;
            Picking::ScreenCoordinates pickingInput(pickCoords.imageX, pickCoords.imageY);

            //returns true if some object was picked
            if(picking.pick(pickingInput, pickingResult))
            {

                //for(Picking::tGroupHandleVector::iterator it = pickingResult.mGroupsPath.begin();
                //    it != pickingResult.mGroupsPath.end();
                //    it++)
                //{
                //    std::cout << &(*(*it)) << " ";
                //}
                //std::cout << ">> " << &(*(pickingResult.mGeometry)) << "/" << pickingResult.mPrimitiveNo << std::endl;

                //moves the box to the coordinates of the picked object
                Transform transform;
                picking.getResultTransformation(pickingResult, transform);
                cursorGroup->setTransform(transform);
            }
        }

        ///

        cameraController.applyChanges();
        cameraController.getParams(camParams);

        renderer->useProgressiveRefinement(allowProgressiveRefinement && !cameraController.cameraChanging() && !renderAnaglyph);
        renderer->setCameraPosition(
            float3(camParams.position.x, camParams.position.y, camParams.position.z),
            float3(camParams.direction.x, camParams.direction.y, camParams.direction.z),
            float3(camParams.up.x, camParams.up.y, camParams.up.z));

        cameraController.acceptChanges();

        if(sceneID == 3)
        {
            if(animLight != 0)
            {
                iesLightAngle += lightAnimSpeed;
                animLight->setParam("offsetAngle", iesLightAngle);
            }
        }
        if(appController.shouldTerminate())
        {
            break;
        }
    }

    delete renderer;

    return 0;
}
#endif

