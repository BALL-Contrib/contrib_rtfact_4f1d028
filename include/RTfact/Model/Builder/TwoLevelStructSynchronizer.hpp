/*
 *  TwoLevelStructSynchronizer.hpp
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

 #ifndef _RTFACT_GRAPHSTRUCTSYNCHRONIZER_HPP_INCLUDED
#define _RTFACT_GRAPHSTRUCTSYNCHRONIZER_HPP_INCLUDED


#include <RTfact/Concept/Builder.hpp>

RTFACT_NAMESPACE_BEGIN

template<class tVector, class tSubVector, class tValueType>
class DoubleVectorIterator{
public:
    typedef tVector        t_Vector;
    typedef tSubVector     t_SubVector;
    typedef tValueType     t_ValueType;

private:
    const t_Vector                  &mVector;
    typename t_Vector::iterator     mTopIterator;
    typename t_SubVector::iterator  mSubIterator;

public:

    DoubleVectorIterator(const t_Vector &aVector, typename t_Vector::iterator aTopIterator)
        : mVector(aVector), mTopIterator(aTopIterator)
    {
        if(mTopIterator != mVector.end())
            mSubIterator = (*mTopIterator)->begin();
    }

    DoubleVectorIterator(const t_Vector &aVector)
        : mVector(aVector), mTopIterator(aVector.begin())
    {
        if(mTopIterator != mVector.end())
            mSubIterator = (*mTopIterator)->begin();
    }

    const t_ValueType& operator*() const
    {
        return *mSubIterator;
    }

    t_ValueType& operator*()
    {
        return *mSubIterator;
    }

    const t_ValueType* operator->() const
    {
        return &*mSubIterator;
    }

    t_ValueType* operator->()
    {
        return &*mSubIterator;
    }

    DoubleVectorIterator& operator++()
    {
        ++mSubIterator;
        if(mSubIterator == (*mTopIterator)->end())
        {
            ++mTopIterator;
            if(mTopIterator == mVector.end())
                mSubIterator = 0;
            else
                mSubIterator = (*mTopIterator)->begin();
        }
        return *this;
    }

    DoubleVectorIterator operator++(int)
    {
	DoubleVectorIterator prevThis = *this;
        ++prevThis;
        return prevThis;
    }

    bool operator==(
        const DoubleVectorIterator& aOther) const
    {
        return (mTopIterator == aOther.mTopIterator &&
            (mTopIterator == mVector.end() || mSubIterator == aOther.mSubIterator));
    }

    bool operator!=(
        const DoubleVectorIterator& aOther) const
    {
        return !(*this == aOther);
    }
};

template <class tInnerStructBuilder, class tOuterStructBuilder>
class TwoLevelStructSynchronizer : public Builder
{
public:
    typedef tInnerStructBuilder t_innerBuilder;
    typedef tInnerStructBuilder t_outerBuilder;

    template <class tScene, class tBuildData, class tTree>
    void build(tScene &aScene, tBuildData& aBuildData, tTree& aOuterTree)
    {
        typedef tScene                                      t_Scene;
        typedef typename t_Scene::t_Primitive               t_Primitive;
        typedef typename t_Scene::t_PrimitiveMap            t_PrimitiveMap;
        typedef typename t_Scene::t_Mesh                    t_Mesh;
        typedef typename t_Scene::t_Geometry                  t_Geometry;
        typedef typename t_Scene::t_GeometryMap               t_GeometryMap;
        typedef typename t_Scene::t_Instance                t_Instance;
        typedef typename t_Scene::t_InstanceMap             t_InstanceMap;
        typedef typename t_Geometry::t_KeyVector              t_InnerVector;
        typedef typename t_Geometry::t_MultiVector            t_OuterVector;
        typedef DoubleVectorIterator<t_OuterVector, t_InnerVector, typename t_PrimitiveMap::key_type>
                                                            t_MultiIterator;

        typedef tBuildData                                  t_BuildData;

        typedef typename t_BuildData::t_InnerStruct         t_InnerStruct;
        typedef typename t_BuildData::t_StructVector        t_StructVector;

        typedef typename t_BuildData::t_AccelInstance       t_AccelInstance;
        typedef typename t_BuildData::t_InstanceVector      t_InstanceVector;

        t_StructVector &structures = aBuildData.mInnerStructs;
        t_GeometryMap    &objects     = aScene.mGeometries;

        t_InstanceVector &accelInstances = aBuildData.mAccelInstances;
        t_InstanceMap    &instances      = aScene.mInstances;

		t_Primitive    *base	   =  0;

        t_innerBuilder  builder;
        t_outerBuilder  outerBuilder;

#ifndef RTFACT_USE_PRIMITIVE_INTMAP
        // Find Base Pointer
        // TODO: Implement this (if we ever want to go back to vectors)
        /*
        for(typename t_MeshMap::key_iterator it = meshes.keysBegin();
            it < meshes.keysEnd(); it++)
        {
          t_Mesh* mesh = meshes[*it];
          if(mesh != NULL && !mesh->mPrimitives.empty())
          {
            if(base == 0 || &mesh->mPrimitives.front() < base)
            {
              base = &mesh->mPrimitives.front();
            }

          }

        }
        */
#endif

        typename t_GeometryMap::key_vector eraseGeometries;

        // TODO Skip Syncho, if scene didn't change at all

        // ----------------------------------------------------------
        // Synchronize Geometries with Inner Acceleration Structures
        // ----------------------------------------------------------

        for(typename t_GeometryMap::key_iterator it = objects.keysBegin();
            it != objects.keysEnd(); it++)
        {
            t_Geometry* object = objects[*it];
            if(object == NULL){
                if(structures.size() > static_cast<size_t>(*it) && structures[*it] != NULL){
                    // Delete structure that is not used by scene anymore
                    delete structures[*it];
                    structures[*it] = NULL;

                }
                // we MUST NOT erase content while traversing IntMap
                // thus we just store all objects to be erased
                eraseGeometries.push_back(*it);
            }
            else
            {
                // Expand Vector to contain the key.
                if(structures.size() <= *it)
                {
                    size_t oldSize = structures.size();
                    structures.resize(*it+1);
                    for(size_t i = oldSize; i < *it+1; i++)
                        structures[i]=NULL;
                }


                if( (structures[*it] == NULL || object->mNeedsAccelStructUpdate)
                    && object->getPrimitiveCount() > 0 )
                {
                    object->updateBounds();
                    object->mNeedsAccelStructUpdate = false;

                    // Update Acceleration Structure.
                    if(structures[*it] == NULL)
                        structures[*it] = new t_InnerStruct();

#ifndef RTFACT_USE_PRIMITIVE_INTMAP
                    // TODO: Implement
#else
                    t_OuterVector vector = object->getKeyMultiVector();
                    builder.build(*structures[*it],
                        t_MultiIterator(vector, vector.begin()),
                        t_MultiIterator(vector, vector.end()),
                        aScene.mPrimitivePool,
                        boost::lambda::bind(
                            &t_Primitive::getBounds,
                            boost::lambda::_1));

#endif


                }
#ifdef RTFACT_USE_PRIMITIVE_INTMAP
                else if(structures[*it] != NULL)
                {
                  structures[*it]->setElementBase(aScene.mPrimitivePool.getElementBase());
                }
#endif

            }
        }
        objects.erase(eraseGeometries.begin(), eraseGeometries.end());

        // ----------------------------------------------------------
        // Synchronize AccelerationInstances with Instances
        // ----------------------------------------------------------

        // TODO Make this more optimized (i.e. keep AccelerationInstances from last frame)
        accelInstances.clear();

        typename t_InstanceMap::key_vector eraseInstances;

        for(typename t_InstanceMap::key_iterator it = instances.keysBegin();
                it != instances.keysEnd(); it++){
            t_Instance *instance = instances[*it];
            if(instance == NULL){
                eraseInstances.push_back(*it);
            }
            else if(structures[instance->mGeometry->mID] != NULL)
            {

                if(instance->mNeedsBoundUpdate)
                    instance->updateBounds();
                accelInstances.push_back(
                        t_AccelInstance(structures[instance->mGeometry->mID],
                                instance));
                accelInstances.back().mAppearanceMap = instance->getFlattenedAppearanceMap();
                instance->mNeedsBoundUpdate = false;
                instance->mNeedsMaterialUpdate = false;
            }

        }
        instances.erase(eraseInstances.begin(), eraseInstances.end());

        // ----------------------------------------------------------
        // Rebuild Outer Acceleration Structure
        // ----------------------------------------------------------

        outerBuilder.build(aOuterTree,
	        accelInstances.begin(),
            accelInstances.end(),
            boost::lambda::bind(
                &t_AccelInstance::getBounds,
                boost::lambda::_1));

    }

};


RTFACT_NAMESPACE_END

#endif // _RTFACT_GRAPHSTRUCTSYNCHRONIZER_HPP_INCLUDED
