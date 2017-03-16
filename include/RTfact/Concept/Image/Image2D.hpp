/* 
 *  Image2D.hpp
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
 *  Created on: 2008-07-16 17:43:42 +0200
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__IMAGE2D_HPP
#define RTFACT__IMAGE2D_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Concept/Image.hpp>

RTFACT_NAMESPACE_BEGIN

class Image2D :
    public Image
{
public:

    uint getResX() const;
    
    uint getResY() const;

    uint getComponentCount() const;

    uint getPitch() const;

    const Component* getFirstComponent(
        const uint aX,
        const uint aY) const;

    Component* getFirstComponent(
        const uint aX,
        const uint aY);
};

RTFACT_NAMESPACE_END

#endif // RTFACT__IMAGE2D_HPP