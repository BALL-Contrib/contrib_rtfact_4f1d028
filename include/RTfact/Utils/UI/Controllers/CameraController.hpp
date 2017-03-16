/* 
 *  CameraController.hpp
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
 *  Author(s): Iliyan Georgiev, Lukas Marsalek, Felix Klein, Vaclav Krajicek
 */
 
 #ifndef RTFACT__CAMERACONTROLLER_HPP
#define RTFACT__CAMERACONTROLLER_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Math/MathUtils.hpp>
#include <RTfact/Utils/IO/Configuration.hpp>

#include <RTfact/Concept/Camera.hpp>
#include <RTfact/Concept/Intersector.hpp>
#include <RTfact/Utils/RayPacket.hpp>

#include <RTfact/Utils/UI/Controller.hpp>

RTFACT_NAMESPACE_BEGIN

namespace UI {

#define RTFACT__MOVE_STEP                 0.1f
#define RTFACT__MOVE_STEP_INCREASE_FACTOR 2.f
#define RTFACT__MOVE_STEP_MIN             0.005f
#define RTFACT__MOVE_STEP_MAX             100000000.f

#define RTFACT__PAN_FACTOR                0.1f
#define RTFACT__ROTATE_FACTOR             0.003f
#define RTFACT__OPENING_ANGLE_STEP        3.f
#define RTFACT__UP_ORIENTATION_ANGLE_STEP 0.01f

#define RTFACT__INCREASE_STEP_SAFE(aStep)                                      \
    std::min(                                                                  \
        aStep * RTFACT__MOVE_STEP_INCREASE_FACTOR,                             \
        RTFACT__MOVE_STEP_MAX)                                                 \

#define RTFACT__DECREASE_STEP_SAFE(aStep)                                      \
    std::max(                                                                  \
        aStep * (1.f / RTFACT__MOVE_STEP_INCREASE_FACTOR),                     \
        RTFACT__MOVE_STEP_MIN)                                                 \

class CameraController :
    public Controller
{
protected:
    Camera::Params mParams;
    Camera::Params mInitialParams;
    Window&        mWindow;

    bool mCameraChanging;

    bool mMoveLeft;
    bool mMoveRight;
    bool mMoveForward;
    bool mMoveBackward;
    bool mMoveUp;
    bool mMoveDown;

    bool mCanMoveLeft;
    bool mCanMoveRight;
    bool mCanMoveForward;
    bool mCanMoveBackward;
    bool mCanMoveUp;
    bool mCanMoveDown;

    bool mUpOrientationChange;
    bool mMousePan;
    bool mCtrlDown;
    bool mMouseCaptured;

    bool mCollisionDetection;

    float mMoveStep;

    float mPanHorizontalAmount;
    float mPanVerticalAmount;

    float mVerticalRotationAngle;
    float mHorizontalRotationAngle;
    float mOpeningAngleChange;
    float mUpOrientationAngleChange;

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
    }

    void resetMouseMoveState()
    {
        mVerticalRotationAngle = mHorizontalRotationAngle =
            mPanHorizontalAmount = mPanVerticalAmount = 
            mOpeningAngleChange = mUpOrientationAngleChange = 0.f;
    }

    static const Vec3f1 rotateVector(
        const Vec3f1& aVec,
        const Vec3f1& aAxis,
        const float aAngle)
    {
        const Vec3f1 c = Math::dot(aVec, aAxis) * aAxis;
        const Vec3f1 u = aVec - c;
        const Vec3f1 v = Math::cross(aAxis, u);

        return c + u * Math::cos(aAngle) + v * Math::sin(aAngle);
    }

    void moveCamera(
        const Vec3f1& aAxis,
        const float aAmount)
    {
        /*RayPacket<1> collisionRay;
        collisionRay.org = mParams.position;
        collisionRay.tMin = RTFACT_RAY_EPSILON;
        collisionRay.tMax = RTFACT_INFINITY;

        typename tIntersector::template Result<1> intersection;
        ActiveMask<1> mask;
        mask.setTrueAndUpdateActiveContainers();

        if(aAmount > 0.f)
        {
            collisionRay.dir = aAxis;
            collisionRay.invDir = collisionRay.dir.reciprocal();

            Intersector::intersect<IsectData::DISTANCE, true>(
                mIntersector, collisionRay, mask, mPrimitive, intersection);

            if(intersection.dist > aAmount)*/
            {
                mParams.position += aAmount * aAxis;
            }
            /*}*/
    }

public:

    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    CameraController(
        Window& aWindow,
        const BBox& aSceneBounds
    ) :
        Controller(aWindow),
        mWindow(aWindow),
        mCameraChanging(false),
        mMoveLeft(false),
        mMoveRight(false),
        mMoveForward(false),
        mMoveBackward(false),
        mMoveUp(false),
        mMoveDown(false),
        mUpOrientationChange(false),
        mMousePan(false),
        mCtrlDown(false),
        mMouseCaptured(false),
        mCollisionDetection(false),
        mMoveStep(aSceneBounds.getExtent().max() / 100.f)
    {
        resetMouseMoveState();
    }

    bool cameraChanging() const
    {
        return mCameraChanging;
    }

    void setParams(
        const Camera::Params& aParams)
    {
        mParams = aParams;
        mInitialParams = aParams;
    }

    void getParams(
        Camera::Params& oParams) const
    {
        oParams = mParams;
    }

    void applyChanges()
    {
        Vec3f<1> cameraLeft = Math::cross(mParams.up, mParams.direction);

        //checkForCollisions();

        /*bool canMoveLeft = true;
             canMoveRight, canMoveForward, canMoveBackward*/

        mCameraChanging |=
            mMoveLeft | mMoveRight | mMoveForward | mMoveBackward |
            mMoveUp | mMoveDown |
            (mHorizontalRotationAngle != 0.f) |
            (mVerticalRotationAngle != 0.f) |
            (mOpeningAngleChange != 0.f) |
            (mUpOrientationAngleChange != 0.f) |
            (mPanHorizontalAmount != 0.f) |
            (mPanVerticalAmount != 0.f);

        if(mCameraChanging)
        {
            float moveLeftAmount     = mMoveLeft     ? mMoveStep : 0.f;
            float moveRightAmount    = mMoveRight    ? mMoveStep : 0.f;
            float moveForwardAmount  = mMoveForward  ? mMoveStep : 0.f;
            float moveBackwardAmount = mMoveBackward ? mMoveStep : 0.f;
            float moveUpAmount       = mMoveUp       ? mMoveStep : 0.f;
            float moveDownAmount     = mMoveDown     ? mMoveStep : 0.f;

            if(mPanHorizontalAmount > 0.f)
            {
                moveLeftAmount += mPanHorizontalAmount * mMoveStep;
            }
            else if(mPanHorizontalAmount < 0.f)
            {
                moveRightAmount += -mPanHorizontalAmount * mMoveStep;
            }

            if(mPanVerticalAmount > 0.f)
            {
                moveUpAmount += mPanVerticalAmount * mMoveStep;
            }
            else if(mPanVerticalAmount < 0.f)
            {
                moveDownAmount += -mPanVerticalAmount * mMoveStep;
            }

            moveCamera(cameraLeft, moveLeftAmount);
            moveCamera(-cameraLeft, moveRightAmount);
            moveCamera(mParams.direction, moveForwardAmount);
            moveCamera(-mParams.direction, moveBackwardAmount);
            moveCamera(mParams.up, moveUpAmount);
            moveCamera(-mParams.up, moveDownAmount);

            // rotate direction vector vertically
            const Vec3f1 directionRotatedUp =
                (Math::cos(mVerticalRotationAngle) * mParams.direction -
                 Math::sin(mVerticalRotationAngle) * mParams.up).normalize();

            // rotate up vector vertically
            const Vec3f1 upRotatedUp =
                 Math::cross(directionRotatedUp,
                             Math::cross(mParams.up, directionRotatedUp)).normalize();

            const Vec3f1 finalDirection = rotateVector(
                directionRotatedUp, mInitialParams.up, -mHorizontalRotationAngle);

            const Vec3f1 finalUp = rotateVector(
                upRotatedUp, mInitialParams.up, -mHorizontalRotationAngle);

            const Vec3f1 finalUp2 = rotateVector(
                finalUp, finalDirection, mUpOrientationAngleChange);

            mParams.direction = finalDirection;
            mParams.up = finalUp2;

            mParams.openingAngle += mOpeningAngleChange;
        }
    }

    void acceptChanges()
    {
        mCameraChanging = false;

        resetMouseMoveState();
    }

protected:

    void handleKey(
        const Event& aEvent)
    {
        const bool keyDown = (aEvent.type == Event::KeyDown);

        if(aEvent.key == Key::Char)
        {
            switch(aEvent.character)
            {
            case 'a':
                mMoveLeft = keyDown;
                break;
            case 'd':
                mMoveRight = keyDown;
                break;
            case 'w':
                mMoveForward = keyDown;
                break;
            case 's':
                mMoveBackward = keyDown;
                break;
            case 'q':
                mMoveUp = keyDown;
                break;
            case 'z':
                mMoveDown = keyDown;
                break;
            case 'v':
                mCollisionDetection ^= true;
                break;
            case 'r':
                mParams = mInitialParams;
                mCameraChanging = true;
                break;
            case '[':
                mMoveStep = RTFACT__DECREASE_STEP_SAFE(mMoveStep);
                break;
            case ']':
                mMoveStep = RTFACT__INCREASE_STEP_SAFE(mMoveStep);
                break;
            }
        }
        else
        {
            switch(aEvent.key)
            {
            case Key::LeftCtrl:
            case Key::RightCtrl:
                mCtrlDown = keyDown;
                mMoveStep = mCtrlDown ?
                    RTFACT__DECREASE_STEP_SAFE(mMoveStep) :
                    RTFACT__INCREASE_STEP_SAFE(mMoveStep);
                break;
            case Key::LeftShift:
            case Key::RightShift:
                mMoveStep = keyDown ?
                    RTFACT__INCREASE_STEP_SAFE(mMoveStep) :
                    RTFACT__DECREASE_STEP_SAFE(mMoveStep);
                break;
            default:
                break;
            }
        }
    }

    void handleKeyDown(
        const Event& aEvent)
    {
        if(aEvent.key == Key::Char)
        {
            switch(aEvent.character)
            {
            case 'h':
                printHelp();
                break;
            case 'c':
                RTFACT_LOG_LINE_F(
                    "# Camera parameters\n"
                    "cameraPos=%1%, %2%, %3%\n"
                    "cameraDir=%4%, %5%, %6%\n"
                    "cameraUp=%7%, %8%, %9%\n",
                    mParams.position.xf % mParams.position.yf % mParams.position.zf %
                    mParams.direction.xf % mParams.direction.yf % mParams.direction.zf %
                    mParams.up.xf % mParams.up.yf % mParams.up.zf);
                break;
            }
        }
    }

    void handleMouseButtonDown(
        const Event& aEvent)
    {
        switch(aEvent.mouseButton)
        {
        case MouseButton::Right:
            mMousePan = (!mCtrlDown) && mMouseCaptured;
            break;
        case MouseButton::Left:
            if(mMouseCaptured)
            {
                if(!mCtrlDown)
                {
                    mWindow.releaseMouse();
                    mMouseCaptured ^= true;
                }
            }
            else
            {
                mWindow.grabMouse();
                mMouseCaptured ^= true;
            }
            break;
        case MouseButton::WheelUp:
            if(mCtrlDown)
            {
                mOpeningAngleChange -= RTFACT__OPENING_ANGLE_STEP;
            }
            else
            {
                mMoveStep = RTFACT__INCREASE_STEP_SAFE(mMoveStep);
            }
            break;
        case MouseButton::WheelDown:
            if(mCtrlDown)
            {
                mOpeningAngleChange += RTFACT__OPENING_ANGLE_STEP;
            }
            else
            {
                mMoveStep = RTFACT__DECREASE_STEP_SAFE(mMoveStep);
            }
            break;
        default:
            break;
        }
    }

    void handleMouseButtonUp(
        const Event& aEvent)
    {
        switch(aEvent.mouseButton)
        {
        case MouseButton::Right:
            mMousePan = false;
            break;
        default:
            break;
        }
    }

    void handleMouseMove(
        const Event& aEvent)
    {
        if(mMouseCaptured)
        {
            if(mMousePan)
            {
                mPanHorizontalAmount -= aEvent.mouseMove.x * RTFACT__PAN_FACTOR;
                mPanVerticalAmount -= aEvent.mouseMove.y * RTFACT__PAN_FACTOR;
            }
            else if(mCtrlDown)
            {
                mUpOrientationAngleChange +=
                    aEvent.mouseMove.x * RTFACT__UP_ORIENTATION_ANGLE_STEP;
            }
            else
            {
                mVerticalRotationAngle +=
                    aEvent.mouseMove.y * RTFACT__ROTATE_FACTOR;

                mHorizontalRotationAngle +=
                    aEvent.mouseMove.x * RTFACT__ROTATE_FACTOR;
            }
        }
    }
};

#undef RTFACT__MOVE_STEP
#undef RTFACT__MOVE_STEP_INCREASE_FACTOR
#undef RTFACT__MOVE_STEP_MIN
#undef RTFACT__MOVE_STEP_MAX

#undef RTFACT__PAN_FACTOR
#undef RTFACT__OPENING_ANGLE_STEP
#undef RTFACT__ROTATE_FACTOR

#undef RTFACT__INCREASE_STEP_SAFE
#undef RTFACT__DECREASE_STEP_SAFE

} // namespace UI

RTFACT_NAMESPACE_END

#endif // RTFACT__CAMERACONTROLLER_HPP
