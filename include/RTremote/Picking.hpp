/* 
 *  Picking.hpp
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
 
 #ifndef RTFACT_REMOTE__PICKING_HPP
#define RTFACT_REMOTE__PICKING_HPP

#include <RTremote/Renderer.hpp>

RTFACT_REMOTE_NAMESPACE_BEGIN

/**
 *  The Picking Class provides interface for picking an object in
 *  renderer window. The result of picking is a path of groups,
 *  geometry and primitive index uniquely identifying visible object.
 */
class RTFACT_REMOTE_API Picking
{
protected:
    Renderer* mRenderer;

public:

    typedef std::vector<GroupHandle> tGroupHandleVector;

    /**
     * Result object containing handles identifying visible
     * instance of the object.
     */
    class Result
    {
    public:
        tGroupHandleVector mGroupsPath;
        GeoHandle mGeometry;
        size_t mPrimitiveNo;

    };

    /**
     * Input screen coordinate of the point to pick.
     */
    class ScreenCoordinates
    {
    public:
        float mPickX;
        float mPickY;

        ScreenCoordinates()
        {}
        ScreenCoordinates(float pickX, float pickY)
          : mPickX(pickX), mPickY(pickY) {
        }
    };

    /**
     * Constructor requiring renderer object.
     * @param renderer Renderer containing scene for picking.
     */
    Picking(Renderer* renderer);

    /**
     * Picking result world transformation.
     * @param result Instance of picking result.
     * @param transform Instance of picking result.
     */
    void getResultTransformation(Result& result, Transform& transform);

    /**
     * Using this method an object on the screen can be picked.
     * @param input Integer point on the screen through which to pick.
     * A ray is constructed through this screen point based on current camera settings in the renderer.
     * @param oResult Result of scene picking.
     * @return True if some object was hit, false otherwise
     */
    bool pick(const ScreenCoordinates &input, Result& oResult);

};

RTFACT_REMOTE_NAMESPACE_END

#endif // RTFACT_REMOTE__PICKING_HPP
