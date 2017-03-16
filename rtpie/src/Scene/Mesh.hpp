/* 
 *  Mesh.hpp
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
 
 #ifndef RTFACT_RTPIE__MESH_HPP
#define RTFACT_RTPIE__MESH_HPP

#include "Config/InternalCommon.hpp"

#include <RTpie/Scene/IMesh.hpp>

RTFACT_RTPIE_FORWARD(Mesh)

#include "Base/SceneObject.hpp"
#include "Scene/Appearance.hpp"

RTFACT_RTPIE_NAMESPACE_BEGIN


class Mesh : public SceneObject, public IMesh
{

    friend class Scene;
    friend class Renderer;
    friend class Instance;


public:

    // IUnknown

    RTFACT_RTPIE_DECLARE_QI

    RTFACT_RTPIE_FORWARD_BASE

    // IMesh

    virtual HRESULT GetGeometry(IGeometry **_retval);

    virtual HRESULT SetPrimitives(
        const uint32 aTriangleCount,
        const int32* aIndices,
        const float* aVertices,
        const float* aNormals,
        const float* aVertexColors,
        const float* aTexCoords
        );

    //virtual void addPrimitive(
    //    const RTfact::RTpie::Triangle& aPrim);

    virtual HRESULT ClearPrimitives();

    virtual HRESULT GetPrimitiveCount(unsigned int *_retval);

    virtual HRESULT SetAppearance(
        IAppearance *aApp);

    // Internal

private:

    t_InternMeshHandle  mMesh;
    AppearancePtr       mAppearance;

public:

    Mesh(Scene *aScene, t_InternMeshHandle aMesh);
    virtual ~Mesh();

};

RTFACT_RTPIE_NAMESPACE_END



#endif // RTFACT_RTPIE__MESH_HPP
