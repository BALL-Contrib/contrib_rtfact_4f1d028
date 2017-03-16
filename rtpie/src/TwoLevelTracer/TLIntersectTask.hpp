/*
 *  TLIntersectTask.hpp
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
 *  Created on: Sun Oct 14 16:27:00 CEST 2012 @643
 *  Author(s): Vaclav Krajicek
 */

 #ifndef RTFACT_RTPIE__TLINTERSECTTASK_HPP
#define RTFACT_RTPIE__TLINTERSECTTASK_HPP

#include "Config/InternalCommon.hpp"

#include <RTpie/Tasks/IIntersectTask.hpp>

#include "TwoLevelTracer/TLTask.hpp"
#include "Data/Camera.hpp"
#include "Scene/Instance.hpp"
#include "Scene/Mesh.hpp"


//-----------------------------------------------------------------------------
// RTfact Includes
//-----------------------------------------------------------------------------
#include <RTfact/Utils/TwoLevelPicking.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

/**
 */
class TLIntersectTask : public TLTask, public IIntersectTask
{
public:

    // IUnknown

    RTFACT_RTPIE_DECLARE_QI

    RTFACT_RTPIE_FORWARD_BASE

    // IPickTask

    virtual HRESULT IntersectRays(
        const float* aOrigins,
        const float* aDirections,
        const unsigned int aRayCount,
        float* oDistances)
    {

        typedef RTfact::TwoLevelPicking<SceneConfig> tPicking;

        tPicking picking;

        picking.intersectRays<t_Tree>(
          mIntersector,
          mTracer->tree,
          aOrigins,
          aDirections,
          aRayCount,
          oDistances);

        return S_OK;
    }

    // TLTask:

protected:

    virtual void sceneChanged() {}

    // Internal

public:

    TLIntersectTask(TwoLevelTracer *aTracer)
        : TLTask(aTracer) {}

    virtual ~TLIntersectTask()
    {
    }

    // RTfact Data
    t_Intersector mIntersector;

};

RTFACT_RTPIE_NAMESPACE_END

#endif // RTFACT_RTPIE__TLINTERSECTTASK_HPP
