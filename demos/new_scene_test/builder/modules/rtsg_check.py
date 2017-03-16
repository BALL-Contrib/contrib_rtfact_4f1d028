/* 
 *  rtsg_check.py
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
 *  Created on: 2009-02-24 16:01:56 +0100
 *  Author(s): Felix Klein
 */
 
 import os
from builder.btools import RegisterCustomTest
from builder.btools import AddConfigKey
from builder.bconfig import getAutoconfPrefix

def CheckRTSG(ctx, write_config_h=False):
    ctx.Message('Checking for RTSG library... ')
    confprefix = getAutoconfPrefix(ctx.env)

    platform = ctx.env['PLATFORM']

    RTSG_DIST_DIR = ctx.env.GetEnvVar('RTSG_DIST_DIR')
    
    if RTSG_DIST_DIR:
        if platform == 'win32':
            rtsgLibFile = 'rtsg.dll'
        else:
            rtsgLibFile = 'librtsg.so'

        libDir = os.path.join(RTSG_DIST_DIR, 'lib')
        rtsgLib = os.path.join(libDir, rtsgLibFile)
        if not os.path.exists(rtsgLib):
            libDir = os.path.join(RTSG_DIST_DIR, 'build', 'lib')
            rtsgLib = os.path.join(libDir, rtsgLibFile)
            if not os.path.exists(rtsgLib):
                ctx.Message('Warning: could not find RTSG library %s. '\
                            % (rtsgLib,))
                ctx.Result(0)
                return 0
        
        incDir = os.path.join(RTSG_DIST_DIR, 'include')
        if not os.path.exists(incDir):
            incDir = os.path.join(RTSG_DIST_DIR, '..', 'include')
            if not os.path.exists(incDir):
                ctx.Message('Warning: could not find RTSG include dir %s. '\
                            % (incDir,))
                ctx.Result(0)
                return 0

        ctx.env.Append(LIBPATH=os.path.abspath(libDir))
        ctx.env.Append(CPPPATH=os.path.abspath(incDir))

    lastLIBS = list(ctx.env.get('LIBS', []))
    
    ctx.env.Append(LIBS = ['rtsg'])
    
    ret = ctx.TryLink("""
    #include "rtsg/base/Config.hpp"
    #include "rtsg/base/Vec3.hpp"
    using namespace rtsg;

    int main(int argc, char **argv)
    {
      Vec3f v(1,2,3);
      return 0;
    }
""", extension='.cpp')
    
    ctx.env.Replace(LIBS = lastLIBS)

    key = confprefix+'HAVE_RTSG'
    if not (write_config_h and AddConfigKey(ctx, key, ret)):
        # no config file is specified or it is disabled, use compiler options
        if ret:
            ctx.env.Append(CPPDEFINES=[key])

    ctx.Result(ret)
    return ret

RegisterCustomTest('CheckRTSG', CheckRTSG)

def CheckRTSG_OpenRT(ctx, write_config_h=True):
    # We assume here that RTSG is available
    ctx.Message('Checking for RTSG/OpenRT library... ')
    confprefix = getAutoconfPrefix(ctx.env)
    
    platform = ctx.env['PLATFORM']

    OPENRT_INCLUDE_DIR = ctx.env.get('OPENRT_INCLUDE_DIR', '')
    
    lastLIBS = list(ctx.env.get('LIBS', []))
    lastCPPPATH = list(ctx.env.get('CPPPATH', []))
    
    rtsglibs = ['rtsg']
    
    rtsgopenrtlibs = ['rtsgortext']
        
    ctx.env.Append(LIBS = rtsgopenrtlibs + rtsglibs)
    ctx.env.Append(CPPPATH = OPENRT_INCLUDE_DIR)
    ret = ctx.TryLink("""
        #include <openrt/rt.h>
        #include <openrt/types.h>
        #include <rtsg/openrt/OpenRTRenderer.hpp>
        int main(int argc, char **argv) {
            ortext::OpenRTRenderer *r = 0;
            return 0;
        }
""", extension='.cpp')
    ctx.env.Replace(LIBS = lastLIBS)
    ctx.env.Replace(CPPPATH = lastCPPPATH)
    if ret:
        ctx.env.Replace(LIBRTSGOPENRT=rtsgopenrtlibs)
        
    key = confprefix+'HAVE_LIBRTSG_OPENRT'
    if not (write_config_h and AddConfigKey(ctx, key, ret)):
        # no config file is specified or it is disabled, use compiler options
        if ret:
            ctx.env.Append(CPPDEFINES=[key])
        
    ctx.Result(ret)
    return ret

RegisterCustomTest('CheckRTSG_OpenRT', CheckRTSG_OpenRT)
