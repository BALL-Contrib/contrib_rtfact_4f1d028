/* 
 *  GraphStructSynchronizer.hpp
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
 *  Author(s): Felix Klein, Petr Kmoch
 */
 
 #ifndef _RTFACT_GRAPHSTRUCTSYNCHRONIZER_HPP_INCLUDED
#define _RTFACT_GRAPHSTRUCTSYNCHRONIZER_HPP_INCLUDED


#include <RTfact/Concept/Builder.hpp>

RTFACT_NAMESPACE_BEGIN

template <class tInnerStructBuilder>
class GraphStructSynchronizer : public Builder
{
public:
    typedef tInnerStructBuilder t_innerBuilder;

    template <class tScene, class tBuildData>
    void build(tScene &aScene, tBuildData& aBuildData)
    {
        typedef tScene                                      t_Scene;
        typedef typename t_Scene::t_Primitive               t_Primitive;
        typedef typename t_Scene::t_MeshMap                 t_MeshMap;
        typedef typename t_Scene::t_Mesh                    t_Mesh;

        typedef tBuildData                                  t_BuildData;
        typedef typename t_BuildData::t_InnerStruct         t_InnerStruct;
        typedef typename t_BuildData::t_StructVector        t_StructVector;

        t_StructVector &structures = aBuildData.mInnerStructs;
        t_MeshMap      &meshes     = aScene.mMeshes;

        t_innerBuilder  builder;

#ifndef RTFACT_USE_PRIMITIVE_INTMAP
        // Find Base Pointer
		t_Primitive    *base	   =  0;
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
#endif

        for(typename t_MeshMap::key_iterator it = meshes.keysBegin();
            it != meshes.keysEnd(); it++)
        {
          /* t_Mesh* mesh = */ meshes[*it];
        }
        typename t_MeshMap::key_vector eraseMeshes;

        for(typename t_MeshMap::key_iterator it = meshes.keysBegin();
            it != meshes.keysEnd(); it++)
        {
            t_Mesh* mesh = meshes[*it];
            if(mesh == NULL){
                if(structures.size() > typename t_StructVector::size_type(*it) && structures[*it] != NULL){
                    // Delete structure that is not used by scene anymore
                    delete structures[*it];
                    structures[*it] = NULL;

                }
                // we MUST NOT erase content while traversing IntMap
                // thus we just store all meshes to be erased
                eraseMeshes.push_back(*it);
            }
            else
            {
                // Expand Vector to contain the key.
                if(structures.size() <= typename t_StructVector::size_type(*it))
                {
                    size_t oldSize = structures.size();
                    structures.resize(*it+1);
                    for(size_t i = oldSize; i < typename t_StructVector::size_type(*it+1); i++)
                        structures[i]=NULL;
                }


                if( (structures[*it] == NULL || mesh->mNeedBuildUpdate)
                    && mesh->getPrimitiveCount() > 0 )
                {
                    // Update Acceleration Structure.
                    if(structures[*it] == NULL)
                        structures[*it] = new t_InnerStruct();

#ifndef RTFACT_USE_PRIMITIVE_INTMAP
                    builder.build(*structures[*it], *base,
						mesh->mPrimitives.begin(),
                        mesh->mPrimitives.end(),
                        boost::lambda::bind(
                            &t_Primitive::getBounds,
                            boost::lambda::_1));
#else
                    builder.build(*structures[*it],
						mesh->mKeys->begin(),
                        mesh->mKeys->end(),
                        aScene.mPrimitivePool,
                        boost::lambda::bind(
                            &t_Primitive::getBounds,
                            boost::lambda::_1));

#endif

                    mesh->mNeedBuildUpdate = false;
                }
#ifdef RTFACT_USE_PRIMITIVE_INTMAP
                else if(structures[*it] != NULL)
                {
                  structures[*it]->setElementBase(aScene.mPrimitivePool.getElementBase());
                }
#endif

            }
        }
        meshes.erase(eraseMeshes.begin(), eraseMeshes.end());
    }

};


RTFACT_NAMESPACE_END

#endif // _RTFACT_GRAPHSTRUCTSYNCHRONIZER_HPP_INCLUDED
