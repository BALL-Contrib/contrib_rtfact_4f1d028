/* 
 *  Instance.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2009  Saarland University
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
 *  Created on: 2009-02-24 16:01:56 +0100
 *  Author(s): Felix Klein
 */
 
 #ifndef _RTFACT_GRAPHSCENE_INSTANCE_HPP_INCLUDED
#define _RTFACT_GRAPHSCENE_INSTANCE_HPP_INCLUDED


#include "Mesh.hpp"

RTFACT_NAMESPACE_BEGIN

template<class tSceneConfig>
class Instance : public Node<typename tSceneConfig::Group , tSceneConfig>
{
public:

    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    //-------------------------------------------------
    // Typedefs
    //-------------------------------------------------
    typedef Node< typename tSceneConfig::Group , tSceneConfig> t_Parent;

    typedef typename tSceneConfig::Scene*           t_ScenePtr;

    typedef typename tSceneConfig::Mesh             t_Mesh;
    typedef typename Traits<t_Mesh>::Ptr            t_MeshHandle;

    typedef typename tSceneConfig::Instance         t_Instance;
    typedef typename Traits<t_Instance>::Ptr        t_InstanceHandle;

    typedef typename tSceneConfig::Group            t_Group;
    typedef typename Traits<t_Group>::Ptr           t_GroupHandle;

    //-------------------------------------------------
    // Fields
    //-------------------------------------------------

    t_MeshHandle        mMesh;

    // TODO: Add Material and other stuff.

    //-------------------------------------------------
    // Functions
    //-------------------------------------------------

    Instance(t_ScenePtr aScene, t_MeshHandle aMesh) : t_Parent(aScene), mMesh(aMesh) {}

    ~Instance()
    {
        assert(t_Parent::mParents.size() == 0);

        // std::cout<< "Delete Instance: " << this;
        // Remove raw pointer of connected mesh.
        if(mMesh)
        {
            mMesh->removeParent(this);
        }
    }

    // --- Build Functions ---------------------------

    void setMesh(t_MeshHandle aMesh)
    {
        aMesh->removeParent(this);
        mMesh= aMesh;
    }

    void setParent(t_Group* aGroup)
    {
        t_Parent::mParents.clear();
        t_Parent::mParents.push_back(aGroup);
    }

    t_GroupHandle getParent()
    {
        RTFACT_USER_ERROR_IF( t_Parent::mParents.size() == 0, \
          "Referred Instance has no parent Group. Instance not deleted due to some other references?" );
        return t_Parent::mParents[0];
    }

	t_MeshHandle getMesh(){
		return mMesh;
	}
};


RTFACT_NAMESPACE_END


#endif
