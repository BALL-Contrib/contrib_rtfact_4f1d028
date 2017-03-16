/* 
 *  Window.hpp
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
 *  Author(s): Iliyan Georgiev, Felix Klein
 */
 
 #ifndef RTFACT__WINDOW_HPP
#define RTFACT__WINDOW_HPP

#include <RTfact/Config/Common.hpp>

#include <string>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <RTfact/Utils/ReferenceCounted.hpp>
#include <RTfact/Utils/UI/Event.hpp>

RTFACT_NAMESPACE_BEGIN

namespace UI {

class Window :
    public ReferenceCounted
{
public:

    typedef boost::function<void(const Event&)> EventHandler;

protected:

    typedef std::vector<EventHandler> t_EventHandlerVector;

    t_EventHandlerVector mEventHandlers;

public:

    virtual ~Window()
    {}

    void registerEventHandler(
        const EventHandler aHandler)
    {

        mEventHandlers.push_back(aHandler);
    }

    void handleEvents()
    {
        if(mEventHandlers.size() == 0)
        {
            return;
        }

        for(;;)
        {
            Event event;

            if(!getNextEvent(event))
            {
                break;
            }

            for(t_EventHandlerVector::iterator it = mEventHandlers.begin();
                it != mEventHandlers.end(); it++)
            {
                (*it)(event);
            }
        }
    }

    virtual void grabMouse() = 0;

    virtual void releaseMouse() = 0;

    virtual void setCaption(
        const char*) = 0;

    virtual bool getNextEvent(
        Event& oEvent) = 0;
};

} // namespace UI

//template<>
//struct Traits<UI::Window>
//{
//    typedef boost::shared_ptr<UI::Window> Ptr;
//};

RTFACT_NAMESPACE_END

#endif // RTFACT__WINDOW_HPP
