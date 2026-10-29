#pragma once
/**
 * @file MemoryManager.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 29, 2026
 *
 * @brief Centralized memory manager with multithreading support.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "peregrine/allocators/Arena.h"
#include "peregrine/allocators/Pool.h"
#include "peregrine/allocators/Stack.h"
#include "peregrine/allocators/TLSF.h"

#include <cstdint>

namespace pmm
{
    using AllocateTag = uint32_t;

    class MemoryManager
    {

    public:
        // BLOCK OUT
    private:
        struct MemoryContext* _ctx{};
    };
} // namespace pmm


#include "MemoryManager.tpp"
