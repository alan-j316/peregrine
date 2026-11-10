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

    /// +---------------------------------------------------------------------------------------+
    /// |                                POOL                                                   |
    /// +---------------------------------------------------------------------------------------+

    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE LocalPool<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createLocalPool(const size_t capacity, const size_t chunkSize,
                                                        const size_t chunkAlignment) noexcept
    {
        // [Chunk 0 + Chunk Misalignment][Chunk 1 + Chunk Misalignment]...
        // Hackers Delight [3-2]
        const auto alignedChunkSize = (chunkSize + (chunkAlignment - 1)) & -chunkAlignment;
        const auto totalSize        = alignedChunkSize * capacity;

        PMM_ASSERT_MSG(totalSize + _usedSize <= _totalSize, "Memory manager full!");
        _usedSize += totalSize;

        const auto buffer = internalAllocator.malloc(totalSize, chunkAlignment);
        return LocalPool<MemPolicy::External, TelemetryPolicy>(buffer, totalSize, alignedChunkSize, chunkAlignment);
    }


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE SafeLocalPool<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createSafeLocalPool(const size_t capacity, const size_t chunkSize,
                                                            const size_t chunkAlignment) noexcept
    {
        // [Chunk 0 + Chunk Misalignment][Chunk 1 + Chunk Misalignment]...
        // Hackers Delight [3-2]
        const auto alignedChunkSize = (chunkSize + (chunkAlignment - 1)) & -chunkAlignment;
        const auto totalSize        = alignedChunkSize * capacity;

        PMM_ASSERT_MSG(totalSize + _usedSize <= _totalSize, "Memory manager full!");
        _usedSize += totalSize;

        const auto buffer = internalAllocator.malloc(totalSize, chunkAlignment);
        return SafeLocalPool<MemPolicy::External, TelemetryPolicy>(buffer, totalSize, alignedChunkSize, chunkAlignment);
    }


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE MTPool<MemPolicy::External, TelemetryPolicy> MemoryManager<TelemetryPolicy, MemoryPolicy>::createMTPool(
        const size_t capacity, const size_t chunkSize, const size_t chunkAlignment) noexcept
    {
        // [Chunk 0 + Chunk Misalignment][Chunk 1 + Chunk Misalignment]...
        // Hackers Delight [3-2]
        const auto alignedChunkSize = (chunkSize + (chunkAlignment - 1)) & -chunkAlignment;
        const auto totalSize        = alignedChunkSize * capacity;

        PMM_ASSERT_MSG(totalSize + _usedSize <= _totalSize, "Memory manager full!");
        _usedSize += totalSize;

        const auto buffer = internalAllocator.malloc(totalSize, chunkAlignment);
        return MTPool<MemPolicy::External, TelemetryPolicy>(buffer, totalSize, alignedChunkSize, chunkAlignment);
    }


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE SafeMTPool<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createSafeMTPool(const size_t capacity, const size_t chunkSize,
                                                         const size_t chunkAlignment) noexcept
    {
        // [Chunk 0 + Chunk Misalignment][Chunk 1 + Chunk Misalignment]...
        // Hackers Delight [3-2]
        const auto alignedChunkSize = (chunkSize + (chunkAlignment - 1)) & -chunkAlignment;
        const auto totalSize        = alignedChunkSize * capacity;

        PMM_ASSERT_MSG(totalSize + _usedSize <= _totalSize, "Memory manager full!");
        _usedSize += totalSize;

        const auto buffer = internalAllocator.malloc(totalSize, chunkAlignment);
        return SafeMTPool<MemPolicy::External, TelemetryPolicy>(buffer, totalSize, alignedChunkSize, chunkAlignment);
    }



    /// +---------------------------------------------------------------------------------------+
    /// |                                 TLSF                                                  |
    /// +---------------------------------------------------------------------------------------+

    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE LocalTLSF<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createLocalTLSF(const size_t size) noexcept
    {
        PMM_ASSERT_MSG(size + _usedSize <= _totalSize, "Memory manager full!");
        _usedSize += size;
        const auto buffer = internalAllocator.malloc(size);

        return LocalTLSF<MemPolicy::External, TelemetryPolicy>(buffer, size);
    }


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE SafeLocalTLSF<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createSafeLocalTLSF(const size_t size) noexcept
    {
        PMM_ASSERT_MSG(size + _usedSize <= _totalSize, "Memory manager full!");
        _usedSize += size;
        const auto buffer = internalAllocator.malloc(size);

        return SafeLocalTLSF<MemPolicy::External, TelemetryPolicy>(buffer, size);
    }


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE MTTLSF<MemPolicy::External, TelemetryPolicy> MemoryManager<TelemetryPolicy, MemoryPolicy>::createMTTLSF(
        const size_t size) noexcept
    {
        PMM_ASSERT_MSG(size + _usedSize <= _totalSize, "Memory manager full!");
        _usedSize += size;
        const auto buffer = internalAllocator.malloc(size);

        return MTTLSF<MemPolicy::External, TelemetryPolicy>(buffer, size);
    }


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE SafeMTTLSF<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createSafeMTTLSF(const size_t size) noexcept
    {
        PMM_ASSERT_MSG(size + _usedSize <= _totalSize, "Memory manager full!");
        _usedSize += size;
        const auto buffer = internalAllocator.malloc(size);

        return SafeMTTLSF<MemPolicy::External, TelemetryPolicy>(buffer, size);
    }

} // namespace pmm
