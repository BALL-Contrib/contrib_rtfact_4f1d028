/* 
 *  ParticleFilter.hpp
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
 *  Created on: 2010-16-14 18:03 +0200
 *  Author(s): Lukas Marsalek
 */
 
#ifndef RTFACT__PARTICLE_FILTER_HPP
#define RTFACT__PARTICLE_FILTER_HPP

#include <RTfact/Config/Common.hpp>
#include <RTfact/Foundations/Parameterizable.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tpImportonStorage,
    class tpMutex>
class ParticleFilter :
    public Parameterizable
{
protected:
    typedef ParticleFilter<tpImportonStorage, tpMutex> thisClass;
    RTFACT_PARAM_INIT(thisClass);

public:

    typedef tpImportonStorage tImportonStorage;

    ParticleFilter()
    {
        initParams();
    }
    
    class ParticleFilterStatistics
    {
       /**
         * TotalFrameLuminance is set through getter/setter because it needs to be thread-safe
        */
        float mTotalFrameLuminance;

        tpMutex mLuminanceMutex;

    public:
        ParticleFilterStatistics()
        {
            clear();
        }

        RTFACT_PARTICLE_FILTER_INLINE void clear()
        {            
            setTotalFrameLuminance(0.f);
            avgPixelLuminance = 0.0001f;
            desiredAvgContrib = 0.01f;
        }

        RTFACT_PARTICLE_FILTER_INLINE void incrementTotalFrameLuminance(const float aIncrement)
        {
            typename tpMutex::scoped_lock lock(mLuminanceMutex);
            mTotalFrameLuminance += aIncrement;
        }

        RTFACT_PARTICLE_FILTER_INLINE void setTotalFrameLuminance(const float aValue)
        {
            typename tpMutex::scoped_lock lock(mLuminanceMutex);
            mTotalFrameLuminance = aValue;
        }

        RTFACT_PARTICLE_FILTER_INLINE float getTotalFrameLuminance()
        {
            return mTotalFrameLuminance;
        }

        float avgPixelLuminance;
        float desiredAvgContrib;
    };

    ParticleFilterStatistics stats;
    tpImportonStorage importonStorage;

    template<
        class tpCamera,
        class tpContext>
    RTFACT_PARTICLE_FILTER_INLINE void startFrame(
        tpCamera& aCamera,
        tpContext& aContext,
        const uint aIteration);

    template<
        class tpCamera,
        class tpContext>
    RTFACT_PARTICLE_FILTER_INLINE void endFrame(
        tpCamera& aCamera,
        tpContext& aContext,
        const uint aIteration);

    template <
        class tpParticle,
        class tpContext>
    RTFACT_PARTICLE_FILTER_INLINE bool acceptParticle(
        tpParticle& aParticle,
        tpContext& aContext);
};

RTFACT_NAMESPACE_END

#endif // RTFACT__PARTICLE_FILTER_HPP
