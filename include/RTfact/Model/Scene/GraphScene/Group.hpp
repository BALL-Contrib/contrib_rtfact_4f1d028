/* 
 *  Group.hpp
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
 *  Author(s): Felix Klein, Petr Kmoch, Vaclav Krajicek
 */
 
 #ifndef _RTFACT_GRAPHSCENE_GROUP_HPP_INCLUDED
#define _RTFACT_GRAPHSCENE_GROUP_HPP_INCLUDED


#include "Instance.hpp"

RTFACT_NAMESPACE_BEGIN

template<class tSceneConfig>
class Group : public Node< typename tSceneConfig::Group , tSceneConfig>
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
    typedef Vector<t_MeshHandle>                    t_MeshVector;

    typedef typename tSceneConfig::Instance         t_Instance;
    typedef typename Traits<t_Instance>::Ptr        t_InstanceHandle;
    typedef Vector<t_InstanceHandle>                t_InstanceVector;

    typedef typename boost::intrusive_ptr<Group>             t_GroupHandle;
    typedef Vector<t_GroupHandle>                   t_GroupVector;

    //-------------------------------------------------
    // Fields
    //-------------------------------------------------

    t_GroupVector       mGroups;
    t_InstanceVector    mInstances;

    Transform       mXfm;
    BBox            mBounds;

    // builder information ----------------------------

    bool mNeedUpdate;
    bool mNeedBuildUpdate;
    int mDepth;
    size_t mNumOfPrimitives;
    size_t mNumOfInstances;
    size_t mSubtreeSize;

    //-------------------------------------------------
    // Functions
    //-------------------------------------------------

    Group(t_ScenePtr s, const Transform &aXfm) :
                t_Parent(s),
                mXfm(aXfm),
                mBounds(BBox::empty()),
                mNeedUpdate(false),
                mDepth(0),
                mNumOfPrimitives(0),
                mNumOfInstances(0),
                mSubtreeSize(0)  {
            //std::cout<< std::endl <<  "Create Group: " << this;
                }

    ~Group()
        {
            //std::cout<< std::endl <<  "Delete Group: " << this;
            assert(t_Parent::mParents.size() == 0);
            // Remove Links from Group Children to this Group.
            for(uint i=0; i< mGroups.size(); ++i)
            {
                mGroups[i]->removeParent(this);
                mGroups[i]->resetDepth();
            }
            // Remove Links from Instance Children to this Group.
            for(unsigned  int i=0; i< mInstances.size(); ++i){
                mInstances[i]->removeParent(this);
            }
        }

    Transform getAccumTransform()
    {
        if(t_Parent::mParents.size() == 0)
            return mXfm;
        else
            return t_Parent::mParents[0]->getAccumTransform() * mXfm;
    }

    void setTransform(const Transform &aXfm)
    {
        mXfm = aXfm;
        // Update Bounds etc.
        t_Parent::mScene->setUpdateGroup(this);
    }

    // --- Build Functions ---------------------------

    void add(t_GroupHandle aGroup)
    {
        // Check if aGroup is already connected to this Group
        for(typename t_GroupVector::iterator it = mGroups.begin(); it != mGroups.end(); it++)
        {
            RTFACT_USER_ERROR_IF(*it == aGroup, "Group cannot have same group twice as child" );
        }
        // Connection
        mGroups.push_back(aGroup);
        aGroup->addParent(this);

		// Update Depth of added group
		aGroup->increaseDepth(mDepth + 1, this);

        // Update Bounds etc.
        t_Parent::mScene->setUpdateGroup(this);
    }

    void remove(t_GroupHandle aGroup)
    {
        for(typename t_GroupVector::iterator it = mGroups.begin(); it != mGroups.end(); it++)
            if(*it == aGroup){
                aGroup->removeParent(this);
                mGroups.erase(it);
                t_Parent::mScene->setUpdateGroup(this);
                return;
            }
    }

    t_InstanceHandle add(t_MeshHandle aMesh)
    {
        // Check if aMesh is already connected to this Group
        for(typename t_InstanceVector::iterator it = mInstances.begin(); it != mInstances.end(); it++)
            RTFACT_USER_ERROR_IF((*it)->mMesh == aMesh, "Group cannot have same mesh twice as child" );
        // Create Instance.
        t_InstanceHandle instance;
        instance = new t_Instance(t_Parent::mScene, aMesh);

        mInstances.push_back(instance);
        instance->setParent(this);

        // Update Bounds etc.
        t_Parent::mScene->setUpdateGroup(this);
        return instance;
    }

    void remove(t_MeshHandle aMesh)
    {
        for(typename t_InstanceVector::iterator it = mInstances.begin(); it != mInstances.end(); it++)
            if((*it)->mMesh == aMesh){
                (*it)->removeParent(this);
                mInstances.erase(it);
                t_Parent::mScene->setUpdateGroup(this);
                return;
            }
    }


    /**
     *  Adds a copy of the Instance to the Group.
     *  returns Handle to new Instance.
     */
    t_InstanceHandle add(t_InstanceHandle aInstance)
    {
        // Check if aInstance is already connected to this Group
        // (i.e. another Instance with same mesh exists)
        for(typename t_GroupVector::iterator it = mInstances.begin(); it != mInstances.end(); it++)
            RTFACT_USER_ERROR_IF(*it == aInstance, "Group cannot have same mesh twice as child" );

        // Create Copy of Instance.
        t_InstanceHandle instance(new t_Instance(aInstance));
        mGroups.push_back(instance);
        instance->setParent(this);


        // Update Bounds etc.
        t_Parent::mScene->setUpdateGroup(this);
    }

	 // --- Get Functions ---------------------------

	size_t getGroupCount(){
		return mGroups.size();
	}

	t_GroupHandle getChildGroup(uint aIdx){
		RTFACT_USER_ERROR_IF(aIdx >= mGroups.size(), \
			"Group Index out of bounds" );
		return mGroups[aIdx];
	}

	size_t getInstanceCount(){
		return mInstances.size();
	}

	t_InstanceHandle getChildInstance(uint aIdx){
		RTFACT_USER_ERROR_IF(aIdx >= mInstances.size(), \
			"Instance Index out of bounds" );
		return mInstances[aIdx];
	}

  size_t getParentCount(){
		return t_Parent::mParents.size();
	}

  t_GroupHandle getParent(uint aIdx){
		RTFACT_USER_ERROR_IF(aIdx >= t_Parent::mParents.size(), \
			"Parent Index out of bounds" );
		return t_Parent::mParents[aIdx];
	}


private:

    void increaseDepth(int aDepth, const Group *aOrigin)
    {
        // No cycles!
        RTFACT_USER_ERROR_IF(this == aOrigin, "Connection of groups results in a cycle" );

        if(mDepth < aDepth)
        {
            mDepth = aDepth;

            // if the group already needs an update, another request is inserted,
            // since the depth has increased.
            if(mNeedUpdate)
                t_Parent::mScene->setUpdateGroup(this);

            for(uint i=0; i< mGroups.size(); ++i)
                mGroups[i]->increaseDepth( aDepth+1, aOrigin);
        }
    }

    void resetDepth()
    {
        int max_depth=0;
        for(uint i=0; i<t_Parent::mParents.size(); ++i)
            if( t_Parent::mParents[i]->mDepth > max_depth )
                max_depth = t_Parent::mParents[i]->mDepth;

        if(mDepth != max_depth + 1)
        {
            mDepth = max_depth + 1;
            for(uint i=0; i < mGroups.size(); ++i)
                mGroups[i]->resetDepth();
        }
    }


};




RTFACT_NAMESPACE_END

#endif
