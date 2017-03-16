/* 
 *  AppearanceHandle.hpp
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
 *  Created on: 2010-09-26 18:55:36 +0200
 *  Author(s): Felix Klein
 */
 
 #ifndef RTFACT_HANDLE_APPEARANCE_HPP
#define RTFACT_HANDLE_APPEARANCE_HPP

#include <RTpieCpp/Config/Common.hpp>

#include <RTpie/Scene/IAppearance.hpp>

#define IMPL_INTERFACE RTfact::RTpie::IAppearance
WRAPPER_FWD(tAppearanceHandle)
#undef IMPL_INTERFACE

#include <RTpieCpp/Base/ParameterizableHandle.hpp>

#define IMPL_INTERFACE RTfact::RTpie::IAppearance
WRAPPER_START(AppearanceHandle, ParameterizableHandle )

    const char * getType()
    IMPLEMENT_WRAPPER_ARG0(const char *, GetType)

WRAPPER_END
#undef IMPL_INTERFACE

#endif // RTFACT_HANDLE_APPEARACNE_HPP
