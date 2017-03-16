/* 
 *  BVH.hpp
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
 *  Created on: 2008-06-09 17:30:26 +0200
 *  Author(s): Iliyan Georgiev, Felix Klein
 */
 
 #ifndef RTFACT__BVH_HPP
#define RTFACT__BVH_HPP

#include <RTfact/Config/Common.hpp>

#include <utility>
#include <limits>

#include <RTfact/Utils/BBox.hpp>
#include <RTfact/Utils/IO/SerializationUtils.hpp>

#include <RTfact/Concept/Scene.hpp>
#include <RTfact/Concept/Primitive.hpp>
#include <RTfact/Model/Primitive/Detail/OffsetElementIterator.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tElement>
class BVH :
    public Primitive
{
    struct Node
    {
        union
        {
            float4 min;

            struct 
            {
                float minX, minY, minZ;
                uint32 firstChildId;
            };
        };

        union
        {
            float4 max;

            struct
            {
                float maxX, maxY, maxZ;
                uint32 nodeTypeAndElemEnd;
            };
        };
        
        enum
        {
            FULL_SHIFT       = 32,
            NODE_TYPE_SHIFT  = FULL_SHIFT - 1,
            DATA_ASSERT_MASK = 0xC0000000,
            EXTRACT_MASK     = 0x3FFFFFFF,
            INNER_INIT_MASK  = 0x40000000,
            LEAF_INIT_MASK   = 0xC0000000
        };

        RTFACT_PRIMITIVE_INLINE void initInner()
        {
            firstChildId = static_cast<uint32>(INNER_INIT_MASK);
            nodeTypeAndElemEnd = static_cast<uint32>(INNER_INIT_MASK);
        }

        RTFACT_PRIMITIVE_INLINE void initLeaf()
        {
            firstChildId = static_cast<uint32>(INNER_INIT_MASK);
            nodeTypeAndElemEnd = static_cast<uint32>(LEAF_INIT_MASK);
        }

        RTFACT_PRIMITIVE_INLINE bool isLeaf() const
        {
            return (nodeTypeAndElemEnd >> NODE_TYPE_SHIFT != 0);
        }

        RTFACT_PRIMITIVE_INLINE const BBox& getBounds() const
        {
            return *reinterpret_cast<const BBox*>(this);
        }

        RTFACT_PRIMITIVE_INLINE uint32 getFirstChild() const
        {
            return firstChildId & EXTRACT_MASK;
        }

        RTFACT_PRIMITIVE_INLINE uint32 getSecondChild() const
        {
            return nodeTypeAndElemEnd & EXTRACT_MASK;
        }

        RTFACT_PRIMITIVE_INLINE uint32 getElementBegin() const
        {
            return getFirstChild();
        }

        RTFACT_PRIMITIVE_INLINE uint32 getElementEnd() const
        {
            return getSecondChild();
        }

        RTFACT_PRIMITIVE_INLINE void setBounds(
            const BBox& aBounds)
        {
            minX = aBounds.min.x;
            minY = aBounds.min.y;
            minZ = aBounds.min.z;

            maxX = aBounds.max.x;
            maxY = aBounds.max.y;
            maxZ = aBounds.max.z;
        }

        RTFACT_PRIMITIVE_INLINE void setFirstChild(
            const uint32 aChildId)
        {
            RTFACT_PRIMITIVE_ASSERT((aChildId & DATA_ASSERT_MASK) == 0);

            firstChildId |= aChildId;
        }

        RTFACT_PRIMITIVE_INLINE void setSecondChild(
            const uint32 aChildId)
        {
            RTFACT_PRIMITIVE_ASSERT((aChildId & DATA_ASSERT_MASK) == 0);

            nodeTypeAndElemEnd |= aChildId;
        }

        RTFACT_PRIMITIVE_INLINE void setElementBegin(
            const uint32 aElemBegin)
        {
            RTFACT_PRIMITIVE_ASSERT((aElemBegin & DATA_ASSERT_MASK) == 0);

            setFirstChild(aElemBegin);
        }

        RTFACT_PRIMITIVE_INLINE void setElementEnd(
            const uint32 aElemEnd)
        {
            RTFACT_PRIMITIVE_ASSERT((aElemEnd & DATA_ASSERT_MASK) == 0);

            setSecondChild(aElemEnd);
        }
    };

public:

    typedef tElement                      Element;
    typedef uint32                        Iterator;
    typedef std::pair<Iterator, Iterator> ChildIteratorPair;
    typedef Detail::OffsetElementIterator<
        Element, Vector<int>::iterator>   ElementIterator;

protected:

    Vector<Node> mNodes;
    Vector<int> mElementOffsets;
    uint32 mNextNodeIndex, mNextLeafIndex;
    Element* mElementBase;

public:

    BVH() :
        mElementBase(RTFACT_NULL)
    {}

    size_t getNodeCount() const
    {
        return mNodes.size();
    }

    size_t getLeafCount() const
    {
        return mElementOffsets.size();
    }

#pragma region Building-related methods

    void setElementBase(
        const Element& aElementBase)
    {
        mElementBase = const_cast<Element*>(&aElementBase);
    }

    Element& getElementBase() const
    {
        return *mElementBase;
    }

    void init(
        const size_t aElementCount)
    {
        mNodes.resize(aElementCount * 2 + 2);
        mElementOffsets.resize(aElementCount);

        mNextNodeIndex = 1;
        mNextLeafIndex = 0;
    }

    RTFACT_PRIMITIVE_INLINE ChildIteratorPair createInnerNode(
        const Iterator aWhere)
    {
        const uint32 firstChildId = mNextNodeIndex;
        const uint32 secondChildId = mNextNodeIndex + 1;
        mNextNodeIndex += 2;

        mNodes[aWhere].initInner();
        mNodes[aWhere].setFirstChild(firstChildId);
        mNodes[aWhere].setSecondChild(secondChildId);
  
        return std::make_pair(firstChildId, secondChildId);
    }

    template<
        class tIterator>
    RTFACT_PRIMITIVE_INLINE void createLeafNode(
        const Iterator aWhere,
        tIterator aElemBegin,
        tIterator aElemEnd)
    {
        RTFACT_PRIMITIVE_ASSERT(mElementBase != RTFACT_NULL);

        mNodes[aWhere].initLeaf();
        mNodes[aWhere].setElementBegin(mNextLeafIndex);

        for(tIterator it = aElemBegin; it != aElemEnd; ++it)
        {
            RTFACT_PRIMITIVE_ASSERT(
                (&(*it) - mElementBase) <
                static_cast<ptrdiff_t>(std::numeric_limits<int>::max()));

            mElementOffsets[mNextLeafIndex++] =
                static_cast<int>(&(*it) - mElementBase);
        }

        mNodes[aWhere].setElementEnd(mNextLeafIndex);
    }

    template<
        class tIterator,
        class tElemExtractor>
    RTFACT_PRIMITIVE_INLINE void createLeafNode(
        const Iterator aWhere,
        tIterator aBegin,
        tIterator aEnd,
        tElemExtractor aElemExtractor)
    {
        RTFACT_PRIMITIVE_ASSERT(mElementBase != RTFACT_NULL);

        mNodes[aWhere].initLeaf();
        mNodes[aWhere].setElementBegin(mNextLeafIndex);

        for(tIterator it = aBegin; it != aEnd; ++it)
        {
            RTFACT_PRIMITIVE_ASSERT(
                (&(aElemExtractor(*it)) - mElementBase) <
                static_cast<ptrdiff_t>(std::numeric_limits<int>::max()));

            mElementOffsets[mNextLeafIndex++] =
                static_cast<int>(&(aElemExtractor(*it)) - mElementBase);
        }

        mNodes[aWhere].setElementEnd(mNextLeafIndex);
    }

    RTFACT_PRIMITIVE_INLINE void setBounds(
        const Iterator aWhere,
        const BBox& aBounds)
    {
        mNodes[aWhere].setBounds(aBounds);
    }

    void finalizeBuild()
    {}

#pragma endregion // Building-related methods

#pragma region Traversal-related methods

    RTFACT_PRIMITIVE_INLINE const BBox& getBounds() const
    {
        return mNodes[0].getBounds();
    }

    RTFACT_PRIMITIVE_INLINE Iterator getRoot() const
    {
        return 0;
    }

    RTFACT_PRIMITIVE_INLINE bool isLeaf(
        const Iterator aWhere) const
    {
        return mNodes[aWhere].isLeaf();
    }

    RTFACT_PRIMITIVE_INLINE const BBox& getBounds(
        const Iterator aWhere) const
    {
        return mNodes[aWhere].getBounds();
    }

    RTFACT_PRIMITIVE_INLINE void getChildren(
        const Iterator aWhere,
        Iterator& oFirst,
        Iterator& oSecond) const
    {
        oFirst  = mNodes[aWhere].getFirstChild();
        oSecond = mNodes[aWhere].getSecondChild();
    }

    RTFACT_PRIMITIVE_INLINE void getElements(
        const Iterator aLeaf,
        ElementIterator& oBegin,
        ElementIterator& oEnd)
    {
        RTFACT_PRIMITIVE_ASSERT(mElementBase != RTFACT_NULL);
        RTFACT_PRIMITIVE_ASSERT(isLeaf(aLeaf));

        oBegin = ElementIterator(mElementBase, mElementOffsets.begin() +
                                               mNodes[aLeaf].getElementBegin());

        oEnd   = ElementIterator(mElementBase, mElementOffsets.begin() +
                                               mNodes[aLeaf].getElementEnd());
    }

    RTFACT_PRIMITIVE_INLINE void getElements(
        const Iterator aFirst,
        const Iterator aSecond, 
        ElementIterator& oBegin,
        ElementIterator& oEnd)
    {
        RTFACT_PRIMITIVE_ASSERT(mElementBase != RTFACT_NULL);

        oBegin = ElementIterator(mElementBase, mElementOffsets.begin() +
                                                aFirst);

        oEnd = ElementIterator(mElementBase, mElementOffsets.begin() +
                                              aSecond);
    }

#pragma endregion // Traversal-related methods

    void serialize(
        IO::Serializer& aSerializer)
    {
        aSerializer & mNodes
                    & mElementOffsets;
    }
};

RTFACT_NAMESPACE_END

RTFACT_SERIALIZATION_REGISTER_SERIALIZE_METHOD_TEMPLATE_CLASS(RTfact::BVH, (class))

#endif // RTFACT__BVH_HPP
