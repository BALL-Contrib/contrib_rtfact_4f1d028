/*
 *  TwoLevelAccelInstance.hpp
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

 #ifndef RTFACT__ACCELERATION_INSTANCE_HPP
#define RTFACT__ACCELERATION_INSTANCE_HPP

#include <RTfact/Utils/Containers/Vector.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tInnerStruct,
    class tInstance>
class TwoLevelAccelInstance :
    public Primitive
{
public:

    typedef tInnerStruct            t_InnerStruct;
    typedef tInstance*              t_InstancePtr;

    t_InnerStruct*                  mStructure;
    Transform                       mXfm;
    BBox                            mBounds;
    Vector<size_t>                  mAppearanceMap;

    t_InstancePtr                   mInstance;

    Vector<Vec3f1>                  mCutPlaneNorms;
    Vector<float>                   mCutPlaneShifts;
    bool                            mCutPlaneCap;
    Vec3f1                          mCapColor;

    bool                            mDropShadow;

    TwoLevelAccelInstance():
        mStructure(NULL)
    {}

    TwoLevelAccelInstance(
        t_InnerStruct* aStructure,
        t_InstancePtr aInstance
    ) :
        mStructure(aStructure),
        mInstance(aInstance),
        mXfm(aInstance->getTransform()),
        mBounds(aInstance->getBounds()),
        mCutPlaneNorms(aInstance->mCutPlaneNorms),
        mCutPlaneShifts(aInstance->mCutPlaneShifts),
        mCutPlaneCap(aInstance->mCutPlaneCap),
        mCapColor(aInstance->mCapColor),
        mDropShadow(aInstance->mDropShadow)
    {
    }

    TwoLevelAccelInstance & operator = (const TwoLevelAccelInstance & other) {
        if(this != &other)
        {
            mStructure = other.mStructure;
            mXfm = other.mXfm;
            mBounds = other.mBounds;
            mAppearanceMap = other.mAppearanceMap;
            mCutPlaneShifts = other.mCutPlaneShifts;
            mCutPlaneNorms = other.mCutPlaneNorms;
            mCutPlaneCap = other.mCutPlaneCap;
            mCapColor = other.mCapColor;
            mDropShadow = other.mDropShadow;

            mInstance = other.mInstance;
        }
        return *this;
    }

    const BBox getBounds() const
    {
        return mBounds;
    }
};


RTFACT_NAMESPACE_END

#endif // RTFACT__ACCELERATION_INSTANCE_HPP
