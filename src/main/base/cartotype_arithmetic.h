/*
CARTOTYPE_ARITHMETIC.H
Copyright (C) 2004-2015 CartoType Ltd.
See www.cartotype.com for more information.
*/

#ifndef CARTOTYPE_ARITHMETIC_H__
#define CARTOTYPE_ARITHMETIC_H__

#include "cartotype_base.h"
#include "cartotype_errors.h"

#include <math.h>

namespace CartoType
{

// forward declarations
class TTransform;
class TFixedSmall;
class TFixed;
class TPointFixed;

/**
A 64-bit integer type like FreeType's FT_Int64, for
use with arithmetic routines borrowed from FreeType.
*/
class TInt64
    {
    public:
    /** The low 32 bits. */
    uint32 iLo;
    /** The high 32 bits. */
    uint32 iHi;
    };

/**
The type of intersection of two line segments,
made up of bit flags for convenience of testing.
*/
enum TIntersectionType
    {
    /** Lines are parallel or coincident. */
    ENoIntersection = 0,
    /** The intersection is before the start of the first segment. */
    EFirstSegmentIntersectionBefore = 1,
    /** The intersection is on the first segment. */
    EFirstSegmentIntersection = 2,
    /** The intersection is after the end of the first segment. */
    EFirstSegmentIntersectionAfter = 4,
    /** The intersection is before the start of the second segment. */
    ESecondSegmentIntersectionBefore = 8,
    /** The intersection is on the second segment. */
    ESecondSegmentIntersection = 16,
    /** The intersection is after the end of the second segment. */
    ESecondSegmentIntersectionAfter = 32,
    
    /** Combination of flags: the intersection is on both segments. */
    EBothSegmentsIntersection = EFirstSegmentIntersection | ESecondSegmentIntersection
    };

/** A wrapper for static arithmetic functions. */
class Arithmetic
    {
    public:
    static int32 MulDiv(int32 aA,int32 aB,int32 aC);
    static void MulTo64(int32 aX,int32 aY,TInt64& aZ);
    static void Add64(TInt64 aX,TInt64 aY,TInt64& aZ);
    static int32 Div64By32(TInt64 aX,int32 aY);
    static TResult LineIntersection(double& aIntersectionX,double& aIntersectionY,
                                    double aStart1X,double aStart1Y,
                                    double aEnd1X,double aEnd1Y,
                                    double aStart2X,double aStart2Y,
                                    double aEnd2X,double aEnd2Y,
                                    TIntersectionType* aIntersectionType = nullptr);
    static TResult LineIntersection(TPoint& aIntersection,
                                    const TPoint& aStart1,const TPoint& aEnd1,
                                    const TPoint& aStart2,const TPoint& aEnd2,
                                    TIntersectionType* aIntersectionType = nullptr);
    static void LineBisection(TPoint& aBisectStart, TPoint& aBisectEnd,
                              const TPoint& aStart1,const TPoint& aEnd1,
                              const TPoint& aStart2,const TPoint& aEnd2);
    static bool LineSegmentContainsPoint(const TPoint& aStart,const TPoint& aEnd,const TPoint& aPoint);
    static double DistanceSquaredFromLineSegmentToPoint(double aStartX,double aStartY,double aEndX,double aEndY,
                                                        double aPointX,double aPointY,
                                                        double* aNearestPointX,double* aNearestPointY);
    static double DistanceSquaredBetweenLineSegments(const TPointFP& aStart1,const TPointFP& aEnd1,
                                                     const TPointFP& aStart2,const TPointFP& aEnd2,
                                                     TPointFP* aNearest1 = nullptr,TPointFP* aNearest2 = nullptr,
                                                     TIntersectionType* aIntersectionType = nullptr);
    static void GetLineTangent(double aX1,double aY1,double aX2,double aY2,double aDistance,double& aPointX,double& aPointY);
    static void GetVertexTangent(TPointFP& aTangentEnd,const TPointFP& aP1,const TPointFP& aP2,const TPointFP& aP3);
    static void GetVertexTangent(TPoint& aTangentEnd,const TPoint& aP1,const TPoint& aP2,const TPoint& aP3);

    /** Return minimum of two numbers. */
    template<typename T> static T Min(T aA,T aB)
        { return aA < aB ? aA : aB; }
    /** Return maximum of two numbers. */
    template<typename T> static T Max(T aA,T aB)
        { return aA > aB ? aA : aB; }
    /** Return absolute value of number. */
    template<typename T> static T Abs(T aValue)
        { return aValue > 0 ? aValue : -aValue; }

    static int32 Sqrt(int32 aValue);
    static int32 CubeRoot(int32 aValue);
    static double Strtod(const char* aString,size_t aLength = UINT32_MAX,const char** aEndPtr = nullptr);
    static double Strtod(const uint16* aString,size_t aLength = UINT32_MAX,const uint16** aEndPtr = nullptr);

    /**
    Rounds a floating-point value to the nearest integer.
    Does not use floor() because it is said to be slow on some platforms.
    */
    static int32 Round(double aValue)
        {
        return (int32)(aValue < 0.0 ? aValue - 0.5 : aValue + 0.5);
        }

    /**
    Return the difference between two angles in radians. 
    The result, expressed in radians, is the angular distance swept out when moving
    from aP to aQ by the shortest route, and thus is in the range -pi ... pi.

    For a coordinate system with Y increasing upwards, positive results show a
    clockwise turn and negative results show an anticlockwise turn.
    */ 
    static double AngleDiff(double aP,double aQ)
        {
        double diff = aP - aQ;
        if (aP > aQ)
            {
            if (diff > KPiDouble)
                diff -= KPiDouble * 2;
            }
        else
            {
            if (diff < -KPiDouble)
                diff += KPiDouble * 2;
            }
        return diff;
        }

    /**
    Return the absolute difference between two angles in radians. 
    The result, expressed in radians, is the angular distance swept out when moving
    from aP to aQ by the shortest route, and thus is in the range 0..pi 
    */ 
    static double AbsoluteAngleDiff(double aP,double aQ)
        {
        double diff = AngleDiff(aP,aQ);
        return diff >= 0 ? diff : -diff;
        }
    
    /** Return the opposite of an angle in radians. */
    static double AngleOpposite(double aAngle)
        {
        if (aAngle >= 0)
            return aAngle - KPiDouble;
        return aAngle + KPiDouble;
        }
    };

/**
A fixed-point number consisting of a 16-bit integer plus 16 fractional bits.
This type and the routines used are based on FreeType's FT_FIXED
type and the FT_MulFix and FT_DivFix functions.
*/
class TFixed
    {
    public:
    /** Construct a TFixed with the value zero. */
    TFixed(): iValue(0) { }
    /** Construct a TFixed from an integer. */
    TFixed(int aValue): iValue(aValue << 16) { }
    /** Construct a TFixed from a value with 0...16 fractional bits. */
    TFixed(int32 aValue,int32 aFractionalBits)
        { Assert(aFractionalBits >= 0 && aFractionalBits <= 16); iValue = aValue << (16 - aFractionalBits); }
    /** Construct a TFixed from a double-precision floating-point number.*/
    TFixed(double aValue) { iValue = Arithmetic::Round(aValue * 65536.0); }
    /** A dummy type allowing construction of a TFixed from a raw value. */
    enum TRaw { ERaw };
    /** Construct a TFixed from a raw value. */
    TFixed(int32 aValue,TRaw): iValue(aValue) { }
    /** A dummy type allowing construction of a TFixed from 64ths. */
    enum T64ths { E64ths };
    /** Construct a TFixed from 64ths. */
    TFixed(int32 aValue,T64ths): iValue(aValue << 10) { }
    /** Construct a TFixed from a TFixedSmall. */
    inline TFixed(const TFixedSmall& aFixedSmall);
    /** Return the value in 65536ths. */
    int32 RawValue() const { return iValue; }
    /** Return the value as a double-precision floating-point number. */
    double FpValue() const { return (double)iValue / 65536.0; }
    /** Set the value in 65536ths. */
    void SetRawValue(int32 aRawValue) { iValue = aRawValue; }
    /** The equality operator. */
    bool operator==(TFixed aFixed) const { return iValue == aFixed.iValue; }
    /** The inequality operator. */
    bool operator!=(TFixed aFixed) const { return iValue != aFixed.iValue; }
    /** The less-than operator. */
    bool operator<(TFixed aFixed) const { return iValue < aFixed.iValue; }
    /** The less-than-or-equal operator. */
    bool operator<=(TFixed aFixed) const { return iValue <= aFixed.iValue; }
    /** The greater-than operator. */
    bool operator>(TFixed aFixed) const { return iValue > aFixed.iValue; }
    /** The greater-than-or-equal operator. */
    bool operator>=(TFixed aFixed) const { return iValue >= aFixed.iValue; }
    /** Return the value rounded to the nearest unit. */
    int32 Rounded() const { return (iValue + 32768) >> 16; }
    /** Return the nearest integer value at or below the actual value. */
    int32 Floor() const { return iValue >> 16; }
    /** Return the nearest integer value at or above the actual value. */
    int32 Ceiling() const { return (iValue + 65535) >> 16; }
    /** Return the value rounded to the nearest 64th. */
    int32 Rounded64ths() const { return (iValue + 512) >> 10; }
    /** Add a fixed-point value. */
    TFixed operator+(TFixed aFixed) const { TFixed f = *this; f.iValue += aFixed.iValue; return f; }
    /** Increment by a fixed-point value. */
    void operator+=(TFixed aFixed) { iValue += aFixed.iValue; }
    /** Subtract a fixed-point value. */
    TFixed operator-(TFixed aFixed) const { TFixed f = *this; f.iValue -= aFixed.iValue; return f; }
    /** Decrement by a fixed-point value. */
    void operator-=(TFixed aFixed) { iValue -= aFixed.iValue; }
    /** An assignment operator to multiply two fixed-point values. */
    void operator*=(TFixed aB)
        {
        if (iValue == 0 || aB.iValue == 0x10000)
            return;
        
        // CartoType testing showed that aB was zero in about 8% of cases.
        if (aB.iValue == 0)
            {
            iValue = 0;
            return;
            }

        int s = 1;
        if (iValue < 0)
            {
            iValue = -iValue;
            s = -1;
            }
        if (aB.iValue < 0)
            {
            aB.iValue = -aB.iValue;
            s = -s;
            }
        if (s > 0)
            iValue = (int64(iValue) * int64(aB.iValue) + 0x8000) >> 16;
        else
            iValue = -((int64(iValue) * int64(aB.iValue) + 0x8000) >> 16);
        }
    /** Multiply by a fixed-point value. */
    TFixed operator*(TFixed aFixed) const { TFixed f = *this; f *= aFixed; return f; }
    /** An assignment operator to multiply by an integer value. */
    void operator*=(int32 aInt) { iValue *= aInt; }
    /** Multiply by an integer. */
    TFixed operator*(int32 aInt) const { TFixed f = *this; f.iValue *= aInt; return f; }
    /** An assignment operator to divide one fixed-point value by another. */
    void operator/=(TFixed aB)
        {
        if (iValue == 0 || aB.iValue == 0x10000)
            return;

        int s = 1;
        if (iValue < 0)
            {
            iValue = -iValue;
            s = -1;
            }
        if (aB.iValue < 0)
            {
            aB.iValue = -aB.iValue;
            s = -s;
            }

        uint32_t q;
        if (aB.iValue == 0)
            /* check for division by 0 */
            q = 0x7FFFFFFFL;
        else
            /* compute result directly */
            q = (uint32_t)((((int64_t)iValue << 16) + (aB.iValue >> 1)) / aB.iValue);

        iValue = s < 0 ? -(int32_t)q : (int32_t)q;
        }

    /** Divide by a fixed-point value. */
    TFixed operator/(TFixed aFixed) const { TFixed f = *this; f /= aFixed; return f; }
    /** An assignment operator to divide by an integer. */
    void operator/=(int32 aInt) { iValue /= aInt; }
    /** Divide by an integer. */
    TFixed operator/(int32 aInt) const { TFixed f = *this; f.iValue /= aInt; return f; }
    /** The unary negation operator. */
    TFixed operator-() const { TFixed f; f.iValue = -iValue; return f; }
    TFixed Sqrt() const;
    TFixedSmall ToRadians() const;
    TFixed AngleDiff(TFixed aAngle) const;
    static TFixed Pi();
    static TFixed HalfPi();
    /** Return the integer part, rounding down. The integer part of 3.6 is 3; the integer part of -0.2 is -1. */
    int32 IntegerPart() const { return iValue >> 16; }
    /** Return the fractional part, rounding down. The fractional part of 3.6 is 0.6; the fractional part of -0.2 is 0.8. */
    TFixed FractionalPart() const { TFixed f = *this; f.iValue &= 0x0000FFFF; return f; }
    /** Return the absolute value of the fixed number. */
    TFixed Abs() const
        {
        if (iValue >= 0)
            return *this;
        return -*this;
        }
    /** Return true if the number is zero. */
    bool IsZero() const { return iValue == 0; }
    /** Return true if the number is non-zero. */
    bool NonZero() const { return iValue != 0; }

    private:
    int32 iValue;
    };

/**
A fixed-point number consisting of a 5-bit integer plus 27 fractional bits.
*/
class TFixedSmall
    {
    public:
    /** Construct a TFixedSmall with the value zero. */
    TFixedSmall(): iValue(0) { }
    /** Construct a TFixedSmall from an integer. */
    TFixedSmall(int32 aValue): iValue(aValue << 27) { }
    /** Construct a TFixedSmall from a value with 0...27 fractional bits. */
    TFixedSmall(int32 aValue,int32 aFractionalBits)
        { Assert(aFractionalBits >= 0 && aFractionalBits <= 27); iValue = aValue << (27 - aFractionalBits); }
    /** A dummy type allowing construction of a TFixedSmall from a raw value. */
    enum TRaw { ERaw };
    /** Construct a TFixedSmall from a raw value. */
    TFixedSmall(int32 aValue,TRaw): iValue(aValue) { }
    /** Construct a TFixedSmall from a TFixed, ignoring overflows */
    TFixedSmall(const TFixed& aFixed)
        { iValue = aFixed.RawValue() << 11; }
    /** Return the value in units of 1/(2^27). */
    int32 RawValue() const { return iValue; }
    /** Set the value in units of 1/(2^27). */
    void SetRawValue(int32 aRawValue) { iValue = aRawValue; }
    /** The equality operator. */
    bool operator==(TFixedSmall aFixed) const { return iValue == aFixed.iValue; }
    /** The inequality operator. */
    bool operator!=(TFixedSmall aFixed) const { return iValue != aFixed.iValue; }
    /** The less-than operator. */
    bool operator<(TFixedSmall aFixed) const { return iValue < aFixed.iValue; }
    /** The less-than-or-equal operator. */
    bool operator<=(TFixedSmall aFixed) const { return iValue <= aFixed.iValue; }
    /** The greater-than operator. */
    bool operator>(TFixedSmall aFixed) const { return iValue > aFixed.iValue; }
    /** The greater-than-or-equal operator. */
    bool operator>=(TFixedSmall aFixed) const { return iValue >= aFixed.iValue; }
    /** Return the value rounded to the nearest unit. */
    int32 Rounded() const { return (iValue + 67108864) >> 27; }
    /** Return the nearest integer value at or below the actual value. */
    int32 Floor() const { return iValue >> 27; }
    /** Return the nearest integer value at or above the actual value. */
    int32 Ceiling() const { return (iValue + ((1 << 27) - 1)) >> 27; }
    /** Add a TFixedSmall value. */
    TFixedSmall operator+(TFixedSmall aFixed) const { TFixedSmall f = *this; f.iValue += aFixed.iValue; return f; }
    /** Increment by a TFixedSmall value. */
    void operator+=(TFixedSmall aFixed) { iValue += aFixed.iValue; }
    /** Subtract a TFixedSmall value. */
    TFixedSmall operator-(TFixedSmall aFixed) const { TFixedSmall f = *this; f.iValue -= aFixed.iValue; return f; }
    /** Decrement by a TFixedSmall value. */
    void operator-=(TFixedSmall aFixed) { iValue -= aFixed.iValue; }
    void operator*=(TFixedSmall aFixed);
    /** Multiply by a TFixedSmall value. */
    TFixedSmall operator*(TFixedSmall aFixed) const { TFixedSmall f = *this; f *= aFixed; return f; }
    /** An assignment operator to multiply by an integer. */
    void operator*=(int32 aInt) { iValue *= aInt; }
    /** Multiply by an integer. */
    TFixedSmall operator*(int32 aInt) const { TFixedSmall f = *this; f.iValue *= aInt; return f; }
    void operator/=(TFixedSmall aFixed);
    /** Divide by a TFixedSmall value. */
    TFixedSmall operator/(TFixedSmall aFixed) const { TFixedSmall f = *this; f /= aFixed; return f; }
    /** An assignment operator to divide by an integer. */
    void operator/=(int32 aInt) { iValue /= aInt; }
    /** Divide by an integer. */
    TFixedSmall operator/(int32 aInt) const { TFixedSmall f = *this; f.iValue /= aInt; return f; }
    /** The unary negation operator. */
    TFixedSmall operator-() const { TFixedSmall f; f.iValue = -iValue; return f; }
    TFixedSmall Sqrt() const;
    TFixedSmall Atanh() const;
    TFixedSmall Ln() const;
    TFixedSmall Exp() const;
    TFixedSmall Atan() const;
    TFixedSmall Asin() const;
    TFixedSmall Acos() const;
    TFixedSmall Sin() const;
    TFixedSmall Cos() const;
    TFixedSmall Tan() const;
    TFixed ToDegrees() const;
    TFixedSmall AngleDiff(TFixedSmall aAngle) const;
    static TFixedSmall Pi();
    static TFixedSmall HalfPi();

    private:
    int32 iValue;
    };

inline TFixed::TFixed(const TFixedSmall& aFixedSmall):
    iValue((aFixedSmall.RawValue() + 1024) >> 11) { }

/** A point class containing two fixed-point numbers; see also TPointFixedSmall. */
class TPointFixed
    {
    public:
    TPointFixed() { }
    TPointFixed(TFixed aX,TFixed aY): iX(aX), iY(aY) { }
    TPointFixed(int32 aX,int32 aY): iX(aX), iY(aY) { }
    TPointFixed(const TPoint& aPoint): iX(aPoint.iX), iY(aPoint.iY) { }
    /** Construct a TPointFixed from a point with 0...16 fractional bits. */
    TPointFixed(const TPoint& aPoint,int32 aFractionalBits): iX(aPoint.iX,aFractionalBits), iY(aPoint.iY,aFractionalBits) { }
    /** Construct a TPointFixed from a point containing raw values (with 16 fractional bits). */
    TPointFixed(const TPoint& aPoint,TFixed::TRaw): iX(aPoint.iX,TFixed::ERaw), iY(aPoint.iY,TFixed::ERaw) { }
    bool operator==(const TPointFixed& aPoint) const { return iX == aPoint.iX && iY == aPoint.iY; }
    bool operator!=(const TPointFixed& aPoint) const { return !(*this == aPoint); }
    /** Offset a point by another point, treated as a positive vector. */
    void operator+=(const TPointFixed& aPoint) { iX += aPoint.iX; iY += aPoint.iY; }
    /** Offset a point by another point, treated as a negative vector. */
    void operator-=(const TPointFixed& aPoint) { iX -= aPoint.iX; iY -= aPoint.iY; }
    /** Multiply the vector represented by the point. */
    void operator*=(const TFixed& aFixed) { iX *= aFixed; iY *= aFixed; }
    /** Return the length of the vector represented by the point. */
    TFixed VectorLength() const
        {
        if (iX.RawValue() == 0)
            return iY.RawValue() >= 0 ? iY : -iY;
        if (iY.RawValue() == 0)
            return iX.RawValue() >= 0 ? iX : -iX;
        return VectorLengthHelper();
        }
    /** Return an integer point in rounded 64ths of the values in the TPointFixed object. */
    TPoint Rounded64ths() const
        {
        int32 x = iX.Rounded64ths();
        int32 y = iY.Rounded64ths();
        return TPoint(x,y);
        }
    /** Return an integer point, rounding the values in the TPointFixed object to units. */
    TPoint Rounded() const
        {
        int32 x = iX.Rounded();
        int32 y = iY.Rounded();
        return TPoint(x,y);
        }
    void Polarize();
    void Rotate(TFixedSmall aAngle);
    TFixedSmall Atan2() const;
    
    /** The x coordinate. */
    TFixed iX;
    /** The y coordinate. */
    TFixed iY;

    private:
    TFixed VectorLengthHelper() const;
    };

/** A point class containing two fixed-point numbers; see also TPointFixed. */
class TPointFixedSmall
    {
    public:
    TPointFixedSmall(): iX(0), iY(0) { }
    TPointFixedSmall(TFixedSmall aX,TFixedSmall aY): iX(aX), iY(aY) { }
    TPointFixedSmall(int32 aX,int32 aY): iX(aX), iY(aY) { }
    TPointFixedSmall(const TPoint& aPoint): iX(aPoint.iX), iY(aPoint.iY) { }
    TPointFixedSmall(const TPointFixed& aPoint): iX(aPoint.iX), iY(aPoint.iY) { }
    /** Construct a TPointFixedSmall from a point with 0...27 fractional bits. */
    TPointFixedSmall(const TPoint& aPoint,int32 aFractionalBits): iX(aPoint.iX,aFractionalBits), iY(aPoint.iY,aFractionalBits) { }
    bool operator==(const TPointFixedSmall& aPoint) const { return iX == aPoint.iX && iY == aPoint.iY; }
    bool operator!=(const TPointFixedSmall& aPoint) const { return !(*this == aPoint); }
    /** Offset a point by another point, treated as a positive vector. */
    void operator+=(const TPointFixedSmall& aPoint) { iX += aPoint.iX; iY += aPoint.iY; }
    /** Offset a point by another point, treated as a negative vector. */
    void operator-=(const TPointFixedSmall& aPoint) { iX -= aPoint.iX; iY -= aPoint.iY; }
    void Rotate(TFixedSmall aAngle);
    void HyperbolicRotate(TFixedSmall aZ);
    TFixedSmall Atan2() const;
    TFixedSmall Atanh() const;

    /** The x coordinate. */
    TFixedSmall iX;
    /** The y coordinate. */
    TFixedSmall iY;
    };

/** A straight line or line segment defined using fixed-point numbers. */
class TLineFixed
    {
    public:
    TLineFixed() { }
    TLineFixed(const TPointFixed& aStart,const TPointFixed& aEnd):
        iStart(aStart),
        iEnd(aEnd),
        iLength(-1,TFixed::ERaw)
        {
        }
    /** Return the length of the line segment. */
    TFixed Length() const
        {
        if (iLength.RawValue() < 0)
            {
            TPointFixed p(iEnd); p -= iStart; iLength = p.VectorLength();
            }
        return iLength;
        }
    /** Return the start of the line segment. */
    const TPointFixed& Start() const { return iStart; }
    /** Return the end of the line segment. */
    const TPointFixed& End() const { return iEnd; }
    bool DistanceExceeds(double aX1,double aY1,double aX2,double aY2,int32 aPoints,double aMaxDistance) const;
    void GetTangent(TFixed aDistance,TPointFixed& aPoint) const;
    TLineFixed Reverse() const { TLineFixed l; l.iStart = iEnd; l.iEnd = iStart; l.iLength = iLength; return l; }

    private:
    TPointFixed iStart;
    TPointFixed iEnd;
    mutable TFixed iLength;
    };

void operator*=(int32& aValue,TFixed aFixed);

/**
Multiply an integer by a fixed-point value and return an integer.
The return value is rounded.
*/
inline int32 operator*(int32 aValue,TFixed aFixed)
    {
    int32 result = aValue;
    result *= aFixed;
    return result;
    }

/**
An arctangent function which checks for two zero arguments and returns zero in that case.
In the standard library atan2(0,0) is undefined, and on Embarcadero C++ Builder it throws an exception.
*/
inline double Atan2(double aY,double aX)
    {
    if (aY == 0 && aX == 0)
        return 0;
    return atan2(aY,aX);
    }

} // namespace CartoType

#endif
