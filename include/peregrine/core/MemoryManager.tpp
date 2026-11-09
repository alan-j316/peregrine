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
          _totalSize{ size },
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
        : _buffer{ static_cast<uint8_t*>(pBuffer) }, _totalSize{ size }, internalAllocator(pBuffer, size)
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



    /// +---------------------------------------------------------------------------------------+
    /// |                                ARENA                                                  |
    /// +---------------------------------------------------------------------------------------+

    template <TelPolicy TelemetryPolicy, MemoryManagerConfig Config, MemPolicy MemoryPolicy>
    PMM_INLINE LocalArena<MemPolicy::External, TelemetryPolicy>* MemoryManager<
        TelemetryPolicy, Config, MemoryPolicy>::createLocalArena(const size_t size) noexcept
    {
        PMM_ASSERT_MSG(size + _usedSize <= _totalSize, "Memory manager full!");
        _usedSize += size;
        const auto buffer = internalAllocator.malloc(size);
#ifdef PMM_ALLOC_MEM_MGR_ON_STACK
        PMM_ASSERT_MSG(!_ctx.localArenas.full(), "Maximum number of local arenas already allocated");
        return _ctx.localArenas.create(buffer, size);
#else
        PMM_ASSERT_MSG(!_ctx->localArenas.full(), "Maximum number of local arenas already allocated");
        return _ctx->localArenas.create(buffer, size);
#endif
    }


    template <TelPolicy TelemetryPolicy, MemoryManagerConfig Config, MemPolicy MemoryPolicy>
    PMM_INLINE SafeLocalArena<MemPolicy::External, TelemetryPolicy>* MemoryManager<
        TelemetryPolicy, Config, MemoryPolicy>::createSafeLocalArena(const size_t size) noexcept
    {
        PMM_ASSERT_MSG(size + _usedSize <= _totalSize, "Memory manager full!");
        _usedSize += size;
        const auto buffer = internalAllocator.malloc(size);
#ifdef PMM_ALLOC_MEM_MGR_ON_STACK
        PMM_ASSERT_MSG(!_ctx->safeLocalArenas.full(), "Maximum number of safe local arenas already allocated");
        return _ctx.safeLocalArenas.create(buffer, size);
#else
        PMM_ASSERT_MSG(!_ctx->safeLocalArenas.full(), "Maximum number of safe local arenas already allocated");
        return _ctx->safeLocalArenas.create(buffer, size);
#endif
    }


    template <TelPolicy TelemetryPolicy, MemoryManagerConfig Config, MemPolicy MemoryPolicy>
    PMM_INLINE MTArena<MemPolicy::External, TelemetryPolicy>* MemoryManager<
        TelemetryPolicy, Config, MemoryPolicy>::createMTArena(const size_t size) noexcept
    {
        PMM_ASSERT_MSG(size + _usedSize <= _totalSize, "Memory manager full!");
        _usedSize += size;
        const auto buffer = internalAllocator.malloc(size);
#ifdef PMM_ALLOC_MEM_MGR_ON_STACK
        PMM_ASSERT_MSG(!_ctx->mtArenas.full(), "Maximum number of multi-threaded arenas already allocated");
        return _ctx.mtArenas.create(buffer, size);
#else
        PMM_ASSERT_MSG(!_ctx->mtArenas.full(), "Maximum number of multi-threaded arenas already allocated");
        return _ctx->mtArenas.create(buffer, size);
#endif
    }


    template <TelPolicy TelemetryPolicy, MemoryManagerConfig Config, MemPolicy MemoryPolicy>
    PMM_INLINE SafeMTArena<MemPolicy::External, TelemetryPolicy>* MemoryManager<
        TelemetryPolicy, Config, MemoryPolicy>::createSafeMTArena(const size_t size) noexcept
    {
        PMM_ASSERT_MSG(size + _usedSize <= _totalSize, "Memory manager full!");
        _usedSize += size;
        const auto buffer = internalAllocator.malloc(size);
#ifdef PMM_ALLOC_MEM_MGR_ON_STACK
        PMM_ASSERT_MSG(!_ctx->safeMTArenas.full(), "Maximum number of safe multi-threaded arenas already allocated");
        return _ctx.safeMTArenas.create(buffer, size);
#else
        PMM_ASSERT_MSG(!_ctx->safeMTArenas.full(), "Maximum number of safe multi-threaded arenas already allocated");
        return _ctx->safeMTArenas.create(buffer, size);
#endif
    }
} // namespace pmm
