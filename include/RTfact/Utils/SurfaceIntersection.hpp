/* 
 *  SurfaceIntersection.hpp
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
 *  Created on: 2008-06-09 17:30:26 +0200
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__SURFACEINTERSECTION_HPP
#define RTFACT__SURFACEINTERSECTION_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Packets/Packet.hpp>
#include <RTfact/Utils/Packets/Vec3f.hpp>

#include <RTfact/Utils/Math/CompileTimeMathUtils.hpp>

RTFACT_NAMESPACE_BEGIN

struct IsectData
{
    enum
    {
        NONE              = (1 << 0),
        DISTANCE          = (1 << 1),
        APPEARANCE_ID     = (1 << 2),
        U_V               = (1 << 3),
        GEOM_NORMAL       = (1 << 4),
        INTERPOLATED_DATA = (1 << 5) | U_V,
        NORMAL            = (1 << 6) | INTERPOLATED_DATA | GEOM_NORMAL,
        TEX_COORDS        = (1 << 7) | INTERPOLATED_DATA,
        USER_PARAMS       = (1 << 8) | INTERPOLATED_DATA,
        VERTEX_COLOR      = (1 << 9) | INTERPOLATED_DATA,
        DERIVATIVES       = (1 << 10) | INTERPOLATED_DATA | GEOM_NORMAL | TEX_COORDS,
        TANGENT_SPACE     = (1 << 11)
    };
};

template<
    uint taSize>
struct SurfaceIntersection
{
    enum
    {
        SIZE = taSize,
        SIZE_SQRT = Math::SquareRoot<SIZE>::value
    };

    ActiveMask<taSize>    mask;
    Packet<taSize, float> dist;
    Packet<taSize, int>   appearanceID;
    Vec3f<taSize>         geomNormal;
    Vec3f<taSize>         normal;
    Packet<taSize, float> u;
    Packet<taSize, float> du;
    Packet<taSize, float> v;
    Packet<taSize, float> dv;
    Vec3f<taSize>         point;
    Vec3f<taSize>         dpdu;
    Vec3f<taSize>         dpdv;
    Packet<taSize, float> texCoord1;
    Packet<taSize, float> dt1du;
    Packet<taSize, float> dt1dv;
    Packet<taSize, float> texCoord2;
    Packet<taSize, float> dt2du;
    Packet<taSize, float> dt2dv;
    Vec3f<taSize>         vertexColor;
    Vec3f<taSize>         tangent;
    Vec3f<taSize>         binormal;

    ActiveMask<taSize>    cap;
    Vec3f<taSize>         cutNormal;
    Vec3f<taSize>         capColor;
};

RTFACT_NAMESPACE_END

#endif // RTFACT__SURFACEINTERSECTION_HPP
