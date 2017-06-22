/*
CARTOTYPE_TREE.H
Copyright (C) 2012-2017 CartoType Ltd.
See www.cartotype.com for more information.
*/

#ifndef CARTOTYPE_TREE_H__
#define CARTOTYPE_TREE_H__

#include <cartotype_base.h>
#include <cartotype_errors.h>

namespace CartoType
{

/**
A templated binary tree class for pointers to objects of type T,
which must have members iLeft, iRight and iParent, all pointers to T,
and must have a key function Key() returning type K,
and a static CompareKeys function returning negative, zero and positive results
according to the order of the comparison.

CPointerTree owns the objects in it if aOwnData is true on construction.
*/
template<class T,class K> class CPointerTree
    {
    public:
    CPointerTree(bool aOwnData):
        iOwnData(aOwnData)
        {
        }

    ~CPointerTree()
        {
        if (!iOwnData)
            return;

        DeleteAll(iRoot);
        }

    void Clear()
        {
        if (iOwnData)
            DeleteAll(iRoot);
        iRoot = nullptr;
        iCount = 0;
        }

    size_t Count() const
        {
        return iCount;
        }

    /**
    Insert a new item into the tree. If aForceInsert is true,
    insert the item even if its key is a duplicate of an existing key.
    Return the item inserted, or found if aForceInsert is false and the new item
    is a duplicate.
    */
    T* Insert(T* aItem,bool aForceInsert = true)
        {
        if (iRoot == nullptr)
            {
            Assert(iCount == 0);
            iRoot = aItem;
            aItem->iLeft = aItem->iRight = aItem->iParent = nullptr;
            iCount = 1;
            return aItem;
            }

        T* parent_node = nullptr;
        T* cur_node = iRoot;
        K key = aItem->Key();
        for (;;)
            {
            parent_node = cur_node;
            int order = T::CompareKeys(key,cur_node->Key());
            if (order < 0)
                {
                cur_node = cur_node->iLeft;
                if (cur_node == nullptr)
                    {
                    parent_node->iLeft = aItem;
                    aItem->iParent = parent_node;
                    aItem->iLeft = aItem->iRight = nullptr;
                    iCount++;
                    return aItem;
                    }
                }
            else
                {
                if (order == 0 && !aForceInsert)
                    return cur_node;

                cur_node = cur_node->iRight;
                if (cur_node == nullptr)
                    {
                    parent_node->iRight = aItem;
                    aItem->iParent = parent_node;
                    aItem->iLeft = aItem->iRight = nullptr;
                    iCount++;
                    return aItem;
                    }
                }
            }
        }

    T* Find(K aKey) const
        {
        T* cur_node = iRoot;
        while (cur_node != nullptr)
            {
            int order = T::CompareKeys(aKey,cur_node->Key());
            if (order == 0)
                break;
            if (order < 0)
                cur_node = cur_node->iLeft;
            else
                cur_node = cur_node->iRight;
            }
        return cur_node;
        }

    T* FindGreaterThanOrEqual(K aKey) const
        {
        T* cur_node = iRoot;
        while (cur_node != nullptr)
            {
            int order = T::CompareKeys(aKey,cur_node->Key());
            if (order == 0)
                break;
            if (order < 0)
                {
                if (cur_node->iLeft == nullptr)
                    return cur_node;
                cur_node = cur_node->iLeft;
                }
            else
                {
                if (cur_node->iRight == nullptr)
                    return Next(cur_node);
                cur_node = cur_node->iRight;
                }
            }
        return cur_node;
        }

    T* Min()
        {
        if (!iRoot)
            return nullptr;
        return Min(iRoot);
        }

    const T* Min() const
        {
        if (!iRoot)
            return nullptr;
        return Min(iRoot);
        }

    T* Max()
        {
        if (!iRoot)
            return nullptr;
        return Max(iRoot);
        }

    const T* Max() const
        {
        if (!iRoot)
            return nullptr;
        return Max(iRoot);
        }

    void Delete(K aKey)
        {
        T* z = Find(aKey);
        if (!z)
            return;
        Delete(z);
        }

    void RemoveNodeWithZeroOrOneChild(T* aNode)
        {
        Assert(aNode->iLeft == nullptr || aNode->iRight == nullptr);
        T* child = aNode->iLeft == nullptr ? aNode->iRight : aNode->iLeft;
        if (aNode->iParent)
            {
            if (aNode->iParent->iLeft == aNode)
                aNode->iParent->iLeft = child;
            else
                aNode->iParent->iRight = child;
            }
        else
            {
            Assert(aNode == iRoot);
            iRoot = child;
            }

        if (child)
            child->iParent = aNode->iParent;
        }

    void Delete(T* aNode,bool aTakeOwnership = false)
        {
        if (!aNode)
            return;

        // No children or one child.
        if (aNode->iLeft == nullptr || aNode->iRight == nullptr)
            RemoveNodeWithZeroOrOneChild(aNode);

        // Two children.
        else
            {
            T* next = Next(aNode);
            RemoveNodeWithZeroOrOneChild(next);
            if (aNode->iParent)
                {
                if (aNode->iParent->iLeft == aNode)
                    aNode->iParent->iLeft = next;
                else
                    aNode->iParent->iRight = next;
                }
            next->iParent = aNode->iParent;
            next->iLeft = aNode->iLeft;
            if (next->iLeft)
                next->iLeft->iParent = next;
            next->iRight = aNode->iRight;
            if (next->iRight)
                next->iRight->iParent = next;
            if (next->iParent == nullptr)
                iRoot = next;
            }

        if (iOwnData && !aTakeOwnership)
            delete aNode;
        else
            aNode->iLeft = aNode->iRight = aNode->iParent = nullptr; // a safeguard against deleting already-deleted items

        Assert(iCount > 0);
        iCount--;
        }

    static T* Prev(T* aNode)
        {
        if (aNode->iLeft)
            return Max(aNode->iLeft);
        T* x = aNode;
        T* y = aNode->iParent;
        while (y && x == y->iLeft)
            {
            x = y;
            y = y->iParent;
            }
        return y;
        }

    static T* Next(T* aNode)
        {
        if (aNode->iRight)
            return Min(aNode->iRight);
        T* x = aNode;
        T* y = aNode->iParent;
        while (y && x == y->iRight)
            {
            x = y;
            y = y->iParent;
            }
        return y;
        }

    static const T* Next(const T* aNode)
        {
        if (aNode->iRight)
            return Min(aNode->iRight);
        const T* x = aNode;
        const T* y = aNode->iParent;
        while (y && x == y->iRight)
            {
            x = y;
            y = y->iParent;
            }
        return y;
        }

    private:
    static void DeleteAll(T* aRoot)
        {
        if (aRoot)
            {
            DeleteAll(aRoot->iLeft);
            DeleteAll(aRoot->iRight);
            delete aRoot;
            }
        }

    static T* Min(T* aNode)
        {
        T* cur_node = aNode;
        while (cur_node && cur_node->iLeft)
            cur_node = cur_node->iLeft;
        return cur_node;
        }

    static T* Max(T* aNode)
        {
        T* cur_node = aNode;
        while (cur_node && cur_node->iRight)
            cur_node = cur_node->iRight;
        return cur_node;
        }

    T* iRoot = nullptr;
    bool iOwnData;
    size_t iCount = 0;
    };


}

#endif
