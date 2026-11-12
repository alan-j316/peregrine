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
    PMM_INLINE constexpr LocalArena<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createLocalArena(const size_t size) noexcept
    {
        const auto buffer = internalAllocator.malloc(size);
        return LocalArena<MemPolicy::External, TelemetryPolicy>(buffer, size);
    }


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE constexpr SafeLocalArena<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createSafeLocalArena(const size_t size) noexcept
    {
        const auto buffer = internalAllocator.malloc(size);
        return SafeLocalArena<MemPolicy::External, TelemetryPolicy>(buffer, size);
    }


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE constexpr MTArena<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createMTArena(const size_t size) noexcept
    {
        const auto buffer = internalAllocator.malloc(size);
        return MTArena<MemPolicy::External, TelemetryPolicy>(buffer, size);
    }


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE constexpr SafeMTArena<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createSafeMTArena(const size_t size) noexcept
    {
        const auto buffer = internalAllocator.malloc(size);
        return SafeMTArena<MemPolicy::External, TelemetryPolicy>(buffer, size);
    }



    /// +---------------------------------------------------------------------------------------+
    /// |                                STACK                                                  |
    /// +---------------------------------------------------------------------------------------+

    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE constexpr LocalStack<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createLocalStack(const size_t size) noexcept
    {
        const auto buffer = internalAllocator.malloc(size);

        return LocalStack<MemPolicy::External, TelemetryPolicy>(buffer, size);
    }


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE constexpr SafeLocalStack<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createSafeLocalStack(const size_t size) noexcept
    {
        const auto buffer = internalAllocator.malloc(size);

        return SafeLocalStack<MemPolicy::External, TelemetryPolicy>(buffer, size);
    }


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE constexpr MTStack<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createMTStack(const size_t size) noexcept
    {
        const auto buffer = internalAllocator.malloc(size);

        return MTStack<MemPolicy::External, TelemetryPolicy>(buffer, size);
    }


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE constexpr SafeMTStack<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createSafeMTStack(const size_t size) noexcept
    {
        const auto buffer = internalAllocator.malloc(size);

        return SafeMTStack<MemPolicy::External, TelemetryPolicy>(buffer, size);
    }

    /// +---------------------------------------------------------------------------------------+
    /// |                                POOL                                                   |
    /// +---------------------------------------------------------------------------------------+

    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE constexpr LocalPool<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createLocalPool(const size_t capacity, const size_t chunkSize,
                                                        const size_t chunkAlignment) noexcept
    {
        // [Chunk 0 + Chunk Misalignment][Chunk 1 + Chunk Misalignment]...
        // Hackers Delight [3-2]
        const auto alignedChunkSize = (chunkSize + (chunkAlignment - 1)) & -chunkAlignment;
        const auto totalSize        = alignedChunkSize * capacity;

        const auto buffer = internalAllocator.malloc(totalSize, chunkAlignment);
        return LocalPool<MemPolicy::External, TelemetryPolicy>(buffer, totalSize, alignedChunkSize, chunkAlignment);
    }


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE constexpr SafeLocalPool<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createSafeLocalPool(const size_t capacity, const size_t chunkSize,
                                                            const size_t chunkAlignment) noexcept
    {
        // [Chunk 0 + Chunk Misalignment][Chunk 1 + Chunk Misalignment]...
        // Hackers Delight [3-2]
        const auto alignedChunkSize = (chunkSize + (chunkAlignment - 1)) & -chunkAlignment;
        const auto totalSize        = alignedChunkSize * capacity;

        const auto buffer = internalAllocator.malloc(totalSize, chunkAlignment);
        return SafeLocalPool<MemPolicy::External, TelemetryPolicy>(buffer, totalSize, alignedChunkSize, chunkAlignment);
    }


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE constexpr MTPool<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createMTPool(const size_t capacity, const size_t chunkSize,
                                                     const size_t chunkAlignment) noexcept
    {
        // [Chunk 0 + Chunk Misalignment][Chunk 1 + Chunk Misalignment]...
        // Hackers Delight [3-2]
        const auto alignedChunkSize = (chunkSize + (chunkAlignment - 1)) & -chunkAlignment;
        const auto totalSize        = alignedChunkSize * capacity;

        const auto buffer = internalAllocator.malloc(totalSize, chunkAlignment);
        return MTPool<MemPolicy::External, TelemetryPolicy>(buffer, totalSize, alignedChunkSize, chunkAlignment);
    }


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE constexpr SafeMTPool<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createSafeMTPool(const size_t capacity, const size_t chunkSize,
                                                         const size_t chunkAlignment) noexcept
    {
        // [Chunk 0 + Chunk Misalignment][Chunk 1 + Chunk Misalignment]...
        // Hackers Delight [3-2]
        const auto alignedChunkSize = (chunkSize + (chunkAlignment - 1)) & -chunkAlignment;
        const auto totalSize        = alignedChunkSize * capacity;

        const auto buffer = internalAllocator.malloc(totalSize, chunkAlignment);
        return SafeMTPool<MemPolicy::External, TelemetryPolicy>(buffer, totalSize, alignedChunkSize, chunkAlignment);
    }



    /// +---------------------------------------------------------------------------------------+
    /// |                                 TLSF                                                  |
    /// +---------------------------------------------------------------------------------------+

    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE constexpr LocalTLSF<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createLocalTLSF(const size_t size) noexcept
    {
        const auto buffer = internalAllocator.malloc(size);

        return LocalTLSF<MemPolicy::External, TelemetryPolicy>(buffer, size);
    }


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE constexpr SafeLocalTLSF<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createSafeLocalTLSF(const size_t size) noexcept
    {
        const auto buffer = internalAllocator.malloc(size);

        return SafeLocalTLSF<MemPolicy::External, TelemetryPolicy>(buffer, size);
    }


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE constexpr MTTLSF<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createMTTLSF(const size_t size) noexcept
    {
        const auto buffer = internalAllocator.malloc(size);

        return MTTLSF<MemPolicy::External, TelemetryPolicy>(buffer, size);
    }


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    PMM_INLINE constexpr SafeMTTLSF<MemPolicy::External, TelemetryPolicy> MemoryManager<
        TelemetryPolicy, MemoryPolicy>::createSafeMTTLSF(const size_t size) noexcept
    {
        const auto buffer = internalAllocator.malloc(size);

        return SafeMTTLSF<MemPolicy::External, TelemetryPolicy>(buffer, size);
    }


    template <TelPolicy TelemetryPolicy, MemPolicy MemoryPolicy>
    template <typename Allocator>
    PMM_INLINE constexpr void MemoryManager<TelemetryPolicy, MemoryPolicy>::free(Allocator& allocator) noexcept
    {
        if constexpr (!std::is_trivially_destructible_v<Allocator>)
        {
            allocator.~Allocator();
        }
        // Free the memory held by the arena.
        // Arena
        if constexpr (std::is_same_v<Allocator, LocalArena<MemPolicy::External, TelemetryPolicy>> ||
                      std::is_same_v<Allocator, SafeLocalArena<MemPolicy::External, TelemetryPolicy>> ||
                      std::is_same_v<Allocator, MTArena<MemPolicy::External, TelemetryPolicy>> ||
                      std::is_same_v<Allocator, SafeMTArena<MemPolicy::External, TelemetryPolicy>>)
        {
            internalAllocator.mfree(allocator._buffer);
        }
        // Stack
        else if constexpr (std::is_same_v<Allocator, LocalStack<MemPolicy::External, TelemetryPolicy>> ||
                           std::is_same_v<Allocator, SafeLocalStack<MemPolicy::External, TelemetryPolicy>> ||
                           std::is_same_v<Allocator, MTStack<MemPolicy::External, TelemetryPolicy>> ||
                           std::is_same_v<Allocator, SafeMTStack<MemPolicy::External, TelemetryPolicy>>)
        {
            internalAllocator.mfree(allocator._buffer);
        }
        // Pool
        else if constexpr (std::is_same_v<Allocator, LocalPool<MemPolicy::External, TelemetryPolicy>> ||
                           std::is_same_v<Allocator, SafeLocalPool<MemPolicy::External, TelemetryPolicy>> ||
                           std::is_same_v<Allocator, MTPool<MemPolicy::External, TelemetryPolicy>> ||
                           std::is_same_v<Allocator, SafeMTPool<MemPolicy::External, TelemetryPolicy>>)
        {
            internalAllocator.mfree(allocator._buffer);
        }
        // TLSF

        // Unsupported allocators/classes
        else
        {
            static_assert(false && "[Memory Manager/Free]: Unsupported allocator!");
        }
    }

} // namespace pmm
