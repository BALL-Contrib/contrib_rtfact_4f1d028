/*
 *  Renderer.hpp
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
 *  Created on: 2008-11-06 02:52:20 +0100
 *  Author(s): Felix Klein, Iliyan Georgiev, Lukas Marsalek, Ken Dahm, Dmitri Rubinstein, Ken Patrick Dahm, Tomas Davidovic, Vaclav Krajicek
 */

#ifndef RTFACT_REMOTE__RENDERER_HPP
#define RTFACT_REMOTE__RENDERER_HPP


#include <RTremote/Config/Common.hpp>


#include <boost/intrusive_ptr.hpp>
#include <vector>
#include <memory>
#include <boost/function.hpp>

#include <RTremote/Parameterizable.hpp>

#define RTFACT_REMOTE_ANYSL_VERSION 1

//Disable MSVC warning about non-exported dll interface caused by boost::intrusive_ptr
#if defined(RTREMOTE_COMPILER_MSVC)
#   pragma warning(push)
#   pragma warning(disable:4251)
#endif

// External Includes
// #include <RTfact/Concept/Appearance/Material.hpp>
// #include <RTfact/Model/Primitive/Triangle.hpp>
// #include <RTfact/Utils/Transform.hpp>
// #include <RTfact/Utils/Containers/Vector.hpp>
//#include <RTfact/Utils/UI/Window.hpp>
#include <RTfact/Utils/ImageClipRegion.hpp>

// Predefined Types
typedef unsigned int Handle;
typedef std::vector<RTfact::Remote::Triangle> TriangleVector;

// Internal Includes
#include <RTremote/GraphEntity.hpp>
#include <RTremote/Geometry.hpp>
#include <RTremote/Group.hpp>
#include <RTremote/RTAppearance.hpp>
#include <RTremote/RTLight.hpp>
#include <RTremote/RTImage.hpp>

#define RTFACT_REMOTE__DEFAULT_RESX 512
#define RTFACT_REMOTE__DEFAULT_RESY 512

RTFACT_REMOTE_NAMESPACE_BEGIN

/* RendererPimpl contains the implementation of the renderer, hidden from
 * the external application
 */
class RendererPimpl;

struct ClipData
{
	unsigned int top;
	unsigned int left;
	unsigned int bottom;
	unsigned int right;
};

/*
 * Parameters for specification of skewed truncated viewing frustum
 * Corresponds to the OpenGL glFrustum call semantic
 */

struct FrustumParameters
{
    float left;
    float right;
    float bottom;
    float top;
    float nearPlane;    //"Plane" is required here because "near" is a pointer typename in C++
    float farPlane;     //"Plane" is required here because "far" is a pointer typename in C++
};

/**
 *  The Renderer class is the core component of RTremote.
 *
 *  The content of the scene can be created over the renderer, using
 *  methods like createGroup() and createGeometry()
 *
 *  With attachFrameBuffer() a frame buffer can be connected to the
 *  renderer. With renderToBuffer() the scene is rendered to the
 *  framebuffer
 */
class RTFACT_REMOTE_API Renderer
{
    friend class Picking;
    friend class Group;
    friend class Geometry;
    friend class GraphEntity;
    friend class Appearance;
    friend class Lightsource;

public:

    /**
     * Names and values of the parameters that can be passed to the Renderer's startup configuration string
     */
    struct RTFACT_REMOTE_API StartupParams
    {
        /**
        * Number of parallel rendering threads. Can take value of -1 or any integer in interval <1, inf).
        * Set it to -1 to use as many threads as available/necessary.
        */
        static const char* PARAM_NAME__NUMBER_OF_RENDER_THREADS;
        /**
         * Granularity of the parallelization - tiles of size granularity x granularity will be assigned to a single render task
         * Set it to -1 to automatically determine the best granularity
         */
        static const char* PARAM_NAME__GRANULARITY;

        /*
         * Default or often used values of the above mentioned parameters
         */
        struct Values
        {
            static const int PARAM_AUTOMATIC = -1;
        };
    };

    //typedef boost::function<void(const UI::Event&)> EventHandler;

    void paramTestFunction(const ParamValue &p);
    void paramTestFunction2(const float3 &p);

    /**
     * Constructor.
     *
     * The root node of the scene is created here as well as a default material.
     *
     * @param aStartupConfiguration is a configuration that must be fixed at creation time and cannot be changed later.
     * The string has the format of key=value pairs, separated by new lines. For available keys and their values, see the StartupParams structure.
     */
    Renderer(const char* aStartupConfiguration = NULL);


    /**
     * Destructor
     */
    ~Renderer();

    //-----------------------------------------------------------------------------
    // Window & Framebuffer
    //-----------------------------------------------------------------------------

    /**
     * attach an float framebuffer to the renderer. The image is rendered to this
     * buffer if the renderToBuffer() function is called
     * @param data a pointer to the framebuffer data
     * @param componentCount number of floats used per pixel (e.g. RGB color has 3 components)
     * @param width width of the framebuffer in pixels
     * @param height height of the framebuffer in pixels
     */
    void attachFrameBuffer(
        float* data, unsigned int componentCount, unsigned int width, unsigned int height);

    /**
     * attach an float framebuffer to the renderer. The image is rendered to this
     * buffer if the renderToBuffer() function is called
     * @param data a pointer to the framebuffer data
     * @param componentCount number of floats used per pixel (e.g. RGB color has 3 components)
     * @param width width of the framebuffer in pixels
     * @param height height of the framebuffer in pixels
		 * @param lineWidth the width of each line in pixels
     */
    void attachFrameBuffer(
        float* data, unsigned int componentCount, unsigned int width, unsigned int height, unsigned int lineWidth);

    /**
     * attach an float framebuffer to the renderer. The tile is rendered to this
     * buffer if the renderToBuffer() function is called
     * @param data a pointer to the framebuffer data
     * @param componentCount number of floats used per pixel (e.g. RGB color has 3 components)
     * @param width width of the framebuffer in pixels
     * @param height height of the framebuffer in pixels
     * @param clip struct with clipping coordinates
     */
    void attachFrameBuffer(
        float* data, unsigned int componentCount, unsigned int width, unsigned int height, ClipData& clip);

    /**
     * attach an integer framebuffer to the renderer. The image is rendered to this
     * buffer if the renderToBuffer() function is called
     * @param data a pointer to the framebuffer data
     * @param componentCount number of integers used per pixel (e.g. RGB color has 3 components)
     * @param width width of the framebuffer in pixels
     * @param height height of the framebuffer in pixels
     */
    void attachFrameBuffer(
        unsigned char* data, unsigned int componentCount, unsigned int width, unsigned int height);

    /**
     * attach an integer framebuffer to the renderer. The image is rendered to this
     * buffer if the renderToBuffer() function is called
     * @param data a pointer to the framebuffer data
     * @param componentCount number of integers used per pixel (e.g. RGB color has 3 components)
     * @param width width of the framebuffer in pixels
     * @param height height of the framebuffer in pixels
		 * @param lineWidth the width of each line in pixels
     */
    void attachFrameBuffer(
        unsigned char* data, unsigned int componentCount, unsigned int width, unsigned int height, unsigned int lineWidth);

    /**
     * attach an integer framebuffer to the renderer. The tile is rendered to this
     * buffer if the renderToBuffer() function is called
     * @param data a pointer to the framebuffer data
     * @param componentCount number of integers used per pixel (e.g. RGB color has 3 components)
     * @param width width of the framebuffer in pixels
     * @param height height of the framebuffer in pixels
     * @param clip struct with clipping coordinates
     */
    void attachFrameBuffer(
        unsigned char* data, unsigned int componentCount, unsigned int width, unsigned int height, ClipData& clip);

    /**
     * attach a distance buffer to the renderer. The renderer stores the depth value of each
     * pixel in the depth buffer.
     * @param data a pointer to the framebuffer data
     * @param width width of the framebuffer in pixels
     * @param height height of the framebuffer in pixels
     */
    void attachDistanceBuffer(
        float* data, unsigned int width, unsigned int height);

    /**
     * attach a normal buffer to the renderer. The renderer stores the normal vector of each
     * pixel in the normal buffer.
     * @param data a pointer to the framebuffer data
     * @param width width of the framebuffer in pixels
     * @param height height of the framebuffer in pixels
     */
    void attachNormalBuffer(
        float* data, unsigned int width, unsigned int height);

	/**
     * attach an float ambient occlusion buffer to the renderer. The amount occlusion is rendered to this
     * buffer if the renderToBuffer() function is called
     * @param data a pointer to the buffer data
     * @param width width of the framebuffer in pixels
     * @param height height of the framebuffer in pixels
     */
    void attachAmbientOcclusionBuffer(
        float* data, unsigned int width, unsigned int height);

    void attachColorBleedingBuffer(
        float* data, unsigned int width, unsigned int height);

    //-----------------------------------------------------------------------------
    // Scene Construction
    //-----------------------------------------------------------------------------

    enum
    {
        GEOMETRYTYPE_STATIC = 1,
        GEOMETRYTYPE_DYNAMIC   = (1 << 1)
    };

    /**
     * Notify the renderer that the scene geometry is about to be constructed/loaded.
     * Call before all createGeometry function calls.
     * @param geometryType type of the scene geometry.
     */
    void startConstruction(uint geometryType);

    /**
     * Notify the renderer that the scene geometry construction is finished so that
     * it can run additional geometry post-processing (flattening,..). Call after all
     * createGeometry function calls.
     * @param geometryType type of the scene geometry.
     */
    void finishConstruction(uint geometryType);

    /**
     * return the root of the scene graph.
     * Note that a new Group Object is created whenever this function is called.
     * @return a GroupHandle to the root node of the scene graph
     */
    GroupHandle getRoot();

    /**
     *  create a group node. The newly created node is not connected to the scene graph.
     *  @param t the transformation used by the group. Use RTfact::Transform::identity() for identity transformation
     *  @return a GroupHandle to the newly created group.
     */
    GroupHandle createGroup(
        const Transform& t);

    /**
     *  create an empty geometry node. The newly created node is not connected to the scene graph.
     *  @return a GeoHandle to the newly created geometry.
     */
    GeoHandle createGeometry();

	  /**
     *  create a geometry node containing all the triangles of the given vector.
     *  The newly created node is not connected to the scene graph.
     *  @param triangles the triangles contained by the geometry
	   *	@return a GeoHandle to the newly created geometry.
     */
    GeoHandle createGeometry(
        TriangleVector &triangles);

	  /**
     *  create a geometry node containing the mesh specified by the arguments.
     *  The newly created node is not connected to the scene graph.
     *  @param aVertices the vertices of the mesh. 3 consecutive floats define a vertex
     *  @param aNormals the normals of the mesh. 3 consecutive floats define a normal
     *  @param aVertexColors the vertex colors of the mesh. 4 consecutive floats define a color
     *  @param aTriangles the triangles of the mesh. 3 consecutive integers are used as indices to the vertex, normal and color arrays to define a triangle.
     *  @param aTriangleCount the number of triangles of the mesh
     *  @param aAppearance the Appearance used by the mesh
	   *	@return a GeoHandle to the newly created geometry.
     */
    GeoHandle createGeometry(
        const float* aVertices,
        const float* aNormals,
        const float* aVertexColors,
        const unsigned int* aTriangles,
        const unsigned int aTriangleCount,
        RTAppearanceHandle aAppearance);

    /**
     *  create a geometry node containing the mesh specified by the arguments.
     *  The newly created node is not connected to the scene graph.
     *  @param aVertices the vertices of the mesh. 3 consecutive floats define a vertex
     *  @param aNormals the normals of the mesh. 3 consecutive floats define a normal
     *  @param aTriangles the triangles of the mesh. 3 consecutive integers are used as indices to the vertex, normal and color arrays to define a triangle.
     *  @param aTriangleCount the number of triangles of the mesh
     *  @param aAppearance the Appearance used by the mesh
	   *	@return a GeoHandle to the newly created geometry.
     */
    GeoHandle createGeometry(
        const float* aVertices,
        const float* aNormals,
        const unsigned int* aTriangles,
        const unsigned int aTriangleCount,
        RTAppearanceHandle aAppearance);

    /**
     *  clears the scene.
     *  the connection of the root node is cut, resulting in deletion
     *  of group nodes, when not otherwise refered.
     *  Mesh data will be cleared ignoring any remaining reference
     *  on Geometry nodes.
     */
    void clearScene();

    /**
     *  travers a number of rays with the given origin and direction through the scene and
     *  store for each ray the distance from it's origin to the first intersection point.
     *  @param aOrigins the origins of the rays. 3 consecutive floats specify one origin.
     *  @param aDirections the directions of the rays. 3 consecutive floats specify one direction.
     *  @param aRayCount the number of rays
     *  @param oDistances the distance result of the traversal is stored at this memory adress. One float for each ray.
     */
    void intersectRays(
        const float* aOrigins,
        const float* aDirections,
        const unsigned int aRayCount,
        float* oDistances);

    /**
     *  create a lightsource.
     *  @param aType the type of the lightsource. Supported values: "PointLight", "SpotLight", "DirectionalLight"
     *  @return an RTLightHandle to the newly created lightsource
     */
    RTLightHandle createLight(
        const std::string& aType);

    /**
     * switch existing light source on.
     * @param aLight an RTLightHandle to the existing light source, obtained for example by createLight
     */
    void switchLightOn(
        RTLightHandle aLight);

    /**
     * switch existing light off.
     * @param aLight and RTLightHandle to an existing light source, obtained for example by createLight
     */
    void switchLightOff(
        RTLightHandle aLight);

    /**
     *  remove a lightsource.
     *  @param aLight the handle to the lightsource to be removed
     */
    void removeLight(
        RTLightHandle aLight);

    /**
     *  Update the acceleration structure of the scene.
     *  Note: The acceleration structure is created automatically, when renderToBuffer() is called.
     */
    void createAccelStruct();

    /**
     *  Set the environment texture of the scene.
     *  @param handle an RTImageHandle to the image to be used as environment texture
     *  @param upDimension dimension of the up axis. 0, 1 or 2 corresponds to X-, Y-, Z-axis respectively. Default value is 1 (Y axis).
     *  @param flip flips the direction of the up axis, e.g. X will become -X, Y will become -Y, and Z will become -Z. Default value is false.
     */
    void setEnvironmentTexture(const RTImageHandle &handle, const uint upDimension = 1, const bool flip = false);


    /**
     *  Set the Environment Texture of the scene.
     *  The given image data has to be encoded in RGB with 3 floats per pixel.
     *  @param data the pixel data of the image
     *  @param componentCount number of component (floats) per pixel
     *  @param width width of the image
     *  @param height height of the image
     */
    void setEnvironmentTexture(
        const float* data,
        const uint componentCount,
        const uint width,
        const uint height);

    /**
     *  Set the Environment Color of the scene.
     *  Sets the environment color (i.e. background) to a solid color defined by RGB
     *  @param aRed Red component, range 0-1
     *  @param aGreen Green component, range 0-1
     *  @param aBlue Blue component, range 0-1
     */
    void setEnvironmentColor(
            const float aRed,
            const float aGreen,
            const float aBlue,
            const float aAlpha = 1.0f);


    /**
     *  Create environment lights in the scene from the texture.
     *  @param aPartitionCount Numer of partitions (lights) that should be used.
     */
    void createEnvironmentLights(
            const unsigned int aPartitionCount);

    //-----------------------------------------------------------------------------
    // Camera Management
    //-----------------------------------------------------------------------------

    /**
     *  Set the Node of the internal scene graph, that contains the camera.
     *  The accumulated transformations of the path from the root to this node
     *  are applied on camera position and orientation.
     *  @param aGroup the handle to the group that contains the camera.
     */
    void setCameraNode(
        GroupHandle aGroup);

    /**
     *  Set camera positition, view direction and up vector.
     *  @param pos the position of the camera
     *  @param view the view direction of the camera
     *  @param up the up vector of the camera
     */
    void setCameraPosition(
        const float3 &pos,
        const float3 &view,
        const float3 &up);

    /**
     *  Set camera resolution.
     *  @param x_resolution the x resolution of the camera
     *  @param y_resolution the y resolution of the camera
     */
    void setCameraResolution(
        const int x_resolution,
        const int y_resolution);

    /**
     * Set the opening angle of the camera and computes the
     * frustum parameters accordingly. This method is mutually
     * exclusive with setFrustum() method.
     * @param angle the opening angle in degrees
     */
    void setCameraOpeningAngle(
        const float angle);
    /**
     *  Returns the opening angle of the camera in degrees.
     *  @return the opening angle of the camera in degrees
     */
    float getCameraOpeningAngle() const;

    /**
     * Set explicitly the frustum parameters
     * Enables to set asymmetric frusta, similar to OpenGL glFrustum call.
     * For simple cases, use the setOpeningAngle() method. This method and
     * the setOpeningAngle() methods are mutually exclusive.
     * @param aParams top, left, bottom, right, near, far distances of the frustum
     */
    void setFrustum(const FrustumParameters& aParams);

    /**
     * Returns the frustum parameters.
     * @return returns top, left, bottom, right, near, far frustum distances
     */
    FrustumParameters getFrustum();

    /**
     * This function sets the camera to directly map between frustum and screen space in the OpenGL-style,
     * without aspect ratio compensation. This is typically used for projectors. For normal
     * LCD monitors, the compensation should be on, e.g. do not call this function.
     * @param aUse true if non-square pixels should be used, false otherwise
     */
    void useNonSquarePixels(bool aUse = true);

    /**
     * Returns true, if non-square pixels are used, e.g. screen space aspect ratio is not compensated for
     * @return true, if non-square pixels are switched on
     */
    bool getNonSquarePixelsOn();

    /**
     *  store camera position, view direction and up vector into the specified arguments.
     *  @param pos the position of the camera
     *  @param view the view direction of the camera
     *  @param up the up vector of the camera
     */
    void getCameraPosition(
        float3 &pos,
        float3 &view,
        float3 &up);

    /** Set camera offset. This included a translation and rotation that applied relative
     *  to the camera position and orientation.
     */
    /*void setCameraOffset(
        const Vec3f<1> &,
        const Transform &);*/

    //-----------------------------------------------------------------------------
    // Rendering
    //-----------------------------------------------------------------------------

    enum
    {
        RENDERMODE_DIRECT_ILLUMINATION = 1,
        RENDERMODE_AMBIENT_OCCLUSION   = (1 << 1),
        RENDERMODE_PROGRESSIVE_GI = (1 << 2)
    };

    /**
    * Sets the rendering mode
    */
    void setRenderMode(uint renderFlag);

    /**
     * Set to true to progressively refine the rendering.
     * This means that successive frames are accumulated
     * (weighted together) with weights corresponding
     * to the inverse of so far accumulated frames.
     *
     * Only color buffer is progressively refined, e.g.
     * all other buffer are kept intact.
     *
     * !Warning! It is the responsibility of the application
     * to check, whether camera is moving or not and whether
     * the progressive refinement is going to cause some
     * troubles or not.
     * @param state true enables the progressive refinement,
     * false disables it.
     */
    void useProgressiveRefinement(bool state);

    /**
     * Returns the state of progressive refinement
     * @return true, if progressive refinement is active
     */
    bool isProgressiveRefinementOn();

    /**
     *  Render the scene to the specified Buffer.
     *  Make sure, that you attached a Buffer to the renderer via attachBuffer()
     *  before calling this function.
     */
    void renderToBuffer();

    /**
    *  Sets the oversampling rate of the pixel sampler. Must be a power of 2
    */
    void setOverSamplingRate(unsigned int rate);

    /**
    *  @return oversampling rate
    */
    unsigned int getOverSamplingRate() const;

    /**
     *  Create Appearance of Type aType
     *  @param aType Type of Appearance
     *  @return Handle to newly created Appearance
     */
    RTAppearanceHandle createAppearance(
    const std::string& aType);

    /**
     *  Get the Appearance under Index aIndex
     *  @return Handle to Appearance under Index aIndex
     */
	RTAppearanceHandle getAppearance(
		uint aIndex
		);

    /**
     *  Create an RTImage with the given attributes
     *  @param aData the pixel data of the image. The data is copied.
     *  @param aComponentCount Number of components per pixel
     *  @param aResX width of the image in pixels
     *  @param aResY height of the image in pixels
     *  @return Handle to the newly created Image
     */
    RTImageHandle createImage(
        RTImage::ComponentType aType,
        const uint aComponentCount,
        const uint aResX,
        const uint aResY);

    /**
     *  Create an RTImage with the given attributes and data
     *  Component Type of the new image is float
     *  @param aData the pixel data of the image. The data is copied.
     *  @param aComponentCount Number of components per pixel
     *  @param aResX width of the image in pixels
     *  @param aResY height of the image in pixels
     *  @return Handle to the newly created Image
     */
    RTImageHandle createImage(
        const float* aData,
        const uint aComponentCount,
        const uint aResX,
        const uint aResY);

    /**
     * Set/get the total number of particle paths to be generated from all light sources
     * in each frame.
     * This corresponds neither to the number of generated particles (as a particle is
     * stored at each surface intersection) nor to the number of used particles (some
     * generated particles might be rejected).
     *
     * For progressive GI a reasonable value is 30-80.
     * @param aCount <0,inf> number of particle light paths generated per frame
     */
    void setLightPathCount(const uint aCount);
    uint getLightPathCount();

    void setMaxParticlePathLength(const uint aLength);
    uint getMaxParticlePathLength();

    void setVPLClampThreshold(const float aThreshold);
    float getVPLClampThreshold();

    void setVPLClampFactor(const float aFactor);
    float getVPLClampFactor();

    void setVPLClampStartValue(const float aValue);
    float getVPLClampStartValue();

    void setImportonCount(const uint aCount);
    uint getImportonCount();

    void setTotalFrameLuminance(const float aLuminance);
    float getTotalFrameLuminance();

    size_t getTotalParticleCount();

    //-----------------------------------------------------------------------------
    // Testing Options
    //-----------------------------------------------------------------------------

    /**
     *  set some Test Variables that might influence the Rendering Result
     *  Mostly for debugging.
     *  Might be removed in future versions.
     */
    void setTestParameter(
        int id,
        float value);

    /**
     *  set some Test Variables that might influence the Rendering Result
     *  Mostly for debugging.
     *  Might be removed in future versions.
     */
    void setTestParameter(
        int id,
        int value);

    /**
     *  set time in miliseconds
     */
    void setTime(
        float milliseconds);

    /**
      * Finds the best manual granularity setting based on the current scene, machine and resolution.
      * This process can be time consuming as it renders several frames with different settings
      * and chooses the one that has rendered fastest.
      *
      * Typical usage is when one given scene is run on the same machine over and over (a demo for example).
      * Determining the best granularity before each startup would too time consuming, but it can still provide some speedup.
      * The way to go is to call this method once and use the value in .ini file together with manual granularity settings.
      */
    int32 findBestGranularity();

private:

    //methods
    void prepareRendering();

    //members
	  std::vector<RTAppearanceHandle>	appearances;
    std::vector<RTLightHandle>		  lightsources;
    bool                    mAnimationPaused;
    uint                    mStillCameraIterations; //If progressive refinement is on, counts the number of accumulated iterations
    bool                    mProgressiveRefinement; //Triggers progressive refinement on and off
    bool                    acc_built;
    //Handle/Body paradigm implementation of the renderer.
    //Why is this a raw pointer: Making it stack-allocated could be dangerous as it holds *a lot* of content
    //and I do not want to cluter stack with it.
    //Making it a boost:scoped_ptr is not possible either, as MSVC complains that it needs to have DLL-interface. But to
    //declare it with DLL-interface, we need explicit specialization of the boost::scoped_ptr, which we cannot do as RendererPimpl
    //is here an incomplete type.
    RendererPimpl         * pimpl;
    RTAppearanceHandle		defaultAppearance;
};

RTFACT_REMOTE_NAMESPACE_END

//disabling MSVC warning 4251
#if defined(RTREMOTE_COMPILER_MSVC)
#   pragma warning(pop)
#endif

#endif // RTFACT_REMOTE__RENDERER_HPP
