/* 
 *  ApplicationController.hpp
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
 *  Created on: 2010-09-15 17:59:35 +0200
 *  Author(s): Felix Klein
 */
 
 #ifndef RTFACT_RTPIE__APPLICATIONCONTROLLER_HPP
#define RTFACT_RTPIE__APPLICATIONCONTROLLER_HPP

#include <RTpie/Config/Common.hpp>

#include <RTfact/Utils/UI/Controllers/ApplicationController.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN

using namespace UI;

class ApplicationController :
    public UI::ApplicationController
{
    typedef UI::ApplicationController t_Base;

    Renderer* renderer;
    int max_expand, filterInstance;

    void handleKeyDown(
        const Event& aEvent)
    {
        t_Base::handleKeyDown(aEvent);

        if(aEvent.key == Key::Char &&
            aEvent.character == '1')
        {
            max_expand*=2;
            renderer->setTestParameter(1, max_expand);
        }
        else if(aEvent.key == Key::Char &&
            aEvent.character == '2')
        {
            if(max_expand>16)
               max_expand*=0.5;
            renderer->setTestParameter(1, max_expand);
        }
        else if(aEvent.key == Key::Char &&
            aEvent.character == '3')
        {
            filterInstance++;
            renderer->setTestParameter(2, filterInstance);
        }
        else if(aEvent.key == Key::Char &&
            aEvent.character == '4')
        {
            if(filterInstance)
                filterInstance--;
            renderer->setTestParameter(2, filterInstance);
        }
        else if(aEvent.key == Key::Space)
        {
            renderer->pauseAnimation(!renderer->animationPaused());
        }
    }

public:

    ApplicationController(
        Window& aWindow,
        Renderer* s
    ) :
        t_Base(aWindow),
        renderer(s),
        max_expand(128),
        filterInstance(0)
    {}
};

RTFACT_RTPIE_NAMESPACE_END

#endif // RTFACT_RTPIE__APPLICATIONCONTROLLER_HPP
