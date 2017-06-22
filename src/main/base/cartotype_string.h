/*
CARTOTYPE_STRING.H
Copyright (C) 2004-2017 CartoType Ltd.
See www.cartotype.com for more information.
*/

#ifndef CARTOTYPE_STRING_H__
#define CARTOTYPE_STRING_H__

#include <cartotype_errors.h>
#include <cartotype_iter.h>
#include <cartotype_char.h>
#include <cartotype_base.h>
#include <cartotype_bidi.h>
#include <cartotype_array.h>
#include <cartotype_arithmetic.h>

#include <locale.h>
#include <string>

namespace CartoType
{

class TText;
class TFixed;

/**
A constant used to mean 'to the end of the string', or 'unknown string length: must be measured'.
It has the same value and definition as std::string::npos.
*/
static const size_t npos = size_t(-1);

class CString;
typedef CDictionary<CString,CString> CStringDictionary;
typedef CDictionary<CString,int> CStringTypeDictionary;

/** A buffer which can create UTF16 text from UTF8. */
class TUtf16Buffer
    {
    public:
    TUtf16Buffer(const uint8* aText,size_t aLength,size_t aExtraLength = 0);

    TStackArray<uint16,128> iText;
    size_t iLength;
    TResult iError = 0;
    };

/** Flags and constants to tell text searching how to match search terms with found strings. */
enum TStringMatchMethod
    {
    /** A flag to match all strings for which the search term is a perfect match or a prefix. */
    EStringMatchPrefixFlag = 1,
    /** A flag to ignore all characters that are not letters or digits when matching. */ 
    EStringMatchIgnoreNonAlphanumericsFlag = 2,
    /** A flag to match accented and unaccented letters. */
    EStringMatchFoldAccentsFlag = 4,
    /** A flag to allow imperfect matches with a small number of omitted, incorrect or extra characters. */
    EStringMatchFuzzyFlag = 8,
    /** A flag to fold letter case. */
    EStringMatchFoldCaseFlag = 16,
    /** A flag to enable fast searching: just get the first matches, not the most relevant ones. */
    EStringMatchFastFlag = 32,

    /** Strings must match exactly. */
    EStringMatchExact = 0,
    /** Fold case when matching strings. */
    EStringMatchFoldCase = EStringMatchFoldCaseFlag,
    /** The search term must be an exact match or a prefix of the found string. */
    EStringMatchPrefix = EStringMatchPrefixFlag,
    /** Allow fuzzy matches: ignore non-alphanumerics, fold accents and allow imperfect matches. */
    EStringMatchFuzzy = EStringMatchIgnoreNonAlphanumericsFlag | EStringMatchFoldAccentsFlag | EStringMatchFuzzyFlag | EStringMatchFoldCaseFlag
    };

/**
The string interface class. All strings implement this interface.
The private part of the interface allows completely general compare, append,
insert, delete and replace functions to be implemented in MString.

Although the M prefix of MString implies that this class is a pure mix-in with
no data, it has one data item, the length, which was moved to the base
class for efficiency reasons after profiling indicated a bottleneck accessing
the various virtual Length functions of derived classes.
*/
class MString
    {
    public:
    /**
    A virtual destructor. Provided so that templated collection classes can be created using
    MString as the template class, while still being able to own the strings.
    */
    virtual ~MString() { }

    template<typename T> MString& operator=(const T& aText)
        {
        Set(aText);
        return *this;
        }
    template<typename T> MString& operator+=(const T& aText)
        {
        Append(aText);
        return *this;
        }

    /** Return the length of the text. */
    size_t Length() const { return iLength; }

    /** Return a constant pointer to Unicode text stored in UTF16 format. */
    virtual const uint16* Text() const = 0;

    static int32 Compare(MIter<int32>& aIter1,MIter<int32>& aIter2,TStringMatchMethod aStringMatchMethod);
    /** Delete the text in the range aStart...aEnd. */
    void Delete(size_t aStart,size_t aEnd) { Replace(aStart,aEnd,(const uint16*)0,0); }
    /** Delete all the text. */
    void Clear()
        {
        if (!Writable())
            {
            assert(false);
            return;
            }
        ResizeBuffer(0);
        }
    /** Set the length of the string to zero without discarding reserved memory if any. */
    void SetLengthToZero() { iLength = 0; }
    
    std::basic_string<uint16_t> CreateUtf16String() const;
    std::string CreateUtf8String() const;

    /** A conversion operator to convert a string to a UTF-8 string. */
    operator std::string() const { return CreateUtf8String(); }

    /** Get the character at the position aIndex. */
    uint16 operator[](size_t aIndex) const { Assert(aIndex < (size_t)iLength); return Text()[aIndex]; }
    
    TText First(size_t aLength) const;
    TText Last(size_t aLength) const;

    /**
    Constants used in the title case dictionary passed to SetCase
    that specifies exceptions to ordinary case conversion when
    converting to title case.
    */
    enum
        {
        /** No special title case treatment. This constant must be zero. */
        EStandardTitleCase = 0,
        /**
        When setting text to title case, do not capitalize the first letter unless the word is initial.
        Used for particles like 'on', 'of', etc. and their upper-case variants.
        */
        ELowerTitleCase,
        /**
        When setting text to title case, leave these strings as they are (they are already upper-case).
        Used for acronyms like US, roman numerals like II, etc.
        */
        EUpperTitleCase
        };

    void SetCase(TLetterCase aCase,const CStringTypeDictionary* aTitleCaseDictionary = 0);
    void SetSentenceCase();

    // functions taking an iterator
    int32 Compare(MIter<int32>& aIter,bool aFoldCase) const;
    TResult Replace(size_t aStart,size_t aEnd,MIter<int32>& aText,size_t aMaxLength);

    // functions taking an MString
    int32 Compare(const MString& aString,bool aFoldCase = false) const;
    int32 Compare(const MString& aString,TStringMatchMethod aStringMatchMethod) const;
    size_t Find(const MString& aString,TStringMatchMethod aStringMatchMethod = EStringMatchExact,size_t* aEndPos = nullptr) const;
    /** Replace the text aStart...aEnd with aString. */
    void Replace(size_t aStart,size_t aEnd,const MString& aString) { Replace(aStart,aEnd,aString.Text(),aString.Length()); }
    /** Insert aString at aIndex. */
    void Insert(size_t aIndex,const MString& aString) { Replace(aIndex,aIndex,aString); }

    /** Append aString. */
    void Append(const MString& aString)
        {
        if (!Writable())
            {
            assert(false);
            return;
            }
        size_t old_length = iLength;
        ResizeBuffer(iLength + aString.iLength);
        memcpy(WritableText() + old_length,aString.Text(),(iLength - old_length) * 2);
        }

    void AppendCommaSeparated(const CString& aLabel,const CString& aText,size_t& aItems,size_t aMaxItems = SIZE_MAX);

    /** Set the string to aString. */
    void Set(const MString& aString)
        {
        if (!Writable())
            {
            assert(false);
            return;
            }
        ResizeBuffer(aString.iLength);
        memcpy(WritableText(),aString.Text(),iLength * 2);
        }

    // functions taking sixteen-bit text
    int32 Compare(const uint16* aText,size_t aLength = npos,bool aFoldCase = false) const;
    void Replace(size_t aStart,size_t aEnd,const uint16* aText,size_t aLength = npos);
    
    /** Insert aText at aIndex. If aLength is npos the text must be null-terminated, otherwise the length is aLength. */
    void Insert(size_t aIndex,const uint16* aText,size_t aLength = npos)
        { Replace(aIndex,aIndex,aText,aLength); }
    
    /** Append aText. If aLength is npos the text must be null-terminated, otherwise the length is aLength. */
    void Append(const uint16* aText,size_t aLength = npos)
        {
        if (!Writable())
            {
            assert(false);
            return;
            }
        if (aLength == npos)
            {
            const uint16* p = aText;
            aLength = 0;
            while (*p++)
                aLength++;
            }
        size_t old_length = iLength;
        ResizeBuffer(iLength + aLength);
        memcpy(WritableText() + old_length,aText,(iLength - old_length) * 2);
        }
    
    /** Set the string to aText. If aLength is npos the text must be null-terminated, otherwise the length is aLength. */
    void Set(const uint16* aText,size_t aLength = npos)
        {
        if (!Writable())
            {
            assert(false);
            return;
            }
        if (aLength == npos)
            {
            const uint16* p = aText;
            aLength = 0;
            while (*p++)
                aLength++;
            }

        ResizeBuffer(aLength);
        memcpy(WritableText(),aText,iLength * 2);
        }

    // functions taking eight-bit text
    int32 Compare(const char* aText,size_t aLength = npos,bool aFoldCase = false) const;
    void Replace(size_t aStart,size_t aEnd,const char* aText,size_t aLength = npos,int32 aCodePage = 0);
    bool operator==(const char* aText) const;
    bool operator%(const char* aText) const;
    /** The inequality operator for comparing MString objects with null-terminated UTF8 strings */
    bool operator!=(const char* aText) const { return !(*this == aText); }
    
    /**
    Insert the eight-bit string aText at aIndex. If aLength is -1 the text must be null-terminated, otherwise the length is aLength.
    A code page of 0 (the default) implies UTF-8.
    */
    void Insert(size_t aIndex,const char* aText,size_t aLength = npos,int32 aCodePage = 0)
        { Replace(aIndex,aIndex,aText,aLength,aCodePage); }
    /**
    Append the eight-bit string aText. If aLength is npos the text must be null-terminated, otherwise the length is aLength.
    A code page of 0 (the default) implies UTF-8.
    */
    void Append(const char* aText,size_t aLength = npos,int32 aCodePage = 0)
        { Insert(Length(),aText,aLength,aCodePage); }

    /**
    Set the string to the UTF-8 string aText. If aLength is -1 the text must be null-terminated, otherwise the length is aLength.
    */
    void Set(const char* aText,size_t aLength = npos)
        {
        if (!Writable())
            {
            assert(false);
            return;
            }
        TUtf16Buffer buffer((const unsigned char*)aText,aLength);
        ResizeBuffer(buffer.iLength);
        memcpy(WritableText(),buffer.iText.Data(),iLength * 2);
        }

    /**
    Set the string to the eight-bit string aText. If aLength is -1 the text must be null-terminated, otherwise the length is aLength.
    A code page of 0 implies UTF-8.
    */
    void Set(const char* aText,size_t aLength,int32 aCodePage)
        { Replace(0,Length(),aText,aLength,aCodePage); }

    // functions taking a character
    /** Replace the range aStart...aEnd with the single character aChar. */
    void Replace(size_t aStart,size_t aEnd,uint16 aChar) { Replace(aStart,aEnd,&aChar,1); }
    /** Insert aChar at aIndex. */
    void Insert(size_t aIndex,uint16 aChar) { Replace(aIndex,aIndex,aChar); }
    /** Append aChar. */
    void Append(uint16 aChar) { Append(&aChar,1); }
    /** Set the entire text to aChar. */
    void Set(uint16 aChar) { Replace(0,Length(),aChar); }

    /** The equality operator. */
    bool operator==(const MString& aString) const { return Compare(aString) == 0; }
    /** The equality operator for comparing MString objects with null-terminated UTF16 strings */
    bool operator==(const uint16* aText) const { return Compare(aText) == 0; }
    /** The inequality operator. */
    bool operator!=(const MString& aString) const { return Compare(aString) != 0; }
    /** The inequality operator for comparing MString objects with null-terminated UTF16 strings */
    bool operator!=(const uint16* aText) const { return Compare(aText) != 0; }

    /** The equality operator, ignoring case. */
    bool operator%(const MString& aString) const { return Compare(aString,true) == 0; }
    /** The equality operator, ignoring case, for comparing MString objects with null-terminated UTF16 strings */
    bool operator%(const uint16* aText) const { return Compare(aText,true) == 0; }
    
    // wild-card comparison
    bool WildMatch(const MString& aWildText) const;
    bool WildMatch(const char* aWildText) const;
    bool LayerMatch(const MString& aWildText) const;
    bool LayerMatch(const char* aWildText) const;

    // fuzzy comparison
    enum
        {
        /** Maximum edit distance allowed by fuzzy matching: the maximum value for aMaxDistance in MString::FuzzyMatch(). */
        KMaxFuzzyDistance = 4
        };
    static bool FuzzyMatch(MIter<int32>& aIter1,MIter<int32>& aIter2,int32 aMaxDistance);
    static bool FuzzyMatch(const char* aText1,const char* aText2,int32 aMaxDistance);

    // comparison operators
    template<class T> bool operator<(const T& aOther) const { return Compare(aOther) < 0; }
    template<class T> bool operator<=(const T& aOther) const { return Compare(aOther) <= 0; }
    template<class T> bool operator>(const T& aOther) const { return Compare(aOther) > 0; }
    template<class T> bool operator>=(const T& aOther) const { return Compare(aOther) >= 0; }

    // conversion functions
    static TResult ToInt32(const uint8* aText,size_t aLength,int32& aValue,size_t& aLengthUsed,int32 aBase = 10);
    TResult ToInt32(int32& aValue,size_t& aLengthUsed,int32 aBase = 10) const;
    TResult ToInt64(int64& aValue,size_t& aLengthUsed,int32 aBase = 10) const;
    TResult ToUint32(uint32& aValue,size_t& aLengthUsed,int32 aBase = 10) const;
    TResult ToFixed(TFixed& aValue,size_t& aLengthUsed) const;
    TResult ToDouble(double& aValue,size_t& aLengthUsed) const;

    // conversion to presentation form: bidirectional reordering, mirroring and contextual shaping
    TResult Shape(TBidiParDir aParDir,CBidiEngine* aBidiEngine,bool aParStart,bool aReorderFontSelectors);

    void Abbreviate(const CStringDictionary& aDictionary);
    void Transliterate(const CStringTypeDictionary* aTitleCaseDictionary = nullptr,const char* aLocale = nullptr);
    
    // line breaking
    bool IsLineBreak(size_t aPos) const;
    size_t LineBreakBefore(size_t aPos) const;
    size_t LineBreakAfter(size_t aPos) const;
    
    // treating a string as a set of string attributes used by a map object
    void SetAttribute(const MString& aKey,const MString& aValue);
    void SetAttribute(const CString& aKey,const CString& aValue);
    TText GetAttribute(const MString& aKey) const;
    TText GetAttribute(const CString& aKey) const;
    bool NextAttribute(size_t& aPos,TText& aKey,TText& aValue) const;
    
    protected:
    MString(): iLength(0) { }
    MString(size_t aLength): iLength(aLength) { }

    /** The length of the text in 16-bit UTF16 values. */
    size_t iLength;

    private:
    /** Return true if the string is writable. */
    virtual bool Writable() const = 0;

    /**
    Return the maximum writable length of the text: the length of the currently reserved buffer.
    Return 0 if no buffer is reserved or if the text is not writable.
    */
    virtual size_t MaxWritableLength() const = 0;

    /**
    Return a non-constant pointer to the text.
    Return null if the text is not writable.
    */
    virtual uint16* WritableText() = 0;

    /**
    Adjust the text buffer to hold up to aNewLength characters;
    the actual new size may be less. Set iLength to the actual new size.
    */
    virtual void ResizeBuffer(size_t aNewLength) = 0;

    void ReplaceAndConvert(size_t aStart,size_t aEnd,const char* aText,size_t aLength,int32 aCodePage);
    void FindStringAttrib(const MString& aKey,size_t& aKeyStart,size_t& aValueStart,size_t& aValueEnd) const;
    };

/** An unmodifiable string that doesn't own its text. */
class TText: public MString
    {
    public:
    /** Construct an empty TText object. */
    TText(): iText(nullptr) { }
    /** Construct a TText object from a null-terminated string. */
    TText(const uint16* aText): iText(aText)
        { 
        iLength = 0;
        if (iText)
            while (*aText++)
                iLength++;
        }
    /** Construct a TText object from a pointer and a length. */
    TText(const uint16* aText,size_t aLength): MString(aLength), iText(aText) { }
    /** Construct a TText object from a pointer and a length. */
    TText(const char16_t* aText,size_t aLength): MString(aLength), iText((const uint16*)aText) { }
    /** Construct a TText object from an MString object. */
    TText(const MString& aString): MString(aString.Length()), iText(aString.Text())  { }

    // virtual functions from MString
    const uint16* Text() const override { return iText; }

    private:
    // virtual functions from MString
    bool Writable() const override { return false; }
    size_t MaxWritableLength() const override { return 0; }
    uint16* WritableText() override { return nullptr; }
    void ResizeBuffer(size_t /*aNewLength*/) override { assert(false); }

    const uint16* iText;
    };

/** A writable string that doesn't own its text. */
class TWritableText: public MString
    {
    public:
    TWritableText(): iText(nullptr), iMaxLength(0) { }
    TWritableText(uint16* aText,size_t aLength):
        MString(aLength),
        iText(aText),
        iMaxLength(aLength)
        {
        }

    // virtual functions from MString
    const uint16* Text() const { return iText; }

    private:
    // virtual functions from MString
    bool Writable() const { return true; }
    size_t MaxWritableLength() const { return iMaxLength; }
    uint16* WritableText() { return iText; }
    void ResizeBuffer(size_t aNewLength) { iLength = Arithmetic::Min(aNewLength,iMaxLength);  }

    uint16* iText;
    size_t iMaxLength;
    };

/** A string that owns its own text, which has a maximum length fixed at compile time. */
template<size_t aMaxLength> class TTextBuffer: public MString
    {
    public:
    /** Construct an empty TTextBuffer object. */
    TTextBuffer() { }

    /** Construct a TTextBuffer object from an 8-bit ASCII string. */
    TTextBuffer(const char* aText)
        {
        iLength = 0;
        while (*aText && iLength < aMaxLength)
            iText[iLength++] = *aText++;
        }

    // virtual functions from MString
    const uint16* Text() const { return iText; }

    private:
    // virtual functions from MString
    bool Writable() const { return true; }
    size_t MaxWritableLength() const { return aMaxLength; }
    uint16* WritableText() { return iText; }
    void ResizeBuffer(size_t aNewLength) { iLength = Arithmetic::Min(aNewLength,aMaxLength);  }

    uint16 iText[aMaxLength];
    };

/** A macro to define the literal TText object aName from the UTF16 string aText. Example: TTextLiteral(mytext,u"hello"). */
#define TTextLiteral(aName,aText) TText static const aName(aText,sizeof(aText) / sizeof(char16_t) - 1);

/** A string that owns its own text, which is freely resizable. */
class CString: public MString
    {
    public:
    /** Construct an empty CString object. */
    CString():
        iText(iOwnText),
        iReserved(KOwnTextLength)
        {
        }
    CString(std::nullptr_t):
        iText(iOwnText),
        iReserved(KOwnTextLength)
        {
        }
    CString(const CString& aOther);
    CString(const MString& aOther);
    CString(CString&& aOther);
    CString& operator=(const CString& aOther);
    CString& operator=(const MString& aOther);
    CString& operator=(CString&& aOther);
    CString(const char* aText,size_t aLength = npos);
    CString(const uint16* aText,size_t aLength = npos);
    CString(const std::string& aText);

    ~CString()
        {
        if (iText != iOwnText)
            delete [] iText;
        }

    using MString::operator[];
    uint16& operator[](size_t aIndex) { Assert(aIndex < (size_t)iLength); return iText[aIndex]; }

    // virtual functions from MString
    const uint16* Text() const override { return iText; }
    
    private:
    
    // virtual functions from MString
    bool Writable() const override { return true; }
    size_t MaxWritableLength() const override { return iReserved; }
    uint16* WritableText() override { return iText; }
    void ResizeBuffer(size_t aNewLength) override;

    enum
        {
        KOwnTextLength = 32
        };

    uint16 iOwnText[KOwnTextLength];
    uint16* iText;
    size_t iReserved;
    };

/** A type for reference-counted strings, which are used for layer names in map objects. */
class CRefCountedString: public std::shared_ptr<CString>
    {
    public:
    CRefCountedString(): std::shared_ptr<CString>(new CString) { } // ensure that the string is not empty on default construction
    CRefCountedString(const CString& aText): std::shared_ptr<CString>(new CString(aText)) { }
    CRefCountedString(std::nullptr_t): std::shared_ptr<CString>() { }
    };

/** An iterator to convert UTF8 text to UTF32. */
class TUtf8ToUtf32: public MIter<int32>
    {
    public:
    TUtf8ToUtf32(const uint8* aText,size_t aLength = npos);
    TResult Next(int32& aValue);
    void Back();
    const uint8* Pos() const { return iPos; }

    private:
    const uint8* iStart;
    const uint8* iPos;
    const uint8* iEnd;
    bool iEof;
    };

/** An iterator to convert UTF16 text to UTF32. */
class TUtf16ToUtf32: public MIter<int32>
    {
    public:
    TUtf16ToUtf32(const uint16* aText,size_t aLength);
    TResult Next(int32& aValue);
    void Back();
    const uint16* Pos() const { return iPos; }

    private:
    const uint16* iStart;
    const uint16* iPos;
    const uint16* iEnd;
    bool iEof;
    };

/** An iterator that does no conversion but simply passes UTF32 text straight through. */
class TUtf32Iter: public MIter<int32>
    {
    public:
    TUtf32Iter(const int32* aText,int32 aLength);
    TResult Next(int32& aValue);
    void Back();
    const int32* Pos() const { return iPos; }

    private:
    const int32* iStart;
    const int32* iPos;
    const int32* iEnd;
    bool iEof;
    };

/** A class to set the C locale. The destructor restores the original locale. */
class CCLocale
    {
    public:
    CCLocale()
        {
#ifndef _WIN32_WCE
        const char* locale = setlocale(LC_ALL,"C");
        if (locale)
            iSavedLocale = locale;
#endif
        }
    ~CCLocale()
        {
#ifndef _WIN32_WCE
        setlocale(LC_ALL,iSavedLocale.c_str());
#endif
        }

    private:
    std::string iSavedLocale;
    };

class TAbbreviationInfo
    {
    public:
    /** The locale (e.g., "en" or "en_GB") for this data: null implies universal data. */
    const char* iLocale; 
    /** An array of abbreviations: pairs of UTF-8 strings in the order long, short. */
    const char* const * iAbbreviation;
    /** The number of strings in iAbbreviation: twice the number of abbreviations. */
    int32 iAbbreviationCount;
    /** An array of words to be put into lower case if the entire string is put into title case. */
    const char* const * iLowerTitleCaseException;
    /** The number of lower-case title-case exceptions. */
    int32 iLowerTitleCaseExceptionCount;
    /** An array of words to be put into upper case if the entire string is put into title case. */
    const char* const * iUpperTitleCaseException;
    /** The number of upper-case title-case exceptions. */
    int32 iUpperTitleCaseExceptionCount;
    };

const TAbbreviationInfo* AbbreviationInfo(const char* aLocale);



} // namespace CartoType

#endif
