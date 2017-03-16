/* 
 *  TimeMeasurer.hpp
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
 *  Created on: 2008-11-02 21:38:37 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__TIME_MEASURER_HPP
#define RTFACT__TIME_MEASURER_HPP

#include <RTfact/Config/Common.hpp>

RTFACT_NAMESPACE_BEGIN

#define RTFACT_TIME_MEASURE_INLINE RTFACT_FORCE_INLINE

class TimeMeasurer
{
    const double mResolution;
    uint64 mStart, mTotal;
    
public:

    TimeMeasurer() :
        mResolution(static_cast<double>(getPerformanceCounterResolution())),
        mStart(0),
        mTotal(0)
    {}

    TimeMeasurer(const uint64 aCPUFrequency) :
        mResolution(static_cast<double>(aCPUFrequency)),
        mStart(0),
        mTotal(0)
    {}

    RTFACT_TIME_MEASURE_INLINE void start()
    {
        mStart = getTimestamp();
    }

    RTFACT_TIME_MEASURE_INLINE void stop()
    {
        uint64 endTime = getTimestamp();

        if(endTime < mStart)
        {
            std::swap(endTime, mStart);
        }
                 
        mTotal += endTime - mStart;
    }

    RTFACT_TIME_MEASURE_INLINE double getTotalTime()
    {
        return (mTotal * 1000) / mResolution;
    }

    RTFACT_TIME_MEASURE_INLINE void reset()
    {
        mStart = mTotal = 0;
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__TIME_MEASURER_HPP
