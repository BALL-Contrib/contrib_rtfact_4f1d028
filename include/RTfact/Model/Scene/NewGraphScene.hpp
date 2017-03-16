/* 
 *  NewGraphScene.hpp
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
 
 #ifndef _RTFACT_GRAPHSCENE_HPP_INCLUDED
#define _RTFACT_GRAPHSCENE_HPP_INCLUDED

#include <queue>
#include <vector>
#include <iostream>
#include <RTfact/Utils/Transform.hpp>
#include <RTfact/Utils/Containers/IntMap.hpp>

#include <RTfact/Model/Scene/GraphScene/Group.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tSceneConfig,
    class tLightConfig,
    class tEnvironmentTexture,
    umask taProperties = 0>
class NewGraphScene :
    public ShadingInfoScene<tSceneConfig, tLightConfig, tEnvironmentTexture, taProperties>
{
    typedef ShadingInfoScene<tSceneConfig, tLightConfig, tEnvironmentTexture, taProperties> t_Base;

public:

    typedef typename tSceneConfig::Primitive        t_Primitive;
    typedef typename tSceneConfig::Mesh             t_Mesh;
    typedef typename tSceneConfig::Instance         t_Instance;
    typedef typename tSceneConfig::Group            t_Group;

    typedef typename Traits<t_Mesh>::Ptr            t_MeshHandle;
    typedef typename Traits<t_Instance>::Ptr        t_InstanceHandle;
    typedef typename Traits<t_Group>::Ptr           t_GroupHandle;

    typedef Vector<t_Primitive>		                t_PrimitiveVector;
    
    typedef IntMap<t_Mesh*>                         t_MeshMap;
    typedef typename t_MeshMap::key_type            t_MeshKey;
    
    typedef Vector<size_t>                          t_MeshIDVector;
    



    struct compareFunction
    {    
        bool operator()(std::pair<t_GroupHandle, int> x, std::pair<t_GroupHandle, int> y) {
            return x.second < y.second;
        }
    };



    // The root geometry:
	t_GroupHandle           mRoot;

    t_MeshMap               mMeshes;
	t_Primitive				mBasePrimitive;
    //t_MeshIDVector          mDeletedMeshes;

    Vector<t_Primitive>     mFlattenPrimitives;


#ifdef RTFACT_USE_PRIMITIVE_INTMAP
    typedef IntMap<t_Primitive>						t_PrimitiveMap;
    t_PrimitiveMap									mPrimitivePool;
#endif



    // FIX ME: priority queue doesn't work with RTfact::Vector on GCC
    // FIX ME: priority queue is buggy with RTfact::FastVector with MSVC (Optimized Code)
    // ReferenceCount is not increased when copying Element into priority queue!
    //std::priority_queue< std::pair<t_GroupHandle,int> , std::vector< std::pair<t_GroupHandle,int> >, compareFunction> update_queue;
    std::priority_queue< std::pair<t_GroupHandle,int> , Vector< std::pair<t_GroupHandle,int> >, compareFunction> update_queue;

    // Some Testfunction that prints the structure of the SceneHandler.
   
    void vecString(const Vec3f<1> &v)
    {
        std::cout<<"<"<<v.xf<<", "<<v.yf<<", "<<v.zf<<">";
    }

    void vecBounds(const BBox &aBounds)
    {
        if(aBounds.min.xf > aBounds.max.xf)
            std::cout<< "[ empty ]";
        else
            std::cout<<"[ " << aBounds.min.xf << " - " << aBounds.max.xf << ", " << aBounds.min.yf << " - " <<
                aBounds.max.yf << ", " << aBounds.min.zf << " - " << aBounds.max.zf << " ]";
    }

    void print_content()
    {
        std::cout<<"\nContent:----------------------------------------------------------";
        //std::cout<<"\n\Meshes - amount: "<<mMeshes.size();
        for(typename t_MeshMap::key_iterator it = mMeshes.keysBegin(); it != mMeshes.keysEnd(); it++)
        {
            t_Mesh *mesh = mMeshes[*it];
            if(!mesh)
                std::cout<<"\n- REMOVED Mesh ";
            else
            {
                std::cout<<"\n- Mesh#"<<mesh->mID<<" Bounds: ";
                vecBounds(mesh->mBounds);

                std::cout<<" Prims: "<<mesh->mPrimitives.size();
                
                for(typename Vector<t_Primitive>::iterator it2=mesh->mPrimitives.begin(); 
                    it2!=mesh->mPrimitives.end(); ++it2)
                {
                    std::cout<<"\n   |-(";vecString((*it2).vertices[0]);std::cout<<", ";
                    vecString((*it2).vertices[1]);std::cout<<", ";vecString((*it2).vertices[2]);
                    std::cout<<")";
                }
                
            }
        }
        std::string pre("");
        std::cout<<"\n\nStructure: ";
        print_structure(pre, mRoot);

        std::cout<<"\n\nGroups to be updated: ";
        Vector< std::pair<t_GroupHandle,int> > queue_vec;
        while(!update_queue.empty())
        {
            std::cout<<"\n- Group#"<< &*update_queue.top().first 
				<< " Depth: " << update_queue.top().second;
            queue_vec.push_back(update_queue.top());
            update_queue.pop();
        }
        while(!queue_vec.empty())
        {
            update_queue.push(queue_vec.back()); 
            queue_vec.pop_back(); 
        }
        /*
        std::cout<<"\n\nErrors: ";
        check_group_linking();
        check_geo_linking();
        */
        std::cout<<"\n----------------------------------------------------------Ende";

    }
    void print_structure(std::string pre, t_GroupHandle group)
    {    
        typedef typename Vector<t_GroupHandle>::iterator t_GroupIt;
        typedef typename Vector<t_InstanceHandle>::iterator t_InstanceIt;
        
        std::cout<<"\n" << pre << "|-(G "<<&(*group)<<") - " << group->mDepth << " - ";
        vecBounds(group->mBounds);

        for(t_InstanceIt it = group->mInstances.begin(); it != group->mInstances.end(); it++)
        {
            t_MeshHandle mesh = (*it)->mMesh;
            std::cout << "\n" << pre << "  |-(Mesh " << (mesh->mID) <<")";
        }
        
        for(t_GroupIt it = group->mGroups.begin(); it != group->mGroups.end(); it++){
            
            print_structure(pre+"  ",*it);
        }
    }

    void check_group_linking(t_GroupHandle node) const{
        // Checks Linking of the Tree starting from the root
        /*
        if(!node)
            node=root;
        if(!node)
            return;

        for(GeoHandleVector::iterator it=node->geoChildren.begin(); it !=node->geoChildren.end(); ++it)
        {
            GeoHandle lg2 = *it;
            GroupHandleVector::iterator it2= lg2->parents.begin();
            while(it2 != lg2->parents.end() && (*it2)!=node) ++it2;
            if(it2 == lg2->parents.end())
                std::cout<<"\n!! Group#"<<node<<" -> Geo#"<<(*it)<<" MISSING PARENT"; 
        }
        for(GroupHandleVector::iterator it=node->groupChildren.begin(); it !=node->groupChildren.end(); ++it)
        {
            GroupHandle lg2= *it;
            GroupHandleVector::iterator it2=lg2->parents.begin();
            while(it2 != lg2->parents.end() && (*it2)!=node) ++it2;
            if(it2 == lg2->parents.end())
                std::cout<<"\n!! Group#"<<node<<" -> Group#"<<(*it)<<" MISSING PARENT"; 
            check_group_linking(lg2);
        }
        for(GroupHandleVector::iterator it=node->parents.begin(); it !=node->parents.end(); ++it)
        {
            GroupHandle lg2= *it;
            GroupHandle::iterator it2= lg2->groupChildren.begin();
            while(it2 != lg2->groupChildren.end() && (*it2)!=node) ++it2;
            if(it2 == lg2->groupChildren.end())
                std::cout<<"\n!! Group#"<<node<<" -> Group#"<<(*it)<<" MISSING CHILD"; 
        }
        */
	}


    NewGraphScene() : mRoot(NULL){
    }

    ~NewGraphScene() {
        // Remove Connection from Meshes
        clearMeshData();
    }

    // ----------------------------------------------------------------
    // Used by external application:
    // ----------------------------------------------------------------

    t_GroupHandle getRoot() const{
		return mRoot;            
    }

    void setRoot(t_GroupHandle aGroup){
		mRoot = aGroup;            
    }

    void clearMeshData()
    {
        for(typename t_MeshMap::key_iterator it = mMeshes.keysBegin(); it != mMeshes.keysEnd(); it++)
        {
            if(mMeshes[*it])
                mMeshes[*it]->mScene=NULL;
        }
        mMeshes.clear();
    }

    size_t getNumOfInstances() const
    {
        return mRoot->mNumOfInstances;
    }

    size_t getNumOfInnernodes() const
    {
        return mRoot->mSubtreeSize - mRoot->mNumOfInstances;
    }
    
    // Update all Boundings and numOfInstances. 
    // Has to be called before rendering starts! 
    void        update()
    {
        typedef typename Vector<t_GroupHandle>::iterator t_GroupIt;
        typedef typename Vector<t_Group*>::iterator t_ParentIt;
        typedef typename Vector<t_InstanceHandle>::iterator t_InstanceIt;

        while(!update_queue.empty())
        {
            t_GroupHandle lg = update_queue.top().first;
            update_queue.pop();
            if(lg->mNeedUpdate)
            {    
                lg->mNeedUpdate = false;
                lg->mNeedBuildUpdate = true;

                lg->mNumOfInstances=0;
                lg->mNumOfPrimitives=0;
                lg->mSubtreeSize=1;
                lg->mBounds=BBox::empty();

                for(t_InstanceIt it=lg->mInstances.begin(); it != lg->mInstances.end(); ++it)
                {
                    ++(lg->mNumOfInstances);
                    ++(lg->mSubtreeSize);
                    lg->mNumOfPrimitives += (*it)->mMesh->getPrimitiveCount();
                    lg->mBounds.extend( lg->mXfm.applyAffine( (*it)->mMesh->mBounds));
                }

                for(t_GroupIt it=lg->mGroups.begin(); it != lg->mGroups.end(); ++it)
                {
                    lg->mNumOfInstances += (*it)->mNumOfInstances;
                    lg->mNumOfPrimitives += (*it)->mNumOfPrimitives;
                    lg->mSubtreeSize += (*it)->mSubtreeSize;
                    lg->mBounds.extend(lg->mXfm.applySafeAffine((*it)->mBounds));
                }

                for(t_ParentIt it=lg->mParents.begin(); it != lg->mParents.end(); ++it)
                    if(!(*it)->mNeedUpdate)
                        setUpdateGroup(*it);
                        
            }
        }
    }

    // Special Methods for Testing:
    void computeFlattenedPrimitives()
    {
        mFlattenPrimitives.clear();
        rec_flatten(mRoot, Transform::identity());
    }

    // ----------------------------------------------------------------
    // Functions used by Node Classes:
    // ----------------------------------------------------------------

    void addMesh(t_Mesh* aMesh)
    {

        size_t id = mMeshes.insert(aMesh);
        aMesh->mID = id;
    }

    void removeMesh(t_Mesh* aMesh)
    {
        //mDeletedMeshes.push_back(aMesh->mID);
        mMeshes[aMesh->mID]= NULL;
    }
    
    void setUpdateGroup(t_GroupHandle lgp)
    {
        lgp->mNeedUpdate=true;
        update_queue.push( std::make_pair(lgp, lgp->mDepth));
    }

private:

    void rec_flatten(t_GroupHandle g, const Transform &t)
    {
        Transform t2= t*(g->mXfm);
        for(uint i=0; i < g->mInstances.size() ; ++i)
        {
            t_MeshHandle mesh= g->mInstances[i]->mMesh;
            for(uint j=0; j<mesh->getPrimitiveCount(); ++j)
            {
                t_Primitive p = mesh->getPrimitive(j);
                for(int k=0; k<3; ++k)
                {
                    p.vertices[k]= t2.applyToPointAffine(p.vertices[k]);
                    p.normals[k]= t2.applyToNormalAffine(p.normals[k]).normalize();

                }
                p.normal= t2.applyToNormalAffine(p.normal).normalize();
                mFlattenPrimitives.push_back(p);
            }
        }

        for(uint i=0; i < g->getGroupCount() ; ++i)
            rec_flatten( g->getChildGroup(i), t2);
    }

};

RTFACT_NAMESPACE_END

#endif // _RTFACT_GRAPHSCENE_HPP_INCLUDED
