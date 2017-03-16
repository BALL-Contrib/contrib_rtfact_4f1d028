/* 
 *  Transform.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2008  Saarland University
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
 *  Created on: 2008-11-02 21:38:37 +0100
 *  Author(s): Iliyan Georgiev, Felix Klein
 */
 
 #ifndef RTFACT__TRANSFORM_HPP
#define RTFACT__TRANSFORM_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Packets/Vec3f.hpp>
#include <RTfact/Utils/Matrix4f.hpp>
#include <RTfact/Utils/BBox.hpp>

#define RTFACT__TRANSFORM_INLINE RTFACT_MATH_INLINE

RTFACT_NAMESPACE_BEGIN

class Transform
{
public:

    Matrix4f matrix, invMatrix;

    RTFACT__TRANSFORM_INLINE Transform()
    {}

    RTFACT__TRANSFORM_INLINE Transform(
        const float a11, const float a12, const float a13, const float a14,
        const float a21, const float a22, const float a23, const float a24,
        const float a31, const float a32, const float a33, const float a34,
        const float a41, const float a42, const float a43, const float a44
    ) :
        matrix(a11, a12, a13, a14,
                a21, a22, a23, a24,
                a31, a32, a33, a34,
                a41, a42, a43, a44),
        invMatrix(matrix.inverse())
    {}

    RTFACT__TRANSFORM_INLINE Transform(
        const Matrix4f& aMat
    ) :
        matrix(aMat),
        invMatrix(matrix.inverse())
    {}

    RTFACT__TRANSFORM_INLINE Transform(
        const Matrix4f& aMat,
        const Matrix4f& aInvMat
    ) :
        matrix(aMat),
        invMatrix(aInvMat)
    {}

    RTFACT__TRANSFORM_INLINE const Transform operator *(
        const Transform& aTransform) const
    {
        Transform ret;

        ret.matrix = matrix * aTransform.matrix;
        ret.invMatrix = aTransform.invMatrix * invMatrix;

        return ret;
    }

    RTFACT__TRANSFORM_INLINE Transform& operator *=(
        const Transform& aTransform)
    {
        matrix *= aTransform.matrix;
        invMatrix = aTransform.invMatrix * invMatrix;

        return *this;
    }

    RTFACT__TRANSFORM_INLINE const Transform inverse() const
    {
        return Transform(invMatrix, matrix);
    }

    RTFACT__TRANSFORM_INLINE const BBox apply(
        const BBox& aBox) const
    {
        const Vec3f<1> v1(aBox.max.xf - aBox.min.xf, 0.f, 0.f);
        const Vec3f<1> v2(0.f, aBox.max.yf - aBox.min.yf, 0.f);
        const Vec3f<1> v3(0.f, 0.f, aBox.max.zf - aBox.min.zf);

        const Vec3f<1> v1Trans = applyToVector(v1);
        const Vec3f<1> v2Trans = applyToVector(v2);
        const Vec3f<1> v3Trans = applyToVector(v3);

        BBox ret = BBox::empty();

        Vec3f<1> pointTrans = applyToPoint(aBox.min);
        ret.extend(pointTrans);

        pointTrans += v1Trans;
        ret.extend(pointTrans);

        pointTrans += v2Trans;
        ret.extend(pointTrans);

        pointTrans -= v1Trans;
        ret.extend(pointTrans);

        pointTrans += v3Trans;
        ret.extend(pointTrans);

        pointTrans += v1Trans;
        ret.extend(pointTrans);

        pointTrans -= v2Trans;
        ret.extend(pointTrans);

        pointTrans -= v1Trans;
        ret.extend(pointTrans);

        return ret;
    }

    RTFACT__TRANSFORM_INLINE const BBox applyInverse(
        const BBox& aBox) const
    {
        const Vec3f<1> v1(aBox.max.xf - aBox.min.xf, 0.f, 0.f);
        const Vec3f<1> v2(0.f, aBox.max.yf - aBox.min.yf, 0.f);
        const Vec3f<1> v3(0.f, 0.f, aBox.max.zf - aBox.min.zf);

        const Vec3f<1> v1Trans = applyInverseToVector(v1);
        const Vec3f<1> v2Trans = applyInverseToVector(v2);
        const Vec3f<1> v3Trans = applyInverseToVector(v3);

        BBox ret = BBox::empty();

        Vec3f<1> pointTrans = applyInverseToPoint(aBox.min);
        ret.extend(pointTrans);

        pointTrans += v1Trans;
        ret.extend(pointTrans);

        pointTrans += v2Trans;
        ret.extend(pointTrans);

        pointTrans -= v1Trans;
        ret.extend(pointTrans);

        pointTrans += v3Trans;
        ret.extend(pointTrans);

        pointTrans += v1Trans;
        ret.extend(pointTrans);

        pointTrans -= v2Trans;
        ret.extend(pointTrans);

        pointTrans -= v1Trans;
        ret.extend(pointTrans);

        return ret;
    }

    RTFACT__TRANSFORM_INLINE const BBox applyAffine(
        const BBox& aBox) const
    {
        const Vec3f<1> v1(aBox.max.xf - aBox.min.xf, 0.f, 0.f);
        const Vec3f<1> v2(0.f, aBox.max.yf - aBox.min.yf, 0.f);
        const Vec3f<1> v3(0.f, 0.f, aBox.max.zf - aBox.min.zf);

        const Vec3f<1> v1Trans = applyToVectorAffine(v1);
        const Vec3f<1> v2Trans = applyToVectorAffine(v2);
        const Vec3f<1> v3Trans = applyToVectorAffine(v3);

        BBox ret = BBox::empty();

        Vec3f<1> pointTrans = applyToPointAffine(aBox.min);
        ret.extend(pointTrans);

        pointTrans += v1Trans;
        ret.extend(pointTrans);

        pointTrans += v2Trans;
        ret.extend(pointTrans);

        pointTrans -= v1Trans;
        ret.extend(pointTrans);

        pointTrans += v3Trans;
        ret.extend(pointTrans);

        pointTrans += v1Trans;
        ret.extend(pointTrans);

        pointTrans -= v2Trans;
        ret.extend(pointTrans);

        pointTrans -= v1Trans;
        ret.extend(pointTrans);
        
        return ret;
    }

    RTFACT__TRANSFORM_INLINE const BBox applyInverseAffine(
        const BBox& aBox) const
    {
        const Vec3f<1> v1(aBox.max.xf - aBox.min.xf, 0.f, 0.f);
        const Vec3f<1> v2(0.f, aBox.max.yf - aBox.min.yf, 0.f);
        const Vec3f<1> v3(0.f, 0.f, aBox.max.zf - aBox.min.zf);

        const Vec3f<1> v1Trans = applyInverseToVectorAffine(v1);
        const Vec3f<1> v2Trans = applyInverseToVectorAffine(v2);
        const Vec3f<1> v3Trans = applyInverseToVectorAffine(v3);

        BBox ret = BBox::empty();

        Vec3f<1> pointTrans = applyInverseToPointAffine(aBox.min);
        ret.extend(pointTrans);

        pointTrans += v1Trans;
        ret.extend(pointTrans);

        pointTrans += v2Trans;
        ret.extend(pointTrans);

        pointTrans -= v1Trans;
        ret.extend(pointTrans);

        pointTrans += v3Trans;
        ret.extend(pointTrans);

        pointTrans += v1Trans;
        ret.extend(pointTrans);

        pointTrans -= v2Trans;
        ret.extend(pointTrans);

        pointTrans -= v1Trans;
        ret.extend(pointTrans);
        
        return ret;
    }

    RTFACT__TRANSFORM_INLINE const BBox applySafe(
        const BBox& aBox) const
    {
        return aBox.isEmpty() ?
            aBox :
            apply(aBox);
    }

    RTFACT__TRANSFORM_INLINE const BBox applyInverseSafe(
        const BBox& aBox) const
    {
        return aBox.isEmpty() ?
            aBox :
            applyInverse(aBox);
    }

    RTFACT__TRANSFORM_INLINE const BBox applySafeAffine(
        const BBox& aBox) const
    {
        return aBox.isEmpty() ?
            aBox :
            applyAffine(aBox);
    }

    RTFACT__TRANSFORM_INLINE const BBox applyInverseSafeAffine(
        const BBox& aBox) const
    {
        return aBox.isEmpty() ?
            aBox :
            applyInverseAffine(aBox);
    }

    template<
        uint taSize>
    RTFACT__TRANSFORM_INLINE const Vec3f<taSize> applyToPoint(
        const Vec3f<taSize>& aPoint) const
    {
        return matrix.multiplyPoint(aPoint);
    }

    template<
        uint taSize>
    RTFACT__TRANSFORM_INLINE const Vec3f<taSize> applyToPointAffine(
        const Vec3f<taSize>& aPoint) const
    {
        return matrix.multiplyPointAffine(aPoint);
    }

    template<
        uint taSize>
    RTFACT__TRANSFORM_INLINE const Vec3f<taSize> applyInverseToPoint(
        const Vec3f<taSize>& aPoint) const
    {
        return invMatrix.multiplyPoint(aPoint);
    }

    template<
        uint taSize>
    RTFACT__TRANSFORM_INLINE const Vec3f<taSize> applyInverseToPointAffine(
        const Vec3f<taSize>& aPoint) const
    {
        return invMatrix.multiplyPointAffine(aPoint);
    }

    template<
        uint taSize>
    RTFACT__TRANSFORM_INLINE const Vec3f<taSize> applyToVector(
        const Vec3f<taSize>& aVec) const
    {
        return matrix.multiplyVector(aVec);
    }

    template<
        uint taSize>
    RTFACT__TRANSFORM_INLINE const Vec3f<taSize> applyToVectorAffine(
        const Vec3f<taSize>& aVec) const
    {
        return matrix.multiplyVectorAffine(aVec);
    }

    template<
        uint taSize>
    RTFACT__TRANSFORM_INLINE const Vec3f<taSize> applyInverseToVector(
        const Vec3f<taSize>& aVec) const
    {
        return invMatrix.multiplyVector(aVec);
    }

    template<
        uint taSize>
    RTFACT__TRANSFORM_INLINE const Vec3f<taSize> applyInverseToVectorAffine(
        const Vec3f<taSize>& aVec) const
    {
        return invMatrix.multiplyVectorAffine(aVec);
    }

    template<
        uint taSize>
    RTFACT__TRANSFORM_INLINE const Vec3f<taSize> applyToNormal(
        const Vec3f<taSize>& aNormal) const
    {
        return invMatrix.multiplyVectorTransposed(aNormal);
    }

    template<
        uint taSize>
    RTFACT__TRANSFORM_INLINE const Vec3f<taSize> applyToNormalAffine(
        const Vec3f<taSize>& aNormal) const
    {
        return invMatrix.multiplyVectorTransposedAffine(aNormal);
    }

    template<
        uint taSize>
    RTFACT__TRANSFORM_INLINE const Vec3f<taSize> applyInverseToNormal(
        const Vec3f<taSize>& aNormal) const
    {
        return matrix.multiplyVectorTransposed(aNormal);
    }

    template<
        uint taSize>
    RTFACT__TRANSFORM_INLINE const Vec3f<taSize> applyInverseToNormalAffine(
        const Vec3f<taSize>& aNormal) const
    {
        return matrix.multiplyVectorTransposedAffine(aNormal);
    }

    template<
        uint taSize>
    RTFACT__TRANSFORM_INLINE const Vec3f<taSize> transformPoint(
        const Vec3f<taSize>& aPoint) const
    {
        return invMatrix.multiplyPoint(aPoint);
    }

    template<
        uint taSize>
    RTFACT__TRANSFORM_INLINE const Vec3f<taSize> transformPointAffine(
        const Vec3f<taSize>& aPoint) const
    {
        return invMatrix.multiplyPointAffine(aPoint);
    }

    template<
        uint taSize>
    RTFACT__TRANSFORM_INLINE const Vec3f<taSize> transformInversePoint(
        const Vec3f<taSize>& aPoint) const
    {
        return matrix.multiplyPoint(aPoint);
    }

    template<
        uint taSize>
    RTFACT__TRANSFORM_INLINE const Vec3f<taSize> transformInversePointAffine(
        const Vec3f<taSize>& aPoint) const
    {
        return matrix.multiplyPointAffine(aPoint);
    }

    template<
        uint taSize>
    RTFACT__TRANSFORM_INLINE const Vec3f<taSize> transformVector(
        const Vec3f<taSize>& aVec) const
    {
        return invMatrix.multiplyVector(aVec);
    }

    template<
        uint taSize>
    RTFACT__TRANSFORM_INLINE const Vec3f<taSize> transformVectorAffine(
        const Vec3f<taSize>& aVec) const
    {
        return invMatrix.multiplyVectorAffine(aVec);
    }

    template<
        uint taSize>
    RTFACT__TRANSFORM_INLINE const Vec3f<taSize> transformInverseVector(
        const Vec3f<taSize>& aVec) const
    {
        return matrix.multiplyVector(aVec);
    }

    template<
        uint taSize>
    RTFACT__TRANSFORM_INLINE const Vec3f<taSize> transformInverseVectorAffine(
        const Vec3f<taSize>& aVec) const
    {
        return matrix.multiplyVectorAffine(aVec);
    }

    template<
        uint taSize>
    RTFACT__TRANSFORM_INLINE const Vec3f<taSize> transformNormal(
        const Vec3f<taSize>& aNormal) const
    {
        return matrix.multiplyVectorTransposed(aNormal);
    }

    template<
        uint taSize>
    RTFACT__TRANSFORM_INLINE const Vec3f<taSize> transformNormalAffine(
        const Vec3f<taSize>& aNormal) const
    {
        return matrix.multiplyVectorTransposed(aNormal);
    }

    template<
        uint taSize>
    RTFACT__TRANSFORM_INLINE const Vec3f<taSize> transformInverseNormal(
        const Vec3f<taSize>& aNormal) const
    {
        return invMatrix.multiplyVectorTransposed(aNormal);
    }

    template<
        uint taSize>
    RTFACT__TRANSFORM_INLINE const Vec3f<taSize> transformInverseNormalAffine(
        const Vec3f<taSize>& aNormal) const
    {
        return invMatrix.multiplyVectorTransposed(aNormal);
    }

    static RTFACT__TRANSFORM_INLINE const Transform identity()
    {
        static const Transform id(Matrix4f::identity(), Matrix4f::identity());

        return id;
    }

    static RTFACT__TRANSFORM_INLINE const Transform translation(
        const float aX,
        const float aY,
        const float aZ)
    {
        return Transform(
            Matrix4f(float4::C_1_0_0_0,
                     float4::C_0_1_0_0,
                     float4::C_0_0_1_0,
                     float4::getInstance(aX, aY, aZ, 1.f)),
            Matrix4f(float4::C_1_0_0_0,
                     float4::C_0_1_0_0,
                     float4::C_0_0_1_0,
                     float4::getInstance(-aX, -aY, -aZ, 1.f)));
    }

    static RTFACT__TRANSFORM_INLINE const Transform translation(
        const Vec3f<1>& aVec)
    {
        return translation(aVec.xf, aVec.yf, aVec.zf);
    }

    static RTFACT__TRANSFORM_INLINE const Transform scale(
        const Vec3f<1>& aVec)
    {
        const float4 reciprocalVec = aVec.data.reciprocal();

        return Transform(
            Matrix4f(float4::getInstance(aVec.xf, 0.f, 0.f, 0.f),
                     float4::getInstance(0.f, aVec.yf, 0.f, 0.f),
                     float4::getInstance(0.f, 0.f, aVec.zf, 0.f),
                     float4::C_0_0_0_1),
            Matrix4f(float4::getInstance(reciprocalVec[0], 0.f, 0.f, 0.f),
                     float4::getInstance(0.f, reciprocalVec[1], 0.f, 0.f),
                     float4::getInstance(0.f, 0.f, reciprocalVec[2], 0.f),
                     float4::C_0_0_0_1));
    }

    static RTFACT_FORCE_INLINE const Transform rotationX(
        const float aAngleInRad)
    {
        return rotationX(Math::sin(aAngleInRad), Math::cos(aAngleInRad));
    }

    static RTFACT__TRANSFORM_INLINE const Transform rotationX(
        const float aAngleSine, const float aAngleCosine)
    {
        return Transform(
            Matrix4f(float4::C_1_0_0_0,
                     float4::getInstance(0.f, aAngleCosine, aAngleSine, 0.f),
                     float4::getInstance(0.f, -aAngleSine, aAngleCosine, 0.f),
                     float4::C_0_0_0_1),
            Matrix4f(float4::C_1_0_0_0,
                     float4::getInstance(0.f, aAngleCosine, -aAngleSine, 0.f),
                     float4::getInstance(0.f, aAngleSine, aAngleCosine, 0.f),
                     float4::C_0_0_0_1));
    }

    static RTFACT_FORCE_INLINE const Transform rotationY(
        const float aAngleInRad)
    {
        return rotationY(Math::sin(aAngleInRad), Math::cos(aAngleInRad));
    }

    static RTFACT__TRANSFORM_INLINE const Transform rotationY(
        const float aAngleSine,
        const float aAngleCosine)
    {
        return Transform(
            Matrix4f(float4::getInstance(aAngleCosine, 0.f, -aAngleSine, 0.f),
                     float4::C_0_1_0_0,
                     float4::getInstance(aAngleSine, 0.f, aAngleCosine, 0.f),
                     float4::C_0_0_0_1),
            Matrix4f(float4::getInstance(aAngleCosine, 0.f, aAngleSine, 0.f),
                     float4::C_0_1_0_0,
                     float4::getInstance(-aAngleSine, 0.f, aAngleCosine, 0.f),
                     float4::C_0_0_0_1));
    }

    static RTFACT_FORCE_INLINE const Transform rotationZ(
        const float aAngleInRad)
    {
        return rotationZ(Math::sin(aAngleInRad), Math::cos(aAngleInRad));
    }

    static RTFACT__TRANSFORM_INLINE const Transform rotationZ(
        const float aAngleSine,
        const float aAngleCosine)
    {
        return Transform(
            Matrix4f(float4::getInstance(aAngleCosine, aAngleSine, 0.f, 0.f),
                     float4::getInstance(-aAngleSine, aAngleCosine, 0.f, 0.f),
                     float4::C_0_0_1_0,
                     float4::C_0_0_0_1),
            Matrix4f(float4::getInstance(aAngleCosine, -aAngleSine, 0.f, 0.f),
                     float4::getInstance(aAngleSine, aAngleCosine, 0.f, 0.f),
                     float4::C_0_0_1_0,
                     float4::C_0_0_0_1));
    }

    static RTFACT_FORCE_INLINE const Transform rotation(
        const Vec3f<1>& aAxisNormalized,
        const float aAngleInRad)
    {
        return rotation(aAxisNormalized, Math::sin(aAngleInRad),
                                         Math::cos(aAngleInRad));
    }

    static RTFACT__TRANSFORM_INLINE const Transform rotation(
        const Vec3f<1>& aAxisNormalized,
        const float aAngleSine,
        const float aAngleCosine)
    {
        const float cos1MinusA = 1.f - aAngleCosine;
        const float4 axisTimesCos1 = float4::getInstance(cos1MinusA, cos1MinusA,
                                                         cos1MinusA, cos1MinusA)
                                     * aAxisNormalized.data;

        Matrix4f matrix;

        matrix.columns[0] = aAxisNormalized.data.shuffle<0, 0, 0, 0>()
                            * axisTimesCos1;
        matrix.columns[0] += float4::getInstance(
                                 aAngleCosine,
                                 aAxisNormalized.z * aAngleSine,
                                -aAxisNormalized.y * aAngleSine,
                                -aAxisNormalized.x);

        matrix.columns[1] = aAxisNormalized.data.shuffle<1, 1, 1, 1>() *
                            axisTimesCos1;
        matrix.columns[1] += float4::getInstance(
                                -aAxisNormalized.z * aAngleSine,
                                 aAngleCosine,
                                 aAxisNormalized.x * aAngleSine,
                                -aAxisNormalized.x);

        matrix.columns[2] = aAxisNormalized.data.shuffle<2, 2, 2, 2>() *
                            axisTimesCos1;
        matrix.columns[2] += float4::getInstance(
                                 aAxisNormalized.y * aAngleSine,
                                -aAxisNormalized.x * aAngleSine,
                                 aAngleCosine,
                                -aAxisNormalized.z);

        matrix.columns[3] = float4::C_0_0_0_1;

        /*
            element[0][0] = a.x * a.x + (1.f - a.x * a.x) * cos_a;
            element[1][0] = a.x * a.y * (1.f - c) + a.z * s;
            element[2][0] = a.x * a.z * (1.f - c) - a.y * s;
            element[3][0] = 0;

            element[0][1] = a.x * a.y * (1.f - c) - a.z * s;
            element[1][1] = a.y * a.y + (1.f - a.y * a.y) * c;
            element[2][1] = a.y * a.z * (1.f - c) + a.x * s;
            element[3][1] = 0;

            element[0][2] = a.x * a.z * (1.f - c) + a.y * s;
            element[1][2] = a.y * a.z * (1.f - c) - a.x * s;
            element[2][2] = a.z * a.z + (1.f - a.z * a.z) * c;
            element[3][2] = 0;

            element[0][3] = 0;        
            element[1][3] = 0;
            element[2][3] = 0;
            element[3][3] = 1;
        */

        return Transform(matrix, matrix.transpose());
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__TRANSFORM_HPP
