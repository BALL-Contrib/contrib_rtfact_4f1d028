/* 
 *  ParameterList.hpp
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
 *  Created on: 2010-02-04 19:25:21 +0100
 *  Author(s): Lukas Marsalek, Ken Dahm, Petr Kmoch
 */
 
 //System-wide
#line 1
SHADER_PARAMETER_MACRO(IGNORE,"Ignore",void,0)
SHADER_PARAMETER_MACRO(FBCOLOR,"FBColor",void,0)
SHADER_PARAMETER_MACRO(FBDISTANCE,"FBDistance",void,0)
SHADER_PARAMETER_MACRO(FBNORMAL,"FBNormal",void,0)
SHADER_PARAMETER_MACRO(FBAMBIENTOCCLUSION, "FBAmbientOcclusion",void,0)
SHADER_PARAMETER_MACRO(FBCOLORBLEEDING, "FBColorBleeding",void,0)
SHADER_PARAMETER_MACRO(WINDOWRES,"WindowRes",void,0)
SHADER_PARAMETER_MACRO(FOCUSDISTANCE,"FocusDistance",void,0)
SHADER_PARAMETER_MACRO(EYETRANSFORM,"EyeTransform",void,0)
//Effect-specific
SHADER_PARAMETER_MACRO(GAMMA,"Gamma",GLfloat,1)
SHADER_PARAMETER_MACRO(AORADIUS,"AORadius",GLfloat,1)
SHADER_PARAMETER_MACRO(AOTANGENTBIAS,"AOTangentBias",GLfloat,1)
SHADER_PARAMETER_MACRO(SSAO,"SSAO",void,0)
SHADER_PARAMETER_MACRO(BLURFALLOFF,"BlurFalloff",void,0)
SHADER_PARAMETER_MACRO(BLURSHARPNESS,"BlurSharpness",GLfloat,1)
SHADER_PARAMETER_MACRO(BLURKERNELRADIUS,"BlurKernelRadius",RTfact::uint,1)

SHADER_PARAMETER_MACRO(RTAO,"RTAO",void,0)
SHADER_PARAMETER_MACRO(RTAOLAMBDA,"AOBlurLambda",GLfloat,1)
SHADER_PARAMETER_MACRO(RTAOPHI,"AOBlurPhi",GLfloat,1)

SHADER_PARAMETER_MACRO(RTCB,"RTCB",void,0)
SHADER_PARAMETER_MACRO(RTCBLAMBDA,"CBBlurLambda",GLfloat,1)
SHADER_PARAMETER_MACRO(RTCBPHI,"CBBlurPhi",GLfloat,1)

#undef SHADER_PARAMETER_MACRO
