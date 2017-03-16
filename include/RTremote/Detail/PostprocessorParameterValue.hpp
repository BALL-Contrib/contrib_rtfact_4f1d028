/* 
 *  PostprocessorParameterValue.hpp
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
 *  Created on: 2010-06-11 12:47:29 +0200
 *  Author(s): Lukas Marsalek
 */
 
 #if BOOST_PP_ITERATION_DEPTH() == 1 //type iteration

#define ARG_TYPE() BOOST_PP_SEQ_ELEM(BOOST_PP_FRAME_ITERATION(1),RTFACT_POSTPROCESSORPARAMETER_TYPES)

#define BOOST_PP_ITERATION_PARAMS_2 (3,(1,4,<RTremote/Detail/PostprocessorParameterValue.hpp>))
#include BOOST_PP_ITERATE()

void getValue(ARG_TYPE() *aValue) const;

const bool setValue(const ARG_TYPE() *aValue);

#undef ARG_TYPE

#elif BOOST_PP_ITERATION_DEPTH() == 2 //argument count iteration

void getValue(BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(),ARG_TYPE() &aValue)) const;

const bool setValue(BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(),const ARG_TYPE() aValue));

#endif
