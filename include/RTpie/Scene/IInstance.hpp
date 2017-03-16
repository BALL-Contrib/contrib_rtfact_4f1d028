/*
 *  IInstance.hpp
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

 #ifndef RTFACT_RTPIE__IINSTANCE_HPP
#define RTFACT_RTPIE__IINSTANCE_HPP

#include <RTpie/Config/Common.hpp>
#include <RTpie/Base/IUnknown.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

struct IInstance;

RTFACT_RTPIE_NAMESPACE_END

#include <RTpie/Scene/IGeometry.hpp>
#include <RTpie/Scene/IAppearance.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

#ifdef GetGeometry
#undef GetGeometry
#endif

#define RTPIE_IINSTANCE_ID \
		{ 0x9559d4cc, 0x9ffe, 0x4312, { 0xa6, 0xff, 0xa3, 0x40, 0xd5, 0xa2, 0x96, 0x50 } }


/**
 *  The IInstance interface can be used to access and modify Scene instances.
 *
 *  An instance can be created from an Geometry (see IGeometry), assigned with a
 *  transformation and appearances that override the appearance assigment of
 *  the object's meshes
 *
 *  Note: Once created from an Geometry, the application HAS to keep the IInstance
 *  handle alive (e.g. don't call Release()) as long as the instance should be displayed.
 *  Once the last reference to an IInstance is released from the application, the instance
 *  is removed from the scene.
 */
struct IInstance : public IUnknown
{

	RTPIE_DECLARE_STATIC_IID_ACCESSOR(RTPIE_IINSTANCE_ID)

    /**
     *  access the object this instance was created from
     *  @param _retval IGeometry interface pointer of the object
     *  @return HRESULT
     */
	virtual HRESULT GetGeometry(IGeometry** _retval) = 0;


    /**
     *  set appearance assigned to a mesh of this instance
     *  Assigned appearance is only valid for the mesh displayed through this instance
     *  Appearances assigned to the mesh are overridden
     *  @param aMesh the mesh the appearance should be assigned to.
     *  Mesh and Instance must belong to the same object.
     *  @param aMat the appearance to be used by the mesh
     *  @return HRESULT
     */
	virtual HRESULT SetAppearance(IMesh *aMesh,
	        IAppearance *aMat
	        ) = 0;

    /**
     *  Clear the Appearance assigned for a mesh.
     *  After calling this, the appearance assigned to the mesh directly is used.
     *  @param aMesh the mesh the appearance assignment should be cleared from.
     *  Mesh and Instance must belong to the same object.
     *  @return HRESULT
     */
	virtual HRESULT ClearAppearance(IMesh *aMesh) = 0;

    /**
     *  Set cut-plane for this instance.
     *  @param aCap whether cut is capped or not.
     *  @param aCapR color of a cap if applied (R channel).
     *  @param aCapG color of a cap if applied (G channel).
     *  @param aCapB color of a cap if applied (B channel).
     *  @param aPlaneCount Number of the plane.
     *  @param aNormX X coordinates of cut-plane norms.
     *  @param aNormY Y coordinates of cut-plane norms.
     *  @param aNormZ Z coordinates of cut-plane norms.
     *  @param aPointX X coordinates of cut-plane origins.
     *  @param aPointY Y coordinates of cut-plane origins.
     *  @param aPointZ Z coordinates of cut-plane origins.
     *  @return HRESULT
     */
    virtual HRESULT SetCutPlanes(bool aCap, float aCapR, float aCapG, float aCapB, 
        unsigned int aPlaneCount,
        float* aNormX, float* aNormY, float* aNormZ,
        float* aPointX, float* aPointY, float* aPointZ) = 0;

    virtual HRESULT SetDropShadow(bool aDropShadow) = 0;

    /**
     *  Get instance bounding box.
     *  @param aMin 3 float array with x,y,z coordinates minima.
     *  @param aMax 3 float array with x,y,z coordinates maxima.
     *  @return HRESULT
     */
    virtual HRESULT GetBounds(float* aMin, float* aMax) = 0;

    /**
     *  Set the transformation of the instance.
     *  The geometry of the object is transformed with the given matrix before being displayed.
     *  See RTpie/DataTypes/TransformTools.hpp for some useful methods to generate matrix data.
     *  @param aMatrix a pointer to a float array with 16 components, descriping the 4x4 matrix
     *  of the transformation. Values are copied.
     *  Array index to matrix component mapping is as follows:
     *  0  4  8 12
     *  1  5  9 13
     *  2  6 10 14
     *  3  7 11 15
     *  @param aInversMatrix a pointer to a float array with 16 components, descriping the 4x4 matrix
     *  of the inverse transformation. Values are copied. hen aMatrix is 0, the invers matrix
     *  is computed automatically (which results in some overhead).
     *  @return HRESULT
     */
	virtual HRESULT SetTransform(
        float *aMatrix, float *aInversMatrix) = 0;

    /**
     *  Get the transformation of the instance.
     *  @param aMatrix address to allocated memory where the 16 float values of the matrix
     *  are copied to.
     *  @param aInversMatrix address to allocated memory where the 16 float values of the inverse
     *  matrix are copied to.
     *  @return HRESULT
     */
	virtual HRESULT GetTransform(float *aMatrix,
	    float *aInversMatrix) = 0;

};

RTFACT_RTPIE_NAMESPACE_END


#endif // RTFACT_RTPIE__INSTANCE_HPP
