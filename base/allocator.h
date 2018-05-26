/* Licensed to the Apache Software Foundation (ASF) under one or more
* contributor license agreements.  See the NOTICE file distributed with
* this work for additional information regarding copyright ownership.
* The ASF licenses this file to You under the Apache License, Version 2.0
* (the "License"); you may not use this file except in compliance with
* the License.  You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/


/*************************************************************************
 * File: alloctor.h
 *************************************************************************
 *\author: starlove
 *************************************************************************/

#ifndef BASE_ALLOCTOR_H
#define BASE_ALLOCTOR_H

#include "debug.h"
#include "memory.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define BL_MEMORY_POOL_ONCE_BLOCKSIZE 256

NS(NBPy)

template <typename T>
class IAllocator{
public:
    virtual void clear() = 0;
    virtual T *allocate() = 0;
    virtual void free(T *) = 0;
};

typedef struct _object_mem{
    _object_mem *next;
}object_mem, *lpobject_mem;

template <typename T>
class CObjectAllocator: public IAllocator<T>{
public:
    CObjectAllocator(){
        m_free = NULL;
    };
    ~CObjectAllocator(){
        clear();
    }

    void clear(){
        lpobject_mem m = m_free;
        while(m){
            lpobject_mem mnext = m->next;
            ::free(m);
            m = mnext;
        }
        m_free = NULL;
    };

    T *allocate(){
    	/*
        T *pt = NULL;
        lpobject_mem m = m_free;
        if(m){
            m_free = m->next;
        }else{
            m = (lpobject_mem)::malloc(sizeof(object_mem) + sizeof(T));
        }
        pt = (T *)CMemory<char>::MovePtr(m, sizeof(object_mem));
        pt = new(pt) T();
        return pt;*/
        return new T();
    }

    void free(T *p){
    	/*
        p->~T();
        lpobject_mem m = (lpobject_mem)CMemory<char>::MovePtr(p, 0 - (int)sizeof(object_mem));
        m->next = m_free;
        m_free = m;*/
        delete p;
    }

private:
    lpobject_mem m_free;
};
/*
template<typename T>
class IAllocator
{
public:
    //virtual size_t realloc( unsigned int ) = 0;
    virtual void clear(bool) = 0;
    virtual T *allocate( bool = true ) = 0;
    virtual void free(T *) = 0;

};

extern volatile int64_t MemoryAllocSize;
extern volatile int64_t MemoryFreeSize;

#pragma pack(1)
struct X_MEMORY_HEAPBLOCK
{
    X_MEMORY_HEAPBLOCK *first;
    X_MEMORY_HEAPBLOCK *left;
    X_MEMORY_HEAPBLOCK *right;

    X_MEMORY_HEAPBLOCK()
    {
        first = NULL;
        left = NULL;
        right = NULL;
    }

    void *getData()
    {
        //return (char *)this + sizeof(X_MEMORY_HEAPBLOCK);
        return CMemory<char>::MovePtr(this, sizeof(X_MEMORY_HEAPBLOCK));
    }
};
#pragma pack()

typedef X_MEMORY_HEAPBLOCK *LPX_MEMORY_HEAPBLOCK;

template<typename T>
class CAllocator : public IAllocator<T>
{
public:
    CAllocator()
    {
        m_pFreeBlocks = NULL;
        m_pUsedBlocks = NULL;
    }

    ~CAllocator()
    {
        force_free_used();
        free_allheap();
        m_pFreeBlocks = NULL;
        m_pUsedBlocks = NULL;
    }

    T *allocate(bool bReAlloc = true)
    {
        T *pData = NULL;
        if( !m_pFreeBlocks && bReAlloc )
            re_alloc_heap();

        if( !m_pFreeBlocks )
            return pData;

        LPX_MEMORY_HEAPBLOCK pCurrBlock = pop_block(m_pFreeBlocks);
        if( !pCurrBlock )
            return pData;

        pData = (T *)pCurrBlock->getData();
        insertused(pCurrBlock);

        return pData;
    }

    void release(T *pAddr)
    {
        if (!pAddr)
        {
            return;
        }

        LPX_MEMORY_HEAPBLOCK pCurrBlock = (LPX_MEMORY_HEAPBLOCK)CMemory<char>::MovePtr(pAddr, 0 - (int)sizeof(X_MEMORY_HEAPBLOCK));
        pCurrBlock = pop_block(pCurrBlock);
        if( !pCurrBlock )
            return;


        insertfree(pCurrBlock);
    }

    // alloc memory-block when the free memory is empty
    size_t realloc( unsigned int nCount = BL_MEMORY_POOL_ONCE_BLOCKSIZE )
    {
        size_t blocksize = (sizeof(X_MEMORY_HEAPBLOCK) + sizeof(T));
        size_t totalsize = blocksize * m_nBlockSize;
        return !re_alloc_heap(nCount) ? 0 : totalsize;
    }

    // clear and free all memory-block by system-kernel
    void clear(bool force = false)
    {
        if(force)
            force_free_used();
        free_allheap();
    }

    // back a memory-block to the allocator
    void free(T *p)
    {
        //print_stacktrace();
        p->~T();
        release(p);
    }

private:

    /// pop a memory-block from the stack-top
    LPX_MEMORY_HEAPBLOCK pop_block( LPX_MEMORY_HEAPBLOCK pCurrBlock )
    {
        if ( !pCurrBlock )
            return NULL;

        LPX_MEMORY_HEAPBLOCK pLeftBlock = pCurrBlock->left;
        LPX_MEMORY_HEAPBLOCK pRightBlock = pCurrBlock->right;

        if( pLeftBlock )
        {
            // adjust right block link
            pLeftBlock->right = pRightBlock;
        }
        else
        {
            // if it's first block, reset the first ptr
            if(pCurrBlock == m_pFreeBlocks)
            {
                m_pFreeBlocks = pRightBlock;
				DEBUG_STATUS("pop_block m_pFreeBlocks set to >> %p", pRightBlock);
            }
            if(pCurrBlock == m_pUsedBlocks)
                m_pUsedBlocks = pRightBlock;
        }

        if ( pRightBlock )
        {
            // adjust left block link
            pRightBlock->left = pLeftBlock;
        }


        pCurrBlock->left = NULL;
        pCurrBlock->right = NULL;
        return pCurrBlock;
    }

    LPX_MEMORY_HEAPBLOCK re_alloc_heap(unsigned int nCount = BL_MEMORY_POOL_ONCE_BLOCKSIZE)
    {
        LPX_MEMORY_HEAPBLOCK pBlocks = NULL;
        // alloc size for block = block-header's size + object's size
        size_t blocksize = (sizeof(X_MEMORY_HEAPBLOCK) + sizeof(T));
        // calc total data-size
        size_t totalsize = blocksize * nCount;
        // alloc system memory
        pBlocks = (LPX_MEMORY_HEAPBLOCK)::malloc(totalsize);

        //if failed to alloc, exit
        if(!pBlocks) return NULL;

    	//DEBUG_STATUS("re_alloc_heap size: %ld", totalsize);

        // reset the memory data
        memset(pBlocks,0,totalsize);

        // make link all blocks
        LPX_MEMORY_HEAPBLOCK pCurrBlock = pBlocks;
        LPX_MEMORY_HEAPBLOCK pLastBlock = NULL;

        // calc end ptr
        void *pEnd = CMemory<char>::MovePtr(pBlocks, totalsize); //((char *)pBlocks) + totalsize;

        int nAllocCount = 0;
        while (pCurrBlock && (CMemory<char>::MovePtr(pCurrBlock, blocksize) <= pEnd))
        {
            pCurrBlock->first = pBlocks;
            pCurrBlock->left = pLastBlock;
            if( CMemory<char>::MovePtr(pCurrBlock, blocksize) < pEnd )
            {
                pCurrBlock->right = (LPX_MEMORY_HEAPBLOCK)CMemory<char>::MovePtr(pCurrBlock, blocksize);
            }
            else
            {
                pCurrBlock->right = NULL;
            }
            pLastBlock = pCurrBlock;
            pCurrBlock = pCurrBlock->right;
            nAllocCount ++;
        }

        // insert blocks to free-stack
        insertfree(pBlocks);
        // set the blocks's first is NULL, mark it for easy free
        pBlocks->first = NULL;
        return pBlocks;
    }

    /// !!! don't use the function, because the block back to allocator is no order, so the stack order is damaged
    void free_heap(LPX_MEMORY_HEAPBLOCK pBlock, int nCount = -1)
    {
        if( !pBlock )
            return;

        if( pBlock->first )
            return;

        if( pBlock != pop_block(pBlock) )
            return;

        ::free(pBlock);
    }

    /// free all the allocator's blocks by sytem-kernel
    size_t free_allheap()
    {
        if (m_pUsedBlocks)
        {
            //print_stacktrace();
            assert(false && "memory block is used!!!!!");

        }

        size_t ret = 0;
        //freelst
        LPX_MEMORY_HEAPBLOCK pCurrBlock = m_pFreeBlocks;
        while (pCurrBlock)
        {
            LPX_MEMORY_HEAPBLOCK pNextBlock = pCurrBlock->right;
            if( !pCurrBlock->first )
            {
                LPX_MEMORY_HEAPBLOCK pCurr = m_pFreeBlocks;
                while (pCurr)
                {
                    LPX_MEMORY_HEAPBLOCK pR = pCurr->right;
                    if( pCurr->first == pCurrBlock )
                        pop_block(pCurr);
                    pCurr = pR;
                }

                pNextBlock = pCurrBlock->right;
                free_heap(pCurrBlock);
            }
            pCurrBlock = pNextBlock;
            ret ++;
        }
        return ret;
    }

    void insertfree(LPX_MEMORY_HEAPBLOCK pBlock)
    {
        if( !pBlock )
            return;

        LPX_MEMORY_HEAPBLOCK pLast = NULL;
        LPX_MEMORY_HEAPBLOCK pCurr = pBlock;
        while ( pCurr )
        {
            pLast = pCurr;
            pCurr = pCurr->right;
        }

        pLast->right = m_pFreeBlocks;
        if( m_pFreeBlocks )
            m_pFreeBlocks->left = pLast;
        m_pFreeBlocks = pBlock;

		DEBUG_STATUS("pop_block m_pFreeBlocks set to >> %p", pBlock);
    }

    void insertused(LPX_MEMORY_HEAPBLOCK pBlock)
    {
        if( !pBlock )
            return;

        pBlock->right = m_pUsedBlocks;
        if( m_pUsedBlocks )
            m_pUsedBlocks->left = pBlock;
        m_pUsedBlocks = pBlock;
    }

    size_t force_free_used(){
        size_t ret = 0;
        //freelst
        LPX_MEMORY_HEAPBLOCK pCurrBlock = m_pUsedBlocks;
        while (pCurrBlock)
        {
            LPX_MEMORY_HEAPBLOCK pNextBlock = pCurrBlock->right;
            if( !pCurrBlock->first )
            {
                LPX_MEMORY_HEAPBLOCK pCurr = m_pUsedBlocks;
                while (pCurr)
                {
                    LPX_MEMORY_HEAPBLOCK pR = pCurr->right;
                    if( pCurr->first == pCurrBlock )
                        pop_block(pCurr);
                    pCurr = pR;
                }

                pNextBlock = pCurrBlock->right;

                ((T*)pCurrBlock->getData())->~T();
                free_heap(pCurrBlock);
            }
            pCurrBlock = pNextBlock;
            ret ++;
        }
        return ret;
    }

private:
    LPX_MEMORY_HEAPBLOCK m_pFreeBlocks;
    LPX_MEMORY_HEAPBLOCK m_pUsedBlocks;
    size_t m_nBlockSize;
};
*/

template<typename T>
class CThreadAllocator : public IAllocator<T> {
public:

    void clear(){
        m_allocator.clear();
    }
    T *allocate(){
        return m_allocator.allocate();
    }

    void free(T *t){
        m_allocator.free(t);
    }

private:
    CObjectAllocator<T> m_allocator;
};

NSE

#endif // BASE_ALLOCTOR_H
