/*
 *  TwoLevelSceneInstance.hpp
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

 #ifndef _RTFACT_TWOLEVELSCENE_INSTANCE_HPP_INCLUDED
#define _RTFACT_TWOLEVELSCENE_INSTANCE_HPP_INCLUDED

RTFACT_NAMESPACE_BEGIN

template<class tSceneConfig>
class TwoLevelSceneInstance : public ReferenceCounted
{
public:

    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    //-------------------------------------------------
    // Typedefs
    //-------------------------------------------------


    typedef typename tSceneConfig::Scene        t_Scene;
    typedef typename tSceneConfig::Scene*       t_ScenePtr;

    typedef typename tSceneConfig::Primitive    t_Primitive;
    typedef Vector<t_Primitive>                 t_PrimitiveVector;

    typedef typename tSceneConfig::Mesh         t_Mesh;
    typedef typename Traits<t_Mesh>::Ptr        t_MeshHandle;


    typedef typename tSceneConfig::Geometry     t_Geometry;
    typedef typename Traits<t_Geometry>::Ptr    t_GeometryHandle;

    typedef typename t_Geometry::t_MeshMap      t_MeshMap;

    typedef typename tSceneConfig::Instance     t_Instance;
    typedef typename Traits<t_Instance>::Ptr    t_InstanceHandle;
    typedef Vector<t_Instance*>                 t_InstanceVector;
    typedef size_t                              t_AppearanceID;
    typedef Vector<t_AppearanceID>                      t_AppearanceMap;
    typedef Vector< Vec3f<1> >                  t_Vec3Vector;
    typedef Vector< float >                     t_FloatVector;

    typedef size_t                              t_InstanceID;

    //-------------------------------------------------
    // Fields
    //-------------------------------------------------

    t_ScenePtr              mScene;
    t_InstanceID            mID;
    t_InstanceID            mExID;
    t_GeometryHandle          mGeometry;

    Transform               mXfm;
    t_Vec3Vector            mCutPlaneNorms;
    t_Vec3Vector            mCutPlanePoints;
    t_FloatVector           mCutPlaneShifts;
    bool                    mCutPlaneCap;
    Vec3f<1>                mCapColor;

    bool                    mDropShadow;

    BBox                    mBounds;
    bool                    mNeedsBoundUpdate;
    bool                    mNeedsMaterialUpdate;

    t_AppearanceMap         mAppearanceMap;

    //-------------------------------------------------
    // Functions
    //-------------------------------------------------

    // Must only be called by scene!
    TwoLevelSceneInstance(t_GeometryHandle aGeometry) : mScene(aGeometry->mScene), mGeometry(aGeometry),
        mXfm(Transform::identity()), mCutPlaneCap(false), mDropShadow(true), mBounds(BBox::empty()), mNeedsBoundUpdate(true), mNeedsMaterialUpdate(true), mExID(t_InstanceID(-1))
    {
        mGeometry->mInstances.push_back(this);
        size_t oldSize = mAppearanceMap.size();
        mAppearanceMap.resize(mGeometry->mMeshes.getMaxKey() + 1);
        for(size_t i=oldSize; i < mAppearanceMap.size(); i++)
            mAppearanceMap[i] = t_AppearanceID(-1);
    }

    ~TwoLevelSceneInstance(){
        if(mScene)
            decouple();
    }

    void setTransform(const Transform &aXfm)
    {
        RTFACT_ASSERT(mScene);

        mXfm = aXfm;
        mNeedsBoundUpdate = true;
    }

    Transform& getTransform()
    {
        return mXfm;
    }

    void setAppearance(const t_MeshHandle &aMesh, const size_t aAppearanceId)
    {
        RTFACT_ASSERT(mScene);
        RTFACT_ASSERT(aMesh->mParentGeometry == mGeometry);

        size_t idx = aMesh->mID;

        RTFACT_ASSERT(idx < mAppearanceMap.size());

        mAppearanceMap[idx] = aAppearanceId;

        mNeedsMaterialUpdate = true;
    }

    void clearAppearance(const t_MeshHandle &aMesh)
    {
        setAppearance(aMesh, t_AppearanceID(-1));
        mNeedsMaterialUpdate = true;
    }

    void setCutPlanes(bool aCap, float aCapR, float aCapG, float aCapB, 
        unsigned int aPlaneCount,
        float* aNormX, float* aNormY, float* aNormZ,
        float* aPointX, float* aPointY, float* aPointZ)
    {
        mCapColor = Vec3f<1>(aCapR, aCapG, aCapB);
        mCutPlaneCap = aCap;
        mCutPlanePoints.clear();
        mCutPlaneNorms.clear();
        mCutPlaneShifts.clear();
        for(unsigned int i = 0; i < aPlaneCount; i++)
        {
            Vec3f<1> n(aNormX[i], aNormY[i], aNormZ[i]);
            n.setNormalized();
            Vec3f<1> p(aPointX[i], aPointY[i], aPointZ[i]);

            float s = - Vec3f<1>::dot(n, p);

            mCutPlanePoints.push_back(p);
            mCutPlaneNorms.push_back(n);
            mCutPlaneShifts.push_back(s);
        }
    }

    void setDropShadow(bool aDropShadow)
    {
        mDropShadow = aDropShadow;
    }

    // Used by Geometry
    void updateMeshAdded(size_t aIdx)
    {
        RTFACT_ASSERT(mScene);
        if( mAppearanceMap.size() <= aIdx )
        {
            size_t oldSize = mAppearanceMap.size();
            mAppearanceMap.resize(aIdx + 1);
            for(size_t i=oldSize; i < mAppearanceMap.size(); i++)
                mAppearanceMap[i] = t_AppearanceID(-1);

        }
        mAppearanceMap[aIdx] = t_AppearanceID(-1);
        mNeedsBoundUpdate = true;
    }

    void updateMeshRemoved(size_t aIdx)
    {
        RTFACT_ASSERT(mScene);
        // Don't do anything
        mNeedsBoundUpdate = true;
    }

    // Used Internally by StructSynchronizer
    void updateBounds()
    {
        mBounds = mXfm.applyAffine(mGeometry->mBounds);
    }

    BBox getBounds(){
        return mBounds;
    }


    t_AppearanceMap getFlattenedAppearanceMap()
    {
        t_AppearanceMap map(mGeometry->mMeshes.getMaxKey() + 1);
        for(typename t_MeshMap::key_iterator it = mGeometry->mMeshes.keysBegin();
            it != mGeometry->mMeshes.keysEnd(); it++)
        {
            if(mAppearanceMap[*it] == t_AppearanceID(-1))
                map[*it] = mGeometry->mMeshes[*it]->mAppearanceId;
            else
                map[*it] = mAppearanceMap[*it];
        }
        return map;
    }


    void decouple()
    {
        RTFACT_ASSERT(mScene);
        mScene->removeInstance(this);
        mScene = 0;

        mGeometry->removeInstance(this);
        mGeometry = 0;
    }

    t_GeometryHandle getGeometry(){
        return mGeometry;
    }

};


RTFACT_NAMESPACE_END

#endif
