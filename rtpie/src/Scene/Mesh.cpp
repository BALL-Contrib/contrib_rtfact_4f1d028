/* 
 *  Mesh.cpp
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
 *  Created on: 2010-09-26 19:11:26 +0200
 *  Author(s): Felix Klein
 */
 
 #define RTFACT_RTPIE_LIB

#include <Scene/Mesh.hpp>
#include <Scene/Instance.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Geometry.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

RTFACT_RTPIE_QI_START(Mesh)
RTFACT_RTPIE_QI_ADD(IMesh)
RTFACT_RTPIE_QI_END

// -------------------------------------------
// helper functions
// -------------------------------------------
RTfact::Vec3f1 createRTfactVec3f(const float3 &v){
    return RTfact::Vec3f1(v[0], v[1], v[2]);
}


/*
RTfact::Triangle createRTfactTriangle(const Triangle &t)
{
    RTfact::Triangle t2;
    for(int i=0; i<3; i++)
    {
        t2.vertices[i] = createRTfactVec3f(t.vertices[i]);
        t2.vertexColors[i] = createRTfactVec3f(t.vertexColors[i]);
        t2.texCoords[i] = t.texCoords[i];
    }
    if(t.normal[0] != 0 || t.normal[1] != 0  || t.normal[2] != 0)
        t2.normal = createRTfactVec3f(t.normal);
    else
        t2.updateNormal();

    if(t.normals[0][0] != 0 || t.normals[0][1] != 0  || t.normals[0][2] != 0)
        for(int i=0; i<3; i++)
            t2.normals[i] = createRTfactVec3f(t.normals[i]);
    else
        for(int i=0; i<3; i++)
            t2.normals[i] = t2.normal;
    return t2;
}
*/


//-----------------------------------------------------------------------------
// Public Class Implementation
//-----------------------------------------------------------------------------


Mesh::Mesh(Scene *aScene, t_InternMeshHandle aMesh) :
        SceneObject(aScene), mMesh(aMesh), mAppearance(0)
{
}

Mesh::~Mesh()
{
    if(mScene){

        t_InternGeometryHandle object = mMesh->getGeometry();
        for(size_t i=0; i < object->getInstanceCount(); ++i){
            Instance *instance;
            mScene->getHandle<Instance>(object->getInstance(i), &instance);
            instance->clearAppearance(mMesh->mID);
            instance->Release();
        }

        mScene->clearHandle(mMesh);
    }

}

HRESULT Mesh::GetGeometry(IGeometry **_retval)
{
    if(!mScene)
        return E_FAIL;
    Geometry* object;
    HRESULT rv = mScene->getHandle(mMesh->getGeometry(), &object );
    if(SUCCEEDED(rv)){
        *_retval = object;
    }
    return rv;
}
HRESULT Mesh::SetPrimitives(
    const uint32 aTriangleCount,
    const int32* aIndices,
    const float* aVertices,
    const float* aNormals,
    const float* aVertexColors,
    const float* aTexCoords
    )
{
    if(!mScene)
        return E_FAIL;
    mScene->notifyChange();

    t_InternMeshHandle mesh = mMesh;
    mesh->clearPrimitives();
    for(uint i = 0; i < aTriangleCount; ++i)
    {
        RTfact::Triangle triangle;
        uint i3[3];

        i3[0] = aIndices[3 * i];
        i3[1] = aIndices[3 * i + 1];
        i3[2] = aIndices[3 * i + 2];

        for(uint j = 0; j < 3; ++j)
        {
            uint j3[3];

            j3[0] = 3 * i3[j];
            j3[1] = 3 * i3[j] + 1;
            j3[2] = 3 * i3[j] + 2;

            triangle.getVertex(j) = Vec3f1(aVertices[j3[0]], aVertices[j3[1]], aVertices[j3[2]]);
            if(aNormals)
                triangle.getNormal(j) = Vec3f1(aNormals[j3[0]], aNormals[j3[1]], aNormals[j3[2]]);
            if(aVertexColors)
                triangle.getVertexColor(j) = Vec3f1(aVertexColors[3 * i3[j]], aVertexColors[3 * i3[j] + 1], aVertexColors[3 * i3[j] + 2]);
            else
                Vec3f1(0.f, 0.f, 1.f);
            if(aTexCoords)
                triangle.getTexCoords(j) =
                    RTfact::Triangle::TextCoordPair(aTexCoords[2 * i3[j]], aTexCoords[2 * i3[j] + 1] );
        }
        triangle.updateNormal();

        if(!aNormals){
            for(uint j = 0; j < 3; ++j)
                triangle.getNormal(j) = triangle.getNormal();
        }


        mesh->addPrimitive(triangle);
    }
    return S_OK;
}

//virtual void addPrimitive(
//    const RTfact::RTpie::Triangle& aPrim);

HRESULT Mesh::ClearPrimitives()
{
    if(!mScene)
        return E_FAIL;
    mScene->notifyChange();

    mMesh->clearPrimitives();
    return S_OK;
}


HRESULT Mesh::GetPrimitiveCount(unsigned int *_retval)
{
    if(!mScene)
        return E_FAIL;

    *_retval = static_cast<unsigned int>(mMesh->getPrimitiveCount());
    return S_OK;
}

HRESULT Mesh::SetAppearance(
    IAppearance *aApp)
{
    if(!mScene)
        return E_FAIL;
    mScene->notifyChange();

    mAppearance = static_cast<Appearance*>(aApp);
    mMesh->setAppearanceId(mAppearance->getID());
    return S_OK;
}



RTFACT_RTPIE_NAMESPACE_END

