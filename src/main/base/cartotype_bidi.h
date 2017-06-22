/*
CARTOTYPE_BIDI.H
Copyright (C) 2008-2017 CartoType Ltd.
See www.cartotype.com for more information.
*/

#ifndef CARTOTYPE_BIDI_H__
#define CARTOTYPE_BIDI_H__

#include "cartotype_char.h"
#include "cartotype_errors.h"

namespace CartoType
{

/** The override status during bidirectional reordering. */
enum TBidiOverride
    {
    ENeutralOverride,
    ELeftToRightOverride,
    ERightToLeftOverride
    };

/** A run of text in a certain birectional type. */
class TBidiRun
    {
    public:
    TBidiRun():
        iLength(0),
        iType(EONBidiType),
        iLevel(0)
        {
        }

    int32 iLength;
    TBidiType iType;
    int32 iLevel;
    };

/** The paragraph direction used during bidirectional reordering. */
enum TBidiParDir
    {
    /**
    Set the paragraph direction from the first strongly directional character.
    If there are none, use left-to-right.
    */
    ELeftToRightPreferred,
    /**
    Set the paragraph direction from the first strongly directional character.
    If there are none, use right-to-left.
    */
    ERightToLeftPreferred,
    /** Set the paragraph direction to left-to-right whatever its content. */
    ELeftToRightForced,
    /** Set the paragraph direction to right-to-left whatever its content. */
    ERightToLeftForced
    };

/**
An engine for doing bidirectional reordering and contextual shaping, and storing state
when reordering and shaping a series of lines.
*/
class CBidiEngine
    {
    public:
    CBidiEngine();
    ~CBidiEngine();
    TResult Order(uint16* aText,size_t aLength,size_t& aNewLength,TBidiParDir aParDir,bool aParStart,
                            int32* aUserData = nullptr);
    static int32 ShapeArabic(uint16* aText,size_t aLength);
    
    /** Return the resolved direction; meaningful only after Order has been called at least once. */
    bool RightToLeft() { return (iStack[iStackLevel].iLevel & 1) != 0; }

    private:
    void operator=(const CBidiEngine&) = delete;
    void operator=(CBidiEngine&&) = delete;
    CBidiEngine(const CBidiEngine&) = delete;
    CBidiEngine(CBidiEngine&&) = delete;

    void Push(bool aRightToLeft,TBidiOverride aOverride);
    void ShapeAndMirror(uint16* aText,size_t& aNewLength,bool aHaveArabic,int32* aUserData);
    
    enum
        {
        KRunArraySize = 8,
        KLevels = 63 // legal levels are 0...62 
        };

    class TStackItem
        {
        public:
        uint8 iLevel;
        uint8 iOverride;
        };

    TBidiRun iRunArray[KRunArraySize]; // a preallocated array for the usual case of a small number of runs
    TBidiRun* iRun;
    size_t iRuns;
    TStackItem iStack[KLevels];
    int32 iStackLevel;
    };

}

#endif
