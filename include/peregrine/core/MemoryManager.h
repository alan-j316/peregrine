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
#include "peregrine/core/PoolRegistry.h"

#include <bitset>
#include <cstdint>

// Allocates memory manager on the stack
// #define PMM_ALLOC_MEM_MGR_ON_STACK
// By default the internal stack is allocated on the OS heap (in front of the requested memory pool)

/**
 * @addtogroup PMM_Core
 * @{
 */

namespace pmm
{
    using AllocateTag = uint32_t;

    /// mt    - multithreadable
    /// local - must not be called in a multithreaded environment.
    /// safe  - non-critical checks like nullptr free are guarded against.
    ///         These will not be present in release mode with the "non-safe" variants.
    struct MemoryManagerConfig
    {
        /// <--------- ARENA --------->
        size_t localArenaCount;
        size_t safeLocalArenaCount;
        size_t mtArenaCount;
        size_t safeMtArenaCount;

        /// <--------- STACK --------->
        size_t localStackCount;
        size_t safeLocalStackCount;
        size_t mtStackCount;
        size_t safeMtStackCount;

        /// <--------- POOL --------->
        size_t localPoolCount;
        size_t safeLocalPoolCount;
        size_t mtPoolCount;
        size_t safeMtPoolCount;

        /// <--------- TLSF --------->
        size_t localTLSFCount;
        size_t safeLocalTLSFCount;
        size_t mtTLSFCount;
        size_t safeMtTLSFCount;

        /// <------- TODO: RESERVED WIP --------->
        /// <--------- DOUBLE STACK --------->
        size_t localDequeCount;
        size_t safeLocalDequeCount;
        size_t mtDequeCount;
        size_t safeMtDequeCount;

        /// <------- TODO: RESERVED WIP --------->
        /// <--------- TAGGED POOL STACK --------->
        size_t localTaggedPoolCount;
        size_t safeLocalTaggedPoolCount;
        size_t mtTaggedPoolCount;
        size_t safeMtTaggedPoolCount;
    };

    /// Default configuration for memory manager. Must be adapted to engine needs at compile time.
    constexpr MemoryManagerConfig DefaultMemoryManagerConfig{ /// <--------- ARENA --------->
                                                              .localArenaCount     = 16,
                                                              .safeLocalArenaCount = 16,
                                                              .mtArenaCount        = 4,
                                                              .safeMtArenaCount    = 4,

                                                              /// <--------- STACK --------->
                                                              .localStackCount     = 8,
                                                              .safeLocalStackCount = 8,
                                                              .mtStackCount        = 4,
                                                              .safeMtStackCount    = 4,

                                                              /// <--------- POOL --------->
                                                              .localPoolCount     = 128,
                                                              .safeLocalPoolCount = 8,
                                                              .mtPoolCount        = 128,
                                                              .safeMtPoolCount    = 8,

                                                              /// <--------- TLSF --------->
                                                              .localTLSFCount     = 1,
                                                              .safeLocalTLSFCount = 1,
                                                              .mtTLSFCount        = 2,
                                                              .safeMtTLSFCount    = 2,

                                                              /// <------- TODO: RESERVED WIP --------->
                                                              /// <--------- DOUBLE STACK --------->
                                                              .localDequeCount     = 0,
                                                              .safeLocalDequeCount = 0,
                                                              .mtDequeCount        = 0,
                                                              .safeMtDequeCount    = 0,

                                                              /// <------- TODO: RESERVED WIP --------->
                                                              /// <--------- TAGGED POOL STACK --------->
                                                              .localTaggedPoolCount     = 0,
                                                              .safeLocalTaggedPoolCount = 0,
                                                              .mtTaggedPoolCount        = 0,
                                                              .safeMtTaggedPoolCount    = 0
    };


    /// @brief Internal context store for the memory manager.
    template <TelPolicy Telemetry, MemoryManagerConfig Config>
    struct MemoryContext
    {
        /// <--------- ARENA --------->
        PoolRegistry<LocalArena<MemPolicy::External, Telemetry>, Config.localArenaCount> localArenas{};
        PoolRegistry<SafeLocalArena<MemPolicy::External, Telemetry>, Config.safeLocalArenaCount> safeLocalArenas{};
        PoolRegistry<MTArena<MemPolicy::External, Telemetry>, Config.mtArenaCount> mtArenas{};
        PoolRegistry<SafeMTArena<MemPolicy::External, Telemetry>, Config.safeMtArenaCount> safeMTArenas{};

        /// <--------- POOL --------->
        PoolRegistry<LocalPool<MemPolicy::External, Telemetry>, Config.localPoolCount> localPools{};
        PoolRegistry<SafeLocalPool<MemPolicy::External, Telemetry>, Config.safeLocalPoolCount> safeLocalPools{};
        PoolRegistry<MTPool<MemPolicy::External, Telemetry>, Config.mtPoolCount> mtPools{};
        PoolRegistry<SafeMTPool<MemPolicy::External, Telemetry>, Config.safeMtPoolCount> safeMTPools{};

        /// <--------- STACK --------->
        PoolRegistry<LocalStack<MemPolicy::External, Telemetry>, Config.localStackCount> localStacks{};
        PoolRegistry<SafeLocalStack<MemPolicy::External, Telemetry>, Config.safeLocalStackCount> safeLocalStacks{};
        PoolRegistry<MTStack<MemPolicy::External, Telemetry>, Config.mtStackCount> mtStacks{};
        PoolRegistry<SafeMTStack<MemPolicy::External, Telemetry>, Config.safeMtStackCount> safeMTStacks{};

        /// <--------- TLSF --------->
        PoolRegistry<LocalTLSF<MemPolicy::External, Telemetry>, Config.localTLSFCount> localTLSFs{};
        PoolRegistry<SafeLocalTLSF<MemPolicy::External, Telemetry>, Config.safeLocalTLSFCount> safeLocalTLSFs{};
        PoolRegistry<MTTLSF<MemPolicy::External, Telemetry>, Config.mtTLSFCount> mtTLSFs{};
        PoolRegistry<SafeMTTLSF<MemPolicy::External, Telemetry>, Config.safeMtTLSFCount> safeMTTLSFs{};


        /// <--------- TODO: Reserved Double Stack --------->
        /// <--------- TODO: Reserved Tagged Pool --------->
    };



    /// Multithreadable memory manager.
    /// @tparam TelemetryPolicy Telemetry policy used by the memory manager. Enabled by default.
    ///                         Enabling Telemetry can consume more memory and can have a mild impact on performance.
    /// @tparam MemoryPolicy    Dictates whether the buffer is supplied externally or owned internally.
    ///                         Internally managed by default.
    /// @tparam Config          The configuration for the memory manager.
    template <TelPolicy TelemetryPolicy = TelPolicy::Enabled, MemoryManagerConfig Config = DefaultMemoryManagerConfig,
              MemPolicy MemoryPolicy = MemPolicy::Internal>
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
         * @brief Get the internal context's memory requirement.
         * @return The context size in bytes.
         */
        [[nodiscard]] static constexpr size_t getContextSize() noexcept;


        /**
         * @brief Create a thread local arena.
         * @param size The size of the arena. Must be less than the total available size.
         * @return A pointer to an arena instance.
         */
        [[nodiscard]] LocalArena<MemPolicy::External, TelemetryPolicy>* createLocalArena(size_t size) noexcept;

        /**
         * @brief Create a thread local arena with safe mode.
         * @param size The size of the arena. Must be less than the total available size.
         * @return A pointer to an arena instance.
         */
        [[nodiscard]] SafeLocalArena<MemPolicy::External, TelemetryPolicy>* createSafeLocalArena(size_t size) noexcept;

        /**
         * @brief Create a multithreading safe arena.
         * @param size The size of the arena. Must be less than the total available size.
         * @return A pointer to an arena instance.
         */
        [[nodiscard]] MTArena<MemPolicy::External, TelemetryPolicy>* createMTArena(size_t size) noexcept;


        /**
         * @brief Create a multithreading safe arena with safe mode.
         * @param size The size of the arena. Must be less than the total available size.
         * @return A pointer to an arena instance.
         */
        [[nodiscard]] SafeMTArena<MemPolicy::External, TelemetryPolicy>* createSafeMTArena(size_t size) noexcept;


        /**
         * @brief Create a thread local stack.
         * @param size The size of the stack. Must be less than the total available size.
         * @return A pointer to an stack instance.
         */
        [[nodiscard]] LocalStack<MemPolicy::External, TelemetryPolicy>* createLocalStack(size_t size) noexcept;

        /**
         * @brief Create a thread local stack with safe mode.
         * @param size The size of the stack. Must be less than the total available size.
         * @return A pointer to an stack instance.
         */
        [[nodiscard]] SafeLocalStack<MemPolicy::External, TelemetryPolicy>* createSafeLocalStack(size_t size) noexcept;

        /**
         * @brief Create a multithreading safe stack.
         * @param size The size of the stack. Must be less than the total available size.
         * @return A pointer to an stack instance.
         */
        [[nodiscard]] MTStack<MemPolicy::External, TelemetryPolicy>* createMTStack(size_t size) noexcept;


        /**
         * @brief Create a multithreading safe stack with safe mode.
         * @param size The size of the stack. Must be less than the total available size.
         * @return A pointer to an stack instance.
         */
        [[nodiscard]] SafeMTStack<MemPolicy::External, TelemetryPolicy>* createSafeMTStack(size_t size) noexcept;



        // BLOCK OUT
        // TODO: add death tests
    private:
        uint8_t* _buffer{};
        size_t _usedSize{ 0 };
        size_t _totalSize;
        /// Internal TLSF allocator for managing memory for the other allocators.
        MTTLSF<MemPolicy::External, TelemetryPolicy> internalAllocator;

#ifdef PMM_ALLOC_MEM_MGR_ON_STACK
        MemoryContext<TelemetryPolicy, Config> _ctx{};
#else
        MemoryContext<TelemetryPolicy, Config>* _ctx{};
#endif
    };
} // namespace pmm


// Size profile
// pmm::MemoryManager manager{};
// static_assert(sizeof(manager) == 217328);

/** @} */

#include "MemoryManager.tpp"
