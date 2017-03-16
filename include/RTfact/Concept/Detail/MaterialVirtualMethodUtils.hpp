/* 
 *  MaterialVirtualMethodUtils.hpp
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
 
 /*
    Note: This file is NOT intended to be included directly.

          RTFACT_MATERIAL_GEN_VIRTAL_METHODS is defined in
          RTfact/Concept/Appearance/Material.hpp.
*/

#define  BOOST_PP_LOCAL_MACRO(n)  RTFACT__MATERIAL_GEN_VIRTAL_METHODS(n)
#define  BOOST_PP_LOCAL_LIMITS    (0, BOOST_PP_SEQ_SIZE(RTFACT_PACKET_SIZES) - 1)
#include BOOST_PP_LOCAL_ITERATE()
