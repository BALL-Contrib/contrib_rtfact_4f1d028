/* 
 *  SettingsController.hpp
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
 *  Created on: 2009-05-11 23:49:46 +0200
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__SETTINGSCONTROLLER_HPP
#define RTFACT__SETTINGSCONTROLLER_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/UI/Controller.hpp>
#include <RTfact/Utils/Containers/Vector.hpp>

RTFACT_NAMESPACE_BEGIN

namespace UI {

class SettingsController :
    public Controller
{
protected:

    struct Setting
    {
        Setting() :
            trigger(0),
            value(0),
            defaultValue(0),
            maxValue(0),
            clearEachFrame(false)
        {}

        char trigger;
        uint value;
        uint defaultValue;
        uint maxValue;
        bool clearEachFrame;
    };

    Vector<Setting> mSettings;

    void handleKeyDown(
        const Event& aEvent)
    {
        for(uint i = 0; i < mSettings.size(); ++i)
        {
            if(aEvent.character == mSettings[i].trigger)
            {
                mSettings[i].value =
                    (mSettings[i].value + 1) % mSettings[i].maxValue;

                break;
            }
        }
    }

public:

    SettingsController(
        Window& aWindow
    ) :
        Controller(aWindow)
    {}

    void setSettingCount(
        const uint aCount)
    {
        mSettings.resize(aCount);
    }

    size_t getSettingCount() const
    {
        return mSettings.size();
    }

    void setSettingProperties(
        const uint aSettingID,
        const char aTrigger,
        const uint aSettingCount,
        const uint aDefaultValue = 0,
        const bool aClearEachFrame = false)
    {
        RTFACT_ASSERT(aSettingID < mSettings.size());

        mSettings[aSettingID].value = aDefaultValue;
        mSettings[aSettingID].defaultValue = aDefaultValue;
        mSettings[aSettingID].maxValue = aSettingCount;
        mSettings[aSettingID].clearEachFrame = aClearEachFrame;
        mSettings[aSettingID].trigger = aTrigger;
    }

    uint getSetting(
        const uint aSettingID) const
    {
        RTFACT_ASSERT(aSettingID < mSettings.size());

        return mSettings[aSettingID].value;
    }

    uint setSetting(
        const uint aSettingID,
        const uint aValue)
    {
        RTFACT_ASSERT(aSettingID < mSettings.size());

        return mSettings[aSettingID].value = aValue;
    }

    void acceptChanges()
    {
        for(uint i = 0; i < mSettings.size(); ++i)
        {
            if(mSettings[i].clearEachFrame)
            {
                mSettings[i].value = mSettings[i].defaultValue;
            }
        }
    }
};

} // namespace UI

RTFACT_NAMESPACE_END

#endif // RTFACT__SETTINGSCONTROLLER_HPP
