/*
CARTOTYPE_BITMAP.H
Copyright (C) 2013-2016 CartoType Ltd.
See www.cartotype.com for more information.
*/

#ifndef CARTOTYPE_BITMAP_H__
#define CARTOTYPE_BITMAP_H__

#include <cartotype_color.h>
#include <cartotype_errors.h>

namespace CartoType
{

class CBitmap;
class MInputStream;
class MOutputStream;

/** A palette of colors used in a bitmap. */
class CPalette
    {
    public:
    CPalette(std::vector<TColor> aColor):
        iColor(aColor)
        {
        }
    const TColor* Color() const { return iColor.data(); }
    size_t ColorCount() const { return iColor.size(); }

    private:
    std::vector<TColor> iColor;
    };

/** A bitmap that does not take ownership of pixel data. */
class TBitmap
    {
    public:
    enum
        {
        /** A mask for the bits in TBitmap::TType that represent the number of bits per pixel. */
        EBitsPerPixelMask = 63,
        /**
        The bit in TBitmap::TType that indicates whether the type is inherently colored,
        which means that its color data is held in the pixel value.
        */
        EColored = 64,
        /**
        The bit in TBitmap::TType that indicates whether the type's data bits are split
        equally between intensity bits and palette bits.
        */
        ETwin = 128,
        /**
        The bit in TBitmap::TType indicating whether the bitmap has a palette.
        If this bit is set, EColored should not also be set.
        */
        EPalette = 256
        };

    /**
    An enumerated type for supported bitmap types.
    The number of bits per pixel is held in the low 6 bits.
    */
    enum TType
        {
        /** One bit per pixel: 1 = foreground color, 0 = background color. */
        E1BitMono = 1,
        /** Eight bits per pixel: 255 = foreground color, 0 = background color. */
        E8BitMono = 8,
        /** 16 bits per pixel, monochrome. */
        E16BitMono = 16,
        /**
        16 bits per pixel, accessed as 16-bit words, not as bytes;
        top 5 bits = red, middle 6 bits = green, low 5 bits = blue.
        */
        E16BitColor = EColored | 16,
        /** 24 bits per pixel: first byte blue, second byte green, third byte red. */
        E24BitColor = EColored | 24,
        /**
        32 bits per pixel: first byte alpha, second byte blue, second byte green, third byte red.
        The red, green and blue values are premultiplied by the alpha value.
        */
        E32BitColor = EColored | 32,
        /**
        8 bits per pixel twin-purpose format: high nybble is the intensity, low nybble is the color,
        which is an index into a palette. This format was designed to allow anti-aliased map labels in
        more than one color to be stored in an 8bpp bitmap.
        */
        E8BitTwin = ETwin | 8,
        /**
        Eight bits per pixel with a 256-entry palette.
        */
        E8BitPalette = EPalette | 8 
        };

    /** Create a bitmap with a specified type, data, and dimensions. */
    TBitmap(TType aType,uint8* aData,int32 aWidth,int32 aHeight,int32 aRowBytes,std::shared_ptr<CPalette> aPalette = nullptr):
        iType(aType),
        iData(aData),
        iWidth(aWidth),
        iHeight(aHeight),
        iRowBytes(aRowBytes),
        iPalette(aPalette)
        {
        }

    typedef TColor (*TColorFunction)(const TBitmap& aBitmap,uint32 aX,uint32 aY);
    TColorFunction ColorFunction() const;
    CBitmap Copy(int32 aExpansion = 0) const;
    CBitmap Blur(bool aGaussian,TFixed aWidth) const;
    CBitmap Palettize() const;
    CBitmap Trim(TRect& aBounds) const;
    CBitmap Clip(TRect aClip) const;
    TResult WritePng(MOutputStream& aOutputStream,bool aPalettize) const;

    /** Return the bitmap type, which indicates its depth and whether it is colored. */
    TType Type() const { return iType; }
    /** Return the bitmap depth: the number of bits used to store each pixel. */
    int32 BitsPerPixel() const { return iType & EBitsPerPixelMask; }
    /** Return a constant pointer to the start of the pixel data. */
    const uint8* Data() const { return iData; }
    /** Return a writable pointer to the start of the pixel data. */
    uint8* Data() { return iData; }
    /** Return the palette if any. */
    std::shared_ptr<CPalette> Palette() const { return iPalette; }
    /**
    Return the number of bytes actually used to store the data. This may include padding
    at the ends of rows.
    */
    int32 DataBytes() const { return iHeight * iRowBytes; }
    /** Return the width in pixels. */
    int32 Width() const { return iWidth; }
    /** Return the height in pixels. */
    int32 Height() const { return iHeight; }
    /** Return the number of bytes used to store each horizontal row of pixels. */
    int32 RowBytes() const { return iRowBytes; }
    /** Clear the pixel data to zeroes. */
    void Clear() { memset(iData,0,iHeight * iRowBytes); }
    /** Clear the pixel data to ones (normally white). */
    void ClearToWhite() { memset(iData,0xFF,iHeight * iRowBytes); }

    protected:
    static TColor Color1BitMono(const TBitmap& aBitmap,uint32 aX,uint32 aY);
    static TColor Color8BitMono(const TBitmap& aBitmap,uint32 aX,uint32 aY);
    static TColor Color16BitColor(const TBitmap& aBitmap,uint32 aX,uint32 aY);
    static TColor Color24BitColor(const TBitmap& aBitmap,uint32 aX,uint32 aY);
    static TColor Color32BitColor(const TBitmap& aBitmap,uint32 aX,uint32 aY);
    static TColor Color8BitTwin(const TBitmap& aBitmap,uint32 aX,uint32 aY);
    static TColor ColorUnsupported(const TBitmap& aBitmap,uint32 aX,uint32 aY);

    TType iType;
    uint8* iData;
    int32 iWidth;
    int32 iHeight;
    int32 iRowBytes;
    std::shared_ptr<CPalette> iPalette;
    };

/** A bitmap that owns its data. */
class CBitmap: public TBitmap
    {
    public:
    CBitmap();
    CBitmap(TType aType,int32 aWidth,int32 aHeight,int32 aRowBytes = 0,std::shared_ptr<CPalette> aPalette = nullptr);
    CBitmap(const TBitmap& aOther);
    CBitmap(CBitmap&& aOther);
    CBitmap& operator=(const TBitmap& aOther);
    CBitmap& operator=(CBitmap&& aOther);
    static std::unique_ptr<CBitmap> New(TResult& aError,MInputStream& aInputStream);
    
    /** Detach the data, transferring ownership to the caller. */
    std::vector<uint8> DetachData() { iData = nullptr; iWidth = iHeight = iRowBytes = 0; return std::move(iOwnData); }

    private:
    std::vector<uint8> iOwnData;
    };
}

#endif
