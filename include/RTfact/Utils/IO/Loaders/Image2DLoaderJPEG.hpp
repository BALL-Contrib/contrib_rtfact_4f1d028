/* 
 *  Image2DLoaderJPEG.hpp
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
 *  Created on: 2009-03-04 02:36:42 +0100
 *  Author(s): Iliyan Georgiev, Rossen Dimov
 */
 
 #ifndef RTFACT__IMAGE2DLOADERJPEG_HPP
#define RTFACT__IMAGE2DLOADERJPEG_HPP

#include <RTfact/Config/Common.hpp>

#include <fstream>

#ifdef RTFACT_OS_WIN
#   include <RTfact/Utils/IO/Loaders/Detail/GDIPlusUtils.hpp>
#else
#   include <jpeglib.h>
#   include <stdlib.h>
#   include <RTfact/Utils/Containers/Vector.hpp>
#endif

#include <RTfact/Utils/IO/Loader.hpp>
#include <RTfact/Utils/ImageTraitUtils.hpp>

#define RTFACT__MAX_BPP 4

RTFACT_NAMESPACE_BEGIN

namespace IO {

template<
    class tImage2D>
class Image2DLoaderJPEG :
    public EntityFileLoader<tImage2D>
{
    typedef typename tImage2D::Component t_Component;

    uint mQuality;

public:

    typedef typename EntityFileLoader<tImage2D>::Entity Entity;

    Image2DLoaderJPEG(
        const uint aQuality = 95
    ) :
        mQuality(aQuality)
    {}

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

            /* these are standard libjpeg structures for reading(decompression) */
	        struct jpeg_decompress_struct cinfo;
	        struct jpeg_error_mgr jerr;
        	
	        FILE *infile = fopen(aPath.c_str(), "rb");

            RTFACT_USER_ERROR_IF_F(!infile, "Cannot open file: %1%", aPath);

            memset(&cinfo, 0, sizeof(cinfo));
        	
	        /* here we set up the standard libjpeg error handler */
	        cinfo.err = jpeg_std_error(&jerr);

	        /* setup decompression process and source, then read JPEG header */
	        jpeg_create_decompress(&cinfo);
	        
            /* this makes the library read from infile */
	        jpeg_stdio_src(&cinfo, infile);
	        
            /* reading the image header which contains image information */
	        jpeg_read_header(&cinfo, TRUE);
	        
	        /* Start decompression jpeg here */
	        jpeg_start_decompress(&cinfo);

            RTFACT_USER_ERROR_IF_F(
                cinfo.num_components != 1 && cinfo.num_components != 3,
                    "Unsupported number of color components (%1%): %2%",
                    cinfo.num_components % aPath);

            const uint componentCount = cinfo.num_components;

            const uint resX = cinfo.output_width;
            const uint resY = cinfo.output_height;
            const uint scanLineWidth = resX * componentCount;

            tImage2D* image = new tImage2D(componentCount, resX, resY);

            JSAMPROW row_pointer[1];

	        /* now actually read the jpeg into the raw buffer */
	        row_pointer[0] = (unsigned char*) malloc(scanLineWidth);

            uint y = resY - 1;
	        
            /* read one scan line at a time */
	        while(cinfo.output_scanline < resY)
	        {
		        jpeg_read_scanlines(&cinfo, row_pointer, 1);

                t_Component* components = image->getFirstComponent(0, y);

		        for(uint x = 0; x < resX; ++x) 
                {
                    for(uint i = 0; i < componentCount; ++i)
                    {
                        components[i] = ImageTraits<tImage2D>::toComponent(
                            row_pointer[0][componentCount * x + i]);
                    }
                }

                --y;
	        }

	        /* wrap up decompression, destroy objects,
               free pointers and close open files */
	        jpeg_finish_decompress(&cinfo);
	        jpeg_destroy_decompress(&cinfo);
	        
            free(row_pointer[0]);
	        fclose(infile);

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
                aPath, "jpeg", aImage.getComponentCount(),
                aImage.getResX(), aImage.getResY(),
                aImage.getFirstComponent(0, 0),
                mQuality);

        #else

            RTFACT_USER_ERROR_IF(aImage.getComponentCount() == 2,
                "Two-component images are not supported");

            struct jpeg_compress_struct cinfo;
            struct jpeg_error_mgr jerr;

            const uint componentCount =
                (aImage.getComponentCount() == 1) ? 1 : 3;

            /* this is a pointer to one row of image data */
            JSAMPROW row_pointer[1];

            FILE *outfile = fopen(aPath.c_str(), "wb");

            RTFACT_USER_ERROR_IF_F(!outfile, "Cannot open file: %1%", aPath);

            cinfo.err = jpeg_std_error(&jerr);
            jpeg_create_compress(&cinfo);
            jpeg_stdio_dest(&cinfo, outfile);

            const uint resX = aImage.getResX();
            const uint resY = aImage.getResY();

            /* Setting the parameters of the output file here */
            cinfo.image_width = resX;
            cinfo.image_height = resY;
            cinfo.input_components = componentCount;
            cinfo.in_color_space = (componentCount == 1) ?
                JCS_GRAYSCALE : JCS_RGB;

            /* default compression parameters  */
            jpeg_set_defaults(&cinfo);

            /* set the quality */
            jpeg_set_quality(&cinfo, mQuality, TRUE);

            /* Now do the compression .. */
            jpeg_start_compress(&cinfo, TRUE);

            Vector<uint8> rowData(resX * componentCount);

            /* like reading a file, this time write one row at a time */
            for(uint y = 0; y < resY; ++y)
            {
                for(uint x = 0; x < resX; ++x)
                {
                    const t_Component* components =
                        aImage.getFirstComponent(x, resY - y - 1);

                    for(uint i = 0; i < componentCount; ++i)
                    {
                        rowData[componentCount * x + i] =
                            ImageTraits<tImage2D>::template
                                convertComponent<uint8>(components[i]);
                    }
                }

                row_pointer[0] = &rowData.front();

                jpeg_write_scanlines(&cinfo, row_pointer, 1);
            }

            /* similar to read file, clean up after we're done compressing */
            jpeg_finish_compress(&cinfo);
            jpeg_destroy_compress(&cinfo);
            fclose( outfile );

        #endif
    }
};

} // namespace IO

RTFACT_NAMESPACE_END

#endif // RTFACT__IMAGE2DLOADERJPEG_HPP
