/*
 *  RTpieDemo.cpp
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

 #include <RTfact/Config/Common.hpp>

#include <RTpieCpp/RayTracerHandle.hpp>

#include <RTpie/Utils/TransformTools.hpp>

#include <RTfact/Config/Init.hpp>

#include <RTfact/Utils/IO/Loaders/Image2DLoaderTGA.hpp>

#include <RTfact/Utils/Containers/IntMap.hpp>

#include <RTfact/Model/Image/BasicImage2D.hpp>
#include <RTfact/Utils/UI/Windows/SDLGLWindow.hpp>
#include <RTfact/Model/Framebuffer/Image2DFramebuffer.hpp>
#include <RTfact/Model/Camera/PinholeCamera.hpp>

#include <RTfact/Utils/FPSMeter.hpp>

#include <RTfact/Utils/UI/Controllers/PickingController.hpp>
#include <RTfact/Utils/UI/Controllers/CameraController.hpp>
#include <RTfact/Utils/UI/Controllers/ApplicationController.hpp>
#include <RTfact/Utils/IO/PathUtils.hpp>


//using namespace RTfact;
using namespace RTfact::UI;
using namespace RTfact::IO;
using namespace RTfact::RTpie;
using namespace RTpieCpp;
using namespace boost;

typedef RTfact::PinholeCamera                               t_Camera;
typedef RTfact::BasicImage2D<float>                         t_ColorImage;
typedef RTfact::BasicImage2D<float>                         t_DistanceImage;
//typedef RTfact::BasicImage2D<unsigned char>                 t_ColorImage;
//typedef RTfact::BasicImage2D<unsigned char>                 t_DistanceImage;
typedef RTfact::Image2DFramebuffer<
                t_ColorImage,
                t_DistanceImage>                            t_Framebuffer;
typedef SDLGLWindow                                         t_Window;
typedef PickingController                                   t_PickingController;


// -------------------------------------------
// Global Materials
// -------------------------------------------

const RTfact::RTpie::int32 GEOPLANE_INDICES[] = {
        0,1,2,0,2,3,
        4,5,6,4,6,7
};

const float GEOPLANE_POSITIONS[] = {
        -3, 0, -3, 3, 0, -3, 3, 0,  3, -3, 0, 3,
        -3, 0, -3, 3, 0, -3, 3, 6, -3, -3, 6, -3
};

const float GEOPLANE_NORMALS[] = {
        0, 1, 0,   0, 1, 0,   0, 1, 0,   0, 1, 0,
        0, 0, -1,   0, 0, -1,   0,  0,-1,   0,  0,-1
};

const RTfact::RTpie::int32 CUBEMESH_INDICES[] = {
        0,1,2,5,4,3,
        8,7,6,9,10,11,
        12,13,14,17,16,15,
        18,19,20,23,22,21,
        26,25,24,27,28,29,
        30,31,32,35,34,33,
};
const float CUBEMESH_POSITIONS[] = {
        // front
        0, 0, 0,   1, 0, 0,   0, 1, 0,
        1, 0, 0,   0, 1, 0,   1, 1, 0,
        // back
        0, 0, 1,   1, 0, 1,   1, 1, 1,
        0, 0, 1,   0, 1, 1,   1, 1, 1,
        // left
        0, 0, 0,   0, 1, 0,   0, 1, 1,
        0, 0, 0,   0, 0, 1,   0, 1, 1,
        // right
        1, 0, 0,   1, 0, 1,   1, 1, 1,
        1, 0, 0,   1, 1, 0,   1, 1, 1,
        // bottom
        0, 0, 0,   1, 0, 0,   1, 0, 1,
        0, 0, 0,   0, 0, 1,   1, 0, 1,
        // top
        0, 1, 0,   1, 1, 0,   1, 1, 1,
        0, 1, 0,   0, 1, 1,   1, 1, 1,
};

const float CUBEMESH_POSITIONS2[] = {
        // front
        -0.2f, -0.2f, -0.2f,   1.2f, -0.2f, -0.2f,   -0.2f, 1.2f, -0.2f,
        1.2f, -0.2f, -0.2f,   -0.2f, 1.2f, -0.2f,   1.2f, 1.2f, -0.2f,
        // back
        -0.2f, -0.2f, 1.2f,   1.2f, -0.2f, 1.2f,   1.2f, 1.2f, 1.2f,
        -0.2f, -0.2f, 1.2f,   -0.2f, 1.2f, 1.2f,   1.2f, 1.2f, 1.2f,
        // left
        -0.2f, -0.2f, -0.2f,   -0.2f, 1.2f, -0.2f,   -0.2f, 1.2f, 1.2f,
        -0.2f, -0.2f, -0.2f,   -0.2f, -0.2f, 1.2f,   -0.2f, 1.2f, 1.2f,
        // right
        1.2f, -0.2f, -0.2f,   1.2f, -0.2f, 1.2f,   1.2f, 1.2f, 1.2f,
        1.2f, -0.2f, -0.2f,   1.2f, 1.2f, -0.2f,   1.2f, 1.2f, 1.2f,
        // bottom
        -0.2f, -0.2f, -0.2f,   1.2f, -0.2f, -0.2f,   1.2f, -0.2f, 1.2f,
        -0.2f, -0.2f, -0.2f,   -0.2f, -0.2f, 1.2f,   1.2f, -0.2f, 1.2f,
        // top
        -0.2f, 1.2f, -0.2f,   1.2f, 1.2f, -0.2f,   1.2f, 1.2f, 1.2f,
        -0.2f, 1.2f, -0.2f,   -0.2f, 1.2f, 1.2f,   1.2f, 1.2f, 1.2f,
};

const float CUBEMESH_NORMALS[] = {
        // front
        0, 0, -1,  0, 0, -1,  0, 0, -1,
        0, 0, -1,  0, 0, -1,  0, 0, -1,
        // back
        0, 0, 1,   0, 0, 1,   0, 0, 1,
        0, 0, 1,   0, 0, 1,   0, 0, 1,
        // left
        -1, 0, 0,  -1, 0, 0,  -1, 0, 0,
        -1, 0, 0,  -1, 0, 0,  -1, 0, 0,
        // right
        1, 0, 0,   1, 0, 0,   1, 0, 0,
        1, 0, 0,   1, 0, 0,   1, 0, 0,
        // bottom
        0, -1, 0,   0, -1, 0,  0, -1, 0,
        0, -1, 0,   0, -1, 0,  0, -1, 0,
        // top
        0, 1, 0,   0, 1, 0,   0, 1, 0,
        0, 1, 0,   0, 1, 0,   0, 1, 0,
};

const RTfact::RTpie::int32 CUTPLANE_INDICES[] = {
    0, 1, 2, 3, 4, 5};
const float CUTPLANE_POSITIONS[] = {
    -2.0f, -2.0f, -2.0f,   2.0f, -2.0f, -2.0f,   -2.0f, 2.0f, -2.0f,
    2.0f, -2.0f, -2.0f,   -2.0f, 2.0f, -2.0f,   2.0f, 2.0f, -2.0f};
const float CUTPLANE_NORMALS[] = {
    0, 0, -1,  0, 0, -1,  0, 0, -1,
    0, 0, -1,  0, 0, -1,  0, 0, -1};

// -------------------------------------------
// Global Handles
// -------------------------------------------
GeometryHandle cubeGeometry;
GeometryHandle bigCubeGeometry;
GeometryHandle cutPlane;
LightHandle light, light2;
MeshHandle cubePlanes[6];
MeshHandle bigCubeMesh;
MeshHandle cutPlaneMesh;
InstanceHandle cubeInstance1, cubeInstance2, cursorInstance;

GeometryHandle groundGeometry;
MeshHandle groundMesh;
InstanceHandle groundInstance;
InstanceHandle cutPlaneInstance;


AppearanceHandle redShader;


// -------------------------------------------
// initialisation functions
// -------------------------------------------
void initEnvTexture(SceneHandle scene,
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


        BufferHandle buffer = CreateBufferHandleUseData(envTex->getResX()*envTex->getResY()*envTex->getComponentCount()*sizeof(float),
            envTex->getFirstComponent(0,0));
        Image2DHandle envTexx = CreateImage2DHandleCopyData(
                IImage2D::COMPONENT_FLOAT,
                envTex->getComponentCount(),
                envTex->getResX(),
                envTex->getResY(),
                buffer);

        scene.setEnvironmentTexture(envTexx);
        scene.setEnvironmentColor(1, 0, 0, 0);

        delete envTex;
    }
}

CameraHandle initCamera(CameraController &cameraController,
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

    CameraHandle camera = CreateCameraHandle();

    camera.setPosition(float3(camParams.position.x, camParams.position.y, camParams.position.z));
    camera.setDirection(float3(camParams.direction.x, camParams.direction.y, camParams.direction.z));
    camera.setUpVector(float3(camParams.up.x, camParams.up.y, camParams.up.z));
    camera.setOpeningAngle(camParams.openingAngle);

    // Configure Camera for Application
    cameraController.setParams(camParams);
    cameraController.applyChanges();
    cameraController.getParams(camParams);

    return camera;
}

void initWindow(t_Window &window,
               Configuration &config)
{
    const RTfact::uint resX = config["resX"].as<RTfact::uint>();
    const RTfact::uint resY = config["resY"].as<RTfact::uint>();
    // Set Window size and caption
    window.init(resX, resY);
    window.setCaption("RTpie Demo");
}

FrameBufferHandle  initFramebuffer(
                     t_Framebuffer &framebuffer,
                     t_ColorImage **colorImageAddr,
                     t_DistanceImage **distanceBufferAddr,
                     Configuration &config)
{
    const RTfact::uint resX = config["resX"].as<RTfact::uint>();
    const RTfact::uint resY = config["resY"].as<RTfact::uint>();

    //color buffer
    *colorImageAddr = new t_ColorImage(3, resX, resY);
    t_ColorImage* colorImage = *colorImageAddr;

    framebuffer.setColorImage(colorImage);

    RTfact::uint colorBufferLength = colorImage->getResX() * colorImage->getResY()* colorImage->getComponentCount() * sizeof(t_ColorImage::Component);
    BufferHandle colorBuffer = CreateBufferHandleUseData(colorBufferLength, colorImage->getFirstComponent(0,0));
    RTpieCpp::Image2DHandle::ComponentType colorImageType = sizeof(t_ColorImage::Component) == 4 ? IImage2D::COMPONENT_FLOAT : IImage2D::COMPONENT_BYTE;
    Image2DHandle colorBufferHandle = CreateImage2DHandleUseData(
            colorImageType,
            3,
            colorImage->getResX(),
            colorImage->getResY(),
            colorBuffer
        );
    FrameBufferHandle renderBuffer = CreateFrameBufferHandle();
    renderBuffer.setColorBuffer(colorBufferHandle);

    //distance buffer
    *distanceBufferAddr = new t_DistanceImage(1, resX, resY);
    t_DistanceImage* distanceImage = *distanceBufferAddr;

    framebuffer.setDistanceImage(distanceImage);

    RTfact::uint distanceBufferLength = distanceImage->getResX() * distanceImage->getResY()* distanceImage->getComponentCount() * sizeof(t_DistanceImage::Component);
    BufferHandle distanceBuffer = CreateBufferHandleUseData(distanceBufferLength, distanceImage->getFirstComponent(0,0));
    RTpieCpp::Image2DHandle::ComponentType distanceImageType = sizeof(t_DistanceImage::Component) == 4 ? IImage2D::COMPONENT_FLOAT : IImage2D::COMPONENT_BYTE;
    Image2DHandle distanceBufferHandle = CreateImage2DHandleUseData(
            distanceImageType,
            1,
            distanceImage->getResX(),
            distanceImage->getResY(),
            distanceBuffer
        );

    renderBuffer.setDistanceBuffer(distanceBufferHandle);

    return renderBuffer;
}

// -------------------------------------------
// scene creation functions
// -------------------------------------------

void createSimpleScene(SceneHandle scene)
{
    // --------------------------------------------------------
    // Lightsources
    // --------------------------------------------------------

    light = scene.createLight("PointLight");
    light.setParam3f("position", float3(-4,4,0));
    light.setParam3f("intensity", float3(70,70,70));
    light.setParam3f("attenuation", float3(0,0,1));

    // --------------------------------------------------------
    // Materials
    // --------------------------------------------------------
    AppearanceHandle cyanShader =
        scene.createAppearance("XML3DPhongShaderGI");
    cyanShader.setParam3f("diffuseColor", float3(0.0f, 1.0f, 1.0f));
    cyanShader.setParam3f("ambientIntensity", float3(.0f, .0f, .0f));

    AppearanceHandle yellowShader =
        scene.createAppearance("XML3DPhongShaderGI");
    yellowShader.setParam3f("diffuseColor", float3(1.0f, 1.0f, 0.0f));
    yellowShader.setParam3f("ambientIntensity", float3(.0f, .0f, .0f));

    AppearanceHandle cutPlaneShader =
        scene.createAppearance("XML3DPhongShaderGI");
    cutPlaneShader.setParam3f("diffuseColor", float3(0.5f, 0.5f, 0.5f));
    cutPlaneShader.setParam3f("ambientIntensity", float3(.0f, .0f, .0f));
    cutPlaneShader.setParamf("transparency", 0.9f);


    // --------------------------------------------------------
    // Geometries
    // --------------------------------------------------------

    bigCubeGeometry = scene.createGeometry();
    bigCubeMesh = bigCubeGeometry.createMesh();
    bigCubeMesh.setPrimitives(
            12,
            CUBEMESH_INDICES,
            CUBEMESH_POSITIONS,
            CUBEMESH_NORMALS,
            0, 0);
    bigCubeMesh.setAppearance(cyanShader);

    groundGeometry = scene.createGeometry();
    groundMesh = groundGeometry.createMesh();
    groundMesh.setPrimitives(
            4,
            GEOPLANE_INDICES,
            GEOPLANE_POSITIONS,
            GEOPLANE_NORMALS,
            0,
            0);
    groundMesh.setAppearance(yellowShader);

    // --------------------------------------------------------
    // Instances
    // --------------------------------------------------------

    float mat[16], invMat[16];

    cursorInstance = bigCubeGeometry.createInstance();

    cubeInstance1 = bigCubeGeometry.createInstance();
    TransformTools::GetTranslation(0, 0, -2.8f, mat, invMat);
    cubeInstance1.setTransform(mat, invMat);

    cubeInstance2 = bigCubeGeometry.createInstance();
    TransformTools::GetTranslation(2, 0, 0, mat, invMat);
    cubeInstance2.setTransform(mat, invMat);

    groundInstance = groundGeometry.createInstance();

    //cut visualization
    cutPlane = scene.createGeometry();
    cutPlaneMesh = cutPlane.createMesh();
    cutPlaneMesh.setPrimitives(
            2,
            CUTPLANE_INDICES,
            CUTPLANE_POSITIONS,
            CUTPLANE_NORMALS,
            0, 0);
    cutPlaneMesh.setAppearance(cutPlaneShader);
    cutPlaneInstance = cutPlane.createInstance();
    cutPlaneInstance.setDropShadow(false);

    //
    float c1[] = {1,0, 0,0, 0,-1,
                  0.5,0, 0,0, 0,0.5};
    cubeInstance1.setCutPlanes(true,float3(1, 0, 0),2,c1+0,c1+2,c1+4,c1+6,c1+8,c1+10);

    float c2[] = {1,1,0,0.5f,0.5f,0};
    cubeInstance2.setCutPlanes(true,float3(0, 1, 0),1,c2+0,c2+1,c2+2,c2+3,c2+4,c2+5);

    float c3[] = {1,1,1,1,1,0};
    cursorInstance.setCutPlanes(true,float3(1, 0, 0),1,c3+0,c3+1,c3+2,c3+3,c3+4,c3+5);

}

void createSceneDefault(SceneHandle scene)
{
    // --------------------------------------------------------
    // Lightsources
    // --------------------------------------------------------

    light = scene.createLight("PointLight");

    //light->setParam("castShadow", false);
    light.setParam3f("position", float3(0,10,0));
    light.setParam3f("intensity", float3(100,100,100));
    light.setParam3f("attenuation", float3(0,0,1));


    light2 = scene.createLight("PointLight");

    light2.setParam3f("position", float3(10,10,0));
    light2.setParam3f("intensity", float3(10,100,10));
    light2.setParam3f("attenuation", float3(0,0,1));


    // --------------------------------------------------------
    // Materials
    // --------------------------------------------------------
    AppearanceHandle blueShader =
        scene.createAppearance("PhongShader");
    blueShader.setParam3f("diffuseColor", float3(0.0f, 0.0f, 1.0f));
    blueShader.setParam3f("ambientIntensity", float3(.7f, .7f, .7f));

    redShader =
        scene.createAppearance("XML3DPhongShader");
    redShader.setParam3f("diffuseColor", float3(1.0f, 0.0f, 0.0f));
    redShader.setParam3f("ambientIntensity", float3(.7f, .7f, .7f));

    AppearanceHandle greenShader =
        scene.createAppearance("PhongShader");
    greenShader.setParam3f("diffuseColor", float3(0.0f, 1.0f, 0.0f));
    greenShader.setParam3f("ambientIntensity", float3(.7f, .7f, .7f));

    AppearanceHandle yellowShader =
        scene.createAppearance("PhongShader");
    yellowShader.setParam3f("diffuseColor", float3(1.0f, 1.0f, 0.0f));
    yellowShader.setParam3f("ambientIntensity", float3(.7f, .7f, .7f));

    AppearanceHandle violetShader =
        scene.createAppearance("PhongShader");
    violetShader.setParam3f("diffuseColor", float3(1.0f, 0.0f, 1.0f));
    violetShader.setParam3f("ambientIntensity", float3(.7f, .7f, .7f));

    AppearanceHandle blaShader =
        scene.createAppearance("PhongShader");
    blaShader.setParam3f("diffuseColor", float3(0.0f, 1.0f, 1.0f));
    blaShader.setParam3f("ambientIntensity", float3(.7f, .7f, .7f));


    AppearanceHandle coloredTriangleShader =
        scene.createAppearance("PhongShader");

    coloredTriangleShader.setParam3f("diffuseColor", float3(1.0f, 1.0f, 0));
    coloredTriangleShader.setParam3f("ambientIntensity", float3(.7f, .7f, .7f));
    coloredTriangleShader.setParamb("useVertexColor", true);

    // --------------------------------------------------------
    // Geometries
    // --------------------------------------------------------

    cubeGeometry = scene.createGeometry();

    for(int i=0; i<6; i++){
        cubePlanes[i] = cubeGeometry.createMesh();
        cubePlanes[i].setPrimitives(
                2,
                &CUBEMESH_INDICES[i*6],
                 CUBEMESH_POSITIONS,
                 CUBEMESH_NORMALS,
                0,
                0);
        cubePlanes[i].setAppearance(blueShader);
    }
    cubePlanes[1].setAppearance(redShader);
    cubePlanes[4].setAppearance(greenShader);
    cubePlanes[5].setAppearance(greenShader);


    bigCubeGeometry = scene.createGeometry();
    bigCubeMesh = bigCubeGeometry.createMesh();
    bigCubeMesh.setPrimitives(
            12,
            CUBEMESH_INDICES,
            CUBEMESH_POSITIONS2,
            CUBEMESH_NORMALS,
            0, 0);
    bigCubeMesh.setAppearance(blaShader);

    // delete 5 Meshes again
    //for(int i=0; i<5; i++){
    //    cubePlanes[i] = MeshHandle();
    //}
    //coloredTriangle->setAppearance(coloredTriangleShader);

    groundMesh = scene.createGeometry().createMesh();
    groundMesh.setPrimitives(
            2,
            &CUBEMESH_INDICES[4*6],
            CUBEMESH_POSITIONS,
            CUBEMESH_NORMALS,
            0,
            0);


    // --------------------------------------------------------
    // Instances
    // --------------------------------------------------------

    float mat[16], invMat[16];
    cubeInstance1 = cubeGeometry.createInstance();

    cubeInstance2 = cubeGeometry.createInstance();
    cubeInstance2.setAppearance(cubePlanes[1], violetShader);
    cubeInstance2.setAppearance(cubePlanes[2], yellowShader);
    cubeInstance2.setAppearance(cubePlanes[3], yellowShader);
    TransformTools::GetTranslation(2.0, 0, 0, mat, invMat);
    cubeInstance2.setTransform(mat, invMat);

    cursorInstance = bigCubeGeometry.createInstance();
    TransformTools::GetTranslation(0, 0, 2, mat, invMat);
    cursorInstance.setTransform(mat, invMat);
    // change color Assignment in second Instance;


    /*
    groundInstance = groundMesh.getGeometry().createInstance();
    groundInstance.setTransform(
            Transform::translation(
                2.0, 0, 0)

    );
     */

    /*
    GeoHandle groundGeoPlane2 = root->getGeometry(0);
    // --------------------------------------------------------
    // Check Mesh comparisions
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


    GroupHandle root2= renderer->getRoot();
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

    */

}

/*
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

    coloredTriangleShader->setParam("diffuseColor", float3(1.0, 1.0, 0));
    coloredTriangleShader->setParam("ambientIntensity", float3(.7, .7, .7));
    coloredTriangleShader->setParam("useVertexColor", true);

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
*/


void createSceneEmptyTest(SceneHandle scene)
{
    // Create Temporal Geometry

    MeshHandle  groundMesh = scene.createGeometry().createMesh();
    groundMesh.setPrimitives(
            2,
            &CUBEMESH_INDICES[4*6],
            CUBEMESH_POSITIONS,
            CUBEMESH_NORMALS,
            0,
            0);
    InstanceHandle groundInstance = groundMesh.getGeometry().createInstance();

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

    // RTpie Scene &Renderer for scene construction and rendering
    SceneHandle             scene;
    RayTracerHandle         rayTracer;
    CameraHandle            camera;
    FrameBufferHandle       renderBuffer;
    RenderTaskHandle        renderTask;

    // SDLGLSLWindow to display the scene
    t_Window                window;

    // FrameBuffer and FrameBufferImage are the connection between window and renderer
    t_ColorImage*           framebufferImage;
    t_DistanceImage*        distanceImage;
    t_Framebuffer           framebuffer;

    // A little application, connected to the window, for camera control and other stuff
    ApplicationController appController(window);
    t_PickingController cameraController(
        window, RTfact::BBox(RTfact::Vec3f1(0, 0, 0), RTfact::Vec3f1(100, 100, 100)));
    RTfact::FPSMeter fpsMeter;

    // ----------------------------------------------------------
    // Initialisation
    // ----------------------------------------------------------
    rayTracer = CreateRayTracerHandle();
    scene    = CreateSceneHandle();

    // ----------------------------------------------------------
    // Attach Scene to Ray Tracer
    // ----------------------------------------------------------
    rayTracer.attachScene(scene);

    // ----------------------------------------------------------
    // RenderTask Creation and Configuration
    // ----------------------------------------------------------

    initWindow(window, config);

    camera = initCamera(cameraController, config);
    renderBuffer = initFramebuffer(framebuffer, &framebufferImage, &distanceImage, config);

    renderTask = rayTracer.createRenderTask();
    renderTask.setCamera(camera);
    renderTask.setFrameBuffer(renderBuffer);
    renderTask.setOverSamplingRate(1);
    renderTask.setAccumulatePixels(true);
    renderTask.setMaxDepth(20);

    // parameterizable interface test
    void *p = 0;
    renderTask.get()->QueryInterface(RTfact::RTpie::IParameterizable::GetIID(), &p);
    if(p)
    {
        RTfact::RTpie::IParameterizable* parameterizable =
            static_cast<RTfact::RTpie::IParameterizable*>(p);
        parameterizable->BeginParams();
        parameterizable->SetParamu("max_particle_depth", 10);
        parameterizable->EndParams();
    }

    int renderMode = 0;
    if(config.exists("renderMode"))
        renderMode = config["renderMode"].as<RTfact::uint>();
    renderTask.setGlobalIllumination(renderMode == 1);

    // ----------------------------------------------------------
    // Scene Configuration & Construction
    // ----------------------------------------------------------
    initEnvTexture(scene, configDir, config);

    int sceneID = 0;
    if(config.exists("sceneID"))
        sceneID = config["sceneID"].as<RTfact::uint>();

    switch(sceneID)
    {
    case 0:
        createSceneDefault(scene);
        break;
    case 1:
        //createSceneDeleteTest(scene);
        break;
    case 2:
        createSceneEmptyTest(scene);
        break;
    case 3:
        createSimpleScene(scene);
        break;
    default:
        createSceneDefault(scene);
        break;
    }



    // ----------------------------------------------------------
    // Render Loop
    // ----------------------------------------------------------
    t_Camera::Params camParams;
    int frames = 0;
    for(;;)
    {

        framebuffer.prePaint();

        fpsMeter.startFrame();

        rayTracer.syncStructures();
        renderTask.run();

        fpsMeter.endFrame();

        framebuffer.postPaint();

        RTFACT_LOG_F("FPS: %3.1f            \r", fpsMeter.getFPS());

        window.update(*framebufferImage);
        window.display();

        window.handleEvents();

        ///picking -----------------------------------------------
        PickingController::tScreenSample pickCoords;
        //returns true if picking was requested by the user, together with the requested screen position
        if(cameraController.pick(pickCoords))
        {
            PickTaskHandle pickTask = rayTracer.createPickTask();
            pickTask.setCamera( camera );
            pickTask.pick( static_cast<int>(pickCoords.imageX),
                            static_cast<int>(pickCoords.imageY),
                    renderBuffer.getWidth(), renderBuffer.getHeight());

            //returns true if some object was picked
            if(pickTask.hasResult())
            {

                //for(Picking::tGroupHandleVector::iterator it = pickingResult.mGroupsPath.begin();
                //    it != pickingResult.mGroupsPath.end();
                //    it++)
                //{
                //    std::cout << &(*(*it)) << " ";
                //}
                //std::cout << ">> " << &(*(pickingResult.mGeometry)) << "/" << pickingResult.mPrimitiveNo << std::endl;

                //moves the box to the coordinates of the picked object
                float mat[16], invMat[16];
                pickTask.getInstance().getTransform(mat, invMat);
                pickTask.getInstance().setAppearance(
                        pickTask.getMesh(), redShader);
                //cursorInstance.setTransform(transform);
            }
        }
        // ---------------------------------------------

        cameraController.applyChanges();
        cameraController.getParams(camParams);

        camera.setPosition( float3(camParams.position.x, camParams.position.y, camParams.position.z) );
        camera.setDirection( float3(camParams.direction.x, camParams.direction.y, camParams.direction.z) );
        camera.setUpVector( float3(camParams.up.x, camParams.up.y, camParams.up.z) );

        cameraController.acceptChanges();

        if(appController.shouldTerminate())
        {
            break;
        }
    }

    return 0;
}


