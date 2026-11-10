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

namespace pmm
{
    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE constexpr MemoryManager<TelemetryPolicy, MemoryPolicy>::MemoryManager(const size_t size) noexcept
        requires(MemoryPolicy == MemPolicy::Internal)
        : _buffer{ static_cast<uint8_t*>(memAlloc(size)) }, _totalSize{ size }, internalAllocator(_buffer, size)
    {}


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE constexpr MemoryManager<TelemetryPolicy, MemoryPolicy>::MemoryManager(void* pBuffer,
                                                                                     const size_t size) noexcept
        requires(MemoryPolicy == MemPolicy::External)
        : _buffer{ static_cast<uint8_t*>(pBuffer) }, _totalSize{ size }, internalAllocator(pBuffer, size)
    {}


    /// +---------------------------------------------------------------------------------------+
    /// |                                ARENA                                                  |
    /// +---------------------------------------------------------------------------------------+

    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE LocalArena<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createLocalArena(const size_t size) noexcept
    {
        PMM_ASSERT_MSG(size + _usedSize <= _totalSize, "Memory manager full!");
        _usedSize += size;
        const auto buffer = internalAllocator.malloc(size);
        return LocalArena<MemPolicy::External, TelemetryPolicy>(buffer, size);
    }


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE SafeLocalArena<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createSafeLocalArena(const size_t size) noexcept
    {
        PMM_ASSERT_MSG(size + _usedSize <= _totalSize, "Memory manager full!");
        _usedSize += size;
        const auto buffer = internalAllocator.malloc(size);
        return SafeLocalArena<MemPolicy::External, TelemetryPolicy>(buffer, size);
    }


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE MTArena<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createMTArena(const size_t size) noexcept
    {
        PMM_ASSERT_MSG(size + _usedSize <= _totalSize, "Memory manager full!");
        _usedSize += size;
        const auto buffer = internalAllocator.malloc(size);
        return MTArena<MemPolicy::External, TelemetryPolicy>(buffer, size);
    }


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE SafeMTArena<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createSafeMTArena(const size_t size) noexcept
    {
        PMM_ASSERT_MSG(size + _usedSize <= _totalSize, "Memory manager full!");
        _usedSize += size;
        const auto buffer = internalAllocator.malloc(size);
        return SafeMTArena<MemPolicy::External, TelemetryPolicy>(buffer, size);
    }



    /// +---------------------------------------------------------------------------------------+
    /// |                                STACK                                                  |
    /// +---------------------------------------------------------------------------------------+

    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE LocalStack<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createLocalStack(const size_t size) noexcept
    {
        PMM_ASSERT_MSG(size + _usedSize <= _totalSize, "Memory manager full!");
        _usedSize += size;
        const auto buffer = internalAllocator.malloc(size);

        return LocalStack<MemPolicy::External, TelemetryPolicy>(buffer, size);
    }


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE SafeLocalStack<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createSafeLocalStack(const size_t size) noexcept
    {
        PMM_ASSERT_MSG(size + _usedSize <= _totalSize, "Memory manager full!");
        _usedSize += size;
        const auto buffer = internalAllocator.malloc(size);

        return SafeLocalStack<MemPolicy::External, TelemetryPolicy>(buffer, size);
    }


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE MTStack<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createMTStack(const size_t size) noexcept
    {
        PMM_ASSERT_MSG(size + _usedSize <= _totalSize, "Memory manager full!");
        _usedSize += size;
        const auto buffer = internalAllocator.malloc(size);

        return MTStack<MemPolicy::External, TelemetryPolicy>(buffer, size);
    }


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE SafeMTStack<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createSafeMTStack(const size_t size) noexcept
    {
        PMM_ASSERT_MSG(size + _usedSize <= _totalSize, "Memory manager full!");
        _usedSize += size;
        const auto buffer = internalAllocator.malloc(size);

        return SafeMTStack<MemPolicy::External, TelemetryPolicy>(buffer, size);
    }

} // namespace pmm
