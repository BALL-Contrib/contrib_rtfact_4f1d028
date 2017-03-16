/* 
 *  gl_check.py
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

def CheckGLUT(ctx, write_config_h=True):
    ctx.Message('Checking for GLUT... ')
    confprefix = getAutoconfPrefix(ctx.env)
    
    platform = ctx.env['PLATFORM']
    
    ret = 0

    code = """
    #include %s
    int main(int argc, char **argv) { 
      glutInit(&argc, argv);
      return 0;
    }
"""

    savedVars = ctx.env.SaveVars('LIBS FRAMEWORKS')

    if platform == 'darwin':
        ctx.env.AppendUnique(FRAMEWORKS=['OpenGL', 'GLUT'])
        glutHeader = '<GLUT/glut.h>'
        ret = ctx.TryLink(code % glutHeader, extension='.c')
        if ret:
            ctx.env.DeclarePackage('gl', FRAMEWORKS=['OpenGL'],
                                   trigger_libs=['GL', 'gl', 'OpenGL'],
                                   trigger_frameworks=['OpenGL'])
            ctx.env.DeclarePackage('glut', FRAMEWORKS=['GLUT'], 
                                   dependencies=['gl'],
                                   trigger_libs=['glut', 'GLUT'],
                                   trigger_frameworks=['GLUT'])
            ctx.env.Replace(GLUT_HEADER=glutHeader)
            ctx.env.Replace(LIBGLUT=['glut'])
        ctx.env.RestoreVars(savedVars)
    if platform == 'win32':
        glutHeader = '<GL/glut.h>'
        ret = ctx.TryLink(code % glutHeader, extension='.c')
        if ret:
            ctx.env.DeclarePackage('gl', LIBS=['opengl32', 'glu32'],
                                   trigger_libs=['GL', 'gl', 'OpenGL'],
                                   trigger_frameworks=['OpenGL'])
            ctx.env.DeclarePackage('glut', LIBS=['glut32'],
                                   dependencies=['gl'],
                                   trigger_libs=['glut', 'GLUT'],
                                   trigger_frameworks=['GLUT'])
            ctx.env.Replace(GLUT_HEADER=glutHeader)
            ctx.env.Replace(LIBGLUT=['glut'])
        ctx.env.RestoreVars(savedVars)
    else:
        glutHeader = '<GL/glut.h>'

        for glutlib in ('glut',):
            ctx.env.Append(LIBS = [glutlib, 'GL'])

            ret = ctx.TryLink(code % glutHeader, extension='.c')

            ctx.env.RestoreVars(savedVars)

            if ret:
                ctx.env.DeclarePackage('gl',  LIBS=['GL'],
                                       trigger_libs=['GL', 'gl', 'OpenGL'],
                                       trigger_frameworks=['OpenGL'])
                ctx.env.DeclarePackage('glut', LIBS=[glutlib],
                                       dependencies=['gl'],
                                       trigger_libs=['glut', 'GLUT', glutlib],
                                       trigger_frameworks=['GLUT'])
                ctx.env.Replace(GLUT_HEADER=glutHeader)
                ctx.env.Replace(LIBGLUT=[glutlib, 'GL'])
                break

    key = confprefix+'HAVE_LIBGLUT'
    if not (write_config_h and AddConfigKey(ctx, key, ret)):
        # no config file is specified or it is disabled, use compiler options
        if ret:
            ctx.env.Append(CPPDEFINES=[key])

    ctx.Result(ret)
    return ret

RegisterCustomTest('CheckGLUT', CheckGLUT)
