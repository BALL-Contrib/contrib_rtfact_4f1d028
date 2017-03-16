/* 
 *  GDIPlusUtils.hpp
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
 *  Created on: 2009-03-04 02:36:42 +0100
 *  Author(s): Iliyan Georgiev, Ken Dahm
 */
 
 #ifndef RTFACT__GDIPLUSUTILS_HPP
#define RTFACT__GDIPLUSUTILS_HPP

#include <RTfact/Config/Common.hpp>

#ifdef NOMINMAX
#   define max(a,b) (((a) > (b)) ? (a) : (b))
#   define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#include <windows.h>
#include <process.h>
#include <GdiPlus.h>
//#include <GdiPlusEnums.h>

#ifdef NOMINMAX
#   undef min
#   undef max
#endif

#include <string>

#include <RTfact/Utils/ImageTraitUtils.hpp>

RTFACT_NAMESPACE_BEGIN

namespace IO {
namespace Detail {

using namespace Gdiplus;

class GDIPlusImageLoader
{
    ULONG_PTR mDGIPlusToken;
    Bitmap* mBitmap;
    uint mComponentCount;
    uint mResX;
    uint mResY;

public:

    GDIPlusImageLoader(
        const std::string& aPath
    ) :
        mBitmap(RTFACT_NULL)
    {
        // Initialize GDI+
        GdiplusStartupInput gdiplusStartupInput;
        GdiplusStartup(&mDGIPlusToken, &gdiplusStartupInput, RTFACT_NULL);

        const std::wstring name(aPath.begin(), aPath.end());

        mBitmap = new Bitmap(name.c_str(), FALSE);

        mResX = mBitmap->GetWidth();
        mResY = mBitmap->GetHeight();

        Gdiplus::PixelFormat pixelFormat = mBitmap->GetPixelFormat();

        if(pixelFormat == PixelFormat16bppARGB1555 ||
           pixelFormat == PixelFormat32bppARGB ||
           pixelFormat == PixelFormat32bppPARGB ||
           pixelFormat == PixelFormat64bppARGB ||
           pixelFormat == PixelFormat64bppPARGB)
        {
            mComponentCount = 4;
        }
        else if(pixelFormat == PixelFormat16bppGrayScale)
        {
            mComponentCount = 1;
        }
        else
        {
            mComponentCount = 3;
        }
    }

    ~GDIPlusImageLoader()
    {
        if(mBitmap)
        {
            delete mBitmap;
        }

        GdiplusShutdown(mDGIPlusToken);
    }

    RTFACT_DEFINE_GET_PROPERTY(uint, ResX);
    RTFACT_DEFINE_GET_PROPERTY(uint, ResY);
    RTFACT_DEFINE_GET_PROPERTY(uint, ComponentCount);

    template<
        class tComponent>
    void readData(
        tComponent* oPixelData)
    {
        RTFACT_ASSERT(mComponentCount >= 3);

        if(!mBitmap)
        {
            return;
        }

        Rect rect(0, 0, mResX, mResY);

        BitmapData pixelData;

        const int pixelFormat = (mComponentCount >= 4) ?
            PixelFormat32bppARGB : PixelFormat24bppRGB;

        mBitmap->LockBits(&rect, ImageLockModeWrite, pixelFormat, &pixelData);

        for(uint y = 0; y < mResY; ++y)
        {
            uint8* ptr = (uint8*) pixelData.Scan0 + pixelData.Stride * (mResY - y - 1);

            for(uint x = 0; x < mResX; ++x)
            {
                tComponent* pixelPtr =
                    oPixelData +
                    y * mResX * mComponentCount +
                    x * mComponentCount;

                pixelPtr[2] = ImageComponentConverter<uint8, tComponent>::convert(*ptr++);
                pixelPtr[1] = ImageComponentConverter<uint8, tComponent>::convert(*ptr++);
                pixelPtr[0] = ImageComponentConverter<uint8, tComponent>::convert(*ptr++);

                uint nextIndex = 3;

                if(mComponentCount >= 4)
                {
                    pixelPtr[nextIndex++] =
                        ImageComponentConverter<uint8, tComponent>::convert(*ptr++);
                }

                for(uint i = nextIndex; i < mComponentCount; ++i)
                {
                    pixelPtr[i] = ImageComponentRange<tComponent>::max();
                }
            }
        }

        mBitmap->UnlockBits(&pixelData);
    }
};

class GDIPlusImageSaver
{
    static int getEncoderCLSID(const WCHAR* format, CLSID* pClsid)
    {
        using namespace Gdiplus;

        UINT  num = 0;          // number of Image encoders
        UINT  size = 0;         // size of the Image encoder array in bytes

        ImageCodecInfo* pImageCodecInfo = RTFACT_NULL;

        GetImageEncodersSize(&num, &size);

        if(size == 0)
        {
            return -1;  // Failure
        }

        pImageCodecInfo = (ImageCodecInfo*)(malloc(size));

        if(pImageCodecInfo == RTFACT_NULL)
        {
            return -1;  // Failure
        }

        GetImageEncoders(num, size, pImageCodecInfo);

        for(UINT j = 0; j < num; ++j)
        {
            if(wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
            {
                *pClsid = pImageCodecInfo[j].Clsid;
                free(pImageCodecInfo);
                return j;  // Success
            }    
        }

        free(pImageCodecInfo);

        return -1;  // Failure
    }

public:

    template<
        class tComponent>
    static void save(
        const std::string& aPath,
        const std::string& aImageType,
        const uint aComponentCount,
        const uint aResX,
        const uint aResY,
        const tComponent* aPixelData,
        const int aQuality = -1)
    {
        RTFACT_ASSERT(aComponentCount >= 3);

        // Initialize GDI+.
        GdiplusStartupInput gdiplusStartupInput;
        ULONG_PTR gdiplusToken;
        GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, RTFACT_NULL);

        CLSID encoderClsid;
        Status stat;

        const int pixelFormat = (aComponentCount >= 4) ?
            PixelFormat32bppARGB : PixelFormat24bppRGB;

        Bitmap *Image = new Bitmap(aResX, aResY, pixelFormat);
        BitmapData data;
        Rect r(0, 0, aResX, aResY);
        Image->LockBits(&r, ImageLockModeWrite, pixelFormat, &data);

        const tComponent* imagePtr = aPixelData;

        for(uint y = 0; y < aResY; y++)
        {
            uint8* ptr = (uint8*)data.Scan0 + data.Stride * (aResY - y - 1);

            for(uint x = 0; x < aResX; x++)
            {
				tComponent minVal = ImageComponentRange<tComponent>::min();
				tComponent maxVal = ImageComponentRange<tComponent>::max();

				ptr[0] = ImageComponentConverter<tComponent, uint8>::convert(std::max(minVal, std::min(maxVal, imagePtr[2])));
				ptr[1] = ImageComponentConverter<tComponent, uint8>::convert(std::max(minVal, std::min(maxVal, imagePtr[1])));
				ptr[2] = ImageComponentConverter<tComponent, uint8>::convert(std::max(minVal, std::min(maxVal, imagePtr[0])));

				if (aComponentCount >= 4)
				{
					ptr[3] = ImageComponentConverter<tComponent, uint8>::convert(std::max(minVal, std::min(maxVal, imagePtr[3])));
					ptr += 4;
					imagePtr += aComponentCount;
				}
				else
				{
					ptr += 3;
					imagePtr += 3;
				}
            }
        }

        Image->UnlockBits(&data);

        const std::string mimeType = "image/" + aImageType;
        const std::wstring wMimeType(mimeType.begin(), mimeType.end());

        // Get the CLSID of the image encoder.
        getEncoderCLSID(&(*wMimeType.begin()), &encoderClsid);

        std::wstring name(aPath.begin(), aPath.end());

        if(aQuality > -1)
        {
            ULONG quality = static_cast<ULONG>(aQuality);

            EncoderParameters params;

            params.Count = 1;
            params.Parameter[0].Guid = EncoderQuality;
            params.Parameter[0].Type = EncoderParameterValueTypeLong;
            params.Parameter[0].NumberOfValues = 1;
            params.Parameter[0].Value = &quality;

            stat = Image->Save(name.c_str(), &encoderClsid, &params);
        }
        else
        {
            stat = Image->Save(name.c_str(), &encoderClsid, RTFACT_NULL);
        }

        delete Image;
        
        GdiplusShutdown(gdiplusToken);
    }
};

} // namespace Detail
} // namespace IO

RTFACT_NAMESPACE_END

#endif // RTFACT__GDIPLUSUTILS_HPP
