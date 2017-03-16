/* 
 *  GraphBVH.hpp
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
 *  Created on: 2008-06-27 11:29:41 +0200
 *  Author(s): Felix Klein, Iliyan Georgiev
 */
 
 #ifndef LACHS_SG_BVH_INCLUDED
#define LACHS_SG_BVH_INCLUDED

#include <RTfact/Model/Primitive/BVH.hpp>
#include <RTfact/Model/Primitive/OrderedBVH.hpp>
#include <RTfact/Model/Scene/GraphScene.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tInnerStruct> 
class GraphBVH :
    public Primitive
{
public:

    struct BuildInstance
    {
        int index;
        Transform trans;
        BBox bounds;

        BuildInstance() :
            index(-1)
        {}
        
        BuildInstance(
            int i,
            const Transform &t,
            const BBox &b
        ):
            index(i),
            trans(t),
            bounds(b)
        {}
    };

    struct Instance
    {
        tInnerStruct* structure;
        Transform trans;
        BBox bounds;

        Instance() :
            structure(RTFACT_NULL)
        {}

        Instance(
            tInnerStruct* s,
            const Transform &t,
            const BBox &b
        ) :
            structure(s),
            trans(t),
            bounds(b)
        {}
    };

    typedef OrderedBVH<Instance> t_Base;

    typedef Vector<BuildInstance> BuildInstanceVector;
    typedef Vector<tInnerStruct> t_InnerStructVector;
    
    typedef Vector<Instance> InstanceVec;

    typedef typename t_Base::Iterator          Iterator;
    typedef typename t_Base::Element           Element;
    typedef typename t_Base::ElementIterator   ElementIterator;
    typedef typename t_Base::ChildIteratorPair ChildIteratorPair;

private:

    InstanceVec instances;
    t_InnerStructVector innerStructs;
    t_Base outerStruct;

    uint mNextInstanceIndex;

public:

    GraphBVH()
    {}

    template<
        class tSceneBase>
    void init(
        const tSceneBase &scene)
    {
        instances.resize(scene.getNumOfInstances());
        innerStructs.resize(scene.geometries.size());
        mNextInstanceIndex = 0;
        // There should be at least one instance in the scene.
        assert(instances.size()>0);
        
        outerStruct.init(scene.getNumOfInstances());
        outerStruct.setElementBase(instances[0]);
    }

#pragma region Building-related methods

    tInnerStruct *getInnerStruct(
        const Iterator geoId)
    {
        if(innerStructs.size() <= geoId)
        {
            innerStructs.resize(geoId + 1);
        }

        return &(innerStructs[geoId]);
    }

    void setElementBase(
        const Instance& aElementBase)
    {
        outerStruct.setElementBase(aElementBase);
    }

    Instance& getElementBase() const
    {
        return outerStruct.getElementBase();
    }

    RTFACT_PRIMITIVE_INLINE ChildIteratorPair createInnerNode(
        const Iterator aWhere)
    {
        return outerStruct.createInnerNode(aWhere);
    }

    template<
        class tIterator>
    RTFACT_PRIMITIVE_INLINE void createLeafNode(
        const Iterator aWhere,
        tIterator aElemBegin,
        tIterator aElemEnd)
    {
        typename InstanceVec::iterator iBegin=instances.begin() + mNextInstanceIndex;

        for(tIterator it = aElemBegin; it != aElemEnd; ++it)
        {
            // Create Instances from BuildInstances
            instances[mNextInstanceIndex++] =
                Instance(&innerStructs[it->index], it->trans, it->bounds);

        }

        typename InstanceVec::iterator iEnd=instances.begin() + mNextInstanceIndex;

        outerStruct.createLeafNode(aWhere,iBegin,iEnd);
    }

    RTFACT_PRIMITIVE_INLINE void setBounds(
        const Iterator aWhere,
        const BBox& aBounds)
    {
        outerStruct.setBounds(aWhere, aBounds);
    }

    void finalizeBuild()
    {
        outerStruct.finalizeBuild();
    }

#pragma endregion   

#pragma region Traversal-related methods

    RTFACT_PRIMITIVE_INLINE const BBox& getBounds() const
    {
        return outerStruct.getBounds();
    }

    RTFACT_PRIMITIVE_INLINE Iterator getRoot() const
    {
        return outerStruct.getRoot();
    }

    RTFACT_PRIMITIVE_INLINE bool isLeaf(
        const Iterator aWhere) const
    {
        return outerStruct.isLeaf(aWhere);
    }

    RTFACT_PRIMITIVE_INLINE const BBox& getBounds(
        const Iterator aWhere) const
    {
        return outerStruct.getBounds(aWhere);
    }

    RTFACT_PRIMITIVE_INLINE void getChildren(
        const Iterator aWhere, 
        const uint aDominantAxis,
        const uint aDirectionSign,
        const Vec3f1& aRayDir,
        Iterator& oFirst,
        Iterator& oSecond) const
    {
        outerStruct.getChildren(
            aWhere, aDominantAxis, aDirectionSign, aRayDir, oFirst, oSecond);
    }

    RTFACT_PRIMITIVE_INLINE void getChildren(
        const Iterator aWhere,
        Iterator& oFirst,
        Iterator& oSecond) const
    {
        outerStruct.getChildren(aWhere, oFirst, oSecond);
    }

    RTFACT_PRIMITIVE_INLINE void getElements(
        const Iterator aLeaf,
        ElementIterator& oBegin,
        ElementIterator& oEnd)
    {
        outerStruct.getElements(aLeaf, oBegin, oEnd);
    }

    RTFACT_PRIMITIVE_INLINE void getElements(
        const Iterator aFirst,
        const Iterator aSecond, 
        ElementIterator& oBegin,
        ElementIterator& oEnd)
    {
        outerStruct.getElements(aFirst, aSecond, oBegin, oEnd);
    }

    RTFACT_PRIMITIVE_INLINE size_t getInstanceCount()
    {
        return instances.size();
    }

    RTFACT_PRIMITIVE_INLINE size_t getPrimitiveCount()
    {
        size_t result = 0;

        for(int i = 0; i < instances.size(); ++i)
        {
            if(instances[i].structure)
            {
                result += instances[i].structure->getLeafCount();
            }
        }

        return result;
    }

#pragma endregion

};

RTFACT_NAMESPACE_END

// RTFACT_SERIALIZATION_REGISTER_SERIALIZE_METHOD_CLASS(RTfact::GraphBVH)

#endif // LACHS_SG_BVH_INCLUDED
