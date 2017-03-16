/* 
 *  uray_check.py
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
 
 from builder.btools import RegisterCustomTest
from builder.btools import AddConfigKey
from builder.bconfig import getAutoconfPrefix
import os.path

def CheckURayUtils(ctx, write_config_h=False):
    ctx.Message('Checking for URayUtils header files... ')
    confprefix = getAutoconfPrefix(ctx.env)
    
    platform = ctx.env['PLATFORM']

    ret = ctx.TryLink("""
    #include "URayUtils/Config/URayConfig.hpp"

    using namespace URay;
    
    int main(int argc, char **argv) {
      uint32 i = 0;
      return 0;
    }
""", extension='.cpp')

    key = confprefix+'HAVE_URAY_UTILS'
    if not (write_config_h and AddConfigKey(ctx, key, ret)):
        # no config file is specified or it is disabled, use compiler options
        if ret:
            ctx.env.Append(CPPDEFINES=[key])
    
    ctx.Result(ret)
    return ret

RegisterCustomTest('CheckURayUtils', CheckURayUtils)

def CheckRTfact(ctx, write_config_h=True):
    ctx.Message('Checking for RTfact library... ')
    confprefix = getAutoconfPrefix(ctx.env)
    
    platform = ctx.env['PLATFORM']
    compiler = ctx.env['compiler']
    isGCC = (compiler == 'GCC')

    lastCCFLAGS = list(ctx.env.get('CCFLAGS', []))

    if isGCC:
        ctx.env.AppendUnique(CCFLAGS = ['-fpermissive'])

    ret = ctx.TryLink("""
    #include "URayUtils/Config/URayConfig.hpp"
    #include "RTfact/Config/Config.hpp"
    #include "RTfact/Concept/Scene.hpp"

    using namespace URay::RTfact;
    using namespace URay;
    
    int main(int argc, char **argv) {
      uint32 i = 0;
      return 0;
    }
""", extension='.cpp')

    key = confprefix+'HAVE_RTFACT'
    if not (write_config_h and AddConfigKey(ctx, key, ret)):
        # no config file is specified or it is disabled, use compiler options
        if ret:
            ctx.env.Append(CPPDEFINES=[key])

    ctx.env.Replace(CCFLAGS = lastCCFLAGS)
    
    ctx.Result(ret)
    return ret

RegisterCustomTest('CheckRTfact', CheckRTfact)
