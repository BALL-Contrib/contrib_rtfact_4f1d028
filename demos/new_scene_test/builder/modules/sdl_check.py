/* 
 *  sdl_check.py
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
from builder.btools import runCommand
from builder.bconfig import getAutoconfPrefix

def CheckSDL(ctx, write_config_h=True):
    ctx.Message('Checking for SDL... ')
    confprefix = getAutoconfPrefix(ctx.env)
    platform = ctx.env['PLATFORM']
    
    if platform == 'win32':
        lastLIBS = list(ctx.env.get('LIBS', []))
        sdllibs = ['SDL', 'SDLmain']

        ctx.env.Append(LIBS = sdllibs)
        ret = ctx.TryLink("""
            #include <SDL.h>
            int main(int argc, char **argv)
            {
                SDL_Init(SDL_INIT_VIDEO);
                SDL_Quit();
                return 0;
            }
            """, extension='.c')
        ctx.env.Replace(LIBS = lastLIBS)
        if ret:
            ctx.env.DeclarePackage('sdl',
                                   trigger_libs=['SDL', 'SDLmain'],
                                   trigger_frameworks=['SDL'],
                                   LIBS = sdllibs)
            ctx.env.Replace(LIBSDL = sdllibs)
    else:
        ret, output = ctx.TryAction('sdl-config --version')
        if ret:
            
            vars = ctx.env.ParseFlags('!sdl-config --cflags --libs')

            ctx.env.DeclarePackage('sdl', vars=vars,
                                   trigger_libs=['SDL', 'SDLmain'],
                                   trigger_frameworks=['SDL'])

            ctx.env.Append(CPPPATH = vars.get('CPPPATH'))
            ctx.env.Append(LIBPATH = vars.get('LIBPATH'))
            ctx.env.Replace(LIBSDL = vars.get('LIBS'))
        
    key = confprefix+'HAVE_LIBSDL'
    if not (write_config_h and AddConfigKey(ctx, key, ret)):
        # no config file is specified or it is disabled, use compiler options
        if ret:
            ctx.env.Append(CPPDEFINES=[key])

    ctx.Result(ret)
    return ret

RegisterCustomTest('CheckSDL', CheckSDL)

def CheckSDLTTF(ctx, write_config_h=True):
    # We assume here that SDL is available
    ctx.Message('Checking for SDL_ttf... ')
    confprefix = getAutoconfPrefix(ctx.env)
    
    platform = ctx.env['PLATFORM']
    lastLIBS = list(ctx.env.get('LIBS', []))
    sdllibs = ctx.env.get('LIBSDL', [])
    
    sdlttflibs = ['SDL_ttf']

    savedVars = None
    if ctx.env.GetPackage('sdl'):
        savedVars = ctx.env.RequirePackage('sdl')
    
    ctx.env.Append(LIBS = sdlttflibs + sdllibs)
    ret = ctx.TryLink("""
        #include <SDL_ttf.h>
        int main(int argc, char **argv) {
            TTF_Init();
            TTF_Quit();
            return 0;
        }
        """, extension='.c')
    ctx.env.Replace(LIBS = lastLIBS)

    if savedVars:
        ctx.env.RestoreVars(savedVars)

    if ret:
        ctx.env.DeclarePackage('sdlttf', vars={'LIBS' : sdlttflibs},
                               dependencies='sdl',
                               trigger_libs=['SDL_ttf'])
        ctx.env.Replace(LIBSDLTTF=sdlttflibs)
        
    key = confprefix+'HAVE_LIBSDL_TTF'
    if not (write_config_h and AddConfigKey(ctx, key, ret)):
        # no config file is specified or it is disabled, use compiler options
        if ret:
            ctx.env.Append(CPPDEFINES=[key])
        
    ctx.Result(ret)
    return ret

RegisterCustomTest('CheckSDLTTF', CheckSDLTTF)
