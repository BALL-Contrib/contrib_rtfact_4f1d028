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
 *  Created on: 2009-02-24 16:01:56 +0100
 *  Author(s): Felix Klein, Petr Kmoch
 */
 
 #ifndef _RTFACT_GRAPHSCENE_MESH_HPP_INCLUDED
#define _RTFACT_GRAPHSCENE_MESH_HPP_INCLUDED


#include "Node.hpp"

RTFACT_NAMESPACE_BEGIN

template<class tSceneConfig>
class Mesh : public Node< typename tSceneConfig::Instance , tSceneConfig>
{
public:

    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    //-------------------------------------------------
    // Typedefs
    //-------------------------------------------------
    typedef Node< typename tSceneConfig::Instance , tSceneConfig> t_Parent;

    typedef typename t_Parent::t_ScenePtr       t_ScenePtr;

    typedef typename tSceneConfig::Mesh         t_Mesh;
    typedef typename Traits<t_Mesh>::Ptr        t_MeshHandle;

    typedef typename tSceneConfig::Instance     t_Instance;
    typedef typename Traits<t_Instance>::Ptr    t_InstanceHandle;

    typedef typename tSceneConfig::Group        t_Group;
    typedef typename Traits<t_Group>::Ptr       t_GroupHandle;


    typedef Vector<t_InstanceHandle>   		    t_InstanceVector;

    typedef typename tSceneConfig::Primitive    t_Primitive;
    typedef Vector<t_Primitive>        		    t_PrimitiveVector;
    
    typedef size_t                   		    t_MeshID;
    

    //-------------------------------------------------
    // Fields
    //-------------------------------------------------

#ifndef RTFACT_USE_PRIMITIVE_INTMAP
    t_PrimitiveVector   mPrimitives;
#else
    typedef typename IntMap<t_Primitive>::key_vector		t_key_vector;
    typedef typename t_key_vector::iterator					t_key_iterator;
    typedef typename IntMap<t_Primitive>::key_type			key_type;

    t_key_vector*											mKeys;
#endif


    t_MeshID            mID;

    BBox                mBounds;
    bool                mNeedBuildUpdate;
    uint                mAppearanceID;

    //-------------------------------------------------
    // Functions
    //-------------------------------------------------

#ifndef RTFACT_USE_PRIMITIVE_INTMAP
    Mesh(t_ScenePtr aScene, const t_PrimitiveVector &aPrimitives) 
		: t_Parent(aScene), mPrimitives(aPrimitives), mBounds(BBox::empty()), mNeedBuildUpdate(true), mAppearanceID(0)
    {
        t_Parent::mScene->addMesh(this);
    }
	Mesh(t_ScenePtr aScene ) : t_Parent(aScene), mBounds(BBox::empty()), mNeedBuildUpdate(true), mAppearanceID(0)
    {
        t_Parent::mScene->addMesh(this);
    }
#else
    Mesh(t_ScenePtr aScene, const t_PrimitiveVector &aPrimitives)
		: t_Parent(aScene), mBounds(BBox::empty()), mNeedBuildUpdate(true)
    {
        //std::cout<< std::endl <<  "Create Mesh: " << this;
        mKeys = t_Parent::mScene->mPrimitivePool.insert(aPrimitives.begin(), aPrimitives.end());
        t_Parent::mScene->addMesh(this);
    }
	Mesh(t_ScenePtr aScene ) : t_Parent(aScene), mBounds(BBox::empty()), mNeedBuildUpdate(true)
    {
		mKeys = new t_key_vector();
        t_Parent::mScene->addMesh(this);
    }

#endif



    ~Mesh()
    {
        // we shouldn't have any parents or else we got a problem
        assert(t_Parent::mParents.size() == 0);

        //std::cout << std::endl << "Delete Mesh: " << this;
        if(t_Parent::mScene)
        {
#ifdef RTFACT_USE_PRIMITIVE_INTMAP
          if(mKeys->size() > 0)
            t_Parent::mScene->mPrimitivePool.erase(mKeys->begin(), mKeys->end());

#endif
            t_Parent::mScene->removeMesh(this);
        }
#ifdef RTFACT_USE_PRIMITIVE_INTMAP
        delete mKeys;
#endif
    }

    // --- Build Functions ---------------------------
    void addPrimitive(const t_Primitive &aPrimitive)
    {
        RTFACT_ASSERT(t_Parent::mScene != 0);

#ifndef RTFACT_USE_PRIMITIVE_INTMAP
        mPrimitives.push_back(aPrimitive);
		mPrimitives[mPrimitives.size()-1].updateNormal();
        mBounds.extend(mPrimitives[mPrimitives.size() -1 ].getBounds());   
#else
        mKeys->push_back(t_Parent::mScene->mPrimitivePool.insert(aPrimitive));
        t_Primitive &p = t_Parent::mScene->mPrimitivePool[mKeys->back()];
		p.updateNormal();
        mBounds.extend(p.getBounds());
#endif
    }

	void setAppearanceId(uint aId)
  {
    mAppearanceID = aId;
    RTFACT_ASSERT(t_Parent::mScene != 0);

#ifndef RTFACT_USE_PRIMITIVE_INTMAP
		for(size_t i=0; i < mPrimitives.size(); i++)
			mPrimitives[i].appearanceID = aId;
#else
		for(t_key_iterator it= mKeys->begin(); it != mKeys->end(); it++)
			t_Parent::mScene->mPrimitivePool[*it].appearanceID = aId;
#endif
    }

	// --- get Functions ---------------------------
	
	uint getAppearanceId()
  {
    RTFACT_ASSERT(t_Parent::mScene != 0);

#ifndef RTFACT_USE_PRIMITIVE_INTMAP
		return mPrimitives[0].appearanceID;
#else
		return t_Parent::mScene->mPrimitivePool[mKeys->front()].appearanceID;
#endif
    }

  size_t getParentCount()
  {
    return t_Parent::mParents.size();
	}

  t_GroupHandle getParent(uint aIdx){
    RTFACT_USER_ERROR_IF(aIdx >= t_Parent::mParents.size(), \
      "Parent Index out of bounds" );
    return t_Parent::mParents[aIdx]->getParent();
	}


	// --- builder Functions

	size_t getPrimitiveCount() const
	{
    RTFACT_ASSERT(t_Parent::mScene != 0);
#ifndef RTFACT_USE_PRIMITIVE_INTMAP
		return mPrimitives.size();
#else
		return mKeys->size();
#endif
	}

	t_Primitive& getPrimitive(const int i) const
	{
    RTFACT_ASSERT(t_Parent::mScene != 0);
#ifndef RTFACT_USE_PRIMITIVE_INTMAP
		return mPrimitives[i];
#else
		return t_Parent::mScene->mPrimitivePool[(*mKeys)[i]];
#endif

	}

};


RTFACT_NAMESPACE_END

#endif
