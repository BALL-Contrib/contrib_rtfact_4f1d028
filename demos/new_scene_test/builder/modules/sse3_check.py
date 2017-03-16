/* 
 *  sse3_check.py
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
from builder.cpuinfo import *
import os.path

def CheckSSE3(ctx, write_config_h=False):
    ctx.Message('Checking for SSE3 availability... ')
    confprefix = getAutoconfPrefix(ctx.env)
    
    has_sse3 = cpu.has_sse3() is not None
    
    key = confprefix + 'HAVE_SSE3'
    if not (write_config_h and AddConfigKey(ctx, key, has_sse3)):
        # no config file is specified or it is disabled, use compiler options
        if has_sse3:
            ctx.env.Append(CPPDEFINES=[key])
    
    ctx.Result(has_sse3)
        
    return has_sse3

RegisterCustomTest('CheckSSE3', CheckSSE3)
