/* 
 *  TestDataIntersector.hpp
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
 *  Created on: 2008-10-03 11:54:00 +0200
 *  Author(s): Felix Klein, Iliyan Georgiev
 */
 
 #ifndef RTFACT__TESTDATAINTERSECTOR_HPP_INCLUDED
#define RTFACT__TESTDATAINTERSECTOR_HPP_INCLUDED

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Primitive.hpp>
#include <RTfact/Utils/RayPacket.hpp>
#include <RTfact/Utils/System/TimeMeasurer.hpp>

RTFACT_NAMESPACE_BEGIN

template<class tIntersector>
class TestDataIntersector :
    public tIntersector
{
public:

    template<
        uint taSize>
    class Result :
        public tIntersector::template Result<taSize>
    {
        typedef typename tIntersector::template Result<taSize> t_Base;

    public:

        using t_Base::SIZE;
        using t_Base::mask;
        using t_Base::dist;
        using t_Base::u;
        using t_Base::v;
        using t_Base::appearanceID;
        using t_Base::geomNormal;
        using t_Base::normal;

        Packet<taSize, float> testData;

        Result(){
            testData = Packet<taSize, float>::replicate(0.f);
        }

    };




    template<
        umask taIntersectionData,
        bool taCommonOrg,
        class tPrimitive,
        uint taSize>
    RTFACT_INTERSECTOR_INLINE void intersect(
        const RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        tPrimitive& aPrimitive,
        Result<taSize>& oResult) const
    {
        oResult.testData += Packet<taSize, float>::C_1;
        tIntersector::template intersect<
        taIntersectionData, taCommonOrg>(
            aRayPacket,
            aRayMask,
            aPrimitive,
            oResult);
    }
    /*
    template<
    umask taIntersectionData,
    uint taSize>
    RTFACT_FORCE_INLINE void finalizeResult(
        Result<taSize>& oResult)
    {   
        tIntersector::template finalizeResult<taIntersectionData, taSize>(oResult );
        for(int i=0; i<taSize; ++i){
            if(oResult.mask[i]){
                
                Vec3f<1> v = (oResult.instanceBase + 
                    oResult.instanceOffsets[i])->trans.applyToNormalAffine(oResult.normal[i]);
                v.setNormalized();
                oResult.normal.xAt(i) = v.x;
                oResult.normal.yAt(i) = v.y;
                oResult.normal.zAt(i) = v.z;
                //oResult.normal[i] = (oResult.instanceBase + 
                //    oResult.instanceOffsets[i])->trans.applyToNormalAffine(oResult.normal[i]);
                
                Vec3f<1> v2 = (oResult.instanceBase + 
                    oResult.instanceOffsets[i])->trans.applyToNormalAffine(oResult.normal[i]);
                v2.setNormalized();
                oResult.normal.xAt(i) = v2.x;
                oResult.normal.yAt(i) = v2.y;
                oResult.normal.zAt(i) = v2.z;
            }
        }
    }
    */
};

RTFACT_NAMESPACE_END

#endif // RTFACT__INSTANCEINTERSECTOR_HPP_INCLUDED
