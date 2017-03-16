/* 
 *  SurfaceShader.hpp
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
 *  Created on: 2009-03-06 23:28:11 +0100
 *  Author(s): Iliyan Georgiev, Petr Kmoch
 */
 
 #ifndef RTFACT__SURFACESHADER_HPP
#define RTFACT__SURFACESHADER_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Appearance.hpp>
#include <RTfact/Concept/Intersector.hpp>
#include <RTfact/Utils/SurfaceShaderContext.hpp>

#include <RTfact/Utils/SurfaceIntersection.hpp>

#include <boost/preprocessor/iteration/local.hpp>

RTFACT_NAMESPACE_BEGIN

#pragma region SurfaceShader class definition

template<
    class tContext>
class SurfaceShader :
    public Appearance
{
public:

    typedef tContext Context;

    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

private:

    #pragma region Generate pure virtual method declarations

    #define RTFACT__SURFACE_SHADER_GEN_BASE_VIRTAL_METHODS_HELPER(aIndex, aCommonOrgType, aStoreAlphaType)\
        virtual void shade_virtual(                                            \
            const RayPacket<RTFACT_PACKET_SIZE_AT(aIndex)>& aRayPacket,        \
            const ActiveMask<RTFACT_PACKET_SIZE_AT(aIndex)>& aRayMask,         \
            SurfaceIntersection<RTFACT_PACKET_SIZE_AT(aIndex)>& aIntersection, \
            tContext& aContext,                                                \
            Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& oResult,                     \
            Packet<RTFACT_PACKET_SIZE_AT(aIndex), float>* oResultAlpha,        \
            aCommonOrgType,                                                    \
            aStoreAlphaType) const = 0;

    #define RTFACT__SURFACE_SHADER_GEN_BASE_VIRTAL_METHODS(aIndex)             \
        RTFACT__SURFACE_SHADER_GEN_BASE_VIRTAL_METHODS_HELPER(aIndex, TrueType, TrueType)\
        RTFACT__SURFACE_SHADER_GEN_BASE_VIRTAL_METHODS_HELPER(aIndex, FalseType, TrueType)\
        RTFACT__SURFACE_SHADER_GEN_BASE_VIRTAL_METHODS_HELPER(aIndex, TrueType, FalseType)\
        RTFACT__SURFACE_SHADER_GEN_BASE_VIRTAL_METHODS_HELPER(aIndex, FalseType, FalseType)\
                                                                               \
        virtual void attenuateLight_virtual(                                   \
            const ActiveMask<RTFACT_PACKET_SIZE_AT(aIndex)>& aMask,            \
            SurfaceIntersection<RTFACT_PACKET_SIZE_AT(aIndex)>&,               \
            Mask<RTFACT_PACKET_SIZE_AT(aIndex)>& oMask,                        \
            Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& oIntensity) const = 0;

    #define  BOOST_PP_LOCAL_MACRO(n)  RTFACT__SURFACE_SHADER_GEN_BASE_VIRTAL_METHODS(n)
    #define  BOOST_PP_LOCAL_LIMITS    (0, RTFACT_PACKET_SIZES_SIZE - 1)
    #include BOOST_PP_LOCAL_ITERATE()

    #undef RTFACT__SURFACE_SHADER_GEN_BASE_VIRTAL_METHODS

    #pragma endregion

public:

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
        Packet<taSize,float>* oResultAlpha = RTFACT_NULL) const
    {
        RTFACT_APPEARANCE_ASSERT(!taStoreAlpha || oResultAlpha != RTFACT_NULL);
        shade_virtual(aRayPacket, aRayMask, aIntersection, aContext,
                      oResult, oResultAlpha, ConditionType<taCommonOrg>(), ConditionType<taStoreAlpha>());
    }

    template<
        uint taSize>
    RTFACT_APPEARANCE_INLINE void attenuateLight(
        const ActiveMask<taSize>& aMask,
        SurfaceIntersection<taSize>& aIntersection,
        Mask<taSize>& oMask,
        Vec3f<taSize>& oIntensity) const
    {
        attenuateLight_virtual(aMask, aIntersection, oMask, oIntensity);
    }
};

#pragma endregion // SurfaceShader class definition

#pragma region Instructions for adding new surface shaders

/*
    To add a new surface shader:

        1)  Inherit your shader class publicly from the abstract SurfaceShader,
            overriding all public methods it defines, providing
            custom implementation for them;

        2)  Put the following line at the very end of the class definition:

            #include RTFACT_SURFACE_SHADER_VIRTUAL_METHODS

            It will define auxiliary private virtual methods,
            unless a custom shader base class is defined.
*/

#pragma region Virtual method generation macro

#define RTFACT__SURFACE_SHADER_GEN_VIRTAL_METHODS_HELPER(aIndex, aCommonOrgType, aStoreAlphaType)\
    virtual void shade_virtual(                                                         \
        const RayPacket<RTFACT_PACKET_SIZE_AT(aIndex)>& aRayPacket,                     \
        const ActiveMask<RTFACT_PACKET_SIZE_AT(aIndex)>& aRayMask,                      \
        SurfaceIntersection<RTFACT_PACKET_SIZE_AT(aIndex)>& aIntersection,              \
        tContext& aContext,                                                             \
        Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& oResult,                                  \
        Packet<RTFACT_PACKET_SIZE_AT(aIndex), float>* oResultAlpha,                     \
        aCommonOrgType,                                                                 \
        aStoreAlphaType) const                                                          \
    {                                                                                   \
        shade<aCommonOrgType::VALUE,aStoreAlphaType::VALUE>(                            \
            aRayPacket, aRayMask, aIntersection, aContext, oResult, oResultAlpha);      \
    }

#define RTFACT__SURFACE_SHADER_GEN_VIRTAL_METHODS(aIndex)                               \
    RTFACT__SURFACE_SHADER_GEN_VIRTAL_METHODS_HELPER(aIndex, TrueType, TrueType)        \
    RTFACT__SURFACE_SHADER_GEN_VIRTAL_METHODS_HELPER(aIndex, FalseType, TrueType)       \
    RTFACT__SURFACE_SHADER_GEN_VIRTAL_METHODS_HELPER(aIndex, TrueType, FalseType)       \
    RTFACT__SURFACE_SHADER_GEN_VIRTAL_METHODS_HELPER(aIndex, FalseType, FalseType)      \
                                                                                        \
    virtual void attenuateLight_virtual(                                                \
        const ActiveMask<RTFACT_PACKET_SIZE_AT(aIndex)>& aMask,                         \
        SurfaceIntersection<RTFACT_PACKET_SIZE_AT(aIndex)>& aIntersection,              \
        Mask<RTFACT_PACKET_SIZE_AT(aIndex)>& oMask,                                     \
        Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& oIntensity) const                         \
    {                                                                                   \
        attenuateLight(aMask, aIntersection, oMask, oIntensity);                        \
    }

#pragma endregion

#define RTFACT_SURFACE_SHADER_VIRTUAL_METHODS                                  \
    <RTfact/Concept/Detail/SurfaceShaderVirtualMethodUtils.hpp>

#pragma endregion

RTFACT_NAMESPACE_END

#endif // RTFACT__SURFACESHADER_HPP
