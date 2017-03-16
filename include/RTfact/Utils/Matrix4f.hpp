/* 
 *  Matrix4f.hpp
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
 *  Created on: 2008-11-02 21:38:37 +0100
 *  Author(s): Iliyan Georgiev, Felix Klein, Alexander Loeffler, Georgiev Iliyan
 */
 
 #ifndef RTFACT__MATRIX4X4_HPP
#define RTFACT__MATRIX4X4_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Packets/Vec3f.hpp>

#define RTFACT__MATRIX_INLINE RTFACT_MATH_INLINE

RTFACT_NAMESPACE_BEGIN

#pragma region Constant and utility function declarations

namespace Detail {

    extern const RTFACT_ALIGN(16) int32 _Sign_PNNP[4];

} // namespace Detail

#define RTFACT__Sign_PNNP                                                      \
    (*(float4::Data*) &Detail::_Sign_PNNP)        // + - - +

#define RTFACT__mm_ror_ps(vec, i)                                              \
    (((i) % 4) ?                                                               \
        (_mm_shuffle_ps(vec,vec, _MM_SHUFFLE((unsigned char)(i + 3) % 4,       \
                                             (unsigned char)(i + 2) % 4,       \
                                             (unsigned char)(i + 1) % 4,       \
                                             (unsigned char)(i + 0) % 4))) :   \
        (vec))

#pragma endregion // Constant and utility function declarations

#pragma region Operator overloading macros

#define RTFACT_MATRIX4F_BINARY_OP(aOperator)                                   \
    RTFACT__MATRIX_INLINE const Matrix4f operator aOperator(                   \
        const Matrix4f& aMat) const                                            \
    {                                                                          \
        Matrix4f ret;                                                          \
                                                                               \
        ret.columns[0] = columns[0] aOperator aMat.columns[0];                 \
        ret.columns[1] = columns[1] aOperator aMat.columns[1];                 \
        ret.columns[2] = columns[2] aOperator aMat.columns[2];                 \
        ret.columns[3] = columns[3] aOperator aMat.columns[3];                 \
                                                                               \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    RTFACT__MATRIX_INLINE Matrix4f& operator aOperator##=(                     \
        const Matrix4f& aMat)                                                  \
    {                                                                          \
        columns[0] aOperator##= aMat.columns[0];                               \
        columns[1] aOperator##= aMat.columns[1];                               \
        columns[2] aOperator##= aMat.columns[2];                               \
        columns[3] aOperator##= aMat.columns[3];                               \
                                                                               \
        return *this;                                                          \
    }

#define RTFACT_MATRIX4F_SCALAR_OP(aOperator)                                   \
    RTFACT__MATRIX_INLINE const Matrix4f operator aOperator (                  \
        const float aValue) const                                              \
    {                                                                          \
        const Data tempValue = Data::replicate(aValue);                        \
        Matrix4f ret;                                                          \
                                                                               \
        ret.columns[0] = columns[0] aOperator tempValue;                       \
        ret.columns[1] = columns[1] aOperator tempValue;                       \
        ret.columns[2] = columns[2] aOperator tempValue;                       \
        ret.columns[3] = columns[3] aOperator tempValue;                       \
                                                                               \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    RTFACT__MATRIX_INLINE Matrix4f& operator aOperator##=(                     \
        const float aValue)                                                    \
    {                                                                          \
        const Data tempValue = Data::replicate(aValue);                        \
                                                                               \
        columns[0] aOperator##= tempValue;                                     \
        columns[1] aOperator##= tempValue;                                     \
        columns[2] aOperator##= tempValue;                                     \
        columns[3] aOperator##= tempValue;                                     \
                                                                               \
        return *this;                                                          \
    }

#pragma endregion

class Matrix4f
{
public:

    typedef float4 Data;

    Data columns[4];

    Matrix4f()
    {}

    RTFACT__MATRIX_INLINE Matrix4f(
        const float a11, const float a12, const float a13, const float a14,
        const float a21, const float a22, const float a23, const float a24,
        const float a31, const float a32, const float a33, const float a34,
        const float a41, const float a42, const float a43, const float a44)
    {
        columns[0][0] = a11; columns[0][1] = a21;
        columns[0][2] = a31; columns[0][3] = a41;

        columns[1][0] = a12; columns[1][1] = a22;
        columns[1][2] = a32; columns[1][3] = a42;

        columns[2][0] = a13; columns[2][1] = a23;
        columns[2][2] = a33; columns[2][3] = a43;

        columns[3][0] = a14; columns[3][1] = a24;
        columns[3][2] = a34; columns[3][3] = a44;
    }

    RTFACT__MATRIX_INLINE Matrix4f(
        const float aMatrix[4][4])
    {
      columns[0][0] = aMatrix[0][0]; columns[0][1] = aMatrix[0][1]; 
      columns[0][2] = aMatrix[0][2]; columns[0][3] = aMatrix[0][3];

      columns[1][0] = aMatrix[1][0]; columns[1][1] = aMatrix[1][1]; 
      columns[1][2] = aMatrix[1][2]; columns[1][3] = aMatrix[1][3];

      columns[2][0] = aMatrix[2][0]; columns[2][1] = aMatrix[2][1]; 
      columns[2][2] = aMatrix[2][2]; columns[2][3] = aMatrix[2][3];

      columns[3][0] = aMatrix[3][0]; columns[3][1] = aMatrix[3][1]; 
      columns[3][2] = aMatrix[3][2]; columns[3][3] = aMatrix[3][3];
    }

    RTFACT__MATRIX_INLINE Matrix4f(
        const Data& aColumn1,
        const Data& aColumn2,
        const Data& aColumn3,
        const Data& aColumn4)
    {
        columns[0] = aColumn1;
        columns[1] = aColumn2;
        columns[2] = aColumn3;
        columns[3] = aColumn4;
    }

    RTFACT__MATRIX_INLINE const float& operator()(
        const uint aRow,
        const uint aColumn) const
    {
        return columns[aColumn][aRow];
    }

    RTFACT__MATRIX_INLINE float& operator()(
        const uint aRow,
        const uint aColumn)
    {
        return columns[aColumn][aRow];
    }

    RTFACT__MATRIX_INLINE const float4 multiply(
        const float4& aVec) const
    {
        return
            columns[0] * aVec.shuffle<0, 0, 0, 0>() +
            columns[1] * aVec.shuffle<1, 1, 1, 1>() +
            columns[2] * aVec.shuffle<2, 2, 2, 2>() +
            columns[3] * aVec.shuffle<3, 3, 3, 3>();
    }

    RTFACT__MATRIX_INLINE const float4 multiplyNormalizedPoint(
        const float4& aVec) const
    {
        return
            columns[0] * aVec.shuffle<0, 0, 0, 0>() +
            columns[1] * aVec.shuffle<1, 1, 1, 1>() +
            columns[2] * aVec.shuffle<2, 2, 2, 2>() +
            columns[3];
    }

    template<
        uint taSize>
    RTFACT__MATRIX_INLINE const Vec3f<taSize> multiplyPoint(
        const Vec3f<taSize>& aVec) const
    {
        Vec3f<taSize> ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
        {
            ret(i).x.data =
                columns[0].shuffle<0, 0, 0, 0>() * aVec(i).x.data +
                columns[1].shuffle<0, 0, 0, 0>() * aVec(i).y.data +
                columns[2].shuffle<0, 0, 0, 0>() * aVec(i).z.data +
                columns[3].shuffle<0, 0, 0, 0>();

            ret(i).y.data =
                columns[0].shuffle<1, 1, 1, 1>() * aVec(i).x.data +
                columns[1].shuffle<1, 1, 1, 1>() * aVec(i).y.data +
                columns[2].shuffle<1, 1, 1, 1>() * aVec(i).z.data +
                columns[3].shuffle<1, 1, 1, 1>();

            ret(i).z.data =
                columns[0].shuffle<2, 2, 2, 2>() * aVec(i).x.data +
                columns[1].shuffle<2, 2, 2, 2>() * aVec(i).y.data +
                columns[2].shuffle<2, 2, 2, 2>() * aVec(i).z.data +
                columns[3].shuffle<2, 2, 2, 2>();

            const float4 w =
                columns[0].shuffle<3, 3, 3, 3>() * aVec(i).x.data +
                columns[1].shuffle<3, 3, 3, 3>() * aVec(i).y.data +
                columns[2].shuffle<3, 3, 3, 3>() * aVec(i).z.data +
                columns[3].shuffle<3, 3, 3, 3>();

            const float4 wRecip = w.reciprocal();

            ret(i).x.data *= wRecip;
            ret(i).y.data *= wRecip;
            ret(i).z.data *= wRecip;
        }

        return ret;
    }

    template<
        uint taSize>
    RTFACT__MATRIX_INLINE const Vec3f<taSize> multiplyPointTransposed(
        const Vec3f<taSize>& aVec) const
    {
        Vec3f<taSize> ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
        {
            ret(i).x.data =
                columns[0].shuffle<0, 0, 0, 0>() * aVec(i).x.data +
                columns[0].shuffle<1, 1, 1, 1>() * aVec(i).y.data +
                columns[0].shuffle<2, 2, 2, 2>() * aVec(i).z.data +
                columns[0].shuffle<3, 3, 3, 3>();

            ret(i).y.data =
                columns[1].shuffle<0, 0, 0, 0>() * aVec(i).x.data +
                columns[1].shuffle<1, 1, 1, 1>() * aVec(i).y.data +
                columns[1].shuffle<2, 2, 2, 2>() * aVec(i).z.data +
                columns[1].shuffle<3, 3, 3, 3>();

            ret(i).z.data =
                columns[2].shuffle<0, 0, 0, 0>() * aVec(i).x.data +
                columns[2].shuffle<1, 1, 1, 1>() * aVec(i).y.data +
                columns[2].shuffle<2, 2, 2, 2>() * aVec(i).z.data +
                columns[2].shuffle<3, 3, 3, 3>();

            const float4 w =
                columns[3].shuffle<0, 0, 0, 0>() * aVec(i).x.data +
                columns[3].shuffle<1, 1, 1, 1>() * aVec(i).y.data +
                columns[3].shuffle<2, 2, 2, 2>() * aVec(i).z.data +
                columns[3].shuffle<3, 3, 3, 3>();

            const float4 wRecip = w.reciprocal();

            ret(i).x.data *= wRecip;
            ret(i).y.data *= wRecip;
            ret(i).z.data *= wRecip;
        }

        return ret;
    }

    template<
        uint taSize>
    RTFACT__MATRIX_INLINE const Vec3f<taSize> multiplyPointAffine(
        const Vec3f<taSize>& aVec) const
    {
        Vec3f<taSize> ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
        {
            ret(i).x.data =
                columns[0].shuffle<0, 0, 0, 0>() * aVec(i).x.data +
                columns[1].shuffle<0, 0, 0, 0>() * aVec(i).y.data +
                columns[2].shuffle<0, 0, 0, 0>() * aVec(i).z.data +
                columns[3].shuffle<0, 0, 0, 0>();

            ret(i).y.data =
                columns[0].shuffle<1, 1, 1, 1>() * aVec(i).x.data +
                columns[1].shuffle<1, 1, 1, 1>() * aVec(i).y.data +
                columns[2].shuffle<1, 1, 1, 1>() * aVec(i).z.data +
                columns[3].shuffle<1, 1, 1, 1>();

            ret(i).z.data =
                columns[0].shuffle<2, 2, 2, 2>() * aVec(i).x.data +
                columns[1].shuffle<2, 2, 2, 2>() * aVec(i).y.data +
                columns[2].shuffle<2, 2, 2, 2>() * aVec(i).z.data +
                columns[3].shuffle<2, 2, 2, 2>();
        }

        return ret;
    }

    template<
        uint taSize>
    RTFACT__MATRIX_INLINE const Vec3f<taSize> multiplyPointTransposedAffine(
        const Vec3f<taSize>& aVec) const
    {
        Vec3f<taSize> ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
        {
            ret(i).x.data =
                columns[0].shuffle<0, 0, 0, 0>() * aVec(i).x.data +
                columns[0].shuffle<1, 1, 1, 1>() * aVec(i).y.data +
                columns[0].shuffle<2, 2, 2, 2>() * aVec(i).z.data +
                columns[0].shuffle<3, 3, 3, 3>();

            ret(i).y.data =
                columns[1].shuffle<0, 0, 0, 0>() * aVec(i).x.data +
                columns[1].shuffle<1, 1, 1, 1>() * aVec(i).y.data +
                columns[1].shuffle<2, 2, 2, 2>() * aVec(i).z.data +
                columns[1].shuffle<3, 3, 3, 3>();

            ret(i).z.data =
                columns[2].shuffle<0, 0, 0, 0>() * aVec(i).x.data +
                columns[2].shuffle<1, 1, 1, 1>() * aVec(i).y.data +
                columns[2].shuffle<2, 2, 2, 2>() * aVec(i).z.data +
                columns[2].shuffle<3, 3, 3, 3>();
        }

        return ret;
    }

    template<
        uint taSize>
    RTFACT__MATRIX_INLINE const Vec3f<taSize> multiplyVector(
        const Vec3f<taSize>& aVec) const
    {
        Vec3f<taSize> ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
        {
            ret(i).x.data =
                columns[0].shuffle<0, 0, 0, 0>() * aVec(i).x.data +
                columns[1].shuffle<0, 0, 0, 0>() * aVec(i).y.data +
                columns[2].shuffle<0, 0, 0, 0>() * aVec(i).z.data;

            ret(i).y.data =
                columns[0].shuffle<1, 1, 1, 1>() * aVec(i).x.data +
                columns[1].shuffle<1, 1, 1, 1>() * aVec(i).y.data +
                columns[2].shuffle<1, 1, 1, 1>() * aVec(i).z.data;

            ret(i).z.data =
                columns[0].shuffle<2, 2, 2, 2>() * aVec(i).x.data +
                columns[1].shuffle<2, 2, 2, 2>() * aVec(i).y.data +
                columns[2].shuffle<2, 2, 2, 2>() * aVec(i).z.data;

            const float4 w =
                columns[0].shuffle<3, 3, 3, 3>() * aVec(i).x.data +
                columns[1].shuffle<3, 3, 3, 3>() * aVec(i).y.data +
                columns[2].shuffle<3, 3, 3, 3>() * aVec(i).z.data;

            const float4 wRecip = w.reciprocal();

            ret(i).x.data *= wRecip;
            ret(i).y.data *= wRecip;
            ret(i).z.data *= wRecip;
        }

        return ret;
    }

    template<
        uint taSize>
    RTFACT__MATRIX_INLINE const Vec3f<taSize> multiplyVectorTransposed(
        const Vec3f<taSize>& aVec) const
    {
        Vec3f<taSize> ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
        {
            ret(i).x.data =
                columns[0].shuffle<0, 0, 0, 0>() * aVec(i).x.data +
                columns[0].shuffle<1, 1, 1, 1>() * aVec(i).y.data +
                columns[0].shuffle<2, 2, 2, 2>() * aVec(i).z.data;

            ret(i).y.data =
                columns[1].shuffle<0, 0, 0, 0>() * aVec(i).x.data +
                columns[1].shuffle<1, 1, 1, 1>() * aVec(i).y.data +
                columns[1].shuffle<2, 2, 2, 2>() * aVec(i).z.data;

            ret(i).z.data =
                columns[2].shuffle<0, 0, 0, 0>() * aVec(i).x.data +
                columns[2].shuffle<1, 1, 1, 1>() * aVec(i).y.data +
                columns[2].shuffle<2, 2, 2, 2>() * aVec(i).z.data;

            const float4 w =
                columns[3].shuffle<0, 0, 0, 0>() * aVec(i).x.data +
                columns[3].shuffle<1, 1, 1, 1>() * aVec(i).y.data +
                columns[3].shuffle<2, 2, 2, 2>() * aVec(i).z.data;

            const float4 wRecip = w.reciprocal();

            ret(i).x.data *= wRecip;
            ret(i).y.data *= wRecip;
            ret(i).z.data *= wRecip;
        }

        return ret;
    }

    template<
        uint taSize>
    RTFACT__MATRIX_INLINE const Vec3f<taSize> multiplyVectorAffine(
        const Vec3f<taSize>& aVec) const
    {
        Vec3f<taSize> ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
        {
            ret(i).x.data =
                columns[0].shuffle<0, 0, 0, 0>() * aVec(i).x.data +
                columns[1].shuffle<0, 0, 0, 0>() * aVec(i).y.data +
                columns[2].shuffle<0, 0, 0, 0>() * aVec(i).z.data;

            ret(i).y.data =
                columns[0].shuffle<1, 1, 1, 1>() * aVec(i).x.data +
                columns[1].shuffle<1, 1, 1, 1>() * aVec(i).y.data +
                columns[2].shuffle<1, 1, 1, 1>() * aVec(i).z.data;

            ret(i).z.data =
                columns[0].shuffle<2, 2, 2, 2>() * aVec(i).x.data +
                columns[1].shuffle<2, 2, 2, 2>() * aVec(i).y.data +
                columns[2].shuffle<2, 2, 2, 2>() * aVec(i).z.data;
        }

        return ret;
    }

    template<
        uint taSize>
    RTFACT__MATRIX_INLINE const Vec3f<taSize> multiplyVectorTransposedAffine(
        const Vec3f<taSize>& aVec) const
    {
        Vec3f<taSize> ret;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
        {
            ret(i).x.data =
                columns[0].shuffle<0, 0, 0, 0>() * aVec(i).x.data +
                columns[0].shuffle<1, 1, 1, 1>() * aVec(i).y.data +
                columns[0].shuffle<2, 2, 2, 2>() * aVec(i).z.data;

            ret(i).y.data =
                columns[1].shuffle<0, 0, 0, 0>() * aVec(i).x.data +
                columns[1].shuffle<1, 1, 1, 1>() * aVec(i).y.data +
                columns[1].shuffle<2, 2, 2, 2>() * aVec(i).z.data;

            ret(i).z.data =
                columns[2].shuffle<0, 0, 0, 0>() * aVec(i).x.data +
                columns[2].shuffle<1, 1, 1, 1>() * aVec(i).y.data +
                columns[2].shuffle<2, 2, 2, 2>() * aVec(i).z.data;
        }

        return ret;
    }

    RTFACT__MATRIX_INLINE const Matrix4f operator*(
        const Matrix4f& aMat) const
    {
        Matrix4f ret;

        ret.columns[0] =
            columns[0] * aMat.columns[0].shuffle<0, 0, 0, 0>() +
            columns[1] * aMat.columns[0].shuffle<1, 1, 1, 1>() +
            columns[2] * aMat.columns[0].shuffle<2, 2, 2, 2>() +
            columns[3] * aMat.columns[0].shuffle<3, 3, 3, 3>();

        ret.columns[1] =
            columns[0] * aMat.columns[1].shuffle<0, 0, 0, 0>() +
            columns[1] * aMat.columns[1].shuffle<1, 1, 1, 1>() +
            columns[2] * aMat.columns[1].shuffle<2, 2, 2, 2>() +
            columns[3] * aMat.columns[1].shuffle<3, 3, 3, 3>();

        ret.columns[2] =
            columns[0] * aMat.columns[2].shuffle<0, 0, 0, 0>() +
            columns[1] * aMat.columns[2].shuffle<1, 1, 1, 1>() +
            columns[2] * aMat.columns[2].shuffle<2, 2, 2, 2>() +
            columns[3] * aMat.columns[2].shuffle<3, 3, 3, 3>();

        ret.columns[3] =
            columns[0] * aMat.columns[3].shuffle<0, 0, 0, 0>() +
            columns[1] * aMat.columns[3].shuffle<1, 1, 1, 1>() +
            columns[2] * aMat.columns[3].shuffle<2, 2, 2, 2>() +
            columns[3] * aMat.columns[3].shuffle<3, 3, 3, 3>();

        return ret;
    }

    RTFACT__MATRIX_INLINE Matrix4f& operator*=(
        const Matrix4f& aMat)
    {
        *this = *this * aMat;

        return *this;
    }

    RTFACT_MATRIX4F_BINARY_OP(+)
    RTFACT_MATRIX4F_BINARY_OP(-)

    RTFACT_MATRIX4F_SCALAR_OP(*)
    RTFACT_MATRIX4F_SCALAR_OP(/)

    RTFACT__MATRIX_INLINE const Matrix4f transpose() const
    {
        Matrix4f ret;

        const __m128 xmm0 = _mm_unpacklo_ps(columns[0].data, columns[1].data);
        const __m128 xmm1 = _mm_unpacklo_ps(columns[2].data, columns[3].data);
        const __m128 xmm2 = _mm_unpackhi_ps(columns[0].data, columns[1].data);
        const __m128 xmm3 = _mm_unpackhi_ps(columns[2].data, columns[3].data);

        ret.columns[0].data = _mm_movelh_ps(xmm0, xmm1);
        ret.columns[1].data = _mm_movehl_ps(xmm1, xmm0);
        ret.columns[2].data = _mm_movelh_ps(xmm2, xmm3);
        ret.columns[3].data = _mm_movehl_ps(xmm3, xmm2);

        return ret;
    }

    RTFACT__MATRIX_INLINE float determinant() const
    {
        __m128 Va, Vb, Vc;
        __m128 r1, r2, r3, t1, t2, sum;
        float4 Det;

        // First, Let's calculate the first four minterms of the first line
        t1 = columns[3].data;
        t2 = RTFACT__mm_ror_ps(columns[2].data, 1);
        Vc = _mm_mul_ps(t2, RTFACT__mm_ror_ps(t1, 0));         // V3'·V4
        Va = _mm_mul_ps(t2, RTFACT__mm_ror_ps(t1, 2));         // V3'·V4"
        Vb = _mm_mul_ps(t2, RTFACT__mm_ror_ps(t1, 3));         // V3'·V4^

        r1 = _mm_sub_ps(RTFACT__mm_ror_ps(Va,1),
                        RTFACT__mm_ror_ps(Vc,2)); // V3"·V4^ - V3^·V4"
        r2 = _mm_sub_ps(RTFACT__mm_ror_ps(Vb,2),
                        RTFACT__mm_ror_ps(Vb,0)); // V3^·V4' - V3'·V4^
        r3 = _mm_sub_ps(RTFACT__mm_ror_ps(Va,0),
                        RTFACT__mm_ror_ps(Vc,1)); // V3'·V4" - V3"·V4'

        Va = RTFACT__mm_ror_ps(columns[1].data, 1);
        sum = _mm_mul_ps(Va,r1);

        Vb = RTFACT__mm_ror_ps(Va,1);
        sum = _mm_add_ps(sum,_mm_mul_ps(Vb,r2));

        Vc = RTFACT__mm_ror_ps(Vb,1);
        sum = _mm_add_ps(sum,_mm_mul_ps(Vc,r3));

        // Now we can calculate the determinant:
        Det.data = _mm_mul_ps(sum, columns[0].data);
        Det.data = _mm_add_ps(Det.data, _mm_movehl_ps(Det.data, Det.data));
        Det.data = _mm_sub_ss(Det.data, _mm_shuffle_ps(Det.data, Det.data, 1));

        return Det.at<0>();
    }

    RTFACT__MATRIX_INLINE const Matrix4f inverse() const
    {
        /* The inverse is calculated using "Divide and Conquer" technique. The 
        original matrix is divide into four 2x2 sub-matrices. Since each 
        register holds four matrix element, the smaller matrices are 
        represented as a registers. Hence we get a better locality of the 
        calculations.*/

        // the four sub-matrices
        __m128 A = _mm_movelh_ps(columns[0].data, columns[1].data),
               B = _mm_movehl_ps(columns[1].data, columns[0].data),
               C = _mm_movelh_ps(columns[2].data, columns[3].data),
               D = _mm_movehl_ps(columns[3].data, columns[2].data);

        // partial inverse of the sub-matrices
        __m128 iA, iB, iC, iD, DC, AB;
        
        // determinant of the sub-matrices
        __m128 dA, dB, dC, dD;

        __m128 det, d, d1, d2;
        __m128 rd;

        //  AB = A# * B
        AB = _mm_mul_ps(_mm_shuffle_ps(A,A,0x0F), B);
        /*AB -= _mm_mul_ps(_mm_shuffle_ps(A,A,0xA5),
                         _mm_shuffle_ps(B,B,0x4E));*/

        AB = _mm_sub_ps(AB, _mm_mul_ps(_mm_shuffle_ps(A,A,0xA5),
            _mm_shuffle_ps(B,B,0x4E)));

        //  DC = D# * C
        DC = _mm_mul_ps(_mm_shuffle_ps(D,D,0x0F), C);
        DC = _mm_sub_ps(DC, _mm_mul_ps(_mm_shuffle_ps(D,D,0xA5),    // -=
                         _mm_shuffle_ps(C,C,0x4E)));

        //  dA = |A|
        dA = _mm_mul_ps(_mm_shuffle_ps(A, A, 0x5F),A);
        dA = _mm_sub_ss(dA, _mm_movehl_ps(dA,dA));
        //  dB = |B|
        dB = _mm_mul_ps(_mm_shuffle_ps(B, B, 0x5F),B);
        dB = _mm_sub_ss(dB, _mm_movehl_ps(dB,dB));

        //  dC = |C|
        dC = _mm_mul_ps(_mm_shuffle_ps(C, C, 0x5F),C);
        dC = _mm_sub_ss(dC, _mm_movehl_ps(dC,dC));
        //  dD = |D|
        dD = _mm_mul_ps(_mm_shuffle_ps(D, D, 0x5F),D);
        dD = _mm_sub_ss(dD, _mm_movehl_ps(dD,dD));

        //  d = trace(AB*DC) = trace(A#*B*D#*C)
        d = _mm_mul_ps(_mm_shuffle_ps(DC,DC,0xD8),AB);

        //  iD = C*A#*B
        iD = _mm_mul_ps(_mm_shuffle_ps(C,C,0xA0), _mm_movelh_ps(AB,AB));
        iD = _mm_add_ps(iD, _mm_mul_ps(_mm_shuffle_ps(C,C,0xF5),
                         _mm_movehl_ps(AB,AB))); // +=
        //  iA = B*D#*C
        iA = _mm_mul_ps(_mm_shuffle_ps(B,B,0xA0), _mm_movelh_ps(DC,DC));
        iA = _mm_add_ps(iA, _mm_mul_ps(_mm_shuffle_ps(B,B,0xF5),
                         _mm_movehl_ps(DC,DC))); // +=

        //  d = trace(AB*DC) = trace(A#*B*D#*C) [continue]
        d = _mm_add_ps(d, _mm_movehl_ps(d, d));
        d = _mm_add_ss(d, _mm_shuffle_ps(d, d, 1));
        d1 = _mm_mul_ps(dA, dD);
        d2 = _mm_mul_ps(dB, dC);

        //  iD = D*|A| - C*A#*B
        iD = _mm_sub_ps(_mm_mul_ps(D, _mm_shuffle_ps(dA,dA,0)), iD);

        //  iA = A*|D| - B*D#*C;
        iA = _mm_sub_ps(_mm_mul_ps(A, _mm_shuffle_ps(dD,dD,0)), iA);

        //  det = |A|*|D| + |B|*|C| - trace(A#*B*D#*C)
        det = _mm_sub_ps(_mm_add_ps(d1, d2), d);
        //rd = _mm_rcp_ps(det);

        const __m128 tempRcp = _mm_rcp_ps(det);

        rd = _mm_sub_ps(_mm_add_ps(tempRcp, tempRcp),
                        _mm_mul_ps(_mm_mul_ps(tempRcp, tempRcp), det));
        
        #ifdef RTFACT_MATRIX_SINGULAR_INVERSE_ZERO

            rd = _mm_and_ps(_mm_cmpneq_ss(det,_mm_setzero_ps()), rd);

        #endif

        //  iB = D * (A#B)# = D*B#*A
        iB = _mm_mul_ps(D, _mm_shuffle_ps(AB,AB,0x33));

        iB = _mm_sub_ps(iB, _mm_mul_ps(_mm_shuffle_ps(D,D,0xB1),
                         _mm_shuffle_ps(AB,AB,0x66)));

        //  iC = A * (D#C)# = A*C#*D
        iC = _mm_mul_ps(A, _mm_shuffle_ps(DC,DC,0x33));
        iC = _mm_sub_ps(iC, _mm_mul_ps(_mm_shuffle_ps(A,A,0xB1),
                         _mm_shuffle_ps(DC,DC,0x66)));

        rd = _mm_shuffle_ps(rd,rd,0);
        rd = _mm_xor_ps(rd, RTFACT__Sign_PNNP);

        //  iB = C*|B| - D*B#*A
        iB = _mm_sub_ps(_mm_mul_ps(C, _mm_shuffle_ps(dB,dB,0)), iB);

        //  iC = B*|C| - A*C#*D;
        iC = _mm_sub_ps(_mm_mul_ps(B, _mm_shuffle_ps(dC,dC,0)), iC);

        //  iX = iX / det
        iA = _mm_mul_ps(iA, rd);
        iB = _mm_mul_ps(iB, rd);
        iC = _mm_mul_ps(iC, rd);
        iD = _mm_mul_ps(iD, rd);

        Matrix4f ret;

        ret.columns[0].data = _mm_shuffle_ps(iA,iB,0x77);
        ret.columns[1].data = _mm_shuffle_ps(iA,iB,0x22);
        ret.columns[2].data = _mm_shuffle_ps(iC,iD,0x77);
        ret.columns[3].data = _mm_shuffle_ps(iC,iD,0x22);

        return ret;
    }

    RTFACT__MATRIX_INLINE static const Matrix4f& identity()
    {
        static Matrix4f mat(
            float4::C_1_0_0_0,
            float4::C_0_1_0_0,
            float4::C_0_0_1_0,
            float4::C_0_0_0_1);

        return mat;
    }
};

template<>
RTFACT__MATRIX_INLINE const Vec3f<1> Matrix4f::multiplyPoint<1>(
    const Vec3f<1>& aVec) const
{
    Vec3f1 res(
        columns[0] * aVec.data.shuffle<0, 0, 0, 0>() +
        columns[1] * aVec.data.shuffle<1, 1, 1, 1>() +
        columns[2] * aVec.data.shuffle<2, 2, 2, 2>() +
        columns[3]);

    return Vec3f1(res.data * res.data.shuffle<3, 3, 3, 3>().reciprocal());
}

template<>
RTFACT__MATRIX_INLINE const Vec3f<1> Matrix4f::multiplyPointTransposed<1>(
    const Vec3f<1>& aVec) const
{
    /*RTFACT_ALIGN_SIMD float res[4];

    res[0] = (columns[0] * aVec.data).addHoriz().at<0>();
    res[1] = (columns[1] * aVec.data).addHoriz().at<0>();
    res[2] = (columns[2] * aVec.data).addHoriz().at<0>();
    res[3] = (columns[3] * aVec.data).addHoriz().at<0>();

    return Vec3f1(float4::getInstance(static_cast<float*>(res)));*/

    //RTFACT_PROGRAM_ERROR("multiplyPointTransposed<1> not implemented");

    return transpose().multiplyPoint(aVec);
}

template<>
RTFACT__MATRIX_INLINE const Vec3f<1> Matrix4f::multiplyPointAffine<1>(
    const Vec3f<1>& aVec) const
{
    return Vec3f<1>(
        columns[0] * aVec.data.shuffle<0, 0, 0, 0>() +
        columns[1] * aVec.data.shuffle<1, 1, 1, 1>() +
        columns[2] * aVec.data.shuffle<2, 2, 2, 2>() +
        columns[3]);
}

template<>
RTFACT__MATRIX_INLINE const Vec3f<1> Matrix4f::multiplyPointTransposedAffine<1>(
    const Vec3f<1>& aVec) const
{
    //RTFACT_PROGRAM_ERROR("multiplyPointTransposedAffine<1> not implemented");

    return transpose().multiplyPointAffine(aVec);
}

template<>
RTFACT__MATRIX_INLINE const Vec3f<1> Matrix4f::multiplyVector<1>(
    const Vec3f<1>& aVec) const
{
    Vec3f1 res(
        columns[0] * aVec.data.shuffle<0, 0, 0, 0>() +
        columns[1] * aVec.data.shuffle<1, 1, 1, 1>() +
        columns[2] * aVec.data.shuffle<2, 2, 2, 2>());

    return Vec3f1(res.data * res.data.shuffle<3, 3, 3, 3>().reciprocal());
}

template<>
RTFACT__MATRIX_INLINE const Vec3f<1> Matrix4f::multiplyVectorTransposed<1>(
    const Vec3f<1>& aVec) const
{
    //RTFACT_PROGRAM_ERROR("multiplyVectorTransposed<1> not implemented");

    return transpose().multiplyVector(aVec);
}

template<>
RTFACT__MATRIX_INLINE const Vec3f<1> Matrix4f::multiplyVectorAffine<1>(
    const Vec3f<1>& aVec) const
{
    return Vec3f<1>(
        columns[0] * aVec.data.shuffle<0, 0, 0, 0>() +
        columns[1] * aVec.data.shuffle<1, 1, 1, 1>() +
        columns[2] * aVec.data.shuffle<2, 2, 2, 2>());
}

template<>
RTFACT__MATRIX_INLINE const Vec3f<1> Matrix4f::multiplyVectorTransposedAffine<1>(
    const Vec3f<1>& aVec) const
{
    //RTFACT_PROGRAM_ERROR("multiplyVectorTransposedAffine<1> not implemented");

    return transpose().multiplyVectorAffine(aVec);
}

RTFACT_NAMESPACE_END

#endif // RTFACT__MATRIX4X4_HPP
