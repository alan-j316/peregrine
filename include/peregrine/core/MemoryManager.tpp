#pragma once
/**
 * @file MemoryManager.tpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 29, 2026
 *
 * @brief Implementation of member function declared in MemoryManager.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "peregrine/utils/Preprocessors.h"

#include <new>

namespace pmm
{
    template <TelPolicy TelemetryPolicy, MemoryManagerConfig Config, MemPolicy MemoryPolicy>
    PMM_INLINE constexpr MemoryManager<TelemetryPolicy, Config, MemoryPolicy>::MemoryManager(const size_t size) noexcept
        requires(MemoryPolicy == MemPolicy::Internal)
        : _buffer{ static_cast<uint8_t*>(memAlloc(size + getContextSize())) },
          internalAllocator(_buffer + getContextSize(), size)
    {
        PMM_ASSERT_MSG(size > 0, "[Memory Manager]: Cannot initialize a zero-sized memory manager.");
        // Initialize the context if manager is not allocated on the stack.
#ifndef PMM_ALLOC_MEM_MGR_ON_STACK
        _ctx = new (_buffer) MemoryContext<TelemetryPolicy, Config>();
#endif
    }


    template <TelPolicy TelemetryPolicy, MemoryManagerConfig Config, MemPolicy MemoryPolicy>
    PMM_INLINE constexpr MemoryManager<TelemetryPolicy, Config, MemoryPolicy>::MemoryManager(void* pBuffer,
                                                                                             const size_t size) noexcept
        requires(MemoryPolicy == MemPolicy::External)
        : _buffer{ static_cast<uint8_t*>(pBuffer) }, internalAllocator(pBuffer, size)
    {
        PMM_ASSERT_MSG(size > 0, "[Memory Manager]: Cannot initialize a zero-sized memory manager.");
        // Initialize the context if manager is not allocated on the stack.
#ifndef PMM_ALLOC_MEM_MGR_ON_STACK
        _ctx = new (_buffer) MemoryContext<TelemetryPolicy, Config>();
#endif
    }


    template <TelPolicy TelemetryPolicy, MemoryManagerConfig Config, MemPolicy MemoryPolicy>
    PMM_INLINE constexpr size_t MemoryManager<TelemetryPolicy, Config, MemoryPolicy>::getContextSize() noexcept
    { return sizeof(MemoryContext<TelemetryPolicy, Config>); }
} // namespace pmm
