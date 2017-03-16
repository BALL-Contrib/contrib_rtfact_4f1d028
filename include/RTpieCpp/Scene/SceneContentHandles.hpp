/*
 *  SceneContentHandles.hpp
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
 *  Created on: 2010-09-26 18:55:36 +0200
 *  Author(s): Felix Klein
 */

 #ifndef RTFACT_HANDLE_OBJECT_HPP
#define RTFACT_HANDLE_OBJECT_HPP

#include <RTpieCpp/Config/Common.hpp>

#include <RTpie/Scene/IInstance.hpp>
#include <RTpie/Scene/IMesh.hpp>
#include <RTpie/Scene/IGeometry.hpp>

#define IMPL_INTERFACE RTfact::RTpie::IGeometry
WRAPPER_FWD(GeometryHandle)
#undef IMPL_INTERFACE

#include <RTpieCpp/Base/BaseHandle.hpp>
#include <RTpieCpp/Scene/AppearanceHandle.hpp>

#define IMPL_INTERFACE RTfact::RTpie::IMesh
WRAPPER_START(MeshHandle, BaseHandle)

    GeometryHandle getGeometry();

    void setPrimitives(
        const RTfact::RTpie::uint32 aTriangleCount,
        const RTfact::RTpie::int32* aIndices,
        const float* aVertices,
        const float* aNormals,
        const float* aVertexColors,
        const float* aTexCoords
        )
    IMPLEMENT_WRAPPER_NO_RETURN( SetPrimitives(aTriangleCount, aIndices, aVertices, aNormals, aVertexColors, aTexCoords))

    void clearPrimitives()
    IMPLEMENT_WRAPPER_NO_RETURN( ClearPrimitives() )

    unsigned int getPrimitiveCount()
    IMPLEMENT_WRAPPER_ARG0(unsigned int, GetPrimitiveCount)

    void setAppearance(AppearanceHandle aApp)
    IMPLEMENT_WRAPPER_NO_RETURN( SetAppearance(aApp.get()) )

WRAPPER_END
#undef IMPL_INTERFACE

#define IMPL_INTERFACE RTfact::RTpie::IInstance
WRAPPER_START(InstanceHandle, BaseHandle)

    GeometryHandle getGeometry();

    void setAppearance(const MeshHandle &aMesh,
            const AppearanceHandle &aMat
            )
    IMPLEMENT_WRAPPER_NO_RETURN( SetAppearance(aMesh.get(), aMat.get()) )

    void setCutPlanes(bool aCap, const float3 &aCapColor, unsigned int aPlaneCount,
        float* aNormX, float* aNormY, float* aNormZ,
        float* aPointX, float* aPointY, float* aPointZ)
    IMPLEMENT_WRAPPER_NO_RETURN( SetCutPlanes(aCap, aCapColor.x, aCapColor.y, aCapColor.z, aPlaneCount, aNormX, aNormY, aNormZ, aPointX, aPointY, aPointZ) )

    void getBounds(float* aMin, float* aMax)
    IMPLEMENT_WRAPPER_NO_RETURN( GetBounds(aMin, aMax) )

    void setDropShadow(bool aDropShadow)
    IMPLEMENT_WRAPPER_NO_RETURN( SetDropShadow(aDropShadow) )

    void clearAppearance(const MeshHandle &aMesh)
    IMPLEMENT_WRAPPER_NO_RETURN( ClearAppearance(aMesh.get()) )

    void setTransform(float *aMatrix, float *aInversMatrix)
    IMPLEMENT_WRAPPER_NO_RETURN( SetTransform(aMatrix, aInversMatrix) )

    void getTransform(float *aMatrix, float *aInversMatrix)
    IMPLEMENT_WRAPPER_NO_RETURN( GetTransform(aMatrix, aInversMatrix) )

WRAPPER_END
#undef IMPL_INTERFACE

#define IMPL_INTERFACE RTfact::RTpie::IGeometry
WRAPPER_START(GeometryHandle, BaseHandle)

    MeshHandle createMesh()
    IMPLEMENT_WRAPPER_ARG0(RTfact::RTpie::IMesh*, CreateMesh)

    InstanceHandle createInstance()
    IMPLEMENT_WRAPPER_ARG0(RTfact::RTpie::IInstance*, CreateInstance)

    unsigned int getPrimitiveCount()
    IMPLEMENT_WRAPPER_ARG0(unsigned int, GetPrimitiveCount)

    unsigned int getMeshCount()
    IMPLEMENT_WRAPPER_ARG0(unsigned int, GetMeshCount)

    MeshHandle getMesh(unsigned int aIdx)
    IMPLEMENT_WRAPPER_ARG1(RTfact::RTpie::IMesh*, GetMesh, aIdx)

    unsigned int getInstanceCount()
    IMPLEMENT_WRAPPER_ARG0(unsigned int, GetInstanceCount)

    InstanceHandle getInstance(unsigned int aIdx)
    IMPLEMENT_WRAPPER_ARG1(RTfact::RTpie::IInstance*, GetInstance, aIdx)

WRAPPER_END
#undef IMPL_INTERFACE


RTFACT_HANDLE_NAMESPACE_BEGIN
#define IMPL_INTERFACE RTfact::RTpie::IMesh


inline GeometryHandle MeshHandle::getGeometry()
IMPLEMENT_WRAPPER_ARG0(RTfact::RTpie::IGeometry*, GetGeometry)

#undef IMPL_INTERFACE

#define IMPL_INTERFACE RTfact::RTpie::IInstance

inline GeometryHandle InstanceHandle::getGeometry()
IMPLEMENT_WRAPPER_ARG0(RTfact::RTpie::IGeometry*, GetGeometry)

#undef IMPL_INTERFACE
RTFACT_HANDLE_NAMESPACE_END


#endif // RTFACT_HANDLE_OBJECT_HPP
