/*
 *  VectorVPLStorage.hpp
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
 *  Created on: 2010-12-14 17:44 +0200
 *  Author(s): Iliyan Georgiev, Lukas Marsalek
 */

#ifndef RTFACT__VECTOR_VPL_STORAGE_HPP
#define RTFACT__VECTOR_VPL_STORAGE_HPP

#include <RTfact/Config/Common.hpp>
#include <RTfact/Concept/ParticleStorage.hpp>
#include <RTfact/Model/Light/HemiCosineLight.hpp>

RTFACT_NAMESPACE_BEGIN

class VectorVPLStorage :
    public ParticleStorage
{
    RTFACT_PARAM_INIT(VectorVPLStorage);

public:

    //Particle is a VPL, e.g. emitting in hemisphere with cosine power
    typedef HemiCosineLight<QuadraticLightAttenuation>   tParticle;
    //Particle query is represented simply by index into the vector
    typedef uint                                        tParticleQuery;

    VectorVPLStorage()
    {
        initParams();
    }

    VectorVPLStorage(uint aExpectedNumVPLs) :
        mParticleContainer(aExpectedNumVPLs)
    {
        //
    }

    template <
        class tpParticle>
    RTFACT_PARTICLE_STORAGE_INLINE void insertParticle(
        tpParticle& aParticle)
    {
        //FIX: make thread-safe and reentrant
        mParticleContainer.push_back(aParticle);
    }

    template <
        class tpParticleQuery>
    RTFACT_PARTICLE_STORAGE_INLINE void initParticleQuery(
        tpParticleQuery& aoQuery)
    {
        aoQuery = 0;
    }

    template <
        class tpParticleQuery>
    RTFACT_PARTICLE_STORAGE_INLINE void finalizeParticleQuery(
        tpParticleQuery& aQuery)
    {
        aQuery = -1;
    }

    template <
        class tpParticleQuery,
        class tpParticle>
    RTFACT_PARTICLE_STORAGE_INLINE bool getNextParticle(
        tpParticleQuery& aoQuery,
        tpParticle& oParticle)
    {
        if(aoQuery < mParticleContainer.size())
        {
            //FIX: Is it really good idea to *copy* the particle all the time?
            //Storing ptr would be possible only with preallocated memory
            //as newing all the time would be worse than copying
            oParticle = mParticleContainer[aoQuery];
            aoQuery++;
            return true;
        }
        return false;
    }

    template <
        class tpParticleQuery,
        class tpParticle>
    RTFACT_PARTICLE_STORAGE_INLINE bool getNextParticle(
        tpParticleQuery& aoQuery,
        tpParticle * & oParticle)
    {
        if(aoQuery < mParticleContainer.size())
        {
            oParticle = &(mParticleContainer[aoQuery]);
            aoQuery++;
            return true;
        }
        return false;
    }

    RTFACT_PARTICLE_STORAGE_INLINE void clear()
    {
        //FIX: make thread safe and reentrant
        mParticleContainer.clear();
    }

    RTFACT_PARTICLE_STORAGE_INLINE size_t numParticles()
    {
        return mParticleContainer.size();
    }

    RTFACT_PARTICLE_STORAGE_INLINE void reserveSize(uint aLeastSize)
    {
        mParticleContainer.reserve(aLeastSize);
    }

protected:
    typedef Vector<tParticle> tParticleContainer;

    tParticleContainer mParticleContainer;
};

RTFACT_NAMESPACE_END

#endif // RTFACT__VECTOR_VPL_STORAGE_HPP
