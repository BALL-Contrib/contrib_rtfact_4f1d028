/*
 *  TwoLevelScene.hpp
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

 #ifndef _RTFACT_TWOLEVELSCENE_HPP_INCLUDED
#define _RTFACT_TWOLEVELSCENE_HPP_INCLUDED

#include <queue>
#include <vector>
#include <iostream>
#include <RTfact/Utils/Transform.hpp>
#include <RTfact/Utils/Containers/IntMap.hpp>
#include <RTfact/Concept/Scene/ShadingInfoScene.hpp>
#include <RTfact/Model/Scene/TwoLevelScene/TwoLevelSceneInstance.hpp>


RTFACT_NAMESPACE_BEGIN

template<
    class tSceneConfig,
    class tLightConfig,
    class tEnvironmentTexture,
    umask taProperties = 0>
class TwoLevelScene :
    public ShadingInfoScene<tSceneConfig, tLightConfig, tEnvironmentTexture, taProperties>
{
    typedef ShadingInfoScene<tSceneConfig, tLightConfig, tEnvironmentTexture, taProperties> t_Base;

public:

    typedef typename tSceneConfig::Primitive        t_Primitive;
    typedef typename tSceneConfig::Geometry           t_Geometry;
    typedef typename tSceneConfig::Mesh             t_Mesh;
    typedef typename tSceneConfig::Instance         t_Instance;

    typedef typename Traits<t_Mesh>::Ptr            t_MeshHandle;
    typedef typename Traits<t_Instance>::Ptr        t_InstanceHandle;
    typedef typename Traits<t_Geometry>::Ptr          t_GeometryHandle;



    typedef Vector<t_Primitive>		                t_PrimitiveVector;

    #ifdef RTFACT_USE_PRIMITIVE_INTMAP
        typedef IntMap<t_Primitive>				    t_PrimitiveMap;
        t_PrimitiveMap							    mPrimitivePool;
    #endif

    typedef IntMap<t_Geometry*>                       t_GeometryMap;
    typedef typename t_GeometryMap::key_type          t_GeometryKey;

    typedef IntMap<t_Instance*>                     t_InstanceMap;
    typedef typename t_InstanceMap::key_type        t_InstanceKey;

    t_GeometryMap                                     mGeometries;
    t_InstanceMap                                   mInstances;

    Vector<t_Primitive>                             mFlattenPrimitives;



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
/*
    void print_content()
    {
        std::cout<<"\nContent:----------------------------------------------------------";
        //std::cout<<"\n\Meshes - amount: "<<mMeshes.size();
        for(t_MeshMap::key_iterator it = mMeshes.keysBegin(); it != mMeshes.keysEnd(); it++)
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


        std::cout<<"\n----------------------------------------------------------Ende";

    }
    void print_structure(std::string pre, t_GroupHandle group)
    {
        typedef Vector<t_GroupHandle>::iterator t_GroupIt;
        typedef Vector<t_InstanceHandle>::iterator t_InstanceIt;

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

  */


    TwoLevelScene() {
    }

    ~TwoLevelScene() {

        for(typename t_GeometryMap::iterator it = mGeometries.begin(); it != mGeometries.end(); ++it)
        {
            if(*it)
            {
                t_GeometryHandle kungFuDeathGrip(*it);
                (*it)->decouple();
            }

        }
    }

    // ----------------------------------------------------------------
    // Used by external application:
    // ----------------------------------------------------------------

    t_Geometry* createGeometry()
    {
        t_Geometry* obj = new t_Geometry(this);
        size_t id = mGeometries.insert(obj);
        obj->mID = id;
        return obj;
    }


    // TODO Try to optimize this method...
    size_t getGeometryCount()
    {
        int count = 0;
        for(typename t_GeometryMap::iterator it = mGeometries.begin(); it != mGeometries.end(); ++it)
            if(*it)
                ++count;
        return count;
    }

    // TODO Try to optimize this method...
    t_Geometry* getGeometry(size_t aIdx)
    {
        for(typename t_GeometryMap::iterator it = mGeometries.begin(); it != mGeometries.end(); ++it)
            if(*it){
                if(aIdx == 0)
                    return *it;
                else
                    --aIdx;
            }
        return 0;
    }

    t_Instance* createInstance(t_GeometryHandle aGeometry)
    {
        t_Instance* instance = new t_Instance(aGeometry);
        size_t id = mInstances.insert(instance);
        instance->mID = id;
        return instance;
    }
    /*
    size_t getInstanceCount() const
    {
        return mInstances.size();
    }

    t_Instance* getInstance(size_t aIdx) const
    {
        return *(mInstances.begin() + aIdx);
    }
    */

    // Special Methods for Testing:
    void computeFlattenedPrimitives()
    {
        // TODO: Implement
        //mFlattenPrimitives.clear();
        //rec_flatten(mRoot, Transform::identity());
    }

    // ----------------------------------------------------------------
    // Functions used by Node Classes:
    // ----------------------------------------------------------------

    void removeGeometry(t_Geometry* aGeometry)
    {
        mGeometries[aGeometry->mID]= NULL;
    }

    void removeInstance(t_Instance* aInstance)
    {
        mInstances[aInstance->mID]= NULL;
    }

private:


 /*
    void rec_flatten(t_GeometryHandle g, const Transform &t)
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
*/
};

RTFACT_NAMESPACE_END

#endif // _RTFACT_TWOLEVELSCENE_HPP_INCLUDED
