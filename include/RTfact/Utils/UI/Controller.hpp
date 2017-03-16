/* 
 *  Controller.hpp
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
 
 #ifndef RTFACT__CONTROLLER_HPP
#define RTFACT__CONTROLLER_HPP

#include <RTfact/Config/Common.hpp>

#include <boost/bind.hpp>

#include <RTfact/Utils/ReferenceCounted.hpp>
#include <RTfact/Utils/UI/Event.hpp>
#include <RTfact/Utils/UI/Window.hpp>

RTFACT_NAMESPACE_BEGIN

namespace UI {

class Controller :
    public ReferenceCounted
{
protected:

    virtual ~Controller()
    {}

    virtual void handleMouseMove(
        const Event&)
    {}

    virtual void handleMouseButton(
        const Event&)
    {}

    virtual void handleMouseButtonDown(
        const Event&)
    {}

    virtual void handleMouseButtonUp(
        const Event&)
    {}

    virtual void handleKey(
        const Event&)
    {}

    virtual void handleKeyDown(
        const Event&)
    {}

    virtual void handleKeyUp(
        const Event&)
    {}

    virtual void handleEvent(
        const Event& aEvent)
    {
        switch(aEvent.type)
        {
            case Event::MouseMove:
                handleMouseMove(aEvent);
                break;
            case Event::MouseButtonDown:
                handleMouseButton(aEvent);
                handleMouseButtonDown(aEvent);
                break;
            case Event::MouseButtonUp:
                handleMouseButton(aEvent);
                handleMouseButtonUp(aEvent);
                break;
            case Event::KeyDown:
                handleKey(aEvent);
                handleKeyDown(aEvent);
                break;
            case Event::KeyUp:
                handleKey(aEvent);
                handleKeyUp(aEvent);
                break;
            default:
                break;
        }
    }

public:

    Controller(
        Window& aWindow)
    {
        aWindow.registerEventHandler(
            boost::bind(&Controller::handleEvent, this, _1));
    }

    virtual void applyChanges()
    {}

    virtual void acceptChanges()
    {}
};

} // namespace UI

RTFACT_NAMESPACE_END

#endif // RTFACT__CONTROLLER_HPP
