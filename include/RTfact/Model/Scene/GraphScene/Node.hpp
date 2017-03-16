/* 
 *  Node.hpp
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
 *  Created on: 2009-02-24 16:01:56 +0100
 *  Author(s): Felix Klein
 */
 
 #ifndef _RTFACT_GRAPHSCENE_NODE_HPP_INCLUDED
#define _RTFACT_GRAPHSCENE_NODE_HPP_INCLUDED

RTFACT_NAMESPACE_BEGIN

template<class tParent, class tSceneConfig>
class Node : public ReferenceCounted
{
public:
    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    //-------------------------------------------------
    // Typedefs
    //-------------------------------------------------

    typedef tParent*                            t_ParentPtr;
    typedef Vector<t_ParentPtr>                 t_ParentVector;
    typedef typename tSceneConfig::Scene        t_Scene;
    typedef typename tSceneConfig::Scene*       t_ScenePtr;

    //-------------------------------------------------
    // Fields
    //-------------------------------------------------

    t_ParentVector  mParents;
    t_ScenePtr      mScene;
    


    //-------------------------------------------------
    // Functions
    //-------------------------------------------------

    Node(t_ScenePtr s) : mScene(s) {}

    void addParent(t_ParentPtr aParent)
    {
        mParents.push_back(aParent);
    }

    void removeParent(t_ParentPtr aParent)
    {
        for(typename t_ParentVector::iterator it = mParents.begin(); it < mParents.end(); it++)
            if(*it == aParent)
            {
                mParents.erase(it);
                return;
            }
    }


    
};

RTFACT_NAMESPACE_END

#endif
