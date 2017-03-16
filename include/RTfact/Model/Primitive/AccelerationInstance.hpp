/* 
 *  AccelerationInstance.hpp
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
 *  Author(s): Felix Klein, Vaclav Krajicek
 */
 
 #ifndef RTFACT__ACCELERATION_INSTANCE_HPP
#define RTFACT__ACCELERATION_INSTANCE_HPP

#include <RTfact/Utils/Containers/Vector.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tInnerStruct,
    class tInstance,
    class tGroup>
class AccelerationInstance :
    public Primitive
{
public:

    typedef tInnerStruct            t_InnerStruct;
    typedef tInstance*              t_InstancePtr;
    typedef typename Traits<tGroup>::Ptr  			t_GroupHandle;
    typedef typename RTfact::Vector<t_GroupHandle> t_GroupHandleVector;

    t_InnerStruct*                  mStructure;
    Transform                       mXfm;
    BBox                            mBounds;
    Vector<size_t>                  mAppearanceMap;

    t_InstancePtr                   mInstance;
    t_GroupHandleVector             mPath;

    AccelerationInstance(): 
        mStructure(NULL)
    {}

    AccelerationInstance(
        t_InnerStruct* aStructure,
        const Transform &aXfm,
        const BBox &aBounds
    ) :
        mStructure(aStructure),
        mXfm(aXfm),
        mBounds(aBounds)
    {}

    AccelerationInstance & operator = (const AccelerationInstance & other) {
        if(this != &other)
        {
            mStructure = other.mStructure;
            mXfm = other.mXfm;
            mBounds = other.mBounds;
            mAppearanceMap = other.mAppearanceMap;

            mInstance = other.mInstance;
            mPath = other.mPath;
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
