/* 
 *  PinholeCamera.hpp
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
 
 #ifndef RTFACT__PINHOLECAMERA_HPP
#define RTFACT__PINHOLECAMERA_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Camera/RayTracingCamera.hpp>

RTFACT_NAMESPACE_BEGIN

class PinholeCamera :
    public RayTracingCamera
{
    float mFocusDist;
    float mAspectRatio;

    // precomputed terms for efficiency

    Vec3f1 mXDir;
    Vec3f1 mYDir;
    Vec3f1 mBaseDir;

    Vec3fC mXDirC;
    Vec3fC mYDirC;
    Vec3fC mBaseDirC;

    Vec3fC mPosC;

public:

    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    void setParams(
        Params& aParams)
    {
        RayTracingCamera::setParams(aParams);

        mFocusDist = 1.f / Math::tan(Math::radians(mOpeningAngle) * 0.5f);
        mAspectRatio = mResXf / mResYf;

        // precompute terms
        mXDir = (2.f * mAspectRatio) / mResX * mXAxis;
        mYDir = 2.f / mResY * mYAxis;
        mBaseDir = mFocusDist * mZAxis - mAspectRatio * mXAxis - mYAxis;

        mPosC = Vec3f<4>::replicate(mPos);
        mXDirC = Vec3f<4>::replicate(mXDir);
        mYDirC = Vec3f<4>::replicate(mYDir);
        mBaseDirC = Vec3f<4>::replicate(mBaseDir);
    }

    template<
        uint taSize>
    RTFACT_CAMERA_INLINE const RayPacket<taSize> generateRay(
        PixelSampler::Sample<taSize>& aSample) const
    {
        typedef typename Packet<taSize, float>::Container t_PacketC;
        typedef typename Vec3f<taSize>::Container t_Vec3fC;

        const t_PacketC zero = t_PacketC::C_0();

        RayPacket<taSize> ret;

        //ret.org(0) = mPosC;

        ret.frustum.primaryRays = true;

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
        {
            ret.org(i) = mPosC;

            const t_Vec3fC dir = aSample.imageX(i) * mXDirC + aSample.imageY(i) * mYDirC + mBaseDirC;

            ret.dir(i) = dir.normalize();
            ret.invDir(i) = ret.dir(i).reciprocal();
            ret.tMin(i) = zero;
            ret.tMax(i) = t_PacketC::C_INFINITY;

            const t_PacketC dotDir = Math::dot(dir, dir);
            const t_PacketC dotDirPow = Math::pow(dotDir, -1.5f);

            ret.dDdx(i) = (dotDir * mXDirC - Math::dot(dir, mXDirC) * dir) * dotDirPow;
            ret.dDdy(i) = (dotDir * mYDirC - Math::dot(dir, mYDirC) * dir) * dotDirPow;

            ret.deltaX(i) = aSample.deltaX(i);
            ret.deltaY(i) = aSample.deltaY(i);
        }

        return ret;
    }
};

template<>
RTFACT_CAMERA_INLINE const RayPacket<1> PinholeCamera::generateRay<1>(
    PixelSampler::Sample<1>& aSample) const
{   
    RayPacket<1> ret;

    ret.org = mPos;

    const Vec3f<1> dir = aSample.imageX * mXDir + aSample.imageY * mYDir + mBaseDir;

    ret.dir = dir.normalize();
    ret.invDir = ret.dir.reciprocal();

    ret.tMin = Packet<1, float>::C_RAY_EPS;
    ret.tMax = Packet<1, float>::C_INFINITY;

    const float dotDir = Math::dot(dir, dir);
    const float dotDirPow = Math::pow(dotDir, -1.5f);

    ret.dDdx = (dotDir * mXDir - Math::dot(dir, mXDir) * dir) * dotDirPow;
    ret.dDdy = (dotDir * mYDir - Math::dot(dir, mYDir) * dir) * dotDirPow;

    ret.deltaX = aSample.deltaX;
    ret.deltaY = aSample.deltaY;

    return ret;
}

RTFACT_NAMESPACE_END

#endif // RTFACT__PINHOLECAMERA_HPP
