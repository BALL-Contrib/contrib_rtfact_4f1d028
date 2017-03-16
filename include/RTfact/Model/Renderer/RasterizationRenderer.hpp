/* 
 *  RasterizationRenderer.hpp
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
 *  Created on: 2008-11-13 03:17:48 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__RASTERIZATIONRENDERER_HPP
#define RTFACT__RASTERIZATIONRENDERER_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Renderer.hpp>
#include <RTfact/Utils/Transform.hpp>

RTFACT_NAMESPACE_BEGIN

class RasterizationRenderer :
    public Renderer
{
public:

    template<
        class tCamera,
        class tScene>
    void init(
        tCamera& aCamera,
        tScene& aScene)
    {}

    template<
        class tCamera,
        class tScene,
        class tFramebuffer>
    void render(
        tCamera& aCamera,
        tScene& aScene,
        tFramebuffer& aFramebuffer,
        const ImageClipRegion& aClipRegion)
    {
        // tCamera will typically be RasterizationCamera (look in
        // RasterizationCamera.hpp for the definition of the Params class)
        typename tCamera::Params camParams;
        
        // writes the current camera settings to camParams
        aCamera.getParams(camParams);

        // Use Vec3f1 as a standard 3-component float vector;
        // you can use Math::cross, Math::dot on Vec3f1's and Math::sin,
        // Math::cos, Math::floor, etc. for floats. Look in Vec3f1.hpp,
        // MathUtils.hpp and PacketMathUtils.hpp to see what else is available
        const Vec3f1 camDir = camParams.direction;

        for(uint y = 0; y < camParams.resY; ++y)
        {
            for(uint x = 0; x < camParams.resX; ++x)
            {
                // the framebuffer will write the x, y, and z components of the
                // Vec3f1 as R, G, and B, given the x and y position of the
                // pixel in the image
                aFramebuffer.write(x, y, x + 1, y + 1, camDir);
            }
        }

        Matrix4f tr(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);

        Vec3f1 v(1, 2, 3);

        Vec3f1 vt = tr.multiplyPoint(v);

        // Some examples
        ////////////////////////////////////////////////////////////////////////

        // This is the 4x4 float matrix implementation in RTfact. Note that
        // elements are stored column-wise in memory - the first column, then
        // the second column, etc.
        Matrix4f matrix;

        // Accessing matrix elements. the first parameter is the row, the second
        // is the column
        matrix(2, 3) = 2.f;

        // Vec3f1 describes both points and vectors
        Vec3f1 vec(1.f, 0.f, 0.f);
        Vec3f1 point(1.f, 2.f, 3.f);

        // Points and vectors are multiplied in different ways with matrices.
        // There is usually no need to work with Matrix4f directly. Use Transform
        // instead
        Vec3f1 multVec = matrix.multiplyVector(vec);
        Vec3f1 multPoint = matrix.multiplyPoint(point);

        // Transformations can be created in a number of ways. They always store
        // a regular and an inverse matrix. See Transform.hpp for details
        Transform trans1(matrix);

        // Transform also has static methods for creating common transformations
        Transform trans = Transform::translation(1.f, 0.5f, 0.f);
        
        // This works too. For more, see Transform.hpp
        trans = Transform::translation(point);

        // Applying generic transformations
        Vec3f1 vecTrans = trans.applyToVector(vec);

        // Transformations are applied differently to standard vectors and normals
        vecTrans = trans.applyToNormal(vec);

        // When you know that a transformation is affine, this will be faster
        vecTrans = trans.applyToVectorAffine(vec);

        // Transformations can be also used for converting points and vectors
        // into a the basis defined by them
        Vec3f1 vecNewBasis = trans.transformVectorAffine(vec);

        // Transformations can be multiplied (algebraically consistent - when
        // applied to a vector/point, first trans will be applied and then trans1)
        Transform accum = trans1 * trans;
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__RASTERIZATIONRENDERER_HPP
