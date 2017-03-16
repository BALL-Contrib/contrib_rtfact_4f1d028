/*
 *  TwoLevelTracer.cpp
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

 #define RTFACT_RTPIE_LIB

#include <TwoLevelTracer/TwoLevelTracer.hpp>
#include <TwoLevelTracer/TLRenderTask.hpp>
#include <TwoLevelTracer/TLPickTask.hpp>
#include <TwoLevelTracer/TLRayGenerationTask.hpp>
#include <TwoLevelTracer/TLRayPickTask.hpp>
#include <TwoLevelTracer/TLIntersectTask.hpp>

#include <RTfact/Utils/FPSMeter.hpp>
#include <boost/pointer_cast.hpp>
#include <boost/cast.hpp>

//-----------------------------------------------------------------------------
// RTfact Includes
//-----------------------------------------------------------------------------
#include <RTfact/Utils/TwoLevelPicking.hpp>

//-----------------------------------------------------------------------------
// RTpie Display Includes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Implementation
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

RTFACT_RTPIE_NAMESPACE_BEGIN


RTFACT_RTPIE_QI_START(TwoLevelTracer)
RTFACT_RTPIE_QI_ADD(IRayTracer)
RTFACT_RTPIE_QI_END



//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

TwoLevelTracer::TwoLevelTracer()
{
    mVersion = 0;
}

TwoLevelTracer::~TwoLevelTracer()
{

}

//-----------------------------------------------------------------------------
// IRacTracer Implementation
//-----------------------------------------------------------------------------

HRESULT TwoLevelTracer::AttachScene(IScene *aScene)
{
    mScene = static_cast<Scene*>(aScene);

    mVersion = mScene->getVersion() - 1;
    return S_OK;
}

bool TwoLevelTracer::isUpToDate()
{
    return mScene->getVersion() == mVersion;
}

HRESULT TwoLevelTracer::SyncStructures()
{
    if(!mScene)
        return E_FAIL;
    // Update Camera
    if( !isUpToDate())
    {
        // Update Appearances etc.
        mScene->update();

        createAccelStruct();

        // Update lights with bounding sphere
        if(mVersion != mScene->getVersion())
        {
            unsigned int lightCount = 0;
            mScene->GetLightCount(&lightCount);
            BoundingSphere bSphere = tree.getBounds().getBoundingSphere();
            for(int i = 0; i < lightCount; i++)
            {
                ILight* light;
                mScene->GetLight(i, &light);
                if(light)
                {
                    light->SetParam3f("boundingSphereCenter", bSphere.center.x, bSphere.center.y, bSphere.center.z);
                    light->SetParamf("boundingSphereRadius", bSphere.radius);
                }
                light->Release();
            }
        }

        mVersion = mScene->getVersion();
    }

    return S_OK;
}


HRESULT TwoLevelTracer::CreateRenderTask(IRenderTask **_retval)
{
    *_retval = 0;
    IRenderTask *task = new TLRenderTask(this);
    task->AddRef();
    *_retval = task;
    return S_OK;
}

HRESULT TwoLevelTracer::CreatePickTask(IPickTask **_retval)
{
    *_retval = 0;
    IPickTask *task = new TLPickTask(this);
    task->AddRef();
    *_retval = task;
    return S_OK;
}

HRESULT TwoLevelTracer::CreateRayGenerationTask(IRayGenerationTask **_retval)
{
    *_retval = 0;
    IRayGenerationTask *task = new TLRayGenerationTask(this);
    task->AddRef();
    *_retval = task;
    return S_OK;
}

HRESULT TwoLevelTracer::CreateRayPickTask(IRayPickTask **_retval)
{
    *_retval = 0;
    IRayPickTask *task = new TLRayPickTask(this);
    task->AddRef();
    *_retval = task;
    return S_OK;
}

HRESULT TwoLevelTracer::CreateIntersectTask(IIntersectTask **_retval)
{
    *_retval = 0;
    IIntersectTask *task = new TLIntersectTask(this);
    task->AddRef();
    *_retval = task;
    return S_OK;
}

//-----------------------------------------------------------------------------
// Helper functions
//-----------------------------------------------------------------------------

void TwoLevelTracer::createAccelStruct()
{
    synchronizer.build(mScene->scene, builddata, tree);
}

//-----------------------------------------------------------------------------

HRESULT CreateRayTracer(IRayTracer** _retval)
{
    IRayTracer* handle = new TwoLevelTracer();
    handle->AddRef();
    *_retval = handle;
    return S_OK;
}

//-----------------------------------------------------------------------------

RTFACT_RTPIE_NAMESPACE_END
