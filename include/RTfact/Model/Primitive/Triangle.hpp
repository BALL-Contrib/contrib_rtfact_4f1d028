/*
 *  Triangle.hpp
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
 *  Author(s): Iliyan Georgiev, Felix Klein
 */

 #ifndef RTFACT__TRIANGLE_HPP
#define RTFACT__TRIANGLE_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/BBox.hpp>
#include <RTfact/Utils/Packets/Vec3f.hpp>

#include <RTfact/Concept/Primitive.hpp>


// #define RTFACT_TRIANGLE_ALIGN_HACK

#ifndef RTFACT_TRIANGLE_ALIGN_HACK
#define RTFACT_TRIANGLE_ALIGN
#else
#define RTFACT_TRIANGLE_ALIGN RTFACT_ALIGN(256)
#endif

RTFACT_NAMESPACE_BEGIN

struct RTFACT_TRIANGLE_ALIGN Triangle :
    public Primitive
{
    typedef Triangle PrimitiveBase;

#ifndef RTFACT_TRIANGLE_ALIGN_HACK
    RTFACT_SIMD_ALIGNED_NEW_OPERATORS
#else
    RTFACT_ALIGNED_NEW_OPERATORS(256)
#endif

    typedef std::pair<float, float> TextCoordPair;

    Vec3f1        vertices[3];
    Vec3f1        normals[3];
    Vec3f1        normal;
    Vec3f1        vertexColors[3];
    Vec3f1        tangent;
    Vec3f1        binormal;
    TextCoordPair texCoords[3];
    uint          appearanceID;

    Triangle()
    {}

    Triangle(
        const Vec3f1& aVertex1,
        const Vec3f1& aVertex2,
        const Vec3f1& aVertex3)
    {
        vertices[0] = aVertex1;
        vertices[1] = aVertex2;
        vertices[2] = aVertex3;

        vertexColors[0] = Vec3f1(1, 0, 0);
        vertexColors[1] = Vec3f1(0, 1, 0);
        vertexColors[2] = Vec3f1(0, 0, 1);

        updateNormal();

        normals[0] = normal;
        normals[1] = normal;
        normals[2] = normal;
    }

    void updateNormal()
    {
        normal = Vec3f1::cross(vertices[2] - vertices[0],
                               vertices[1] - vertices[0]).normalize();
    }

    const Vec3f1& getNormal() const
    {
        return normal;
    }

    const Vec3f1& getNormal(
        const uint aIndex) const
    {
        RTFACT_ASSERT(aIndex < 3);

        return normals[aIndex];
    }

    Vec3f1& getNormal(
        const uint aIndex)
    {
        RTFACT_ASSERT(aIndex < 3);

        return normals[aIndex];
    }

    const Vec3f1& getVertex(
        const uint aIndex) const
    {
        RTFACT_ASSERT(aIndex < 3);

        return vertices[aIndex];
    }

    Vec3f1& getVertex(
        const uint aIndex)
    {
        RTFACT_ASSERT(aIndex < 3);

        return vertices[aIndex];
    }

    const Vec3f1& getVertexColor(
        const uint aIndex) const
    {
        RTFACT_ASSERT(aIndex < 3);

        return vertexColors[aIndex];
    }

    Vec3f1& getVertexColor(
        const uint aIndex)
    {
        RTFACT_ASSERT(aIndex < 3);

        return vertexColors[aIndex];
    }

    const TextCoordPair& getTexCoords(
        const uint aIndex) const
    {
        RTFACT_ASSERT(aIndex < 3);

        return texCoords[aIndex];
    }

    TextCoordPair& getTexCoords(
        const uint aIndex)
    {
        RTFACT_ASSERT(aIndex < 3);

        return texCoords[aIndex];
    }

    int getAppearanceID() const
    {
        return appearanceID;
    }

    const BBox getBounds() const
    {
        return BBox(
            Vec3f1::min(vertices[0], Vec3f1::min(vertices[1], vertices[2])),
            Vec3f1::max(vertices[0], Vec3f1::max(vertices[1], vertices[2])));
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__TRIANGLE_HPP
