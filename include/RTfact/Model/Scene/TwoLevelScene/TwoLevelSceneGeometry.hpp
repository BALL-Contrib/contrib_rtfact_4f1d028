/*
 *  TwoLevelSceneGeometry.hpp
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
 *  Created on: 2010-09-15 17:54:13 +0200
 *  Author(s): Felix Klein
 */

 #ifndef _RTFACT_TWOLEVELSCENE_OBJECT_HPP_INCLUDED
#define _RTFACT_TWOLEVELSCENE_OBJECT_HPP_INCLUDED

RTFACT_NAMESPACE_BEGIN


template<class tSceneConfig>
class TwoLevelSceneGeometry : public ReferenceCounted
{
public:

    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    //-------------------------------------------------
    // Typedefs
    //-------------------------------------------------

    typedef typename tSceneConfig::Scene        t_Scene;
    typedef typename tSceneConfig::Scene*       t_ScenePtr;

    typedef typename tSceneConfig::Primitive    t_Primitive;
    typedef Vector<t_Primitive>        		    t_PrimitiveVector;

    typedef typename tSceneConfig::Mesh         t_Mesh;
    typedef typename Traits<t_Mesh>::Ptr        t_MeshHandle;
    typedef IntMap<t_Mesh*>                     t_MeshMap;
    typedef typename t_Mesh::t_key_vector       t_KeyVector;
    typedef Vector<t_KeyVector*>                t_MultiVector;

    typedef typename tSceneConfig::Geometry       t_Geometry;
    typedef typename Traits<t_Geometry>::Ptr      t_GeometryHandle;

    typedef typename tSceneConfig::Instance     t_Instance;
    typedef typename Traits<t_Instance>::Ptr    t_InstanceHandle;
    typedef Vector<t_Instance*>                 t_InstanceVector;

    typedef size_t                   		    t_GeometryID;



    //-------------------------------------------------
    // Fields
    //-------------------------------------------------
    t_ScenePtr              mScene;
    t_GeometryID              mID;
    t_GeometryID              mExID;
    t_MeshMap               mMeshes;
    t_InstanceVector        mInstances;

    BBox                    mBounds;
    bool                    mNeedsAccelStructUpdate;


    //-------------------------------------------------
    // Functions
    //-------------------------------------------------

    // Must only be called by scene!
    TwoLevelSceneGeometry(t_ScenePtr aScene) :
            mScene(aScene),
            mBounds(BBox::empty()),
            mNeedsAccelStructUpdate(false),
            mExID(t_GeometryID(-1))
    {
    }

    ~TwoLevelSceneGeometry()
    {
        if(mScene)
            decouple();
    }

    t_InstanceHandle createInstance()
    {
        return mScene->createInstance(this);
    }


    t_MeshHandle createMesh()
    {
        t_Mesh* mesh = new t_Mesh(this);
        mNeedsAccelStructUpdate = true;
        mesh->mID = mMeshes.insert(mesh);

        for(typename t_InstanceVector::iterator it = mInstances.begin();
            it != mInstances.end(); it++)
            (*it)->updateMeshAdded(mesh->mID);
        return mesh;
    }

    size_t getPrimitiveCount(){
        size_t totalCount = 0;
        for(typename t_MeshMap::iterator it = mMeshes.begin();
            it != mMeshes.end(); it++)
            totalCount += (*it)->getPrimitiveCount();

        return totalCount;
    }

    t_MultiVector getKeyMultiVector(){
        t_MultiVector vector;
        for(typename t_MeshMap::iterator it = mMeshes.begin();
            it != mMeshes.end(); it++)
            vector.push_back((*it)->mKeys);
        return vector;
    }

    void removeInstance(const t_Instance *aInstance)
    {
        if(mScene)
        {
            for(typename t_InstanceVector::iterator it = mInstances.begin();
                it != mInstances.end(); )
                if(*it == aInstance)
                    mInstances.erase(it);
                else
                    it++;
        }
    }

    void removeMesh(const t_Mesh *aMesh)
    {
        if(mScene)
        {
            mMeshes.erase(aMesh->mID);

            for(typename t_InstanceVector::iterator it = mInstances.begin();
                it != mInstances.end(); it++)
                (*it)->updateMeshRemoved(aMesh->mID);
        }

    }

    void decouple()
    {
        RTFACT_ASSERT(mScene);
        mScene->removeGeometry(this);
        mScene = 0;
        for(typename t_InstanceVector::iterator it = mInstances.begin();
            it != mInstances.end(); it++)
            (*it)->decouple();
        mInstances.clear();
        for(typename t_MeshMap::iterator it = mMeshes.begin();
            it != mMeshes.end(); it++)
            (*it)->decouple();
        mMeshes.clear();


    }

    // Used Internally by StructSynchronizer
    void updateBounds()
    {
        mBounds = BBox::empty();
        for(typename t_MeshMap::iterator it = mMeshes.begin();
            it != mMeshes.end(); it++)
            mBounds.extend((*it)->mBounds);
        // All Instances of this object have to be updated.
        for(typename t_InstanceVector::iterator it = mInstances.begin();
            it != mInstances.end(); it++)
            (*it)->mNeedsBoundUpdate = true;
    }

    void forwardMaterialUpdate()
    {
        // All Instances of this object have to be updated concerning material
        for(typename t_InstanceVector::iterator it = mInstances.begin();
            it != mInstances.end(); it++)
            (*it)->mNeedsMaterialUpdate = true;
    }

    // only used internally:
    t_MeshHandle getMeshByKey(uint id){
        return mMeshes[id];
    }


    // Access Methods

    size_t getMeshCount(){
        return mMeshes.size();
    }

    t_MeshHandle getMesh(size_t aIdx){
        RTFACT_ASSERT(aIdx < getMeshCount());
        return *(mMeshes.begin() + aIdx);
    }

    size_t getInstanceCount(){
        return mInstances.size();
    }

    t_InstanceHandle getInstance(size_t aIdx){
        RTFACT_ASSERT(aIdx < getInstanceCount());
        return *(mInstances.begin() + aIdx);
    }



};


RTFACT_NAMESPACE_END

#endif
