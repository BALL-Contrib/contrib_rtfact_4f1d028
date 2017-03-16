/* 
 *  HandledScene.hpp
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
 *  Created on: 2008-06-20 17:30:16 +0200
 *  Author(s): Felix Klein, Iliyan Georgiev
 */
 
 #ifndef RTFACT__HANDLEDSCENE_HPP
#define RTFACT__HANDLEDSCENE_HPP

#include <RTfact/Concept/Scene/ShadingInfoScene.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tAppearanceConfig, 
    class tLightConfig,
    class tEnvironmentTexture,
    umask taProperties = 0>
class HandledScene : 
    public ShadingInfoScene<
        tAppearanceConfig, tLightConfig, tEnvironmentTexture, taProperties>
{
public:

    typedef int         HandleIndex;
    typedef Vector<int> HandleIndexVec;

    struct Handle
    {
        enum
        {
            EMPTY = -1
        };

        int type;
        int index;
    };

protected:

    typedef Vector<Handle> t_handleVector;

    mutable t_handleVector handles;

    // Functions for Handles:


    HandleIndex createHandle(const int aType, const int aIndex) const
    {
        for(uint i=0; i<handles.size(); ++i)
        {
            if(handles[i].type == aType && handles[i].index == aIndex)
            {
                return i;
            }
        }

        const size_t index = handles.size();

        handles.resize(index + 1);

        handles[index].type = aType;
        handles[index].index = aIndex;

        return index;
    }

    Handle& getHandle(const HandleIndex aIndex)
    {
        return handles[aIndex];
    }

    const Handle& getHandle(const HandleIndex aIndex) const
    {
        return handles[aIndex];
    }

    void releaseHandle(const Handle& aHandle) const
    {
        for(typename t_handleVector::iterator it = handles.begin();
            it != handles.end(); ++it)
        {
            if(it->type == aHandle.type && it->index == aHandle.index)
            {
                it->type = Handle::EMPTY;
            }
        }
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__HANDLEDSCENE_HPP
