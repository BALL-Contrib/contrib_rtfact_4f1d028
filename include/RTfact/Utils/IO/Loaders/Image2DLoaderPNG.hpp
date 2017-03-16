/* 
 *  Image2DLoaderPNG.hpp
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
 *  Created on: 2009-02-27 16:31:30 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__IMAGE2DLOADERPNG_HPP
#define RTFACT__IMAGE2DLOADERPNG_HPP

#include <RTfact/Config/Common.hpp>

#include <fstream>

#ifdef RTFACT_OS_WIN
#   include <RTfact/Utils/IO/Loaders/Detail/GDIPlusUtils.hpp>
#else
#   include <png.h>
#   include <stdlib.h>
#   include <stdarg.h>
#   include <RTfact/Utils/Containers/Vector.hpp>
#endif

#include <RTfact/Utils/IO/Loader.hpp>
#include <RTfact/Utils/ImageTraitUtils.hpp>

#define RTFACT__MAX_BPP 4

RTFACT_NAMESPACE_BEGIN

namespace IO {

template<
    class tImage2D>
class Image2DLoaderPNG :
    public EntityFileLoader<tImage2D>
{
    typedef typename tImage2D::Component t_Component;

public:

    typedef typename EntityFileLoader<tImage2D>::Entity Entity;

    Entity* load(
        const std::string &aPath,
        const Configuration& aConfig = Configuration())
    {
        #ifdef RTFACT_OS_WIN

            std::ifstream in(aPath.c_str(), std::ios_base::binary);

            RTFACT_USER_ERROR_IF_F(!in, "Cannot open file: %1%", aPath);

            in.close();

            Detail::GDIPlusImageLoader loader(aPath);

            tImage2D* image = new tImage2D;
            
            image->init(
                loader.getComponentCount(), loader.getResX(), loader.getResY());

            loader.readData(image->getFirstComponent(0, 0));

            return image;

        #else

            unsigned char buf[RTFACT__MAX_BPP];
            png_structp png_ptr;
            png_infop info_ptr;
            png_uint_32 resX, resY, scanline_size;
            int bit_depth, color_type, interlace_type;
            
            FILE *fp = fopen(aPath.c_str(), "rb");

            RTFACT_USER_ERROR_IF_F(!fp, "Cannot open file: %1%", aPath);

            if(fread(buf, 1, RTFACT__MAX_BPP, fp) != RTFACT__MAX_BPP)
            {
                RTFACT_USER_ERROR_F("Unrecognized PNG file format: %1%", aPath);
            }

            png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
            
            RTFACT_USER_ERROR_IF_F(!png_ptr, "Damaged PNG file: %1%", aPath);

            info_ptr = png_create_info_struct(png_ptr);

            if(!info_ptr)
            {
                png_destroy_read_struct(&png_ptr, 0, 0);
                
                RTFACT_USER_ERROR_F("Damaged PNG file: %1%", aPath);
            }

            if(setjmp(png_jmpbuf(png_ptr)))
            {
                // Free all of the memory associated with the png_ptr and info_ptr
                png_destroy_read_struct(&png_ptr, &info_ptr, 0);

                // If we get here, we had a problem reading the file
                RTFACT_USER_ERROR_F("Damaged PNG file: %1%", aPath);
            }

            png_init_io(png_ptr, fp);
            png_set_sig_bytes(png_ptr, RTFACT__MAX_BPP);

            png_read_info(png_ptr, info_ptr);
            png_get_IHDR(
                png_ptr, info_ptr, &resX, &resY, &bit_depth, 
                &color_type, &interlace_type, 0, 0);

            if(interlace_type != PNG_INTERLACE_NONE)
            {
                // Free all of the memory associated with the png_ptr and info_ptr
                png_destroy_read_struct(&png_ptr, &info_ptr, 0);
                
                RTFACT_USER_ERROR_F("Damaged PNG file: %1%", aPath);
            }

            // tell libpng to strip 16 bit/color files down to 8 bits/color
            if(bit_depth == 16)
            {
                png_set_strip_16(png_ptr);
            }

            // Extract multiple pixels with bit depths of 1, 2, and 4 from a single
            // byte into separate bytes (useful for paletted and grayscale images).
            if(bit_depth < 8)
            {
                png_set_packing(png_ptr);
            }

            // Expand paletted colors into true RGB triplets
            if(color_type == PNG_COLOR_TYPE_PALETTE)
            {
                png_set_palette_to_rgb(png_ptr);
            }

            // Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel
            if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
            {
                png_set_gray_1_2_4_to_8(png_ptr);
            }

            // Expand paletted or RGB images with transparency to full alpha channels
            // so the data will be available as RGBA quartets.
            if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
            {
                png_set_tRNS_to_alpha(png_ptr);
            }

            // read updated info
            png_read_update_info(png_ptr, info_ptr);
            png_get_IHDR(
                png_ptr, info_ptr, &resX, &resY, &bit_depth, &color_type,
                &interlace_type, RTFACT_NULL, RTFACT_NULL);

            // determine number of componentCount : 1,2,3 or 4
            scanline_size = png_get_rowbytes(png_ptr, info_ptr);
            
            const uint componentCount =  scanline_size / resX;
            // uint componentCount = png_get_channels(png_ptr, info_ptr);

            if(componentCount != 1 && componentCount != 3 && componentCount != 4)
            {
                // Free all of the memory associated with the png_ptr and info_ptr
                png_destroy_read_struct(&png_ptr, &info_ptr, 0);
                
                RTFACT_USER_ERROR_F(
                    "Unsupported number of channels (%1%) : %2%",
                    componentCount % aPath);
            }

            Vector<uint8> imageData(resX * resY * componentCount);
            uint8* begin = &imageData.front();
            uint8* rowPtr = &imageData[resX * (resY - 1) * componentCount];

            for(;;)
            {
                png_read_row(png_ptr, rowPtr, 0);

                if(rowPtr == begin)
                {
                    break;
                }

                rowPtr -= scanline_size;
            }

            png_read_end(png_ptr, info_ptr);
            png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)RTFACT_NULL);

            tImage2D* image = new tImage2D(componentCount, resX, resY);

            t_Component* pixelPtr = image->getFirstComponent(0, 0);
            uint8* ptr = &imageData.front();
            
            for(size_t y = 0; y < resY; ++y)
            {
                for(size_t x = 0; x < resX; ++x)
                {
                    for(uint i = 0; i < componentCount; ++i)
                    {
                        pixelPtr[i] =
                            ImageComponentConverter<uint8, t_Component>::convert(
                                ptr[i]);
                    }

                    pixelPtr += componentCount;
                    ptr += componentCount;
                }
            }
            
            return image;

        #endif
    }

    void save(
        const Entity& aImage,
        const std::string &aPath,
        const Configuration& aConfig = Configuration())
    {
        #ifdef RTFACT_OS_WIN

            Detail::GDIPlusImageSaver::save(
                aPath, "png", aImage.getComponentCount(),
                aImage.getResX(), aImage.getResY(),
                aImage.getFirstComponent(0, 0));

        #else

            const uint resX = aImage.getResX();
            const uint resY = aImage.getResY();

            RTFACT_USER_ERROR_IF(aImage.getComponentCount() == 2,
                "Two-component images are not supported");

            const uint componentCount =
                (aImage.getComponentCount() >= 4) ? 4 : aImage.getComponentCount();

            Vector<png_byte> byteData(resX * resY * componentCount);

            Vector<png_byte>::iterator ptr = byteData.begin();

            for(uint y = 0; y < resY; ++y)
            {
                for(uint x = 0; x < resX; ++x)
                {
                    const t_Component* components =
                        aImage.getFirstComponent(x, resY - y - 1);

                    if(componentCount == 1)
                    {
                        if(components[0] > ImageTraits<tImage2D>::componentMax())
                        {
                            ptr[0] = 255;
                        }
                        else if(components[0] < ImageTraits<tImage2D>::componentMin())
                        {
                            ptr[0] = 0;
                        }
                        else
                        {
                            ptr[0] = ImageTraits<tImage2D>::template
                                convertComponent<uint8>(components[0]);
                        }
                    }
                    else
                    {
                        if(components[0] > ImageTraits<tImage2D>::componentMax() ||
                           components[1] > ImageTraits<tImage2D>::componentMax() ||
                           components[2] > ImageTraits<tImage2D>::componentMax())
                        {
                            ptr[0] = ptr[1] = ptr[2] = 255;
                        }
                        else if(components[0] < ImageTraits<tImage2D>::componentMin() ||
                                components[1] < ImageTraits<tImage2D>::componentMin() ||
                                components[2] < ImageTraits<tImage2D>::componentMin())
                        {
                            ptr[0] = ptr[1] = ptr[2] = 0;
                        }
                        else
                        {
                            ptr[0] = ImageTraits<tImage2D>::template
                                convertComponent<uint8>(components[0]);

                            ptr[1] = ImageTraits<tImage2D>::template
                                convertComponent<uint8>(components[1]);

                            ptr[2] = ImageTraits<tImage2D>::template
                                convertComponent<uint8>(components[2]);
                        }

                        if(componentCount >= 4)
                        {
                            ptr[3] = ImageTraits<tImage2D>::template
                                convertComponent<uint8>(components[3]);

                            ptr += 4;
                        }
                        else
                        {
                            ptr += 3;
                        }
                    }
                }
            }

            Vector<png_byte*> rowData(resY);

            for(uint i = 0; i < resY; i++)
            {
                rowData[i] = i * resX * componentCount + &byteData.front();
            }

            /* create file */
            FILE *fp = fopen(aPath.c_str(), "wb");
            
            RTFACT_USER_ERROR_IF_F(!fp, "Cannot open file: %1%", aPath.c_str());

            /* initialize stuff */
            png_structp png_ptr;
            png_ptr = png_create_write_struct(
                PNG_LIBPNG_VER_STRING, RTFACT_NULL, RTFACT_NULL, RTFACT_NULL);

            RTFACT_USER_ERROR_IF_F(!png_ptr,
                "Error writing file: %1%", aPath.c_str());

            png_infop info_ptr = png_create_info_struct(png_ptr);

            RTFACT_USER_ERROR_IF_F(!info_ptr,
                "Error writing file: %1%", aPath.c_str());

            RTFACT_USER_ERROR_IF_F(setjmp(png_jmpbuf(png_ptr)),
                "Error writing file: %1%", aPath.c_str());

            png_init_io(png_ptr, fp);

            /* write header */
            RTFACT_USER_ERROR_IF_F(setjmp(png_jmpbuf(png_ptr)),
                "Error writing file: %1%", aPath.c_str());

            const int colorType = (componentCount == 4) ?
                PNG_COLOR_TYPE_RGB_ALPHA : PNG_COLOR_TYPE_RGB;

            png_set_IHDR(png_ptr, info_ptr, resX, resY,
                8, colorType, PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

            png_write_info(png_ptr, info_ptr);

            /* write bytes */
            RTFACT_USER_ERROR_IF_F(setjmp(png_jmpbuf(png_ptr)),
                "Error writing file: %1%", aPath.c_str());

            png_write_image(png_ptr, (png_byte**)&rowData.front());

            /* end write */
            RTFACT_USER_ERROR_IF_F(setjmp(png_jmpbuf(png_ptr)),
                "Error writing file: %1%", aPath.c_str());

            png_write_end(png_ptr, RTFACT_NULL);

            fclose(fp);

        #endif
    }
};

} // namespace IO

RTFACT_NAMESPACE_END

#endif // RTFACT__IMAGE2DLOADERPNG_HPP
