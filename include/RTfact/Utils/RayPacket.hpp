/*
 *  RayPacket.hpp
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
 *  Author(s): Iliyan Georgiev, Georgiev Iliyan
 */

 #ifndef RTFACT__RAYPACKET_HPP
#define RTFACT__RAYPACKET_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/BBox.hpp>

RTFACT_NAMESPACE_BEGIN

#define RAYPACKET_MAXDEPTH 10

struct RayType
{
    enum
    {
        ShadingRay = 0,
        ImportonRay = 1,
        ParticleRay = 2,
        ShadowRay = 3
    };
};

struct RayFrustum
{
    uint dominantAxis;
    uint dominantAxisSign;
    uint dirSigns[3];

    // corner rays
    Vec3f<4> org;
    Vec3f<4> dir;
    //Vec3f<4> cornerInvDir;

    // planes
    Vec3f<4> planeNormal;
    Packet<4, float> planeDist;

    //not actual coordinates, but their indices
    uint extremeBoundingCoordsX[4];
    uint extremeBoundingCoordsY[4];
    uint extremeBoundingCoordsZ[4];

    bool primaryRays;
    float refractionIndex;
    bool inside;
    bool bad;

    RayFrustum() :
        primaryRays(false),
        refractionIndex(0.f),
        inside(false),
        bad(false)
    {}

    template<
        bool taCommonOrg,
        uint taSize>
    RTFACT_RAY_PACKET_INLINE void init(
        const Vec3f<taSize>& aOrg,
        const Vec3f<taSize>& aDir,
        const Vec3f<taSize>& aInvDir,
        const ActiveMask<taSize>& aMask,
        const BBox& aBounds)
    {
        typedef typename Vec3f<taSize>::Container         t_Vec3fC;
        typedef typename Packet<taSize, float>::Container t_FPacketC;
        typedef typename Packet<taSize, int>::Container   t_IPacketC;
        typedef Mask<t_Vec3fC::SIZE>                      t_MaskC;
        typedef BitMask<t_Vec3fC::SIZE>                   t_BitMaskC;

        static const uint mod3[8] = {0, 1, 2, 0, 1};

        const uint axisSigns[3] =
        {
            aDir.xAt(0) < 0.f,
            aDir.yAt(0) < 0.f,
            aDir.zAt(0) < 0.f
        };

        const t_IPacketC signMasks[3] =
        {
            t_IPacketC::replicate(axisSigns[0] << 31),
            t_IPacketC::replicate(axisSigns[1] << 31),
            t_IPacketC::replicate(axisSigns[2] << 31)
        };

        t_MaskC signsAgreeMasksC[3];

        signsAgreeMasksC[0] = t_MaskC::C_FALSE();
        signsAgreeMasksC[1] = signsAgreeMasksC[0];
        signsAgreeMasksC[2] = signsAgreeMasksC[0];

        t_FPacketC componentSumsC[3];

        const t_FPacketC zero = t_FPacketC::C_0();

        componentSumsC[0] = zero;
        componentSumsC[1] = zero;
        componentSumsC[2] = zero;

        // check if all rays have the same sign on this axis
        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            //TODO: fix
            signsAgreeMasksC[0].data |=
                aMask(i).data & (signMasks[0].data ^ aDir(i).x.data.asInt4());

            signsAgreeMasksC[0] |= (aDir(i).x == zero);

            componentSumsC[0] += aDir(i).x;

            signsAgreeMasksC[1].data |=
                aMask(i).data & (signMasks[1].data ^ aDir(i).y.data.asInt4());

            signsAgreeMasksC[1] |= (aDir(i).y == zero);

            componentSumsC[1] += aDir(i).y;

            signsAgreeMasksC[2].data |=
                aMask(i).data & (signMasks[2].data ^ aDir(i).z.data.asInt4());

            signsAgreeMasksC[2] |= (aDir(i).z == zero);

            componentSumsC[2] += aDir(i).z;
        }

        const bool signsAgree[3] =
        {
            signsAgreeMasksC[0].isFalse(),
            signsAgreeMasksC[1].isFalse(),
            signsAgreeMasksC[2].isFalse()
        };

        // directions signs disagree on all axes, make the corner rays and
        // bounding planes degenerate so that they never cull anything
        if(!(signsAgree[0] | signsAgree[1] | signsAgree[2]))
        {
            dominantAxis = 0;
            dominantAxisSign = 0;

            org = aOrg(0);

            dir = Vec3f<4>(
                Packet<4, float>(1.f, 0.f, 0.f, -0.33f),
                Packet<4, float>(0.f, 1.f, 0.f, -0.33f),
                Packet<4, float>(0.f, 0.f, 1.f, -0.33f));

            planeNormal = Vec3f<4>::C_0_0_0();

            planeDist = Packet<4, float>::C_MAX;

            for(uint i = 0; i < 4; ++i)
            {
                extremeBoundingCoordsX[i] = 0;
                extremeBoundingCoordsY[i] = 0;
                extremeBoundingCoordsZ[i] = 0;
            }

            // for debugging only
            bad = true;

            return;
        }

        const float componentSumsAbs[3] =
        {
            Math::abs(componentSumsC[0].addHoriz()),
            Math::abs(componentSumsC[1].addHoriz()),
            Math::abs(componentSumsC[2].addHoriz())
        };

        const bool componentSumsAbs01 = componentSumsAbs[0] > componentSumsAbs[1];
        const bool componentSumsAbs12 = componentSumsAbs[1] > componentSumsAbs[2];
        const bool componentSumsAbs20 = componentSumsAbs[2] > componentSumsAbs[0];

        uint sortedComponentSumsIndices[3];

        if(componentSumsAbs01)
        {
            if(componentSumsAbs12)
            {
                sortedComponentSumsIndices[0] = 0;
                sortedComponentSumsIndices[1] = 1;
                sortedComponentSumsIndices[2] = 2;
            }
            else
            {
                sortedComponentSumsIndices[2] = 1;

                if(componentSumsAbs20)
                {
                    sortedComponentSumsIndices[0] = 2;
                    sortedComponentSumsIndices[1] = 0;
                }
                else
                {
                    sortedComponentSumsIndices[0] = 0;
                    sortedComponentSumsIndices[1] = 2;
                }
            }
        }
        else
        {
            if(componentSumsAbs12)
            {
                sortedComponentSumsIndices[0] = 1;

                if(componentSumsAbs20)
                {
                    sortedComponentSumsIndices[1] = 2;
                    sortedComponentSumsIndices[2] = 0;
                }
                else
                {
                    sortedComponentSumsIndices[1] = 0;
                    sortedComponentSumsIndices[2] = 2;
                }
            }
            else
            {
                sortedComponentSumsIndices[0] = 2;
                sortedComponentSumsIndices[1] = 1;
                sortedComponentSumsIndices[2] = 0;
            }
        }


        uint axisIndex;

        for(axisIndex = 0; axisIndex < 3; ++ axisIndex)
        {
            if(signsAgree[sortedComponentSumsIndices[axisIndex]])
            {
                break;
            }
        }

        dominantAxis = sortedComponentSumsIndices[axisIndex];
        dominantAxisSign = axisSigns[dominantAxis];

        const uint dim0 = dominantAxis;
        const uint dim1 = mod3[dim0 + 1];
        const uint dim2 = mod3[dim0 + 2];

        if(primaryRays)
        {
            #define RTFACT__TOP_LEFT     0
            #define RTFACT__TOP_RIGHT    Packet<taSize, float>::SIZE_SQRT - 1
            #define RTFACT__BOTTOM_LEFT  taSize - Packet<taSize, float>::SIZE_SQRT
            #define RTFACT__BOTTOM_RIGHT taSize - 1

            dir.x = Packet<4, float>(
                aDir.xAt(RTFACT__TOP_LEFT),
                aDir.xAt(RTFACT__TOP_RIGHT),
                aDir.xAt(RTFACT__BOTTOM_RIGHT),
                aDir.xAt(RTFACT__BOTTOM_LEFT));

            dir.y = Packet<4, float>(
                aDir.yAt(RTFACT__TOP_LEFT),
                aDir.yAt(RTFACT__TOP_RIGHT),
                aDir.yAt(RTFACT__BOTTOM_RIGHT),
                aDir.yAt(RTFACT__BOTTOM_LEFT));

            dir.z = Packet<4, float>(
                aDir.zAt(RTFACT__TOP_LEFT),
                aDir.zAt(RTFACT__TOP_RIGHT),
                aDir.zAt(RTFACT__BOTTOM_RIGHT),
                aDir.zAt(RTFACT__BOTTOM_LEFT));

            const Vec3f<4> toCross = dir.shuffle<3, 0, 1, 2>();

            planeNormal = Math::cross(toCross, dir);

            org = aOrg(0);

            planeDist = Math::dot(planeNormal, org);

            #undef RTFACT__TOP_LEFT
            #undef RTFACT__TOP_RIGHT
            #undef RTFACT__BOTTOM_LEFT
            #undef RTFACT__BOTTOM_RIGHT
        }
        else
        {
            if(taCommonOrg) // e.g. shadow rays
            {
                t_FPacketC minComponentsDim1, minComponentsDim2,
                           maxComponentsDim1, maxComponentsDim2;

                minComponentsDim1 = minComponentsDim2 = t_FPacketC::C_MAX;
                maxComponentsDim1 = maxComponentsDim2 = t_FPacketC::C_MIN;

                RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
                {
                    const t_FPacketC dirDim1XinvDirDim0 =
                        aDir(i).get(dim1) * aInvDir(i).get(dim0);

                    const t_FPacketC dirDim2XinvDirDim0 =
                        aDir(i).get(dim2) * aInvDir(i).get(dim0);

                    minComponentsDim1 = aMask(i).iif(
                        Math::min(minComponentsDim1, dirDim1XinvDirDim0),
                        minComponentsDim1);

                    minComponentsDim2 = aMask(i).iif(
                        Math::min(minComponentsDim2, dirDim2XinvDirDim0),
                        minComponentsDim2);

                    maxComponentsDim1 = aMask(i).iif(
                        Math::max(maxComponentsDim1, dirDim1XinvDirDim0),
                        maxComponentsDim1);

                    maxComponentsDim2 = aMask(i).iif(
                        Math::max(maxComponentsDim2, dirDim2XinvDirDim0),
                        maxComponentsDim2);
                }

                const float minDim1 = minComponentsDim1.min();
                const float minDim2 = minComponentsDim2.min();
                const float maxDim1 = maxComponentsDim1.max();
                const float maxDim2 = maxComponentsDim2.max();

                dir.get(dim1) = Packet<4, float>(minDim1, maxDim1, maxDim1, minDim1);
                dir.get(dim2) = Packet<4, float>(minDim2, minDim2, maxDim2, maxDim2);
                dir.get(dim0) = Packet<4, float>::replicate(1.f);

                dir.setNormalized();

                if(dominantAxisSign)
                {
                    dir = -dir;

                    const Vec3f<4> toCross = dir.shuffle<3, 0, 1, 2>();
                    planeNormal = Math::cross(toCross, dir);
                }
                else
                {
                    const Vec3f<4> toCross = dir.shuffle<3, 0, 1, 2>();
                    planeNormal = Math::cross(dir, toCross);
                }

                org = aOrg(0);

                planeDist = Math::dot(planeNormal, org);
            }
            else // e.g. reflection, refraction rays
            {
                float dim0Near, dim0Far;

                // find the near plane of the bounding box of the ray origins
                // and the far plane of the input bounding box according to the
                // direction sign
                if(dominantAxisSign)
                {
                    t_FPacketC maxOrgDim0 = t_FPacketC::C_MIN;

                    RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
                    {
                        maxOrgDim0 = aMask(i).iif(
                            Math::max(aOrg(i).get(dim0), maxOrgDim0), maxOrgDim0);
                    }

                    dim0Near = maxOrgDim0.max();
                    dim0Far = aBounds.min.get(dim0);
                }
                else
                {
                    t_FPacketC minOrgDim0 = t_FPacketC::C_MAX;

                    RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
                    {
                        minOrgDim0 = aMask(i).iif(
                            Math::min(aOrg(i).get(dim0), minOrgDim0), minOrgDim0);
                    }

                    dim0Near = minOrgDim0.min();
                    dim0Far = aBounds.max.get(dim0);
                }

                const t_FPacketC dim0NearC = t_FPacketC::replicate(dim0Near);
                const t_FPacketC dim0FarC  = t_FPacketC::replicate(dim0Far);

                t_FPacketC minDim1NearC, maxDim1NearC, minDim1FarC, maxDim1FarC;
                t_FPacketC minDim2NearC, maxDim2NearC, minDim2FarC, maxDim2FarC;

                minDim1NearC = minDim1FarC = minDim2NearC = minDim2FarC = t_FPacketC::C_MAX;
                maxDim1NearC = maxDim1FarC = maxDim2NearC = maxDim2FarC = t_FPacketC::C_MIN;

                // intersect the found near and far planes with the rays and
                // get the min/max of dim1 and dim2
                RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
                {
                    const t_FPacketC tNear = (dim0NearC - aOrg(i).get(dim0)) * aInvDir(i).get(dim0);
                    const t_FPacketC tFar  = (dim0FarC  - aOrg(i).get(dim0)) * aInvDir(i).get(dim0);

                    const t_FPacketC dim1Near = aOrg(i).get(dim1) + tNear * aDir(i).get(dim1);
                    const t_FPacketC dim1Far  = aOrg(i).get(dim1) + tFar  * aDir(i).get(dim1);

                    const t_FPacketC dim2Near = aOrg(i).get(dim2) + tNear * aDir(i).get(dim2);
                    const t_FPacketC dim2Far  = aOrg(i).get(dim2) + tFar  * aDir(i).get(dim2);

                    minDim1NearC = aMask(i).iif(Math::min(dim1Near, minDim1NearC), minDim1NearC);
                    maxDim1NearC = aMask(i).iif(Math::max(dim1Near, maxDim1NearC), maxDim1NearC);

                    minDim1FarC = aMask(i).iif(Math::min(dim1Far, minDim1FarC), minDim1FarC);
                    maxDim1FarC = aMask(i).iif(Math::max(dim1Far, maxDim1FarC), maxDim1FarC);

                    minDim2NearC = aMask(i).iif(Math::min(dim2Near, minDim2NearC), minDim2NearC);
                    maxDim2NearC = aMask(i).iif(Math::max(dim2Near, maxDim2NearC), maxDim2NearC);

                    minDim2FarC = aMask(i).iif(Math::min(dim2Far, minDim2FarC), minDim2FarC);
                    maxDim2FarC = aMask(i).iif(Math::max(dim2Far, maxDim2FarC), maxDim2FarC);
                }

                const float minDim1Near = minDim1NearC.min();
                const float maxDim1Near = maxDim1NearC.max();

                const float minDim1Far = minDim1FarC.min();
                const float maxDim1Far = maxDim1FarC.max();

                const float minDim2Near = minDim2NearC.min();
                const float maxDim2Near = maxDim2NearC.max();

                const float minDim2Far = minDim2FarC.min();
                const float maxDim2Far = maxDim2FarC.max();

                Vec3f<4> org2, dir2;
                Vec3f<4> planeNormal2;

                org2.get(dim0) = dim0NearC;
                dir2.get(dim0) = dim0FarC - dim0NearC;

                if(dominantAxisSign)
                {
                    org2.get(dim1) = Packet<4, float>(maxDim1Near, minDim1Near, minDim1Near, maxDim1Near);
                    org2.get(dim2) = Packet<4, float>(maxDim2Near, maxDim2Near, minDim2Near, minDim2Near);

                    dir2.get(dim1) = Packet<4, float>(maxDim1Far,  minDim1Far,  minDim1Far,  maxDim1Far) - org2.get(dim1);
                    dir2.get(dim2) = Packet<4, float>(maxDim2Far,  maxDim2Far,  minDim2Far,  minDim2Far) - org2.get(dim2);

                    dir2.setNormalized();

                    const Vec3f<4> toCross = dir2.shuffle<3, 0, 1, 2>();
                    planeNormal2 = Math::cross(toCross, dir2);

                    planeDist = Math::dot(planeNormal2, org2);
                }
                else
                {
                    org2.get(dim1) = Packet<4, float>(minDim1Near, maxDim1Near, maxDim1Near, minDim1Near);
                    org2.get(dim2) = Packet<4, float>(minDim2Near, minDim2Near, maxDim2Near, maxDim2Near);

                    dir2.get(dim1) = Packet<4, float>(minDim1Far,  maxDim1Far,  maxDim1Far,  minDim1Far ) - org2.get(dim1);
                    dir2.get(dim2) = Packet<4, float>(minDim2Far,  minDim2Far,  maxDim2Far,  maxDim2Far ) - org2.get(dim2);

                    dir2.setNormalized();

                    const Vec3f<4> toCross = dir2.shuffle<3, 0, 1, 2>();
                    planeNormal2 = Math::cross(dir2, toCross);

                    planeDist = Math::dot(planeNormal2, org2);
                }

                org = org2;
                dir = dir2;
                planeNormal = planeNormal2;
            }
        }

        //cornerInvDir = dir.reciprocal();

        // compute indices for coordinates of the extreme points of bounding boxes
        for(uint i = 0; i < 4; ++i)
        {
            extremeBoundingCoordsX[i] = (planeNormal.x[i] > 0.f) ?  0 : 4;
            extremeBoundingCoordsY[i] = (planeNormal.y[i] > 0.f) ?  1 : 5;
            extremeBoundingCoordsZ[i] = (planeNormal.z[i] > 0.f) ?  2 : 6;
        }
    }

    RTFACT_RAY_PACKET_INLINE bool misses(
        const BBox& aBounds) const
    {
        const float* boundsAsArray = (float*)(&aBounds);
        Vec3f<4> extremePoints;

        extremePoints.x = Packet<4, float>(
            boundsAsArray[extremeBoundingCoordsX[0]],
            boundsAsArray[extremeBoundingCoordsX[1]],
            boundsAsArray[extremeBoundingCoordsX[2]],
            boundsAsArray[extremeBoundingCoordsX[3]]);

        extremePoints.y = Packet<4, float>(
            boundsAsArray[extremeBoundingCoordsY[0]],
            boundsAsArray[extremeBoundingCoordsY[1]],
            boundsAsArray[extremeBoundingCoordsY[2]],
            boundsAsArray[extremeBoundingCoordsY[3]]);

        extremePoints.z = Packet<4, float>(
            boundsAsArray[extremeBoundingCoordsZ[0]],
            boundsAsArray[extremeBoundingCoordsZ[1]],
            boundsAsArray[extremeBoundingCoordsZ[2]],
            boundsAsArray[extremeBoundingCoordsZ[3]]);

        const Packet<4, float> extremePtDotPlaneNormal =
            Math::dot(planeNormal, extremePoints);

        return !(extremePtDotPlaneNormal > planeDist).isFalse();
    }
};

template<
    uint taSize>
struct RayPacket
{
    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    enum
    {
        SIZE = taSize,
        SIZE_SQRT = Vec3f<taSize>::SIZE_SQRT,
        CONTAINER_COUNT = Vec3f<taSize>::CONTAINER_COUNT
    };

    Vec3f<taSize> org;
    Vec3f<taSize> dir;
    Vec3f<taSize> invDir;

    Packet<taSize, float> tMin;
    Packet<taSize, float> tMax;

    Vec3f<taSize> dDdx;
    Vec3f<taSize> dDdy;
    Packet<taSize, float> deltaX;
    Packet<taSize, float> deltaY;

    Packet<taSize, int> randomSeed;
    Packet<taSize, int> primeBase;
    Vec3f<taSize> cumulativeFactor;

    RayFrustum frustum;

    uint depth;
    bool inside;
    uint maxDepth;

    uint rayType;

    RayPacket() :
        depth(0),
        inside(false),
        rayType(RayType::ShadingRay),
        maxDepth(RAYPACKET_MAXDEPTH),
        randomSeed(Packet<taSize, int>::replicate(0)),
        primeBase(Packet<taSize, int>::replicate(20))
    {}

    template<
        bool taCommonOrg>
    RTFACT_RAY_PACKET_INLINE void init(
        const ActiveMask<taSize>& aMask,
        const BBox& aBounds)
    {
        frustum.init<taCommonOrg>(org, dir, invDir, aMask, aBounds);
    }

    RTFACT_RAY_PACKET_INLINE const RayPacket<1> operator[](
        const uint aIndex) const;

    RTFACT_RAY_PACKET_INLINE bool dirSignsAgree() const
    {
        BitMask<taSize> mask;
        mask.setFalse();

        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
        {
            mask.setContainer(i,  dir(i).x.getSignIntMask()
                                ^ dir(i).y.getSignIntMask()
                                ^ dir(i).z.getSignIntMask());
        }

        return (mask.isTrue() || mask.isFalse());
    }

    template<
        bool taCommonOrg>
    RTFACT_RAY_PACKET_INLINE const Vec3f<taSize> evaluate(
        const Packet<taSize, float>& aDistance,
        const ActiveMask<taSize>& aRayMask) const
    {
        Vec3f<taSize> ret;

        if(taCommonOrg)
        {
            RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
            {
                ret(i) = org(0) + aDistance(i) * dir(i);
            }
        }
        else
        {
            RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
            {
                ret(i) = org(i) + aDistance(i) * dir(i);
            }
        }

        return ret;
    }
};

template<>
struct RayPacket<1>
{
    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    Vec3f1 org;
    Vec3f1 dir;
    Vec3f1 invDir;

    FPacket1 tMin;
    FPacket1 tMax;
    IPacket1 randomSeed;
    IPacket1 primeBase;
    Vec3f1 cumulativeFactor;

    Vec3f<1> dDdx;
    Vec3f<1> dDdy;
    Packet<1, float> deltaX;
    Packet<1, float> deltaY;

    uint depth;
    bool inside;
    uint maxDepth;

    uint rayType;

    RayPacket() :
        randomSeed(IPacket1::C_0()),
        primeBase(IPacket1::replicate(20)),
        depth(0),
        inside(false),
        maxDepth(RAYPACKET_MAXDEPTH),
        rayType(RayType::ShadingRay)
    {}

    RayPacket(
        const Vec3f1& aOrg,
        const Vec3f1& aDir,
        const float aEpsilon = RTFACT_RAY_EPSILON
    ) :
        org(aOrg),
        dir(aDir),
        invDir(aDir.reciprocal()),
        tMin(FPacket1::replicate(aEpsilon)),
        tMax(FPacket1::replicate(RTFACT_INFINITY)),
        rayType(RayType::ShadingRay),
        depth(0),
        maxDepth(RAYPACKET_MAXDEPTH),
        randomSeed(IPacket1::C_0()),
        primeBase(IPacket1::replicate(20))
    {}

    RayPacket(
        const Vec3f1& aOrg,
        const Vec3f1& aDir,
        const float aDistance,
        const float aEpsilon
    ) :
        org(aOrg),
        dir(aDir),
        invDir(aDir.reciprocal()),
        tMin(FPacket1::replicate(aEpsilon)),
        tMax(FPacket1::replicate(aDistance - aEpsilon)),
        depth(0),
        maxDepth(RAYPACKET_MAXDEPTH),
        rayType(RayType::ShadingRay),
        randomSeed(IPacket1::C_0()),
        primeBase(IPacket1::replicate(20))
    {}

    template<
        bool taCommonOrg>
    void init(
        const ActiveMask<1>&,
        const BBox&)
    {}

    RTFACT_RAY_PACKET_INLINE const RayPacket<1> operator[](
        const uint aIndex) const
    {
        RTFACT_ASSERT(aIndex == 0);

        return *this;
    }

    RTFACT_RAY_PACKET_INLINE bool dirSignsAgree() const
    {
        return true;
    }

    template<
        bool taCommonOrg>
    RTFACT_RAY_PACKET_INLINE const Vec3f<1> evaluate(
        const Packet<1, float>& aDistance,
        const ActiveMask<1>&) const
    {
        return org + aDistance * dir;
    }

    RTFACT_RAY_PACKET_INLINE const Vec3f<1> evaluate(
        const float aDistance) const
    {
        return org + aDistance * dir;
    }
};

template<
    uint taSize>
RTFACT_RAY_PACKET_INLINE const RayPacket<1> RayPacket<taSize>::operator[](
    const uint aIndex) const
{
    RTFACT_ASSERT(aIndex < taSize);

    RayPacket<1> ret;

    ret.org = org[aIndex];
    ret.dir = dir[aIndex];
    ret.invDir = invDir[aIndex];
    ret.tMin = tMin[aIndex];
    ret.tMax = tMax[aIndex];

    return ret;
}

template<>
RTFACT_RAY_PACKET_INLINE bool RayPacket<16>::dirSignsAgree() const
{
    BitMask<16> mask;
    mask.setFalse();

    mask.setContainer(0,  dir(0).x.getSignIntMask()
                        ^ dir(0).y.getSignIntMask()
                        ^ dir(0).z.getSignIntMask());

    mask.setContainer(1,  dir(1).x.getSignIntMask()
                        ^ dir(1).y.getSignIntMask()
                        ^ dir(1).z.getSignIntMask());

    mask.setContainer(2,  dir(2).x.getSignIntMask()
                        ^ dir(2).y.getSignIntMask()
                        ^ dir(2).z.getSignIntMask());

    mask.setContainer(3, dir(3).x.getSignIntMask()
                       ^ dir(3).y.getSignIntMask()
                       ^ dir(3).z.getSignIntMask());

    return (mask.isTrue() || mask.isFalse());
}

template<>
RTFACT_RAY_PACKET_INLINE bool RayPacket<4>::dirSignsAgree() const
{
    BitMask<4> mask = dir.x.getSignBitMask()
                    ^ dir.y.getSignBitMask()
                    ^ dir.z.getSignBitMask();

    return (mask.isTrue() || mask.isFalse());
}

typedef RayPacket<1> Ray;

RTFACT_NAMESPACE_END

#endif // RTFACT__RAYPACKET_HPP
