/* 
 *  Geometry.hpp
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
 
 #ifndef RTFACT_RTPIE__OBJECT_HPP
#define RTFACT_RTPIE__OBJECT_HPP

#include "Config/InternalCommon.hpp"

#include <RTpie/Scene/IGeometry.hpp>

RTFACT_RTPIE_FORWARD(Geometry)

#include "Base/SceneObject.hpp"

RTFACT_RTPIE_NAMESPACE_BEGIN

class Geometry :
    public SceneObject, public IGeometry
{
    friend class Scene;
    friend class Renderer;

public:

    // IUnknown

    RTFACT_RTPIE_DECLARE_QI

    RTFACT_RTPIE_FORWARD_BASE

    // IGeometry

    virtual HRESULT CreateMesh(IMesh** _retval);

    virtual HRESULT CreateInstance(IInstance** _retval);

    virtual HRESULT GetPrimitiveCount(unsigned int *_retval);

    virtual HRESULT GetMeshCount(unsigned int *_retval);

    virtual HRESULT GetMesh(unsigned int aIdx, IMesh** _retval);

    virtual HRESULT GetInstanceCount(unsigned int *_retval);

    virtual HRESULT GetInstance(unsigned int aIdx, IInstance** _retval);

    // Internal

private:

    t_InternGeometryHandle  mGeometry;

public:

    Geometry(Scene *aScene, t_InternGeometryHandle aGeometry);
    virtual ~Geometry();

};

RTFACT_RTPIE_NAMESPACE_END



#endif // RTFACT_RTPIE__OBJECT_HPP
