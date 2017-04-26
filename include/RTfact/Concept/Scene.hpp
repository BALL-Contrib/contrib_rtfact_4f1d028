/*
 *  Scene.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2008  Saarland University
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
 *  Author(s): Iliyan Georgiev
 */

 #ifndef RTFACT__SCENE_HPP
#define RTFACT__SCENE_HPP

#include <RTfact/Config/Common.hpp>

#include <boost/intrusive_ptr.hpp>

#include <RTfact/Utils/Containers/HashMap.hpp>
#include <RTfact/Utils/ReferenceCounted.hpp>

RTFACT_NAMESPACE_BEGIN

class RTFACT_ALIGN_SIMD Scene :
    public ReferenceCounted
{
public:

    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    struct RenderStruct :
        public ReferenceCounted
    {
        RTFACT_SIMD_ALIGNED_NEW_OPERATORS

        virtual ~RenderStruct()
        {}
    };

protected:

	typedef HashMap<const void*,
                    boost::intrusive_ptr<RenderStruct> > t_RenderStructMap;

	HashMap<const void*,
            boost::intrusive_ptr<RenderStruct> > mRenderStructs;

public:

    virtual ~Scene()
    {}

    bool containsStruct(
        const void* const aOwner) const
    {
        return (mRenderStructs.find(aOwner) != mRenderStructs.end());
    }

    template<
        class tRenderStruct>
    void addStruct(
        const void* const aOwner,
        const boost::intrusive_ptr<tRenderStruct> aStructPointer)
    {
        RTFACT_ASSERT(!containsStruct(aOwner));

        mRenderStructs.insert(t_RenderStructMap::value_type(
            aOwner, boost::static_pointer_cast<RenderStruct>(aStructPointer)));
    }

    template<
        class tRenderStruct>
    boost::intrusive_ptr<tRenderStruct> getStruct(
        const void* const aOwner) const
    {
        RTFACT_ASSERT(containsStruct(aOwner));

        return boost::dynamic_pointer_cast<tRenderStruct>(
            mRenderStructs.find(aOwner)->second);
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__SCENE_HPP
