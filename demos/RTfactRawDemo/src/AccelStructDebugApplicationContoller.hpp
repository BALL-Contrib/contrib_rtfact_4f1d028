/* 
 *  AccelStructDebugApplicationContoller.hpp
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
 *  Created on: 2010-08-21 21:49:13 +0200
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACTDEMO__DAPPLICATIONCONTROLLER_HPP
#define RTFACTDEMO__DAPPLICATIONCONTROLLER_HPP

#include <RTfact/Utils/UI/Controllers/ApplicationController.hpp>

using namespace RTfact;

class AccelStructDebugApplicationContoller :
    public UI::ApplicationController
{
    typedef UI::ApplicationController t_Base;

    uint mTreeLevel;

public:

    AccelStructDebugApplicationContoller(
        UI::Window& aWindow,
        uint aSettingCount
    ) :
        t_Base(aWindow, aSettingCount),
        mTreeLevel(1)
    {}

    void handleKeyDown(
        const UI::Event& aEvent)
    {
        t_Base::handleKeyDown(aEvent);

        if(aEvent.key == UI::Key::Char)
        {
            if(aEvent.character == '.')
            {
                ++mTreeLevel;
            }
            else if(aEvent.character == ',')
            {
                --mTreeLevel;
            }
        }
    }

    uint getTreeLevel() const
    {
        return mTreeLevel;
    }
};

#endif // RTFACTDEMO__DAPPLICATIONCONTROLLER_HPP
