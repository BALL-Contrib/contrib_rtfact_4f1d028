/* 
 *  Light.hpp
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
 
#ifndef RTFACT__LIGHT_HPP
#define RTFACT__LIGHT_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Packets/Packet.hpp>
#include <RTfact/Utils/Packets/Vec3f.hpp>
#include <RTfact/Utils/ReferenceCounted.hpp>

RTFACT_NAMESPACE_BEGIN

#pragma region Light class definition

class Light :
    public ReferenceCounted
{
public:

    typedef Light LightBase;
        
    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

private:
    
    #pragma region Generate pure virtual method declarations

    #define RTFACT__LIGHT_GEN_BASE_VIRTAL_METHODS(aIndex)                      \
        virtual void sample_virtual(                                           \
            const Vec2f<RTFACT_PACKET_SIZE_AT(aIndex)>& aPositionSamples,      \
            const Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& aPoint,                \
            const ActiveMask<RTFACT_PACKET_SIZE_AT(aIndex)>& aMask,            \
            Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& oDirection,                  \
            Packet<RTFACT_PACKET_SIZE_AT(aIndex), float>& oDistance,           \
            Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& oRadiance,                   \
            Vec3f<1> *oCommonSamplePosition = NULL) const = 0;                 \
                                                                               \
        virtual void sample_virtual(                                           \
            const Vec2f<RTFACT_PACKET_SIZE_AT(aIndex)>& aPositionSamples,      \
            const Vec2f<RTFACT_PACKET_SIZE_AT(aIndex)>& aDirectionSamples,     \
            const ActiveMask<RTFACT_PACKET_SIZE_AT(aIndex)>& aMask,            \
            Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& oPosition,                   \
            Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& oDirection,                  \
            Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& oRadiance,                   \
            Packet<RTFACT_PACKET_SIZE_AT(aIndex), float>& oPdf) const = 0;     \



    #define  BOOST_PP_LOCAL_MACRO(n)  RTFACT__LIGHT_GEN_BASE_VIRTAL_METHODS(n)
    #define  BOOST_PP_LOCAL_LIMITS    (0, RTFACT_PACKET_SIZES_SIZE - 1)
    #include BOOST_PP_LOCAL_ITERATE()

    #undef RTFACT__LIGHT_GEN_BASE_VIRTAL_METHODS

    #pragma endregion

protected:

    enum
    {
        IS_DELTA              = 1,
        HAS_COMMON_SAMPLE_POS = 1 << 1,
        CASTS_SHADOWS         = 1 << 2
    };
    
    umask mProperties;

    Light(
        const umask aProperties
    ) :
        mProperties(aProperties)
    {}

    RTFACT_FORCE_INLINE bool getProperties(
        const umask aProperties) const
    {
        return RTFACT_CHECK_UMASK(mProperties, aProperties);
    }

    RTFACT_FORCE_INLINE void setProperties(
        const umask aProperties)
    {
        mProperties |= aProperties;
    }

    RTFACT_FORCE_INLINE void unsetProperties(
        const umask aProperties)
    {
        mProperties &= ~aProperties;
    }

public:

    virtual ~Light()
    {}

    template<
        uint taSize>
    RTFACT_FORCE_INLINE void illuminatePoint(
        const Vec2f<taSize>& aPositionSamples,
        const Vec3f<taSize>& aPoint,
        const ActiveMask<taSize>& aMask,
        Vec3f<taSize>& oDirection,
        Packet<taSize, float>& oDistance,
        Vec3f<taSize>& oRadiance,
        Vec3f<1> *oCommonSamplePosition = NULL) const    
    {
        sample_virtual(
            aPositionSamples,
            aPoint, aMask, oDirection, oDistance, oRadiance,
            oCommonSamplePosition);
    }

    template<
        uint taSize>
    RTFACT_LIGHT_INLINE void sample(
        const Vec2f<taSize>& aPositionSamples,
        const Vec2f<taSize>& aDirectionSamples,
        const ActiveMask<taSize>& aMask,
        Vec3f<taSize>& oPosition,
        Vec3f<taSize>& oDirection,
        Vec3f<taSize>& oRadiance,
        Packet<taSize, float>& oPdf) const
    {
        sample_virtual(
            aPositionSamples, aDirectionSamples, aMask,
            oPosition, oDirection, oRadiance, oPdf);
    }

    RTFACT_LIGHT_INLINE virtual const Vec3f<1> getTotalPower() const = 0;

    RTFACT_LIGHT_INLINE virtual void setBoundingSphere(const BoundingSphere& aBSphere)
    {}

    RTFACT_LIGHT_INLINE virtual BoundingSphere* getBoundingSphere()
    {
        return NULL;
    }

    RTFACT_FORCE_INLINE bool getIsDelta() const
    {
        return getProperties(IS_DELTA);
    }

    RTFACT_FORCE_INLINE bool getHasCommonSamplePosition() const
    {
        return getProperties(HAS_COMMON_SAMPLE_POS);
    }

    RTFACT_FORCE_INLINE bool getCastsShadows() const
    {
        return getProperties(CASTS_SHADOWS);
    }

    RTFACT_FORCE_INLINE void setCastsShadows(
        const bool aState)
    {
        if(aState)
        {
            setProperties(CASTS_SHADOWS);
        }
        else
        {
            unsetProperties(CASTS_SHADOWS);
        }
    }
};

#pragma endregion

#pragma region Instructions for adding new lights

/*
    To add a new light type:

        1)  Inherit your light class publicly from the abstract Light,
            overriding all public methods it defines, providing
            custom implementation for them;

        2)  Put the following line at the very end of the class definition:

            #include RTFACT_LIGHT_VIRTUAL_METHODS

            It will define auxiliary private virtual methods,
            unless a custom light base class is defined.

*/

#pragma region Virtual method generation macro

#define RTFACT__LIGHT_GEN_VIRTAL_METHODS(aIndex)                               \
    virtual void sample_virtual(                                               \
        const Vec2f<RTFACT_PACKET_SIZE_AT(aIndex)>& aPositionSamples,          \
        const Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& aPoint,                    \
        const ActiveMask<RTFACT_PACKET_SIZE_AT(aIndex)>& aMask,                \
        Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& oDirection,                      \
        Packet<RTFACT_PACKET_SIZE_AT(aIndex), float>& oDistance,               \
        Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& oRadiance,                       \
        Vec3f<1> *oCommonSamplePosition = NULL) const                          \
    {                                                                          \
        illuminatePoint<RTFACT_PACKET_SIZE_AT(aIndex)>(                        \
            aPositionSamples,                                                  \
            aPoint, aMask, oDirection, oDistance, oRadiance,                   \
            oCommonSamplePosition);                                            \
    }                                                                          \
                                                                               \
    virtual void sample_virtual(                                               \
        const Vec2f<RTFACT_PACKET_SIZE_AT(aIndex)>& aPositionSamples,          \
        const Vec2f<RTFACT_PACKET_SIZE_AT(aIndex)>& aDirectionSamples,         \
        const ActiveMask<RTFACT_PACKET_SIZE_AT(aIndex)>& aMask,                \
        Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& oPosition,                       \
        Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& oDirection,                      \
        Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& oRadiance,                       \
        Packet<RTFACT_PACKET_SIZE_AT(aIndex), float>& oPdf) const              \
    {                                                                          \
        sample<RTFACT_PACKET_SIZE_AT(aIndex)>(                                 \
            aPositionSamples, aDirectionSamples, aMask,                        \
            oPosition, oDirection, oRadiance, oPdf);                           \
    }

#pragma endregion

#define RTFACT_LIGHT_VIRTUAL_METHODS                                           \
    <RTfact/Concept/Detail/LightVirtualMethodUtils.hpp>

#pragma endregion

RTFACT_NAMESPACE_END

#endif // RTFACT__LIGHT_HPP
