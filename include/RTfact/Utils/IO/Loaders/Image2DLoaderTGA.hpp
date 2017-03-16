/*
 *  Image2DLoaderTGA.hpp
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
 *  Created on: 2009-02-27 16:31:30 +0100
 *  Author(s): Iliyan Georgiev, Petr Kmoch, Felix Klein
 */

 #ifndef RTFACT__FLOAT3IMAGE2DTGALOADER_CPP
#define RTFACT__FLOAT3IMAGE2DTGALOADER_CPP

#include <RTfact/Config/Common.hpp>

#include <fstream>

#include <RTfact/Utils/IO/Loader.hpp>
#include <RTfact/Utils/ImageTraitUtils.hpp>

RTFACT_NAMESPACE_BEGIN

namespace IO {

template<
    class tImage2D>
class Image2DLoaderTGA :
    public EntityFileLoader<tImage2D>
{
public:

    typedef typename EntityFileLoader<tImage2D>::Entity Entity;

    Entity* load(
        const std::string &aPath,
        const Configuration& aConfig = Configuration())
    {
        uint8 header[18];

        std::ifstream in(aPath.c_str(), std::ios_base::binary);

        RTFACT_USER_ERROR_IF_F(!in, "Cannot open file: %1%", aPath);

        in.read(reinterpret_cast<char*>(header), 18);

        uint idLength = header[0];
        //uint mapType = header[1]; // currently unused
        uint imageType = header[2];
        uint bitsPerPixel = header[16];

        RTFACT_USER_ERROR_IF_F(imageType != 2 && imageType != 3,
            "Unsupported TGA image type (%1%): %2%", imageType % aPath);

        RTFACT_USER_ERROR_IF_F(
            (bitsPerPixel != 32 && bitsPerPixel != 24 && bitsPerPixel != 16),
            "Unsupported pixel depth (%1%) - only 16, 24, and 32 are supported: %2%",
            bitsPerPixel % aPath);

        uint resX = header[12] + 256 * header[13];
        uint resY = header[14] + 256 * header[15];
        uint componentCount = (imageType == 3) ? 1 : 3;

        tImage2D* image = new tImage2D;
        image->init(componentCount, resX, resY);

        const uint bytesPerPixel = bitsPerPixel / 8;

        uint8* inputPtr = new uint8[bytesPerPixel];

        const float maxChannelValue = (bytesPerPixel > 2) ? 255.f : 31.f;

        in.ignore(idLength);

        const bool topLeftCorner = (((header[17] >> 5) & 1) == 1);

        // invert the Y axis of the image (tex coords usually have (0,0) at the
        // bottom left corner, while images have it at the top-left corner
        uint yStartValue = topLeftCorner ? resY - 1 : 0;
        uint yEndValue   = topLeftCorner ? uint(-1) : resY;
        uint yStep       = topLeftCorner ? uint(-1) : 1;

        for(uint y = yStartValue; y != yEndValue; y += yStep)
        {
            for(uint x = 0; x < resX; ++x)
            {
                in.read(reinterpret_cast<char*>(inputPtr), bytesPerPixel);

                if(componentCount == 1)
                {
                    *image->getFirstComponent(x, y) =
                        ImageTraits<tImage2D>::toComponent(
                            static_cast<float>(*inputPtr) / maxChannelValue);;
                }
                else
                {
                    uint8 r, g, b;

                    switch(bitsPerPixel)
                    {
                    case 16:
                        r = (inputPtr[1] >> 2) & 31;
                        g = ((inputPtr[0] >> 3) & 28) | (inputPtr[1] & 3);
                        b = inputPtr[0] & 31;
                        break;
                    case 24:
                        r = inputPtr[2];
                        g = inputPtr[1];
                        b = inputPtr[0];
                        break;
                    case 32:
                        r = inputPtr[2];
                        g = inputPtr[1];
                        b = inputPtr[0];
                        break;
                    default:
                        //this never happens, but prevents an "uninitialized r,g,b" warning from GCC
                        r=g=b=0;
                        break;
                    }

                    typename tImage2D::Component* components =
                        image->getFirstComponent(x, y);

                    components[0] = ImageTraits<tImage2D>::toComponent(
                        static_cast<float>(r) / maxChannelValue);

                    components[1] = ImageTraits<tImage2D>::toComponent(
                        static_cast<float>(g) / maxChannelValue);

                    components[2] = ImageTraits<tImage2D>::toComponent(
                        static_cast<float>(b) / maxChannelValue);
                }
            }
        }

        delete [] inputPtr;

        in.close();

        return image;
    }

    void save(
        const Entity& aImage,
        const std::string &aPath,
        const Configuration& aConfig = Configuration())
    {
        RTFACT_USER_ERROR_IF(aImage.getComponentCount() == 2,
            "2-component image types are not not supported");

        uint8 header[18];

        std::ofstream out(aPath.c_str(), std::ios_base::binary);

        RTFACT_USER_ERROR_IF_F(!out, "Cannot open file for writing: %1%", aPath);

        const uint resX = aImage.getResX();
        const uint resY = aImage.getResY();

        header[0] = 0;
        header[1] = 0;
        header[2] = (aImage.getComponentCount() == 1) ? 3 : 2;
        header[3] = 0;
        header[4] = 0;
        header[5] = 0;
        header[6] = 0;
        header[7] = 0;
        header[8] = 0;
        header[9] = 0;
        header[10] = 0;
        header[11] = 0;
        header[12] = static_cast<uint8>(resX & 255);
        header[13] = static_cast<uint8>(resX >> 8);
        header[14] = static_cast<uint8>(resY & 255);
        header[15] = static_cast<uint8>(resY >> 8);
        header[16] = 24;
        header[17] = 0; // set to 32 to flip the image

        out.write(reinterpret_cast<const char*>(header), 18);

        for(uint y = 0; y < resY; ++y)
        {
            for(uint x = 0; x < resX; ++x)
            {
                const typename tImage2D::Component* components =
                    aImage.getFirstComponent(x, y);

                if(aImage.getComponentCount() == 1)
                {
                    uint8 outputData;

                    if(components[0] > ImageTraits<tImage2D>::componentMax())
                    {
                        outputData = 255;
                    }
                    else if(components[0] < ImageTraits<tImage2D>::componentMin())
                    {
                        outputData = 0;
                    }
                    else
                    {
                        outputData = ImageTraits<tImage2D>::template
                            convertComponent<uint8>(components[0]);
                    }

                    out.write(reinterpret_cast<const char*>(&outputData), 1);
                }
                else
                {
                    uint8 outputData[3];

                    if(components[0] > ImageTraits<tImage2D>::componentMax() ||
                       components[1] > ImageTraits<tImage2D>::componentMax() ||
                       components[2] > ImageTraits<tImage2D>::componentMax())
                    {
                        outputData[0] = outputData[1] = outputData[2] = 255;
                    }
                    else if(components[0] < ImageTraits<tImage2D>::componentMin() ||
                            components[1] < ImageTraits<tImage2D>::componentMin() ||
                            components[2] < ImageTraits<tImage2D>::componentMin())
                    {
                        outputData[0] = outputData[1] = outputData[2] = 0;
                    }
                    else
                    {
                        outputData[0] = ImageTraits<tImage2D>::template
                            convertComponent<uint8>(components[2]);

                        outputData[1] = ImageTraits<tImage2D>::template
                            convertComponent<uint8>(components[1]);

                        outputData[2] = ImageTraits<tImage2D>::template
                            convertComponent<uint8>(components[0]);
                    }

                    out.write(reinterpret_cast<const char*>(outputData), 3);
                }
            }
        }

        out.close();
    }
};

} // namespace IO

RTFACT_NAMESPACE_END

#endif // RTFACT__FLOAT3IMAGE2DTGALOADER_CPP
