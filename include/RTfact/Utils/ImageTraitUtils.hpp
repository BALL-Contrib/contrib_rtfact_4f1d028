/* 
 *  ImageTraitUtils.hpp
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
 *  Created on: 2009-03-04 02:36:42 +0100
 *  Author(s): Iliyan Georgiev, Alexander Löffler
 */
 
 #ifndef RTFACT__IMAGETRAITUTILS_HPP
#define RTFACT__IMAGETRAITUTILS_HPP

#include <RTfact/Config/Common.hpp>
#include <RTfact/Utils/Math/MathUtils.hpp>

RTFACT_NAMESPACE_BEGIN

template<
    class tComponent>
struct ImageComponentRange
{
    static RTFACT_FORCE_INLINE tComponent min()
    {
        return static_cast<tComponent>(0);
    }

    static RTFACT_FORCE_INLINE tComponent max()
    {
        return std::numeric_limits<tComponent>::max();
    }
};

template<>
struct ImageComponentRange<float>
{
    static RTFACT_FORCE_INLINE float min()
    {
        return 0.f;
    }

    static RTFACT_FORCE_INLINE float max()
    {
        return 1.f;
    }
};

template<
    class tInputComponent,
    class tOutputComponent>
struct ImageComponentConverter
{
    static RTFACT_FORCE_INLINE tOutputComponent convert(
        const tInputComponent aValue)
    {
        const float valueNormalized =
            static_cast<float>(aValue) *
            (1.f / static_cast<float>(std::numeric_limits<tInputComponent>::max()));

        return static_cast<tOutputComponent>(
            valueNormalized *
            static_cast<float>(std::numeric_limits<tOutputComponent>::max()));
    }
};

template<
    class tComponent>
struct ImageComponentConverter<tComponent, tComponent>
{
    static RTFACT_FORCE_INLINE tComponent convert(
        const tComponent aValue)
    {
        return aValue;
    }
};

template<
    class tComponent>
struct ImageComponentConverter<tComponent, float>
{
    static RTFACT_FORCE_INLINE float convert(
        const tComponent aValue)
    {
        return
            static_cast<float>(aValue) *
            (1.f / static_cast<float>(std::numeric_limits<tComponent>::max()));
    }
};

template<
    class tComponent>
struct ImageComponentConverter<float, tComponent>
{
    static RTFACT_FORCE_INLINE tComponent convert(
        const float aValue)
    {
        return static_cast<tComponent>(
            Math::clamp(
                aValue,
                ImageComponentRange<float>::min(),
                ImageComponentRange<float>::max()) *
            static_cast<float>(std::numeric_limits<tComponent>::max()));
    }
};

template<>
struct ImageComponentConverter<float, float>
{
    static RTFACT_FORCE_INLINE float convert(
        const float aValue)
    {
        return aValue;
    }
};

template<
    class tType>
struct ImageTraits
{
    typedef typename tType::Component Component;

    template<
        class tComponent>
    static RTFACT_FORCE_INLINE tComponent convertComponent(
        const Component aValue)
    {
        return ImageComponentConverter<Component, tComponent>::convert(aValue);
    }

    template<
        class tComponent>
    static RTFACT_FORCE_INLINE Component toComponent(
        const tComponent aValue)
    {
        return ImageComponentConverter<tComponent, Component>::convert(aValue);
    }

    static RTFACT_FORCE_INLINE Component componentMin()
    {
        return ImageComponentRange<Component>::min();
    }

    static RTFACT_FORCE_INLINE Component componentMax()
    {
        return ImageComponentRange<Component>::max();
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__IMAGETRAITUTILS_HPP
