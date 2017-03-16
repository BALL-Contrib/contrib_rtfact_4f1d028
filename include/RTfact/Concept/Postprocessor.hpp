/* 
 *  Postprocessor.hpp
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
 *  Created on: 2010-06-11 12:01:36 +0200
 *  Author(s): Lukas Marsalek, Petr Kmoch
 */
 
 #ifndef RTFACT__POSTPROCESSOR_HPP
#define RTFACT__POSTPROCESSOR_HPP

#include <RTfact/Config/Common.hpp>

RTFACT_NAMESPACE_BEGIN

class Postprocessor
{
public:
    template <
        typename tColorImage2D,
        typename tDistanceImage2D,
        typename tNormalImage2D>
    void fillFBTextures(
        const tColorImage2D &aColorImage,
        const tDistanceImage2D &aDistanceImage,
        const tNormalImage2D &aNormalImage,
        const uint aTexX,
        const uint aTexY) const;
    
    void render(
        const uint aResX,
        const uint aResY,
        const uint aTexX,
        const uint aTexY) const;
};

RTFACT_NAMESPACE_END

#endif // RTFACT__POSTPROCESSOR_HPP
