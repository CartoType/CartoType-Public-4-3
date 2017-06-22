/*
CARTOTYPE_CHAR.H
Copyright (C) 2004-2017 CartoType Ltd.
See www.cartotype.com for more information.
*/

#ifndef CARTOTYPE_CHAR_H__
#define CARTOTYPE_CHAR_H__

#include "cartotype_types.h"

namespace CartoType
{

/**
The case of letters and strings in Latin and a few other alphabetic scripts.
*/
enum TLetterCase
    {
    /** A code used when the letter case is to be left as it is, or is irrelevant. */
    ENoLetterCase,
    /** The case of the letters 'a', 'b', c', etc. */
    ELowerCase,
    /** The case of the letters 'A', 'B', 'C, etc. */
    EUpperCase,
    /**
    The case of letters like U+01C8 'Lj' and a few others; also indicates
    that the first letter of each word should be changed to titlecase.
    */
    ETitleCase,
    };

/**
The general category of a character as defined in the Unicode Standard
(http://www.unicode.org/versions/Unicode5.0.0/ch04.pdf).
Each constant is a single bit so that masks can be made by combining them.
*/
enum TCharCategory
    {
    ELuCategory = 1,			// Letter, uppercase
    ELlCategory = 2,			// Letter, lowercase
    ELtCategory = 4,			// Letter, titlecase
    ELmCategory = 8,			// Letter, modifier
    ELoCategory = 0x10,			// Letter, other
    EMnCategory = 0x20,			// Mark, nonspacing
    EMcCategory = 0x40,			// Mark, spacing combining
    EMeCategory = 0x80,			// Mark, enclosing
    ENdCategory = 0x100,		// Number, decimal digit
    ENlCategory = 0x200,		// Number, letter
    ENoCategory = 0x400,		// Number, other
    EPcCategory = 0x800,		// Punctuation, connector
    EPdCategory = 0x1000,		// Punctuation, dash
    EPsCategory = 0x2000,		// Punctuation, open
    EPeCategory = 0x4000,		// Punctuation, close
    EPiCategory = 0x8000,		// Punctuation, initial quote (may behave like Ps or Pe depending on usage)
    EPfCategory = 0x10000,		// Punctuation, final quote (may behave like Ps or Pe depending on usage)
    EPoCategory = 0x20000,		// Punctuation, other
    ESmCategory = 0x40000,		// Symbol, math
    EScCategory = 0x80000,		// Symbol, currency
    ESkCategory = 0x100000,		// Symbol, modifier
    ESoCategory = 0x200000,		// Symbol, other
    EZsCategory = 0x400000,		// Separator, space
    EZlCategory = 0x800000,		// Separator, line
    EZpCategory = 0x1000000,	// Separator, paragraph
    ECcCategory = 0x2000000,	// Other, control
    ECfCategory = 0x4000000,	// Other, format
    ECsCategory = 0x8000000,	// Other, surrogate
    ECoCategory = 0x10000000,	// Other, private use
    ECnCategory = 0x20000000,	// Other, not assigned (including noncharacters)

    /**
    A flag to indicate a character outside the categories Lu and Lt that has a lower-case variant.
    It is used internally.
    */
    EUpperCaseFlag = 0x40000000 
    };

/**
The bidirectional type of a character as used in the Unicode Bidirectional Algorithm.
Each constant is a single bit so that masks can be made by combining them.
*/
enum TBidiType
    {
    ELBidiType = 1,
    ELREBidiType = 2,
    ELROBidiType = 4,
    ERBidiType = 8,
    EALBidiType = 0x10,
    ERLEBidiType = 0x20,
    ERLOBidiType = 0x40,
    EPDFBidiType = 0x80,
    EENBidiType = 0x100,
    EESBidiType = 0x200,
    EETBidiType = 0x400,
    EANBidiType = 0x800,
    ECSBidiType = 0x1000,
    ENSMBidiType = 0x2000,
    EBNBidiType = 0x4000,
    EBBidiType = 0x8000,
    ESBidiType = 0x10000,
    EWSBidiType = 0x20000,
    EONBidiType = 0x40000
    };

/** Groups of bidirectional types. */
enum
    {
    EStrongLeftToRightBidiTypes = ELBidiType | ELREBidiType | ELROBidiType,
    EStrongRightToLeftBidiTypes = ERBidiType | EALBidiType | ERLEBidiType | ERLOBidiType,
    EStrongBidiTypes = EStrongLeftToRightBidiTypes | EStrongRightToLeftBidiTypes
    };

/**
A class to provide Unicode character properties.
A TChar object holds a single character encoded using its Unicode
code point. Surrogates have no meaning in this context. All signed 32-bit
values are valid but not all are defined as characters. Non-character
values give consistent default values: for example, the uppercase version
of a non-character value is the same value.
*/
class TChar
    {
    public:
    enum
        {
        /**
        A case variant can be no longer than this number of UTF32 characters.
        For example, the upper-case variant of U+00DF is 'SS' and is
        two characters long.
        */
        EMaxCaseVariantLength = 2
        };

    enum
        {
        /** The start of a range of private use characters to select fonts. */
        KFirstFontSelector = 0xF000,
        /** A private use character to select the default font. */
        KDefaultFontSelector = 0xF0FF,
        /** The end of a range of private use characters to select fonts. */
        KLastFontSelector = KDefaultFontSelector,
        /** The start of a range of private use characters to select box styles (background and outline). */
        KFirstBoxStyleSelector = 0xF100,
        /** The end of a range of private use characters to select box styles (background and outline). */
        KLastBoxStyleSelector = 0xF1FE,
        /** A character to mark the end of a box. */
        KEndOfBox = 0xF1FF,
        /** A character to set the baseline of a box to the baseline of the current line. */
        KBaselineSelector = 0xF200
        };

    /** Create a TChar with the character code 0. */
    TChar(): iCode(0) { }

    /** Construct a TChar from a Unicode code point (without checking its validity as such). */
    TChar(int32 aCode): iCode(aCode) { }

    void GetLowerCase(int32* aText,int32& aTextLength) const;
    void GetTitleCase(int32* aText,int32& aTextLength) const;
    void GetUpperCase(int32* aText,int32& aTextLength) const;
    TCharCategory Category() const;
    TBidiType BidiType() const;
    int32 Mirrored() const;
    int32 AccentStripped() const;
    bool IsAlphanumeric() const;

    /**
    Return true if a character code is in a word. Words are defined as sequences
    of letters and combining marks.
    */
    static bool InWord(int32 aCode)
        {
        if (aCode < 'A' && aCode != '\'' && aCode != ':')
            return false;
        if (aCode <= 'Z')
            return true;
        if (aCode < 'a')
            return false;
        if (aCode <= 'z')
            return true;
        if (aCode <= 0xBF)
            return false;
        if (aCode <= 0x17F)
            return true;
        return aCode == 0x2019 || // right single quote
            (TChar(aCode).Category() & (ELuCategory | ELlCategory | ELtCategory | ELmCategory | ELoCategory | EMnCategory | EMcCategory | EMeCategory)) != 0;
        }

    /** The Unicode code point of the character. */
    int32 iCode;
    };

}

#endif
