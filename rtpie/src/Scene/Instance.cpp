/*
 *  Instance.cpp
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

#include <Scene/Instance.hpp>

#include <Scene/Scene.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/Mesh.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

RTFACT_RTPIE_QI_START(Instance)
RTFACT_RTPIE_QI_ADD(IInstance)
RTFACT_RTPIE_QI_END

// -------------------------------------------
// helper functions
// -------------------------------------------
RTfact::Transform createRTfactTransform(float *aM, float *aInvM)
{
    /*
    float aM2[4][4];
    memcpy(aM2, aM, sizeof(float)*16);
    float *test = reinterpret_cast<float*>(aM2);
    Matrix4f m1( aM2 );
    */

    Matrix4f m1(aM[0], aM[4], aM[8], aM[12],
            aM[1], aM[5], aM[9], aM[13],
            aM[2], aM[6], aM[10], aM[14],
            aM[3], aM[7], aM[11], aM[15]);

    if(aInvM){
        Matrix4f m2(aInvM[0], aInvM[4], aInvM[8], aInvM[12],
                aInvM[1], aInvM[5], aInvM[9], aInvM[13],
                aInvM[2], aInvM[6], aInvM[10], aInvM[14],
                aInvM[3], aInvM[7], aInvM[11], aInvM[15]);
        return RTfact::Transform(m1, m2);
    }
    else
        return RTfact::Transform(m1);
}

void copyTransformFloats(float *matrix, float *invMatrix, const RTfact::Transform &t)
{
    memcpy(matrix, t.matrix.columns, sizeof(float)*16);
    memcpy(invMatrix, t.invMatrix.columns, sizeof(float)*16);
    /*
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            matrix[i*4 + j] = t.matrix.columns[i][j];
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            invMatrix[i*4 + j]  = t.invMatrix.columns[i][j];
    */
}



// -------------------------------------------
// Constructors
// -------------------------------------------

Instance::Instance(Scene *aScene, t_InternInstanceHandle aInstance) :
        SceneObject(aScene), mInstance(aInstance)
{
}

Instance::~Instance()
{
    if(mScene){
        mScene->clearHandle(mInstance);
    }
}

// -------------------------------------------
// IInstance
// -------------------------------------------

HRESULT Instance::GetGeometry(IGeometry** _retval)
{
    if(!mScene)
        return E_FAIL;
    Geometry* object;
    HRESULT rv = mScene->getHandle(mInstance->getGeometry(), &object );
    if(SUCCEEDED(rv)){
        *_retval = object;
    }
    return rv;
}

HRESULT Instance::SetAppearance(IMesh *aMesh,
        IAppearance *aMat)
{
    if(!mScene)
        return E_FAIL;
    mScene->notifyChange();

    size_t appId = static_cast<Appearance*>(aMat)->getID();


    size_t meshId = static_cast<Mesh*>(aMesh)->mMesh->mID;
    if(mAppearances.size() <= meshId)
        mAppearances.resize(meshId + 1);
    mAppearances[meshId] = static_cast<Appearance*>(aMat);

    mInstance->setAppearance(static_cast<Mesh*>(aMesh)->mMesh, appId);
    return S_OK;
}

HRESULT Instance::SetCutPlanes(bool aCap, float aCapR, float aCapG, float aCapB,
    unsigned int aPlaneCount,
    float* aNormX, float* aNormY, float* aNormZ,
    float* aPointX, float* aPointY, float* aPointZ)
{
    if(!mScene)
        return E_FAIL;
    //mScene->notifyChange();
    mInstance->setCutPlanes(aCap, aCapR, aCapG, aCapB, aPlaneCount,
        aNormX, aNormY, aNormZ,
        aPointX, aPointY, aPointZ);
    return S_OK;
}

HRESULT Instance::SetDropShadow(bool aDropShadow)
{
    mInstance->setDropShadow(aDropShadow);
    return S_OK;
}

HRESULT Instance::GetBounds(float* aMin, float* aMax)
{
    BBox bbox = mInstance->getBounds();
    aMin[0] = bbox.min(0).x;
    aMin[1] = bbox.min(0).y;
    aMin[2] = bbox.min(0).z;
    aMax[0] = bbox.max(0).x;
    aMax[1] = bbox.max(0).y;
    aMax[2] = bbox.max(0).z;
    return S_OK;
}

HRESULT Instance::ClearAppearance(IMesh *aMesh)
{
    if(!mScene)
        return E_FAIL;
    mScene->notifyChange();
    mInstance->clearAppearance(static_cast<Mesh*>(aMesh)->mMesh);
    return S_OK;
}

HRESULT Instance::SetTransform(
    float *aMatrix, float *aInversMatrix)
{
    if(!mScene)
        return E_FAIL;
    mScene->notifyChange();
    mInstance->setTransform(createRTfactTransform(aMatrix, aInversMatrix));
    return S_OK;
}

HRESULT Instance::GetTransform(float *aMatrix,
    float *aInversMatrix)
{
    if(!mScene)
        return E_FAIL;
    copyTransformFloats( aMatrix, aInversMatrix, mInstance->getTransform() );
    return S_OK;
}


RTFACT_RTPIE_NAMESPACE_END

