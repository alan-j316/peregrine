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
#include "peregrine/core/Alias.h"

#include <bitset>
#include <cstdint>

/**
 * @addtogroup PMM_Core
 * @{
 */

namespace pmm
{
    /// Multithreadable memory manager.
    /// @tparam TelemetryPolicy Telemetry policy used by the memory manager. Enabled by default.
    ///                         Enabling Telemetry can consume more memory and can have a mild impact on performance.
    /// @tparam MemoryPolicy    Dictates whether the buffer is supplied externally or owned internally.
    ///                         Internally managed by default.
    template <TelPolicy TelemetryPolicy = TelPolicy::Enabled, MemPolicy MemoryPolicy = MemPolicy::Internal>
    class MemoryManager
    {
    public:
        /**
         * @brief Initialize a memory manager with the given @p size.
         * @param size Size in bytes.
         *
         * @note API special for MemPolicy::Internal.
         * @note The buffer will be owned and managed internally.
         */
        explicit constexpr MemoryManager(size_t size) noexcept
            requires(MemoryPolicy == MemPolicy::Internal);


        /**
         * @brief Initialize a memory manager with the given @p size and buffer.
         * @param pBuffer A pointer to the buffer to use.
         * @param size    Size in bytes.
         *
         * @note API special for MemPolicy::External.
         * @note The pBuffer should account for the internal context's size, which can be queried with
         *       @ref MemoryManager<...>::getContextSize() if `PMM_ALLOC_MEM_MGR_ON_STACK` is not defined.
         */
        constexpr MemoryManager(void* pBuffer, size_t size) noexcept
            requires(MemoryPolicy == MemPolicy::External);


        /**
         * @brief Create a thread local arena.
         * @param size The size of the arena. Must be less than the total available size.
         * @return An arena instance.
         */
        [[nodiscard]] constexpr LocalArena<MemPolicy::External, TelemetryPolicy> createLocalArena(size_t size) noexcept;


        /**
         * @brief Create a thread local arena with safe mode.
         * @param size The size of the arena. Must be less than the total available size.
         * @return An arena instance.
         */
        [[nodiscard]] constexpr SafeLocalArena<MemPolicy::External, TelemetryPolicy> createSafeLocalArena(
            size_t size) noexcept;

        /**
         * @brief Create a multithreading safe arena.
         * @param size The size of the arena. Must be less than the total available size.
         * @return An arena instance.
         */
        [[nodiscard]] constexpr MTArena<MemPolicy::External, TelemetryPolicy> createMTArena(size_t size) noexcept;


        /**
         * @brief Create a multithreading safe arena with safe mode.
         * @param size The size of the arena. Must be less than the total available size.
         * @return An arena instance.
         */
        [[nodiscard]] constexpr SafeMTArena<MemPolicy::External, TelemetryPolicy> createSafeMTArena(
            size_t size) noexcept;


        /**
         * @brief Create a thread local stack.
         * @param size The size of the stack. Must be less than the total available size.
         * @return A stack instance.
         */
        [[nodiscard]] constexpr LocalStack<MemPolicy::External, TelemetryPolicy> createLocalStack(size_t size) noexcept;

        /**
         * @brief Create a thread local stack with safe mode.
         * @param size The size of the stack. Must be less than the total available size.
         * @return A stack instance.
         */
        [[nodiscard]] constexpr SafeLocalStack<MemPolicy::External, TelemetryPolicy> createSafeLocalStack(
            size_t size) noexcept;

        /**
         * @brief Create a multithreading safe stack.
         * @param size The size of the stack. Must be less than the total available size.
         * @return A stack instance.
         */
        [[nodiscard]] constexpr MTStack<MemPolicy::External, TelemetryPolicy> createMTStack(size_t size) noexcept;


        /**
         * @brief Create a multithreading safe stack with safe mode.
         * @param size The size of the stack. Must be less than the total available size.
         * @return A stack instance.
         */
        [[nodiscard]] constexpr SafeMTStack<MemPolicy::External, TelemetryPolicy> createSafeMTStack(
            size_t size) noexcept;


        /**
         * @brief Create a thread local pool.
         * @param capacity       The total number of element every held by the pool.
         * @param chunkSize      Size requirement of the element.
         * @param chunkAlignment Alignment requirement of the element.
         * @return A pool instance.
         */
        [[nodiscard]] constexpr LocalPool<MemPolicy::External, TelemetryPolicy> createLocalPool(
            size_t capacity, size_t chunkSize, size_t chunkAlignment = sizeof(void*)) noexcept;

        /**
         * @brief Create a thread local pool with safe mode.
         * @param capacity       The total number of element every held by the pool.
         * @param chunkSize      Size requirement of the element.
         * @param chunkAlignment Alignment requirement of the element.
         * @return A pool instance.
         */
        [[nodiscard]] constexpr SafeLocalPool<MemPolicy::External, TelemetryPolicy> createSafeLocalPool(
            size_t capacity, size_t chunkSize, size_t chunkAlignment) noexcept;

        /**
         * @brief Create a multithreading safe pool.
         * @param capacity       The total number of element every held by the pool.
         * @param chunkSize      Size requirement of the element.
         * @param chunkAlignment Alignment requirement of the element.
         * @return A pool instance.
         */
        [[nodiscard]] constexpr MTPool<MemPolicy::External, TelemetryPolicy> createMTPool(
            size_t capacity, size_t chunkSize, size_t chunkAlignment) noexcept;


        /**
         * @brief Create a multithreading safe pool with safe mode.
         * @param capacity       The number of elements in the pool.
         * @param chunkSize      Size requirement of the element.
         * @param chunkAlignment Alignment requirement of the element.
         * @return A pool instance.
         */
        [[nodiscard]] constexpr SafeMTPool<MemPolicy::External, TelemetryPolicy> createSafeMTPool(
            size_t capacity, size_t chunkSize, size_t chunkAlignment) noexcept;

        /**
         * @brief Create a thread local TLSF.
         * @param size The size of the TLSF. Must be less than the total available size.
         * @return A TLSF instance.
         */
        [[nodiscard]] constexpr LocalTLSF<MemPolicy::External, TelemetryPolicy> createLocalTLSF(size_t size) noexcept;


        /**
         * @brief Create a thread local TLSF with safe mode.
         * @param size The size of the TLSF. Must be less than the total available size.
         * @return A TLSF instance.
         */
        [[nodiscard]] constexpr SafeLocalTLSF<MemPolicy::External, TelemetryPolicy> createSafeLocalTLSF(
            size_t size) noexcept;


        /**
         * @brief Create a multithreading safe TLSF.
         * @param size The size of the TLSF. Must be less than the total available size.
         * @return A TLSF instance.
         */
        [[nodiscard]] constexpr MTTLSF<MemPolicy::External, TelemetryPolicy> createMTTLSF(size_t size) noexcept;


        /**
         * @brief Create a multithreading safe TLSF with safe mode.
         * @param size The size of the TLSF. Must be less than the total available size.
         * @return A TLSF instance.
         */
        [[nodiscard]] constexpr SafeMTTLSF<MemPolicy::External, TelemetryPolicy> createSafeMTTLSF(size_t size) noexcept;


        template <typename Allocator>
        constexpr void free(Allocator& allocator) noexcept;

        // TODO: Add resize
        // TODO: Add free
        // TODO: add death tests
    private:
        uint8_t* _buffer{};
        size_t _totalSize;
        mt::SpinLock _lock{}; // TODO: Add MT support and tests
        /// Internal TLSF allocator for managing memory for the other allocators.
        MTTLSF<MemPolicy::External, TelemetryPolicy> internalAllocator;
        // TODO: Add allocator tags(make tags owned by allocators)
    };
} // namespace pmm


// Size profile
// pmm::MemoryManager manager{};
// static_assert(sizeof(manager) == 217328);

/** @} */

#include "MemoryManager.tpp"
