/* 
 *  XML3DMatteShader.hpp
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
 *  Created on: 2010-10-13 16:38:43 +0200
 *  Author(s): Felix Klein
 */
 
 #ifndef RTFACT__XML3DMATTESHADER_HPP
#define RTFACT__XML3DMATTESHADER_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Appearance/SurfaceShader.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tContext,
    class tTexture>
class XML3DMatteShader :
    public SurfaceShader<tContext>
{
    tTexture mTexture;
	Vec3f1 mColor;

public:

	XML3DMatteShader() :
		mColor(1.0f, 1.0f, 1.0f)
	{}

    XML3DMatteShader(
        const tTexture& aTexture
    ) :
        mTexture(aTexture),
		mColor(1.0f, 1.0f, 1.0f)
    {}

    tTexture &getTexture() { return mTexture; }

    RTFACT_DEFINE_PROPERTY(Vec3f1&,          Color);
    RTFACT_DEFINE_PROPERTY(tTexture&, Texture);



    template<
        bool taCommonOrg,
        bool taStoreAlpha,
        uint taSize>
    RTFACT_APPEARANCE_INLINE void shade(
        const RayPacket<taSize>& aRayPacket,
        const ActiveMask<taSize>& aRayMask,
        SurfaceIntersection<taSize>& aIntersection,
        tContext& aContext,
        Vec3f<taSize>& oResult,
        Packet<taSize, float>* oResultAlpha = RTFACT_NULL) const
    {
        RTFACT_APPEARANCE_ASSERT(!taStoreAlpha || oResultAlpha != RTFACT_NULL);

        typedef Vec3f<taSize>                 t_Vec3f;
        typedef Packet<taSize, float>         t_FPacket;
        typedef typename t_FPacket::Container t_FPacketC;
        typedef typename t_Vec3f::Container   t_Vec3fC;

        const t_Vec3fC colorC = t_Vec3fC::replicate(mColor);

        t_Vec3f texColor;

        mTexture.template sample<true, false>(
                aIntersection, aRayMask, &texColor);

		RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
        {
			oResult(i) = aRayMask(i).iif(texColor(i)*colorC(i), oResult(i));
			if (taStoreAlpha)
			{
				RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aRayMask)
				{
					(*oResultAlpha)(i) = aRayMask(i).iif(
						t_FPacketC::C_1,
						(*oResultAlpha)(i));
				}
			}
		}


    }

    template<
        bool taCommonOrg,
        bool taStoreAlpha>
    RTFACT_APPEARANCE_INLINE void shade(
        const RayPacket<1>& aRayPacket,
        const ActiveMask<1>& aRayMask,
        SurfaceIntersection<1>& aIntersection,
        tContext& aContext,
        Vec3f<1>& oResult,
        Packet<1, float>* oResultAlpha = RTFACT_NULL) const
    {}

    template<
        uint taSize>
    RTFACT_APPEARANCE_INLINE void attenuateLight(
        const ActiveMask<taSize>& aMask,
        SurfaceIntersection<taSize>& aIntersection,
        Mask<taSize>& oMask,
        Vec3f<taSize>& oIntensity) const
    {
        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            oMask(i) &= ~aMask(i);
        }
    }

    #include RTFACT_SURFACE_SHADER_VIRTUAL_METHODS
};

RTFACT_NAMESPACE_END

#endif // RTFACT__XML3DMATTESHADER_HPP
