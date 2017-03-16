/*
 *  TwoLevelSceneMesh.hpp
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

 #ifndef _RTFACT_TWOLEVELSCENE_MESH_HPP_INCLUDED
#define _RTFACT_TWOLEVELSCENE_MESH_HPP_INCLUDED

RTFACT_NAMESPACE_BEGIN

template<class tSceneConfig>
class TwoLevelSceneMesh : public ReferenceCounted
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
    typedef Vector<t_MeshHandle>                t_MeshVector;

    typedef typename tSceneConfig::Geometry       t_Geometry;
    typedef typename Traits<t_Geometry>::Ptr      t_GeometryHandle;

    typedef size_t                   		    t_MeshID;

    //-------------------------------------------------
    // Fields
    //-------------------------------------------------
    t_ScenePtr                                          mScene;

#ifndef RTFACT_USE_PRIMITIVE_INTMAP
    t_PrimitiveVector                                   mPrimitives;
#else
    typedef typename IntMap<t_Primitive>::key_vector	t_key_vector;
    typedef typename t_key_vector::iterator				t_key_iterator;
    typedef typename IntMap<t_Primitive>::key_type	    key_type;
    t_key_vector*									    mKeys;
#endif
    size_t                                              mAppearanceId;

    t_GeometryHandle                                      mParentGeometry;
    t_MeshID                                            mID;
    t_MeshID                                            mExID;
    BBox                                                mBounds;

    //-------------------------------------------------
    // Functions
    //-------------------------------------------------


    // Constructor should only be used by TwoLevelSceneGeometry
#ifndef RTFACT_USE_PRIMITIVE_INTMAP

    TwoLevelSceneMesh(t_GeometryPtr aParent) :
            mScene(aParent->mScene),
            mParent(aParent),
            mAppearanceId(0),
            mBounds(BBox::empty())

    {
        RTFACT_ASSERT(mScene);
    }

#else

    TwoLevelSceneMesh(t_GeometryHandle aParent) :
            mScene(aParent->mScene),
            mParentGeometry(aParent),
            mAppearanceId(0),
            mBounds(BBox::empty()),
            mKeys(new t_key_vector()),
            mExID(t_MeshID(-1))
    {
        RTFACT_ASSERT(mScene);
    }

#endif

    ~TwoLevelSceneMesh()
    {
        if(mScene)
        {
            decouple();
        }
    }

    void addPrimitive(const t_Primitive &aPrimitive)
    {
        RTFACT_ASSERT(mScene != 0);
        // TODO Double Check if normals are initialized

#ifndef RTFACT_USE_PRIMITIVE_INTMAP
        mPrimitives.push_back(aPrimitive);
        t_Primitive &p = mPrimitives[mPrimitives.size() -1 ];
#else
        mKeys->push_back(mScene->mPrimitivePool.insert(aPrimitive));
        t_Primitive &p = mScene->mPrimitivePool[mKeys->back()];
#endif
        mBounds.extend(p.getBounds());
        // TODO Remove apperanceID hack
		p.appearanceID = static_cast<RTfact::uint>(mID);
        mParentGeometry->mNeedsAccelStructUpdate = true;
    }

    void addPrimitives(const t_PrimitiveVector &aPrimitives)
    {
        RTFACT_ASSERT(mScene != 0);


        for(int i=0; i < aPrimitives.size(); i++)\
        {
            addPrimitive(aPrimitives[i]);
        }
    }

    void clearPrimitives(){
        mScene->mPrimitivePool.erase(mKeys->begin(), mKeys->end());
        mKeys->clear();
        mBounds = BBox::empty();
    }

    t_GeometryHandle getGeometry()
    {
        RTFACT_ASSERT(mScene != 0);
        return mParentGeometry;
    }

    size_t getAppearanceId()
    {
        RTFACT_ASSERT(mScene != 0);
	    return mAppearanceId;
    }

    void setAppearanceId(size_t aId)
    {
        RTFACT_ASSERT(mScene != 0);
        mAppearanceId = aId;
        mParentGeometry->forwardMaterialUpdate();
    }


	size_t getPrimitiveCount() const
	{
        RTFACT_ASSERT(mScene != 0);
#ifndef RTFACT_USE_PRIMITIVE_INTMAP
		return mPrimitives.size();
#else
		return mKeys->size();
#endif
	}

	t_Primitive& getPrimitive(const int i) const
	{
        RTFACT_ASSERT(mScene != 0);
#ifndef RTFACT_USE_PRIMITIVE_INTMAP
		return mPrimitives[i];
#else
		return mScene->mPrimitivePool[(*mKeys)[i]];
#endif

	}

    void decouple()
    {
        RTFACT_ASSERT(mScene);

        mScene->mPrimitivePool.erase(mKeys->begin(), mKeys->end());
        mScene = 0;
        delete mKeys;

        mParentGeometry->removeMesh(this);
        mParentGeometry = 0;
    }

};


RTFACT_NAMESPACE_END

#endif
