/* 
 *  Material.hpp
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
 *  Created on: 2009-03-06 23:28:11 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__MATERIAL_HPP
#define RTFACT__MATERIAL_HPP

#include <RTfact/Config/Common.hpp>

#include <boost/preprocessor/iteration/local.hpp>

#include <RTfact/Concept/Appearance.hpp>

#include <RTfact/Utils/SurfaceIntersection.hpp>
#include <RTfact/Utils/Math/SamplingUtils.hpp>

RTFACT_NAMESPACE_BEGIN

#pragma region BxDFType enum definition

enum BxDFType
{
    BSDF_REFLECTION       = 1 << 0,
    BSDF_TRANSMISSION     = 1 << 1,
    BSDF_DIFFUSE          = 1 << 2,
    BSDF_GLOSSY           = 1 << 3,
    BSDF_SPECULAR         = 1 << 4,
    BSDF_ALL_TYPES        = BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR,
    BSDF_ALL_REFLECTION   = BSDF_REFLECTION | BSDF_ALL_TYPES,
    BSDF_ALL_TRANSMISSION = BSDF_TRANSMISSION | BSDF_ALL_TYPES,
    BSDF_ALL              = BSDF_ALL_REFLECTION | BSDF_ALL_TRANSMISSION
};

#pragma endregion

#pragma region Material class definition

class Material :
    public Appearance
{
public:

    typedef Material AppearanceBase;
    
    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

private:

    bool mIsDelta;

    #pragma region Generate pure virtual method declarations

    #define RTFACT__MATERIAL_GEN_BASE_VIRTAL_METHODS(aIndex)                   \
        virtual void evaluate_virtual(                                         \
            const Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& aOutgoingDir,          \
            const Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& aIncomingDir,          \
            SurfaceIntersection<RTFACT_PACKET_SIZE_AT(aIndex)>& aIntersection, \
            const ActiveMask<RTFACT_PACKET_SIZE_AT(aIndex)>& aMask,            \
            Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& oResult,                     \
            const BxDFType aBxDFType = BSDF_ALL) const = 0;                    \
                                                                               \
        virtual void emittance_virtual(                                        \
            const Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& aOutgoingDir,          \
            SurfaceIntersection<RTFACT_PACKET_SIZE_AT(aIndex)>& aIntersection, \
            const ActiveMask<RTFACT_PACKET_SIZE_AT(aIndex)>& aMask,            \
            Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& oResult)const = 0;

    #define  BOOST_PP_LOCAL_MACRO(n)  RTFACT__MATERIAL_GEN_BASE_VIRTAL_METHODS(n)
    #define  BOOST_PP_LOCAL_LIMITS    (0, RTFACT_PACKET_SIZES_SIZE - 1)
    #include BOOST_PP_LOCAL_ITERATE()

    #undef RTFACT__MATERIAL_GEN_BASE_VIRTAL_METHODS

    #pragma endregion

public:

    Material(
        const bool aIsDelta
    ) :
        mIsDelta(aIsDelta)
    {}

    template<
        uint taSize>
    RTFACT_APPEARANCE_INLINE void evaluate(
        const Vec3f<taSize>& aOutgoingDir,
        const Vec3f<taSize>& aIncomingDir,
        SurfaceIntersection<taSize>& aIntersection,
        const ActiveMask<taSize>& aMask,
        Vec3f<taSize>& oResult,
        const BxDFType aBxDFType = BSDF_ALL) const
    {
        evaluate_virtual(aOutgoingDir, aIncomingDir, aIntersection, aMask, oResult, aBxDFType);
    }

    template<
        uint taSize>
    RTFACT_APPEARANCE_INLINE void emittance(
        const Vec3f<taSize>& aOutgoingDir,
        SurfaceIntersection<taSize>& aIntersection,
        const ActiveMask<taSize>& aMask,
        Vec3f<taSize>& oResult) const
    {
        return emittance_virtual(aOutgoingDir, aIntersection, aMask, oResult);
    }

    RTFACT_APPEARANCE_INLINE bool isDelta() const
    {
        return mIsDelta;
    }
};

#pragma endregion

#pragma region Instructions for adding new materials

/*
    To add a new material:

        1)  Inherit your material class publicly from the abstract Material,
            overriding all public methods it defines, providing
            custom implementation for them;

        2)  Put the following line at the very end of the class definition:

                #include RTFACT_MATERIAL_VIRTUAL_METHODS

            It will define auxiliary private virtual methods,
            unless a custom material base class is defined.

*/

#pragma region Virtual method generation macro

#define RTFACT__MATERIAL_GEN_VIRTAL_METHODS(aIndex)                            \
    void evaluate_virtual(                                                     \
        const Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& aOutgoingDir,              \
        const Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& aIncomingDir,              \
        SurfaceIntersection<RTFACT_PACKET_SIZE_AT(aIndex)>& aIntersection,     \
        const ActiveMask<RTFACT_PACKET_SIZE_AT(aIndex)>& aMask,                \
        Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& oResult,                         \
        const BxDFType aBxDFType = BSDF_ALL) const                             \
    {                                                                          \
        return evaluate<RTFACT_PACKET_SIZE_AT(aIndex)>(                        \
            aOutgoingDir, aIncomingDir, aIntersection, aMask, oResult, aBxDFType);\
    }                                                                          \
                                                                               \
    void emittance_virtual(                                                    \
        const Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& aOutgoingDir,              \
        SurfaceIntersection<RTFACT_PACKET_SIZE_AT(aIndex)>& aIntersection,     \
        const ActiveMask<RTFACT_PACKET_SIZE_AT(aIndex)>& aMask,                \
        Vec3f<RTFACT_PACKET_SIZE_AT(aIndex)>& oResult) const                   \
    {                                                                          \
        return emittance<RTFACT_PACKET_SIZE_AT(aIndex)>(                       \
            aOutgoingDir, aIntersection, aMask, oResult);                      \
    }

#pragma endregion

#define RTFACT_MATERIAL_VIRTUAL_METHODS                                        \
    <RTfact/Concept/Detail/MaterialVirtualMethodUtils.hpp>

#pragma endregion

RTFACT_NAMESPACE_END

#endif // RTFACT__MATERIAL_HPP
