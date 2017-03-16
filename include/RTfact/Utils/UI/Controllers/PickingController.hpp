/* 
 *  PickingController.hpp
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
 *  Created on: 2010-02-22 11:22:10 +0100
 *  Author(s): Vaclav Krajicek, Lukas Marsalek, Petr Kmoch
 */
 
 #ifndef RTFACT__PICKINGCONTROLLER_HPP
#define RTFACT__PICKINGCONTROLLER_HPP

#include <RTfact/Utils/UI/Controller.hpp>
#include <RTfact/Utils/UI/Controllers/CameraController.hpp>

RTFACT_NAMESPACE_BEGIN

namespace UI {

class PickingController :
    public CameraController
{
protected:

    int mX;
    int mY;
    bool mShiftDown;
    int mPickX;
    int mPickY;
    bool mPicked;

    static void printHelp()
    {
        RTFACT_LOG_LINE("");
        RTFACT_LOG_LINE("> Left mouse button grabs the mouse (required for mouse navigation)");
        RTFACT_LOG_LINE("> W/S/A/D keys move forward/backward/left/right");
        RTFACT_LOG_LINE("> Q/Z keys move up/down");
        RTFACT_LOG_LINE("> R key resets camera");
        RTFACT_LOG_LINE("> C key prints basic camera parameters");
        RTFACT_LOG_LINE("> [ and ] keys decrease and increase the move step");
        RTFACT_LOG_LINE("> CTRL + mouse wheel increases/decreases camera opening angle");
        RTFACT_LOG_LINE("> Hold the right mouse button to pan the camera");
        RTFACT_LOG_LINE("> ALT + left click demonstrates picking. A box is added to the position of the picked object.");
    }

    void handleKey(
        const Event& aEvent)
    {
        const bool keyDown = (aEvent.type == Event::KeyDown);

        switch(aEvent.key)
        {
            case Key::LeftShift:
                mShiftDown = keyDown;
        default:
            break;
        }

        CameraController::handleKey(aEvent);
    }

    void handleMouseButtonDown(
        const Event& aEvent)
    {
        if((aEvent.mouseButton == MouseButton::Left) && mShiftDown && !mMouseCaptured)
        {
            mPickX = mX;
            mPickY = mY;
            mPicked = true;
        }
        else
        {
            CameraController::handleMouseButtonDown(aEvent);
        }
    }

    void handleMouseMove(
        const Event& aEvent)
    {
        mX = aEvent.mousePosition.x;
        mY = aEvent.mousePosition.y;
        CameraController::handleMouseMove(aEvent);
    }

public:

    typedef RTfact::PixelSampler::Sample<1> tScreenSample;

    bool pick(tScreenSample& pickCoords)
    {
        if(mPicked)
        {
          mPicked = false;
          pickCoords.imageX = float(mPickX);
          pickCoords.imageY = float(mParams.resY - mPickY);
          return true;
        }
        return false;
    }

    PickingController(
        Window& aWindow,
        const BBox& aSceneBounds
    ) :
        CameraController(aWindow, aSceneBounds),
        mShiftDown(false),
        mPicked(false)
    {}

};

} // namespace UI

RTFACT_NAMESPACE_END

#endif // RTFACT__PICKINGCONTROLLER_HPP
