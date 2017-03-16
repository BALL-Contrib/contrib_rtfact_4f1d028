/*
 *  SDLWindow.hpp
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
 *  Created on: 2008-11-06 02:30:21 +0100
 *  Author(s): Iliyan Georgiev, Dmitri Rubinstein, Lukas Marsalek
 */

#ifndef RTFACT__SDLWINDOW_HPP
#define RTFACT__SDLWINDOW_HPP

#include <RTfact/Config/Common.hpp>

#include <cstdlib>
#include <string>
#include <SDL.h>

#include <RTfact/Utils/Math/MathUtils.hpp>
#include <RTfact/Utils/UI/Window.hpp>

RTFACT_NAMESPACE_BEGIN

namespace UI {

class SDLWindow :
    public Window
{
    enum
    {
        COLOR_DEPTH = 32
    };

    int mMouseX;
    int mMouseY;

    bool mUpdateMouseCoords;
    bool mInitialized;

    typedef SDLWindow * SDLWindowRawPtr;

    static void atExitHandler()
    {
        const SDLWindowRawPtr &instance = getInstancePtr();
        if (instance != 0)
        {
            // When we are here than this is actually a bug, because
            // cleanup should be called from SDLWindow destructor !
            RTFACT_LOG_ERROR("SDLWindow destructor was not called properly !");
            instance->cleanup();
        }
    }

    /** Returns a reference to a raw pointer to a single instance of SDLWindow
     *  This is by design a raw pointer and not an intrusive counted.
     *  Because we allow only one instance of SDLWindow (or from SDLWindow
     *  inherited class) to exist, instance will be always either 0 or
     *  point to the single existing instance of the SDLWindow.
     *  SDLWindow destructor will prevent having a dangling pointer to the
     *  instance by assigning null to the reference to it.
     */
    static SDLWindowRawPtr &getInstancePtr()
    {
        static SDLWindowRawPtr instance = 0;
        static bool atExitHandlerInstalled = false;

        if (!atExitHandlerInstalled)
        {
            atexit(atExitHandler);
            atExitHandlerInstalled = true;
        }

        return instance;
    }

    void cleanup()
    {
        SDL_Quit();
    }

protected:

    uint mResX;
    uint mResY;

    int mMouseGrabX;
    int mMouseGrabY;

public:

    SDLWindow() :
        mUpdateMouseCoords(true),
        mMouseGrabX(0),
        mMouseGrabY(0)
    {
        SDLWindowRawPtr &instance = getInstancePtr();
        RTFACT_PROGRAM_ERROR_IF(instance != 0,
            "You can't have more than one SDLWindow instance at the same time");

        instance = this;
    }

    ~SDLWindow()
    {
        SDLWindowRawPtr &instance = getInstancePtr();
        instance = 0; // prevent to call cleanup() twice from atExitHandler()
        cleanup();
    }

protected:

    SDL_Surface* init(
        const uint aResX,
        const uint aResY,
        const uint aFlags)
    {
        mResX = aResX;
        mResY = aResY;

        // init SDL's video subsystem
        RTFACT_PROGRAM_ERROR_IF_F(SDL_Init(SDL_INIT_VIDEO) < 0,
            "Cannot initialize SDL video mode: %1%",
            SDL_GetError());

        return SDL_SetVideoMode(mResX, mResY, COLOR_DEPTH, aFlags);
    }

public:

    void setCaption(
        const char* aCaption)
    {
        SDL_WM_SetCaption(aCaption, "");
    }

    bool getNextEvent(Event& oEvent)
    {
        SDL_Event event;

        if(!SDL_PollEvent(&event))
        {
            return false;
        }

        switch(event.type)
        {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
            {
                oEvent.type = (event.type == SDL_KEYDOWN) ?
                    Event::KeyDown :
                    Event::KeyUp;

                #define RTFACT__CASE_KEY(aSDLType, aKey)                       \
                    case aSDLType:                                             \
                        oEvent.key = Key::aKey;                                \
                        break;

                #define RTFACT__CASE_CHAR(aSDLType, aChar)                     \
                    case aSDLType:                                             \
                        oEvent.key = Key::Char;                                \
                        oEvent.character = aChar;                              \
                        break;

                switch(event.key.keysym.sym)
                {
                    RTFACT__CASE_KEY(SDLK_BACKSPACE, BackSpace)
                    RTFACT__CASE_KEY(SDLK_TAB,       Tab)
                    RTFACT__CASE_KEY(SDLK_RETURN,    Return)
                    RTFACT__CASE_KEY(SDLK_PAUSE,     Pause)
                    RTFACT__CASE_KEY(SDLK_ESCAPE,    Escape)
                    RTFACT__CASE_KEY(SDLK_SPACE,     Space)
                    RTFACT__CASE_KEY(SDLK_INSERT,    Insert)
                    RTFACT__CASE_KEY(SDLK_DELETE,    Delete)
                    RTFACT__CASE_KEY(SDLK_HOME,      Home)
                    RTFACT__CASE_KEY(SDLK_END,       End)
                    RTFACT__CASE_KEY(SDLK_PAGEUP,    PageUp)
                    RTFACT__CASE_KEY(SDLK_PAGEDOWN,  PageDown)
                    RTFACT__CASE_KEY(SDLK_NUMLOCK,   NumLock)
                    RTFACT__CASE_KEY(SDLK_CAPSLOCK,  CapsLock)
                    RTFACT__CASE_KEY(SDLK_SCROLLOCK, ScrollLock)
                    RTFACT__CASE_KEY(SDLK_LCTRL,     LeftCtrl)
                    RTFACT__CASE_KEY(SDLK_RCTRL,     RightCtrl)
                    RTFACT__CASE_KEY(SDLK_LALT,      LeftAlt)
                    RTFACT__CASE_KEY(SDLK_RALT,      RightAlt)
                    RTFACT__CASE_KEY(SDLK_LSHIFT,    LeftShift)
                    RTFACT__CASE_KEY(SDLK_RSHIFT,    RightShift)
                    RTFACT__CASE_KEY(SDLK_LSUPER,    LeftWinKey)
                    RTFACT__CASE_KEY(SDLK_RSUPER,    RightWinKey)
                    RTFACT__CASE_KEY(SDLK_MENU,      Menu)
                    RTFACT__CASE_KEY(SDLK_KP0,       KeyPad0)
                    RTFACT__CASE_KEY(SDLK_KP1,       KeyPad1)
                    RTFACT__CASE_KEY(SDLK_KP2,       KeyPad2)
                    RTFACT__CASE_KEY(SDLK_KP3,       KeyPad3)
                    RTFACT__CASE_KEY(SDLK_KP4,       KeyPad4)
                    RTFACT__CASE_KEY(SDLK_KP5,       KeyPad5)
                    RTFACT__CASE_KEY(SDLK_KP6,       KeyPad6)
                    RTFACT__CASE_KEY(SDLK_KP7,       KeyPad7)
                    RTFACT__CASE_KEY(SDLK_KP8,       KeyPad8)
                    RTFACT__CASE_KEY(SDLK_KP9,       KeyPad9)
                    RTFACT__CASE_KEY(SDLK_F1,        F1)
                    RTFACT__CASE_KEY(SDLK_F2,        F2)
                    RTFACT__CASE_KEY(SDLK_F3,        F3)
                    RTFACT__CASE_KEY(SDLK_F4,        F4)
                    RTFACT__CASE_KEY(SDLK_F5,        F5)
                    RTFACT__CASE_KEY(SDLK_F6,        F6)
                    RTFACT__CASE_KEY(SDLK_F7,        F7)
                    RTFACT__CASE_KEY(SDLK_F8,        F8)
                    RTFACT__CASE_KEY(SDLK_F9,        F9)
                    RTFACT__CASE_KEY(SDLK_F10,       F10)
                    RTFACT__CASE_KEY(SDLK_F11,       F11)
                    RTFACT__CASE_KEY(SDLK_F12,       F12)
                    RTFACT__CASE_KEY(SDLK_LEFT,      Left)
                    RTFACT__CASE_KEY(SDLK_RIGHT,     Right)
                    RTFACT__CASE_KEY(SDLK_UP,        Up)
                    RTFACT__CASE_KEY(SDLK_DOWN,      Down)

                    RTFACT__CASE_CHAR(SDLK_a,            'a')
                    RTFACT__CASE_CHAR(SDLK_b,            'b')
                    RTFACT__CASE_CHAR(SDLK_c,            'c')
                    RTFACT__CASE_CHAR(SDLK_d,            'd')
                    RTFACT__CASE_CHAR(SDLK_e,            'e')
                    RTFACT__CASE_CHAR(SDLK_f,            'f')
                    RTFACT__CASE_CHAR(SDLK_g,            'g')
                    RTFACT__CASE_CHAR(SDLK_h,            'h')
                    RTFACT__CASE_CHAR(SDLK_i,            'i')
                    RTFACT__CASE_CHAR(SDLK_j,            'j')
                    RTFACT__CASE_CHAR(SDLK_k,            'k')
                    RTFACT__CASE_CHAR(SDLK_l,            'l')
                    RTFACT__CASE_CHAR(SDLK_m,            'm')
                    RTFACT__CASE_CHAR(SDLK_n,            'n')
                    RTFACT__CASE_CHAR(SDLK_o,            'o')
                    RTFACT__CASE_CHAR(SDLK_p,            'p')
                    RTFACT__CASE_CHAR(SDLK_q,            'q')
                    RTFACT__CASE_CHAR(SDLK_r,            'r')
                    RTFACT__CASE_CHAR(SDLK_s,            's')
                    RTFACT__CASE_CHAR(SDLK_t,            't')
                    RTFACT__CASE_CHAR(SDLK_u,            'u')
                    RTFACT__CASE_CHAR(SDLK_v,            'v')
                    RTFACT__CASE_CHAR(SDLK_w,            'w')
                    RTFACT__CASE_CHAR(SDLK_x,            'x')
                    RTFACT__CASE_CHAR(SDLK_y,            'y')
                    RTFACT__CASE_CHAR(SDLK_z,            'z')

                    RTFACT__CASE_CHAR(SDLK_BACKQUOTE,    '`')
                    RTFACT__CASE_CHAR(SDLK_0,            '0')
                    RTFACT__CASE_CHAR(SDLK_1,            '1')
                    RTFACT__CASE_CHAR(SDLK_2,            '2')
                    RTFACT__CASE_CHAR(SDLK_3,            '3')
                    RTFACT__CASE_CHAR(SDLK_4,            '4')
                    RTFACT__CASE_CHAR(SDLK_5,            '5')
                    RTFACT__CASE_CHAR(SDLK_6,            '6')
                    RTFACT__CASE_CHAR(SDLK_7,            '7')
                    RTFACT__CASE_CHAR(SDLK_8,            '8')
                    RTFACT__CASE_CHAR(SDLK_9,            '9')

                    RTFACT__CASE_CHAR(SDLK_MINUS,        '-')
                    RTFACT__CASE_CHAR(SDLK_EQUALS,       '=')
                    RTFACT__CASE_CHAR(SDLK_LEFTBRACKET,  '[')
                    RTFACT__CASE_CHAR(SDLK_RIGHTBRACKET, ']')
                    RTFACT__CASE_CHAR(SDLK_SEMICOLON,    ';')
                    RTFACT__CASE_CHAR(SDLK_QUOTE,        '\'')
                    RTFACT__CASE_CHAR(SDLK_BACKSLASH,    '\\')
                    RTFACT__CASE_CHAR(SDLK_PERIOD,       '.')
                    RTFACT__CASE_CHAR(SDLK_COMMA,        ',')
                    RTFACT__CASE_CHAR(SDLK_SLASH,        '/')

                    RTFACT__CASE_KEY(SDLK_KP_PERIOD,   KeyPadPeriod)
                    RTFACT__CASE_KEY(SDLK_KP_DIVIDE,   KeyPadDivide)
                    RTFACT__CASE_KEY(SDLK_KP_MULTIPLY, KeyPadMultiply)
                    RTFACT__CASE_KEY(SDLK_KP_MINUS,    KeyPadMinus)
                    RTFACT__CASE_KEY(SDLK_KP_PLUS,     KeyPadPlus)
                    RTFACT__CASE_KEY(SDLK_KP_ENTER,    KeyPadEnter)
                    default: break;
                }

                #undef  RTFACT__CASE_KEY
                #undef  RTFACT__CASE_CHAR

                #define RTFACT__IF_MOD(aSDLType, aModifier)                    \
                    if(event.key.keysym.mod & aSDLType)                        \
                    {                                                          \
                        oEvent.keyModifier = static_cast<KeyModifier::Type>    \
                            (oEvent.keyModifier | KeyModifier::aModifier);     \
                    }

                #define RTFACT__IF_MOD_EX(aSDLType, aModifier, aCommonModifier)\
                    if(event.key.keysym.mod & aSDLType)                        \
                    {                                                          \
                        oEvent.keyModifier = static_cast<KeyModifier::Type>(   \
                            oEvent.keyModifier |                               \
                            KeyModifier::aModifier |                           \
                            KeyModifier::aCommonModifier);                     \
                    }

                oEvent.keyModifier = KeyModifier::None;

                RTFACT__IF_MOD   (KMOD_NUM,    NumLock)
                RTFACT__IF_MOD   (KMOD_CAPS,   CapsLock)
                RTFACT__IF_MOD_EX(KMOD_LCTRL,  LeftCtrl,   Ctrl)
                RTFACT__IF_MOD_EX(KMOD_RCTRL,  RightCtrl,  Ctrl)
                RTFACT__IF_MOD_EX(KMOD_LALT,   LeftAlt,    Alt)
                RTFACT__IF_MOD_EX(KMOD_RALT,   RightAlt,   Alt)
                RTFACT__IF_MOD_EX(KMOD_LSHIFT, LeftShift,  Shift)
                RTFACT__IF_MOD_EX(KMOD_RSHIFT, RightShift, Shift)

                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            {
                oEvent.type = (event.type == SDL_MOUSEBUTTONDOWN) ?
                    Event::MouseButtonDown :
                    Event::MouseButtonUp;

                switch(event.button.button)
                {
                    case SDL_BUTTON_LEFT:
                        oEvent.mouseButton = MouseButton::Left;
                        break;
                    case SDL_BUTTON_MIDDLE:
                        oEvent.mouseButton = MouseButton::Middle;
                        break;
                    case SDL_BUTTON_RIGHT:
                        oEvent.mouseButton = MouseButton::Right;
                        break;
                    case SDL_BUTTON_WHEELUP:
                        oEvent.mouseButton = MouseButton::WheelUp;
                        break;
                    case SDL_BUTTON_WHEELDOWN:
                        oEvent.mouseButton = MouseButton::WheelDown;
                        break;
                }

                break;
            }
            case SDL_MOUSEMOTION:
            {
                if(mUpdateMouseCoords)
                {
                    mMouseX = event.motion.x;
                    mMouseY = event.motion.y;

                    mUpdateMouseCoords = false;
                }
                else
                {
                    mMouseX += event.motion.xrel;
                    mMouseY += event.motion.yrel;
                }

                oEvent.type = Event::MouseMove;
                oEvent.mousePosition.x = mMouseX;
                oEvent.mousePosition.y = mMouseY;
                oEvent.mouseMove.x = event.motion.xrel;
                oEvent.mouseMove.y = event.motion.yrel;

                break;
            }
            case SDL_VIDEORESIZE:
            {
                SDL_ResizeEvent* resizeEvent =
                    reinterpret_cast<SDL_ResizeEvent*>(&event);
                oEvent.type = Event::Resize;
                oEvent.resolution.x = resizeEvent->w;
                oEvent.resolution.y = resizeEvent->h;
                break;
            }
            case SDL_QUIT:
            {
                oEvent.type = Event::Quit;
                break;
            }
            default:
            {
                // in the case we don't support SDL event set type to NoEvent
                // (for events like SDL_VIDEOEXPOSE or SDL_ACTIVEEVENT)
                oEvent.type = Event::NoEvent;
            }
        }

        return true;

        #undef RTFACT__CASE_KEY
        #undef RTFACT__CASE_CHAR
        #undef RTFACT__IF_MOD
        #undef RTFACT__IF_MOD_EX
    }

    void grabMouse()
    {
        SDL_ShowCursor(SDL_DISABLE);
        SDL_WM_GrabInput(SDL_GRAB_ON);
        mMouseGrabX = mMouseX;
        mMouseGrabY = mMouseY;
        mUpdateMouseCoords = true;
    }

    void releaseMouse()
    {
        SDL_WM_GrabInput(SDL_GRAB_OFF);
        SDL_WarpMouse(mMouseGrabX, mMouseGrabY);
        SDL_ShowCursor(SDL_ENABLE);

        mUpdateMouseCoords = true;
    }

    uint getResX() const { return mResX; }

    uint getResY() const { return mResY; }

    #undef RTFACT__ASSERT_INITIALIZED
};

} // namespace UI

RTFACT_NAMESPACE_END

#endif // RTFACT__SDLWINDOW_HPP
