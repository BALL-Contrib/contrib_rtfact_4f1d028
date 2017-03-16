/* 
 *  imglib_check.py
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
from builder.bconfig import filterOut

def CheckPNG(ctx, write_config_h=True):
    ctx.Message('Checking for libpng... ')
    confprefix = getAutoconfPrefix(ctx.env)
    
    platform = ctx.env['PLATFORM']
    lastLIBS = list(ctx.env.get('LIBS', []))

    if platform == 'win32':
        ZLIB=['zlib']
    else:
        ZLIB=['z']

    ret = 0
    # "libpng" variant is required here because scons seems to remove
    # remove sometimes lib prefix (at least in mingw mode)
    for pnglib in ('png', 'libpng', '"libpng"'):
        for includeDir in ('', 'libpng/'):
            ctx.env.Append(LIBS = [pnglib] + ZLIB)
            ret = ctx.TryLink("""
                #include <%spng.h>
                int main(int argc, char **argv) { 
                  png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
                  return 0;
                }
""" % includeDir, extension='.c')
            ctx.env.Replace(LIBS = lastLIBS)
            if ret:
                ctx.env.Replace(LIBPNG=[pnglib] + ZLIB)
                break
        if ret:
            break

    key = confprefix+'HAVE_LIBPNG'
    if not (write_config_h and AddConfigKey(ctx, key, ret)):
        # no config file is specified or it is disabled, use compiler options
        if ret:
            ctx.env.Append(CPPDEFINES=[key])
    
    ctx.Result(ret)
    return ret

RegisterCustomTest('CheckPNG', CheckPNG)

def CheckJPEG(ctx, write_config_h=True):
    ctx.Message('Checking for libjpeg... ')
    confprefix = getAutoconfPrefix(ctx.env)

    platform = ctx.env['PLATFORM']
    lastLIBS = list(ctx.env.get('LIBS', []))

    # "libjpeg" variant is required here because scons seems to remove
    # remove sometimes lib prefix (at least in mingw mode)
    for jpeglib in ('jpeg', 'libjpeg', '"libjpeg"'):
        ctx.env.Append(LIBS = [jpeglib])
        ret = ctx.TryLink("""
    #include <stdio.h>
    #include <jpeglib.h>
    int main(int argc, char **argv) { 
      struct jpeg_decompress_struct cinfo;
      jpeg_destroy_decompress(&cinfo);
      return 0;
    }
""", extension='.c')
        ctx.env.Replace(LIBS = lastLIBS)
        if ret:
            ctx.env.Replace(LIBJPEG=[jpeglib])
            break

    key = confprefix+'HAVE_LIBJPEG'
    if not (write_config_h and AddConfigKey(ctx, key, ret)):
        # no config file is specified or it is disabled, use compiler options
        if ret:
            ctx.env.Append(CPPDEFINES=[key])

    ctx.Result(ret)
    return ret

RegisterCustomTest('CheckJPEG', CheckJPEG)

def CheckTIFF(ctx, write_config_h=True):
    ctx.Message('Checking for libtiff... ')
    confprefix = getAutoconfPrefix(ctx.env)

    platform = ctx.env['PLATFORM']
    lastLIBS = list(ctx.env.get('LIBS', []))

    for tifflib in ('tiff', 'libtiff'):
        ctx.env.Append(LIBS = [tifflib])
        ret = ctx.TryLink("""
    #include <stdio.h>
    #include <tiffio.h>
    int main(int argc, char **argv) {
      TIFF *tif;
      tif = TIFFOpen("test","r");
      TIFFClose(tif);
      return 0;
    }
""", extension='.c')
        ctx.env.Replace(LIBS = lastLIBS)
        if ret:
            ctx.env.Replace(LIBTIFF=[tifflib])
            break
        
    key = confprefix+'HAVE_LIBTIFF'
    if not (write_config_h and AddConfigKey(ctx, key, ret)):
        # no config file is specified or it is disabled, use compiler options
        if ret:
            ctx.env.Append(CPPDEFINES=[key])
    
    ctx.Result(ret)
    return ret

RegisterCustomTest('CheckTIFF', CheckTIFF)

def CheckImageMagick(ctx, write_config_h=True):
    ctx.Message('Checking for ImageMagick... ')
    confprefix = getAutoconfPrefix(ctx.env)
    
    ret, output = ctx.TryAction('Magick-config --version')
    if ret:
        # removed --cflags due to the problems with some options:
        #                  conflicts between icc and g++ options
        ctx.env.ParseConfig('Magick-config --cppflags --ldflags')
        if ctx.env.get('CCFLAGS'):
            ctx.env['CCFLAGS'] = filterOut('-no-undefined', ctx.env['CCFLAGS'])

    key = confprefix+'HAVE_LIBMAGICK'
    if not (write_config_h and AddConfigKey(ctx, key, ret)):
        # no config file is specified or it is disabled, use compiler options
        if ret:
            ctx.env.Append(CPPDEFINES=[key])
    
    ctx.Result(ret)
    return ret

RegisterCustomTest('CheckImageMagick', CheckImageMagick)
