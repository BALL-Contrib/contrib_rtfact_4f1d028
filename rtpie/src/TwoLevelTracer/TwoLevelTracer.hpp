/*
 *  TwoLevelTracer.hpp
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

 #ifndef RTFACT_RTPIE__TWOLEVELRACER_HPP
#define RTFACT_RTPIE__TWOLEVELRACER_HPP

#include "Config/InternalCommon.hpp"

#include <RTpie/IRayTracer.hpp>

RTFACT_RTPIE_FORWARD(TwoLevelTracer)

#include "Base/Base.hpp"
#include "Scene/Scene.hpp"


#define RTFACT_RTPIE__DEFAULT_RESX 512
#define RTFACT_RTPIE__DEFAULT_RESY 512

RTFACT_RTPIE_NAMESPACE_BEGIN

class TwoLevelTracer : public Base, public IRayTracer
{
public:

    // IUnknown

    RTFACT_RTPIE_DECLARE_QI

    RTFACT_RTPIE_FORWARD_BASE

    // IRayTracer

    virtual HRESULT AttachScene(IScene *aScene);

    virtual HRESULT SyncStructures();

    virtual HRESULT CreateRenderTask(IRenderTask **_retval);

    virtual HRESULT CreatePickTask(IPickTask **_retval);

    virtual HRESULT CreateRayGenerationTask(IRayGenerationTask **_retval);

    virtual HRESULT CreateRayPickTask(IRayPickTask **_retval);

    virtual HRESULT CreateIntersectTask(IIntersectTask **_retval);

    // Internal

    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    //Constructor.
    TwoLevelTracer();


    virtual ~TwoLevelTracer();



private:

    void createAccelStruct();

public:

    //---------------------------------------------------------------------
    // Task Interface
    //---------------------------------------------------------------------

    Scene* getScene(){ return mScene.get(); }
    bool isUpToDate();

private:

    //---------------------------------------------------------------------
    // API Data
    //---------------------------------------------------------------------

    ScenePtr        mScene;
    // Version of the scene on last synchronization
    unsigned int    mVersion;

    //---------------------------------------------------------------------
    // RTfact Data
    //---------------------------------------------------------------------

public:
    // Acceleration Structure
    t_Tree tree;

    // Building & Rendering

    t_AccelData builddata;
    t_Synchronizer synchronizer;

};

RTFACT_RTPIE_NAMESPACE_END

#endif // RTFACT_RTPIE__RENDERER_HPP
