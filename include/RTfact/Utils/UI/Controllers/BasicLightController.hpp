/* 
 *  BasicLightController.hpp
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
 *  Created on: 2008-11-06 02:30:21 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__BASICLIGHTCONTROLLER_HPP
#define RTFACT__BASICLIGHTCONTROLLER_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/IO/Configuration.hpp>
#include <RTfact/Utils/UI/Controller.hpp>

RTFACT_NAMESPACE_BEGIN

namespace UI {

template<
    class tShadingInfoScene>
class BasicLightController :
    public Controller
{
    tShadingInfoScene& mScene;
    uint mMaxStepCount;
    uint mStepsMade;
    uint mAxis;
    float mStep;

public:

    BasicLightController(
        Window& aWindow,
        tShadingInfoScene& aScene,
        const IO::Configuration& aConfig
    ) :
        Controller(aWindow),
        mScene(aScene),
        mMaxStepCount(aConfig["lightMoveStepCount"].as<uint>()),
        mStepsMade(0),
        mAxis(aConfig["lightMoveAxis"].as<uint>()),
        mStep(aConfig["lightMoveStep"].as<float>())
    {}

    void applyChanges()
    {
        if(mStepsMade == mMaxStepCount)
        {
            mStepsMade = 0;
            mStep = -mStep;
        }
        else
        {
            ++mStepsMade;

            PointLight<>* light = static_cast<PointLight<>*>(mScene.lights[0]);

            Vec3f1 lightPos = light->getPosition();

            lightPos.get(mAxis) += mStep;

            light->setPosition(lightPos);
        }
    }
};

} // namespace UI

RTFACT_NAMESPACE_END

#endif // RTFACT__BASICLIGHTCONTROLLER_HPP
