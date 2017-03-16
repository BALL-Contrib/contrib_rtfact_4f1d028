/* 
 *  inimage_check.py
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

def CheckInImage(ctx, write_config_h=True, add_to_cppdefines=False):
    ctx.Message('Checking for InImage library... ')
    confprefix = getAutoconfPrefix(ctx.env)
    
    platform = ctx.env['PLATFORM']
    ROOT_DIR = ctx.env.GetRootDir()
    INIMAGE_DIST_DIR = ctx.env.GetEnvVar('INIMAGE_DIST_DIR')

    if not INIMAGE_DIST_DIR:
        # is inImage a sub module ?
        p = os.path.join(ROOT_DIR, 'inImage')
        if not os.path.exists(p):
            # is inImage one dir higher ?
            p = os.path.join(ROOT_DIR, '..', 'inImage')

        if os.path.exists(p):
            INIMAGE_DIST_DIR = p
        else:
            INIMAGE_DIST_DIR = None
            ctx.Message('Warning: could not find inImage, you need to define INIMAGE_DIST_DIR environment variable. ')


    if INIMAGE_DIST_DIR:
        
        if platform == 'win32':
            imgLibFile = 'InImage.dll'
        else:
            imgLibFile = 'libInImage.so'

        libDir = os.path.join(INIMAGE_DIST_DIR, 'lib')
        imgLib = os.path.join(libDir, imgLibFile)
        if not os.path.exists(imgLib):
            libDir = os.path.join(INIMAGE_DIST_DIR, 'build', 'lib')
            imgLib = os.path.join(libDir, imgLibFile)
            if not os.path.exists(imgLib):
                ctx.Message('Warning: could not find inImage library %s. ' % (imgLib,))
                if write_config_h:
                    AddConfigKey(ctx, confprefix+'HAVE_LIBINIMAGE', 0)
                ctx.Result(0)
                return 0
                
        incDir = os.path.join(INIMAGE_DIST_DIR, 'include')
        if not os.path.exists(incDir):
            incDir = os.path.join(INIMAGE_DIST_DIR, '..', 'include')
            if not os.path.exists(incDir):
                ctx.Message('Warning: could not find inImage include dir %s. ' % (incDir,))
                if write_config_h:
                    AddConfigKey(ctx, confprefix+'HAVE_LIBINIMAGE', 0)
                ctx.Result(0)
                return 0

        ctx.env.Replace(INIMAGE_LIB_DIR=os.path.abspath(libDir))
        ctx.env.Replace(INIMAGE_INCLUDE_DIR=os.path.abspath(incDir))

        ctx.env.DeclarePackage('inimage',
                               CPPPATH=[os.path.abspath(incDir)],
                               LIBPATH=[os.path.abspath(libDir)],
                               LIBS=['InImage'],
                               trigger_libs=['InImage', 'inimage'])
        
        ret = 1
    else:
        ret = 0

    key = confprefix+'HAVE_LIBINIMAGE'
    if not (write_config_h and AddConfigKey(ctx, key, ret)):
        # no config file is specified or it is disabled, use compiler options
        if ret and add_to_cppdefines:
            ctx.env.Append(CPPDEFINES=[key])

    ctx.Result(ret)
    return ret

RegisterCustomTest('CheckInImage', CheckInImage)
