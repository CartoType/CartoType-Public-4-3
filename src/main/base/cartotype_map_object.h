/*
CARTOTYPE_MAP_OBJECT.H
Copyright (C) 2013-2017 CartoType Ltd.
See www.cartotype.com for more information.
*/

#ifndef CARTOTYPE_MAP_OBJECT_H__
#define CARTOTYPE_MAP_OBJECT_H__

#include <cartotype_string.h>
#include <cartotype_path.h>
#include <cartotype_address.h>
#include <cartotype_transform.h>

namespace CartoType
{
class CBasicMapObject;
class CMapAttributeTest;
class CMapObject;
class TMapTransform;
class CProjection;

/** A type for arrays of map objects returned by search functions. */
typedef std::vector<std::unique_ptr<CMapObject>> CMapObjectArray;

/**
The abstract base map object class. A map object is a path that
also possesses cartographic attributes.
*/
class CMapObject: public MPath
    {
    public:
    virtual ~CMapObject()
        {
        }
    
    /**
    Return all the string attributes, null-separated, as key-value pairs separated
    by an equals sign. The first key must be empty, and means the label or name.
    */
    virtual TText StringAttributes() const = 0;
    
    /** Return the default label or name of the object: the first, unnamed, string attribute. */
    virtual TText Label() const = 0;

    /** Return a writable interface to the contour indexed by aIndex. */
    virtual MWritableContour& WritableContour(size_t aIndex) = 0;

    /** Return a pointer to writable string attributes if possible; used by makemap when compressing strings. */
    virtual MString* WritableStringAttributes() { return nullptr; }

    virtual const TBitmap* Bitmap() const;
    virtual const TTransformFP* BitmapTransform() const;
    virtual void Normalize();
    virtual void Simplify(int32 aResolution);
    virtual void Split(size_t aMaxPoints,CMapObjectArray& aExtraObjectArray);

    TText GetStringAttribute(const MString& aName) const
        {
        return StringAttributes().GetAttribute(aName);
        };
    
    TText GetStringAttribute(const CString& aName) const
        {
        return StringAttributes().GetAttribute(aName);
        }
    
    TText GetStringAttributeForLocale(const MString& aName,const char* aLocale) const;
    
    TText GetStringAttributeForLocale(const CString& aName,const char* aLocale) const
        {
        return GetStringAttributeForLocale((const MString&)aName,aLocale);
        }

    bool GetStringAttributeUsingPath(const MString& aPath,const MString& aDefault,TText& aValue) const;

    /**	
    Return the identifier of this object.
    Identifiers are opaque 64-bit numbers that are specific to a database.
    The value zero is reserved and means 'no identifier'.
    */
    uint64 Id() const { return iId; }

    /** Return the type of the object. */
    TMapObjectType Type() const { return iType; }
    
    /** Return the integer attribute. */
    int32 IntAttribute() const { return iIntAttribute; }

    /** Return the name of the layer this object belongs to. */
    const MString& LayerName() const { return *iLayer; }

    /** Return the reference-counted name of the layer. */
    CRefCountedString LayerRef() const { return iLayer; }

    std::string Description() const;
    std::string VerboseDescription() const;
    TPointFP Center() const;
    bool GetHeight(TPoint aPoint,bool aHaveMetres,const TTransformFP* aInverseTransform,int32& aHeight) const;
    TGeoCodeType GetGeoCodeType() const;
    void GetGeoCodeItem(CGeoCodeItem& aGeoCodeItem,const char* aLocale) const;
    double MaxSpeed() const;
    double Top() const;
    double Bottom() const;
    double DimensionAttribute(const MString& aKey) const;
    bool NextStringAttribute(size_t& aPos,TText& aName,TText& aValue) const;
    bool Mergeable(const CMapObject& aOther) const;
    void Project(const TMapTransform& aProjection);
    void Project(const CProjection& aProjection);
    void Smooth();
    void Offset(int32 aDx,int32 aDy);
    CMapObject* Clip(const TRect& aClip) const;
    void SetType(TMapObjectType aType) { iType = aType; }
    TResult Write(TDataOutputStream& aOutput,const CProjection* aProjectionFromLatLong = nullptr) const;
    double Area(TResult& aError,const CProjection& aProjection) const;
    double LengthOrPerimeter(TResult& aError,const CProjection& aProjection) const;

    std::unique_ptr<CMapObject> Copy(TResult& aError) const;

    void SetId(uint64 aId) { iId = aId; }
    void SetIntAttribute(int32 aValue) { iIntAttribute = aValue; }
    void SetLayer(CRefCountedString aLayer) { iLayer = aLayer; }

    /** Information returned by GetMatch. */
    class CMatch
        {
        public:
        /** The name of the attribute in which the matched text was found. */
        CString iKey;
        /** The value of the attribute in which the matched text was found. */ 
        CString iValue;
        /** The start position of the matched text within the value. */
        size_t iStart;
        /** The end position of the matched text within the value. */
        size_t iEnd;
        };
    
    TResult GetMatch(CMatch& aMatch,const MString& aText,TStringMatchMethod aMatchMethod,const MString* aAttributes = nullptr,bool aPhrase = true) const;

    enum TMatchType
        {
        ENoMatch,
        EPartialMatch,
        EFuzzyMatch,
        EFullMatch
        };

    TMatchType MatchType(const MString& aText) const;

    /** A type for arbitrary user-defined data associated with a map object. */
    union TUserData
        {
        TUserData() { memset(this,0,sizeof(TUserData)); }

        int64 iInt;
        void* iPtr;
        };

    /** Set the user data to a 64-bit integer. */
    void SetUserData(int64 aData) { iUserData.iInt = aData; }
    /** Return the user data as a 64-bit integer. */
    int64 UserData() const { return iUserData.iInt; }
    /** Set the user data to a pointer. */
    void SetUserDataToPointer(void* aData) { iUserData.iPtr = aData; }
    /** Return the user data as a pointer. */
    void* UserDataAsPointer() const { return iUserData.iPtr; }
    /** Set the user data to a union */
    void SetUserDataToUnion(TUserData aUserData) { iUserData = aUserData; }
    /** Return the user data as a union. */
    TUserData UserDataAsUnion() const { return iUserData; }
        
    protected:
    CMapObject(CRefCountedString aLayer,TMapObjectType aType):
        iLayer(aLayer),
        iType(aType)
        {
        assert(aLayer);
        }
    CMapObject(CRefCountedString aLayer,TMapObjectType aType,TUserData aUserData):
        iLayer(aLayer),
        iType(aType),
        iUserData(aUserData)
        {
        assert(aLayer);
        }
    CMapObject(const CMapObject& aOther):
        iLayer(aOther.iLayer),
        iType(aOther.iType),
        iIntAttribute(aOther.iIntAttribute),
        iUserData(aOther.iUserData)
        {
        }

    CRefCountedString iLayer;               // the layer this object belongs to
    uint64 iId = 0;							// the object's identifier
    TMapObjectType iType;					// the type of the object: point, line, polygon, etc.
    int32 iIntAttribute = 0;                // the integer attribute, used by convention for road types, routing flags, or sub-types of layers
    TUserData iUserData;				    // user data; can hold an integer or a pointer; can be used to associate any value with a map object
    };

/**
Construct an integer attribute from an optional three-letter code, placed
in the high 16 bits by encoding each letter in five bits, and a type number placed in
the low 16 bits.

The three-letter code is used only if it consists of exactly three lower-case letters
in the range a...z.

Three-letter codes are used for mnemonic purposes, as for example "pub" for a public house,
and can be tested easily in style sheets.
*/
inline int32 IntAttribute(const char* aThreeLetterCode,int aType)
    {
    int32 a = 0;
    if (aThreeLetterCode &&
        strlen(aThreeLetterCode) == 3 &&
        aThreeLetterCode[0] >= 'a' && aThreeLetterCode[0] <= 'z' &&
        aThreeLetterCode[1] >= 'a' && aThreeLetterCode[1] <= 'z' &&
        aThreeLetterCode[2] >= 'a' && aThreeLetterCode[2] <= 'z')
        a = ((((aThreeLetterCode[0])-'a') << 27) | (((aThreeLetterCode[1])-'a') << 22) | (((aThreeLetterCode[2])-'a') << 17));
    a |= (aType & 0xFFFF);
    return a;
    }

inline bool MapObjectUserDataLessThan(const std::unique_ptr<CMapObject>& aP,const std::unique_ptr<CMapObject>& aQ)
    {
    return aP->UserData() < aQ->UserData();
    }

}

#endif
