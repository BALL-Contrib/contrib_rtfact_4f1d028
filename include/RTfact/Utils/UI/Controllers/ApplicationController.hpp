/* 
 *  ApplicationController.hpp
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
 *  Created on: 2008-11-06 02:30:21 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__APPLICATIONCONTROLLER_HPP
#define RTFACT__APPLICATIONCONTROLLER_HPP

#include <RTfact/Config/Common.hpp>

#include <string>

#include <RTfact/Utils/UI/Controller.hpp>

RTFACT_NAMESPACE_BEGIN

namespace UI {

class ApplicationController :
    public Controller
{
protected:

    bool mAccumulationEnabled;
    bool mShouldTerminate;
    bool mShouldResize;
    uint mNewResX;
    uint mNewResY;
    uint mSetting;
    uint mSettingCount;
    const Window& mWindow;

    static void printHelp()
    {
        RTFACT_LOG_LINE("");
        RTFACT_LOG_LINE("> CTRL + C and ESC keys terminate the program");
        RTFACT_LOG_LINE("> SPACE key pauses animation and activates antialiasing mode");
    }

    void handleKeyDown(
        const Event& aEvent)
    {
        if(aEvent.key == Key::Char)
        {
            if(aEvent.character == 'c' &&
               aEvent.keyModifier & KeyModifier::Ctrl)
            {
                exit(0);
            }
            else if(aEvent.character == 'h')
            {
                printHelp();
            }
        }
        else if(aEvent.key == Key::Space)
        {
            mSetting = (mSetting + 1) % mSettingCount;
        }
        else if(aEvent.key == Key::Escape)
        {
            mShouldTerminate = true;
        }
    }

    void handleEvent(
        const Event& aEvent)
    {
        if(aEvent.type == Event::Quit)
        {
            mShouldTerminate = true;
        }
        else if(aEvent.type == Event::Resize)
        {
            mShouldResize = true;
            mNewResX = aEvent.resolution.x;
            mNewResY = aEvent.resolution.y;
        }
        else
        {
            Controller::handleEvent(aEvent);
        }
    }

public:

    ApplicationController(
        Window& aWindow,
        uint aSettingCount = 1
    ) :
        Controller(aWindow),
        mAccumulationEnabled(false),
        mShouldTerminate(false),
        mShouldResize(false),
        mSetting(0),
        mSettingCount(aSettingCount),
        mWindow(aWindow)
    {}

    uint getSetting() const
    {
        return mSetting;
    }

    bool shouldTerminate() const
    {
        return mShouldTerminate;
    }

    bool shouldResize(
        uint& oResX,
        uint& oResY)
    {
        if(mShouldResize)
        {
            oResX = mNewResX;
            oResY = mNewResY;
        }

        return mShouldResize;
    }

    void acceptChanges()
    {
        mShouldResize = false;
    }
};

} // namespace UI

RTFACT_NAMESPACE_END

#endif // RTFACT__APPLICATIONCONTROLLER_HPP
