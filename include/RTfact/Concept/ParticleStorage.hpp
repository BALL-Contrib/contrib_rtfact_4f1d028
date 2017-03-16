/* 
 *  ParticleStorage.hpp
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
 *  Author(s): Lukas Marsalek
 */
 
#ifndef RTFACT__PARTICLE_STORAGE_HPP
#define RTFACT__PARTICLE_STORAGE_HPP

#include <RTfact/Config/Common.hpp>
#include <RTfact/Foundations/Parameterizable.hpp>

RTFACT_NAMESPACE_BEGIN

class ParticleStorage :
    public Parameterizable
{
protected:
    RTFACT_PARAM_INIT(ParticleStorage);

public:       

    ParticleStorage()
    {
        initParams();
    }
    
    template <
        class tpParticle>
    RTFACT_PARTICLE_STORAGE_INLINE void insertParticle(
        tpParticle& aParticle);
    
    template <
        class tpParticleQuery>
    RTFACT_PARTICLE_STORAGE_INLINE void initParticleQuery(        
        tpParticleQuery& aoQuery);

    template <
        class tpParticleQuery>
    RTFACT_PARTICLE_STORAGE_INLINE void finalizeParticleQuery(
        tpParticleQuery& aQuery);    

    template <
        class tpParticleQuery,
        class tpParticle>
    RTFACT_PARTICLE_STORAGE_INLINE bool getNextParticle(
        tpParticleQuery& aoQuery,
        tpParticle& oParticle);

    RTFACT_PARTICLE_STORAGE_INLINE void clear();

    RTFACT_PARTICLE_STORAGE_INLINE size_t numParticles();

    RTFACT_PARTICLE_STORAGE_INLINE void reserveSize(uint aLeastSize);
};

RTFACT_NAMESPACE_END

#endif // RTFACT__PARTICLE_STORAGE_HPP
