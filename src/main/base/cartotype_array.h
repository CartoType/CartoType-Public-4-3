/*
CARTOTYPE_ARRAY.H
Copyright (C) 2004-2017 CartoType Ltd.
See www.cartotype.com for more information.
*/

#ifndef CARTOTYPE_ARRAY_H__
#define CARTOTYPE_ARRAY_H__

#include <cartotype_types.h>
#include <cartotype_errors.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <array>
#include <memory>

namespace CartoType
{

/** An array that is kept on the stack if it has aMaxStackElements or fewer, otherwise on the heap. */
template<class T,size_t aMaxStackElements> class TStackArray
    {
    public:
    TStackArray(size_t aElements):
        iElements(aElements)
        {
        if (aElements > aMaxStackElements)
            iDataPtr = new T[aElements];
        else
            iDataPtr = iData;
        }
    ~TStackArray()
        {
        if (iElements > aMaxStackElements)
            delete [] iDataPtr;
        }

    /** Return the number of elements. */
    size_t Count() const { return iElements; }
    /** Return a writable pointer to the underlying C++ array of elements. */
    T* Data() { return iDataPtr; }
    /** Return a read-only pointer to the underlying C++ array of elements. */
    const T* Data() const { return (const T*)iDataPtr; }
    /** The writable array index operator, allowing the array to be treated like a C++ array. */
    T& operator[](size_t aIndex) { assert(aIndex < iElements); return iDataPtr[aIndex]; }
    /** The constant array index operator, allowing the array to be treated like a C++ array. */
    const T& operator[](size_t aIndex) const { assert(aIndex < iElements); return iDataPtr[aIndex]; }

    private:
    size_t iElements;
    T* iDataPtr;
    T iData[aMaxStackElements];
    };

/**
A limited-functionality variable-size array for small objects,
optimised for the cases of 0...N elements in the array.
*/
template<class T,size_t N> class CSmallArray
    {
    public:
    CSmallArray()
        {
        }
    ~CSmallArray()
        {
        if (iElements > N)
            delete iData.iPointer;
        }
    
    /** Delete all the elements. */
    void Clear()
        {
        if (iElements > N)
            delete iData.iPointer;
        iElements = 0;
        }

    /** Return the number of elements. */
    size_t Count() const { return (int32)iElements; }
    /** Return a writable pointer to the underlying C++ array of elements. */
    T* Data() { return iElements <= N ? iData.iValue : iData.iPointer->data(); }
    /** Return a read-only pointer to the underlying C++ array of elements. */
    const T* Data() const { return iElements <= N ? iData.iValue : iData.iPointer->data(); }
    /** The writable array index operator, allowing the array to be treated like a C++ array. */
    T& operator[](size_t aIndex) { assert(aIndex < iElements); return Data()[aIndex]; }
    /** The constant array index operator, allowing the array to be treated like a C++ array. */
    const T& operator[](size_t aIndex) const { assert(aIndex < iElements); return Data()[aIndex]; }
    /** Append a single element. */
    void Append(T aElement)
        {
        if (iElements < N)
            iData.iValue[iElements++] = aElement;
        else
            {
            if (iElements == N)
                {
                std::unique_ptr<std::vector<T>> v { new std::vector<T> };
                for (size_t i = 0; i < N; i++)
                    v->push_back(std::move(iData.iValue[i]));
                iData.iPointer = v.release();
                }

            iData.iPointer->push_back(aElement);
            iElements++;
            }
        }
    
    /** Delete a single element. */
    void Delete(size_t aIndex)
        {
        assert(aIndex < iElements);
        iElements--;
        if (iElements < N)
            {
            for (size_t i = aIndex; i < iElements; i++)
                iData.iValue[i] = std::move(iData.iValue[i + 1]);
            return;	        
            }
        iData.iPointer->erase(iData.iPointer->begin() + aIndex,iData.iPointer->begin() + aIndex + 1);
        if (iElements == N)
            {
            std::vector<T>* v = iData.iPointer;
            for (size_t i = 0; i < N; i++)
                iData.iValue[i] = std::move((*v)[i]);
            delete v;
            }
        }

    T* begin() { return iElements <= N ? iData.iValue : iData.iPointer->data(); }
    const T* begin() const { return iElements <= N ? iData.iValue : iData.iPointer->data(); }
    T* end() { return begin() + iElements; }
    const T* end() const { return begin() + iElements; }

    private:
    size_t iElements = 0;
    union TData
        {
        T iValue[N]; 
        std::vector<T>* iPointer;
        };
    TData iData;
    };

template<typename T> using CCompactArray = CSmallArray<T,1>;

template<typename T> void inline DeleteNulls(std::vector<std::unique_ptr<T>>& aVector)
    {
    auto iter = std::remove_if(aVector.begin(),aVector.end(),[](std::unique_ptr<T>& aObject) { return !aObject; });
    aVector.erase(iter,aVector.end());
    }

} // namespace CartoType

#endif
