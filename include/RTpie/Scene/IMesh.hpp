/* 
 *  IMesh.hpp
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
 
 #ifndef RTFACT_RTPIE__IMESH_HPP
#define RTFACT_RTPIE__IMESH_HPP

#include <RTpie/Config/Common.hpp>
#include <RTpie/Base/IUnknown.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

struct IMesh;

RTFACT_RTPIE_NAMESPACE_END

#include <RTpie/Scene/IGeometry.hpp>
#include <RTpie/Scene/IAppearance.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

#ifdef GetGeometry
#undef GetGeometry
#endif

#define RTPIE_IMESH_ID \
		{ 0x54878d32, 0xc8f6, 0x4aed, { 0xb8, 0xdd, 0x6b, 0x27, 0x26, 0xcf, 0xc4, 0xb7 } }


/**
 *  The IMesh interface can be used to access and modify Scene meshes.
 *
 *  A Mesh of RTpie represents the smallest unit of geometry. It can be created
 *  through an Geometry (see IGeometry) and assigned a number of primitives via
 *  SetPrimitives().
 *
 *  Note: Once created from an Geometry, the application HAS to keep the IMesh
 *  handle alive (e.g. don't call Release()) as long as the mesh should be displayed.
 *  Once the last reference to an IMesh is released from the application, the mesh
 *  is removed from the scene.
 */
struct IMesh : public IUnknown
{

	RTPIE_DECLARE_STATIC_IID_ACCESSOR(RTPIE_IMESH_ID)

    /**
     *  access the object this mesh was created from
     *  @param _retval IGeometry interface pointer of the object
     *  @return HRESULT
     */
    virtual HRESULT GetGeometry(IGeometry **_retval) = 0;


    /**
     *  Set the primitives of the mesh. Existing primitives are replaced
     *  @param aTriangleCount the number of triangles
     *  @param aIndices and index array describing the vertex connection to triangles.
     *  3 consecutive integers are used as indices to the vertex, normal and color
     *  arrays to define a triangle.
     *  @param aVertices the vertices of the mesh. 3 consecutive floats define a vertex.
     *  @param aNormals the normals of the mesh. 3 consecutive floats define a normal
     *  Set aNormals to 0 if no vertex normals are defined.
     *  @param aVertexColors the vertex colors of the mesh. 3 consecutive floats define a color.
     *  Set aVertexColors to 0 if no vertex colors are defined.
     *  @param aTexCoords the texture coordinates of the mesh. 2 consecutive floats
     *  define a texture coordinate.
     *  Set aTexCoords to 0 if no texture coordinates are defined.
     *  @return HRESULT
   */
    virtual HRESULT SetPrimitives(
        const uint32 aTriangleCount,
        const int32* aIndices,
        const float* aVertices,
        const float* aNormals,
        const float* aVertexColors,
        const float* aTexCoords
        ) = 0;


    /**
     *  remove all Primitives from the mesh
     *  @return HRESULT
     */
    virtual HRESULT ClearPrimitives() = 0;

    /**
     *  get the number of primitives (triangles) of the mesh
     *  @param _retval the number of primitives
     *  @return HRESULT
     */
    virtual HRESULT GetPrimitiveCount(unsigned int *_retval) = 0;


    /**
     *  set the appearance of the mesh
     *  Note: appearance assignment for each mesh can be overwritten via an instance
     *  @param aApp the appearance to be used by the mesh
     *  @return HRESULT
     */
    virtual HRESULT SetAppearance(
        IAppearance *aApp) = 0;


};

RTFACT_RTPIE_NAMESPACE_END



#endif // RTFACT_RTPIE__MESH_HPP
