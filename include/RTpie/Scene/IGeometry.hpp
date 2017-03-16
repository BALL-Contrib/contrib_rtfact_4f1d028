/* 
 *  IGeometry.hpp
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
 
 #ifndef RTFACT_RTPIE__IOBJECT_HPP
#define RTFACT_RTPIE__IOBJECT_HPP

#include <RTpie/Config/Common.hpp>
#include <RTpie/Base/IUnknown.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

struct IGeometry;

RTFACT_RTPIE_NAMESPACE_END

#include <RTpie/Scene/IMesh.hpp>
#include <RTpie/Scene/IInstance.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

#define RTPIE_IOBJECT_ID \
		{ 0xbce87ccf, 0x1522, 0x48a0, { 0x97, 0x9b, 0x71, 0xae, 0xbf, 0x30, 0xc0, 0x41 } }


/**
 *  The IGeometry interface can be used to access and modify Scene Geometries.
 *
 *  The Geometry is a central entity to manage the geometry of the scene.
 *  Each Geometry contains 0 or more meshes (see IMesh) and can be instantiated
 *  0 or more times (see IInstance).
 *
 *  Meshes and Instances are created through an Geometry.
 *  While the Geometry provides functions to access all its meshes and instances it
 *  only keeps WEAK REFERENCES. This means, that any Mesh or Instance created through
 *  an object will be removed as soon as the application releases the last reference
 *  to them.
 *
 *  ------------------------------------------------------------------------------------
 *  Important Node on Performance:
 *  ------------------------------------------------------------------------------------
 *  The renderer configuration of RTpie uses a Two-Level acceleration structure, i.e. an
 *  "outer acceleration structure" containing multiple "inner acceleration structures"
 *  containing primitives.
 *
 *  An inner acceleration structures is created for each Geometry.
 *
 *  In order to increase rendering performance, it is recommended to decrease the number
 *  of objects in the scene.
 *  However, note that this in turn decreases the build performance of the
 *  inner acceleration structure, since they contain more primitives.
 *
 *  A general rule of thump to increase performance is to merge static geometry or locally
 *  static geometry (i.e. a number of meshes that are always transformed in the same manner)
 *  into one object respectively.
 */
struct IGeometry : public IUnknown
{

	RTPIE_DECLARE_STATIC_IID_ACCESSOR(RTPIE_IOBJECT_ID)

	/**
	 *  Create a mesh
	 *  @param _retval IMesh interface pointer of the newly created mesh
     *  @return HRESULT
	 */
	virtual HRESULT CreateMesh(IMesh** _retval) = 0;

    /**
     *  Create an instance of this object
     *  @param _retval IInstance interface pointer of the newly created instance
     *  @return HRESULT
     */
	virtual HRESULT CreateInstance(IInstance** _retval) = 0;


    /**
     *  Get the number of primitives of this object
     *  This is equal to the sum of primitives of all Meshes contained in this object
     *  @param _retval the number of primitives
     *  @return HRESULT
     */
    virtual HRESULT GetPrimitiveCount(unsigned int *_retval) = 0;


    /**
     *  Get the number of meshes of this object
     *  @param _retval the number of meshes
     *  @return HRESULT
     */
    virtual HRESULT GetMeshCount(unsigned int *_retval) = 0;

    /**
     *  Access a mesh by index
     *  @param aIdx the index. Must be above 0 and smaller than the
     *  mesh count
     *  @param _retval IMesh interface pointer of the mesh
     *  @return HRESULT
     */
    virtual HRESULT GetMesh(unsigned int aIdx, IMesh** _retval) = 0;

    /**
     *  Get the number of instances of this object
     *  @param _retval the number of instances
     *  @return HRESULT
     */
    virtual HRESULT GetInstanceCount(unsigned int *_retval) = 0;

    /**
     *  Access an instance by index
     *  @param aIdx the index. Must be above 0 and smaller than the
     *  instance count
     *  @param _retval IInstance interface pointer of the instance
     *  @return HRESULT
     */
    virtual HRESULT GetInstance(unsigned int aIdx, IInstance** _retval) = 0;

};

RTFACT_RTPIE_NAMESPACE_END


#endif // RTFACT_RTPIE__OBJECT_HPP
