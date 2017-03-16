/*
 *  Instance.hpp
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

 #ifndef RTFACT_RTPIE__INSTANCE_HPP
#define RTFACT_RTPIE__INSTANCE_HPP

#include "Config/InternalCommon.hpp"

#include <RTpie/Scene/IInstance.hpp>

RTFACT_RTPIE_FORWARD(Instance)

#include "Base/SceneObject.hpp"
#include "Scene/Appearance.hpp"

RTFACT_RTPIE_NAMESPACE_BEGIN


class Instance : public SceneObject, public IInstance
{
    friend class Scene;
    friend class Renderer;

public:

    // IUnknown

    RTFACT_RTPIE_DECLARE_QI

    RTFACT_RTPIE_FORWARD_BASE

    // IInstance

    virtual HRESULT GetGeometry(IGeometry** _retval);

    virtual HRESULT SetAppearance(IMesh *aMesh,
            IAppearance *aMat
            );

    virtual HRESULT SetCutPlanes(bool aCap, float aCapR, float aCapG, float aCapB, 
        unsigned int aPlaneCount,
        float* aNormX, float* aNormY, float* aNormZ,
        float* aPointX, float* aPointY, float* aPointZ);

    virtual HRESULT SetDropShadow(bool aDropShadow);

    virtual HRESULT GetBounds(float* aMin, float* aMax);

    virtual HRESULT ClearAppearance(IMesh *aMesh);

    virtual HRESULT SetTransform(
        float *aMatrix, float *aInversMatrix);

    virtual HRESULT GetTransform(float *aMatrix,
        float *aInversMatrix);

    // Internal

private:

    t_InternInstanceHandle              mInstance;
    std::vector<AppearancePtr>          mAppearances;

public:



    Instance(Scene *aScene, t_InternInstanceHandle aInstance);
    virtual ~Instance();

    void clearAppearance(size_t id){
        if(mAppearances.size() > id)
            mAppearances[id] = 0;
    }

};

RTFACT_RTPIE_NAMESPACE_END


#endif // RTFACT_RTPIE__INSTANCE_HPP
