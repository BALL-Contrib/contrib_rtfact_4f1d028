/* 
 *  FPSMeter.hpp
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
 
 #ifndef RTFACT__FPSMETER_HPP
#define RTFACT__FPSMETER_HPP

#include <RTfact/Config/Common.hpp>

#include <deque>

#include <RTfact/Utils/System/TimeMeasurer.hpp>

#define RTFACT__DEFAULT_FRAME_AVERAGE_COUNT 4

RTFACT_NAMESPACE_BEGIN

class FPSMeter
{
    std::deque<double> mFps;
    std::deque<double> mWeights;

    TimeMeasurer mMeasurer;

    template<
        class tIterator>
    void init(
        tIterator aWeightStart,
        tIterator aWeightEnd)
    {
        mWeights.resize(aWeightEnd - aWeightStart);
        mFps.resize(aWeightEnd - aWeightStart);

        std::fill(mFps.begin(), mFps.end(), 0.);
        std::copy(aWeightStart, aWeightEnd, mWeights.begin());
    }

public:

    template<
        class tIterator>
    FPSMeter(
        tIterator aWeightStart,
        tIterator aWeightEnd)
    {
        init(aWeightStart, aWeightEnd);
    }

    FPSMeter()
    {
        std::vector<double> w(RTFACT__DEFAULT_FRAME_AVERAGE_COUNT);
        std::fill(w.begin(), w.end(), 1. / RTFACT__DEFAULT_FRAME_AVERAGE_COUNT);
        init(w.begin(), w.end());
    }


    RTFACT_INLINE void startFrame()
    {
        mMeasurer.reset();
        mMeasurer.start();
    }
    RTFACT_INLINE void endFrame()
    {
        mMeasurer.stop();
        mFps.pop_front();
        mFps.push_back(1000 / mMeasurer.getTotalTime());
    }

    double getFPS() const
    {
        double total = 0.;

        for(std::deque<double>::const_iterator itFPS = mFps.begin(),
                                               itWeight = mWeights.begin();
            itFPS != mFps.end();
            itFPS++, itWeight++)
        {
            total += *itFPS * *itWeight;
        }

        return total;
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__FPSMETER_HPP
