/*
CARTOTYPE_STYLE_SHEET_DATA.H
Copyright (C) 2016 CartoType Ltd.
See www.cartotype.com for more information.
*/

#ifndef CARTOTYPE_STYLE_SHEET_DATA_H__
#define CARTOTYPE_STYLE_SHEET_DATA_H__

#include <cartotype_stream.h>
#include <string>

namespace CartoType
{

/** Style sheet data: the text, a stream to read it, and the filename if any. */
class CStyleSheetData
    {
    public:
    CStyleSheetData();
    CStyleSheetData(const uint8* aData,size_t aLength);
    CStyleSheetData(const CStyleSheetData& aOther);
    CStyleSheetData& operator=(const CStyleSheetData& aOther);
    static std::unique_ptr<CStyleSheetData> New(TResult& aError,const char* aFileName);
    static std::unique_ptr<CStyleSheetData> New(TResult& aError,const uint8* aData,size_t aLength);
    TResult Reload();
    TMemoryInputStream& Stream() { return iStream; }
    const std::string& FileName() const { return iFileName; }
    const std::string& Text() const { return iText; }

    private:
    std::string iText;
    std::string iFileName;
    TMemoryInputStream iStream;
    };

}

#endif // CARTOTYPE_STYLE_SHEET_DATA_H__
