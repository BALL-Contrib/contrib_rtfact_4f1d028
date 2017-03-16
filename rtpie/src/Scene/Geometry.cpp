/* 
 *  Geometry.cpp
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

#include <Scene/Geometry.hpp>
#include <Scene/Mesh.hpp>
#include <Scene/Instance.hpp>
#include <Scene/Scene.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

RTFACT_RTPIE_QI_START(Geometry)
RTFACT_RTPIE_QI_ADD(IGeometry)
RTFACT_RTPIE_QI_END

//-----------------------------------------------------------------------------
// Public Class Implementation
//-----------------------------------------------------------------------------


Geometry::Geometry(Scene *aScene, t_InternGeometryHandle aGeometry) :
        SceneObject(aScene), mGeometry(aGeometry)
{
}

Geometry::~Geometry()
{
    if(mScene){
        mScene->clearHandle(mGeometry);
    }
}


HRESULT Geometry::CreateMesh(IMesh** _retval){
    if(!mScene)
        return E_FAIL;
    mScene->notifyChange();

    Mesh *mesh;
    HRESULT rv = mScene->getHandle(mGeometry->createMesh(),&mesh);
    if(SUCCEEDED(rv))
        *_retval = mesh;
    return S_OK;
}

HRESULT Geometry::CreateInstance(IInstance** _retval){
    if(!mScene)
        return E_FAIL;
    mScene->notifyChange();

    Instance *instance;
    HRESULT rv = mScene->getHandle(mGeometry->createInstance(),&instance);
    if(SUCCEEDED(rv))
        *_retval = instance;
    return S_OK;
}

HRESULT Geometry::GetPrimitiveCount(unsigned int *_retval){
    if(!mScene)
        return E_FAIL;

    *_retval = static_cast<unsigned int>(mGeometry->getPrimitiveCount());
    return S_OK;
}

HRESULT Geometry::GetMeshCount(unsigned int *_retval){
    if(!mScene)
        return E_FAIL;

    *_retval = static_cast<unsigned int>(mGeometry->getMeshCount());
    return S_OK;
}

HRESULT Geometry::GetMesh(unsigned int aIdx, IMesh** _retval){
    if(!mScene)
        return E_FAIL;

    *_retval = 0;
    if(aIdx >= mGeometry->getMeshCount())
        return E_INVALIDARG;

    Mesh *mesh;
    HRESULT rv = mScene->getHandle(mGeometry->getMesh(aIdx),&mesh);
    if(SUCCEEDED(rv))
        *_retval = mesh;
    return S_OK;
}

HRESULT Geometry::GetInstanceCount(unsigned int *_retval){
    if(!mScene)
        return E_FAIL;

    *_retval = static_cast<unsigned int>(mGeometry->getInstanceCount());
    return S_OK;
}

HRESULT Geometry::GetInstance(unsigned int aIdx, IInstance** _retval){
    if(!mScene)
        return E_FAIL;

    *_retval = 0;
    if( aIdx >= mGeometry->getInstanceCount())
        return E_INVALIDARG;

    Instance *instance;
    HRESULT rv = mScene->getHandle(mGeometry->getInstance(aIdx),&instance);
    if(SUCCEEDED(rv))
        *_retval = instance;
    return S_OK;
}

RTFACT_RTPIE_NAMESPACE_END


