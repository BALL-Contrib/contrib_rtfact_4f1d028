/* 
 *  SAHParameters.hpp
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
 *  Created on: 2008-06-09 17:30:26 +0200
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__SAHPARAMETERS_HPP
#define RTFACT__SAHPARAMETERS_HPP

#include <RTfact/Config/Common.hpp>

RTFACT_NAMESPACE_BEGIN

class SAHParameters
{
    uint mMaxUnsuccsessfulSteps;
    float mTerminateQualityXintersectionCost;
    float m_ATCSuccessfulStepMinQualityXintersectionCost;

public:

    float traversalCost, intersectionCost;
    float ATCSuccessfulStepMinQuality;
    float terminateQuality; //=0.99
    float ATCOffset, ATCCoef;
    float ATCDepthOffset, ATCDepthCoef;
    int maxUnsuccsessfulSteps;

    SAHParameters() 
    {
        //traversalCost = 1;
        //intersectionCost = 1.5f;
        //ATCSuccessfulStepMinQuality = 0.75f;
        //ATCDepthOffset = 2;
        //ATCDepthCoef = 1.2f;
        //ATCCoef = 0.2f;
        //ATCOffset = 1.0f;
        //terminateQuality = 0.99f;
        traversalCost = 1.f;
        intersectionCost = 2.f;
        ATCSuccessfulStepMinQuality = 0.85f;
        ATCDepthOffset = 2.f;
        ATCDepthCoef = 0.9f;
        ATCCoef = 0.27f;
        ATCOffset = 0.5f;
        terminateQuality = 0.95f;
        maxUnsuccsessfulSteps = 3;
    };

    void init(
        size_t aInitialObjCnt)
    {
        float dMax = ATCDepthOffset + ATCDepthCoef * (Math::log2((float)aInitialObjCnt));
        mMaxUnsuccsessfulSteps = (uint)(ATCOffset + ATCCoef * dMax);
        mTerminateQualityXintersectionCost = terminateQuality * intersectionCost;
        m_ATCSuccessfulStepMinQualityXintersectionCost = ATCSuccessfulStepMinQuality * intersectionCost;
    }

    RTFACT_FORCE_INLINE bool isStepSuccessfull(
        float aCost, size_t aObjCnt) const
    {   
        return aCost < m_ATCSuccessfulStepMinQualityXintersectionCost * (float)aObjCnt;
    }

    RTFACT_FORCE_INLINE bool shouldTerminate(
        uint,
        uint aUnsuccessfulSteps,
        float,
        size_t aObjCnt) const
    {
        //        return aObjCnt < 16 /*|| aDepth > 24*/;
        return 
            aObjCnt == 0 
            || aUnsuccessfulSteps > mMaxUnsuccsessfulSteps 
            /*|| aCost >= mTerminateQualityXintersectionCost * (float)aObjCnt*/;
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__SAHPARAMETERS_HPP
