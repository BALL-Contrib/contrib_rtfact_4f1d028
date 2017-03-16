/* 
 *  Event.hpp
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
 
 #ifndef RTFACT__EVENT_HPP
#define RTFACT__EVENT_HPP

//#include <RTfact/Config/Common.hpp>

RTFACT_NAMESPACE_BEGIN

namespace UI {

struct Point2D
{
    int x;
    int y;
};

struct MouseButton
{
    enum Type
    {
        Left      = 1,
        Middle    = 1 << 1,
        Right     = 1 << 2,
        WheelUp   = 1 << 3,
        WheelDown = 1 << 4
    };
};

struct Key
{
    enum Type
    {
        Char,
        BackSpace,
        Tab,
        Return,
        Pause,
        Escape,
        Space,
        Insert,
        Delete,
        Home,
        End,
        PageUp,
        PageDown,
        NumLock,
        CapsLock,
        ScrollLock,
        LeftCtrl,
        RightCtrl,
        LeftAlt,
        RightAlt,
        LeftShift,
        RightShift,
        LeftWinKey,
        RightWinKey,
        Menu,
        KeyPad0, KeyPad1, KeyPad2, KeyPad3, KeyPad4,
        KeyPad5, KeyPad6, KeyPad7, KeyPad8, KeyPad9,
        KeyPadPeriod, KeyPadDivide, KeyPadMultiply,
        KeyPadMinus, KeyPadPlus, KeyPadEnter,
        F1,  F2,  F3,  F4,  F5,  F6,
        F7,  F8,  F9, F10, F11, F12,
        Up,
        Down,
        Left,
        Right
    };
};

struct KeyModifier
{
    enum Type
    {
        None       = 0,
        NumLock    = 1 << 0,
        CapsLock   = 1 << 1,
        Ctrl       = 1 << 2,
        Alt        = 1 << 3,
        Shift      = 1 << 4,
        LeftCtrl   = 1 << 5,
        RightCtrl  = 1 << 6,
        LeftAlt    = 1 << 7,
        RightAlt   = 1 << 8,
        LeftShift  = 1 << 9,
        RightShift = 1 << 10
    };
};

struct Event 
{
    enum Type
    {
        NoEvent,
        MouseMove,
        MouseButtonDown,
        MouseButtonUp,
        KeyUp,
        KeyDown,
        Resize,
        Quit
    };

    Type              type;
    MouseButton::Type mouseButton;
    Key::Type         key;
    KeyModifier::Type keyModifier;
    char              character;
    Point2D           mousePosition;
    Point2D           mouseMove;
    Point2D           resolution;
};

} // namespace UI

RTFACT_NAMESPACE_END

#endif // RTFACT__EVENT_HPP
