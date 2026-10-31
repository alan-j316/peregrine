#pragma once
/**
 * @file Alias.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 30, 2026
 *
 * @brief Alias for commonly used allocators.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "peregrine/allocators/Arena.h"
#include "peregrine/allocators/Pool.h"
#include "peregrine/allocators/Stack.h"
#include "peregrine/allocators/TLSF.h"

/**
 * @addtogroup PMM_Core
 * @{
 */

namespace pmm
{

    /// +-----------------------------------------------------------------------------------------------------------+
    /// |                                    ALLOCATOR ALIASES                                                      |
    /// +-----------------------------------------------------------------------------------------------------------+

    /// Aliases (MT - Multithreaded)
    /// MT<Allocator>        - Guarantees safety in a multithreaded environment.
    /// SafeMT<Allocator>    - Multithreading support and performs safety checks such as for `nullptr` in Release Mode.
    /// Local<Allocator>     - Not suited for multi-threaded access as the allocations are not synchronized.
    /// SafeLocal<Allocator> - No MT support but has safety catches in Release Mode.
    ///
    /// @note All allocators are internally managed and has telemetry turned off by default.
    /// @note Regardless of Safety Policy, all allocator are checked for unsafe parameters like `nullptr`s
    ///       with ASSERTS in Debug Mode.

    // ---------
    //   POOL
    // ---------
    template <MemPolicy MemoryPolicy = MemPolicy::Internal, TelPolicy TelemetryPolicy = TelPolicy::Disabled>
    using MTPool = Pool<MemoryPolicy, TelemetryPolicy, SafeModePolicy::Unsafe, mt::MTPolicy::SpinLock>;

    template <MemPolicy MemoryPolicy = MemPolicy::Internal, TelPolicy TelemetryPolicy = TelPolicy::Disabled>
    using SafeMTPool = Pool<MemoryPolicy, TelemetryPolicy, SafeModePolicy::Safe, mt::MTPolicy::SpinLock>;

    template <MemPolicy MemoryPolicy = MemPolicy::Internal, TelPolicy TelemetryPolicy = TelPolicy::Disabled>
    using LocalPool = Pool<MemoryPolicy, TelemetryPolicy, SafeModePolicy::Unsafe, mt::MTPolicy::NoMT>;

    template <MemPolicy MemoryPolicy = MemPolicy::Internal, TelPolicy TelemetryPolicy = TelPolicy::Disabled>
    using SafeLocalPool = Pool<MemoryPolicy, TelemetryPolicy, SafeModePolicy::Safe, mt::MTPolicy::NoMT>;



    // ---------
    //   STACK
    // ---------

    template <MemPolicy MemoryPolicy = MemPolicy::Internal, TelPolicy TelemetryPolicy = TelPolicy::Disabled>
    using MTStack =
        Stack<StackPolicy::Strict, MemoryPolicy, TelemetryPolicy, SafeModePolicy::Unsafe, mt::MTPolicy::SpinLock>;

    template <MemPolicy MemoryPolicy = MemPolicy::Internal, TelPolicy TelemetryPolicy = TelPolicy::Disabled>
    using SafeMTStack =
        Stack<StackPolicy::Strict, MemoryPolicy, TelemetryPolicy, SafeModePolicy::Safe, mt::MTPolicy::SpinLock>;

    template <MemPolicy MemoryPolicy = MemPolicy::Internal, TelPolicy TelemetryPolicy = TelPolicy::Disabled>
    using LocalStack =
        Stack<StackPolicy::Strict, MemoryPolicy, TelemetryPolicy, SafeModePolicy::Unsafe, mt::MTPolicy::NoMT>;

    template <MemPolicy MemoryPolicy = MemPolicy::Internal, TelPolicy TelemetryPolicy = TelPolicy::Disabled>
    using SafeLocalStack =
        Stack<StackPolicy::Strict, MemoryPolicy, TelemetryPolicy, SafeModePolicy::Safe, mt::MTPolicy::NoMT>;



    // ---------
    //   ARENA
    // ---------

    template <MemPolicy MemoryPolicy = MemPolicy::Internal, TelPolicy TelemetryPolicy = TelPolicy::Disabled>
    using MTArena = Arena<MemoryPolicy, TelemetryPolicy, SafeModePolicy::Unsafe, mt::MTPolicy::SpinLock>;

    template <MemPolicy MemoryPolicy = MemPolicy::Internal, TelPolicy TelemetryPolicy = TelPolicy::Disabled>
    using SafeMTArena = Arena<MemoryPolicy, TelemetryPolicy, SafeModePolicy::Safe, mt::MTPolicy::SpinLock>;

    template <MemPolicy MemoryPolicy = MemPolicy::Internal, TelPolicy TelemetryPolicy = TelPolicy::Disabled>
    using LocalArena = Arena<MemoryPolicy, TelemetryPolicy, SafeModePolicy::Unsafe, mt::MTPolicy::NoMT>;

    template <MemPolicy MemoryPolicy = MemPolicy::Internal, TelPolicy TelemetryPolicy = TelPolicy::Disabled>
    using SafeLocalArena = Arena<MemoryPolicy, TelemetryPolicy, SafeModePolicy::Safe, mt::MTPolicy::NoMT>;



    // ---------
    //   TLSF
    // ---------

    template <MemPolicy MemoryPolicy = MemPolicy::Internal, TelPolicy TelemetryPolicy = TelPolicy::Disabled>
    using MTTLSF = TLSF<MemoryPolicy, TelemetryPolicy, SafeModePolicy::Unsafe, mt::MTPolicy::SpinLock>;

    template <MemPolicy MemoryPolicy = MemPolicy::Internal, TelPolicy TelemetryPolicy = TelPolicy::Disabled>
    using SafeMTTLSF = TLSF<MemoryPolicy, TelemetryPolicy, SafeModePolicy::Safe, mt::MTPolicy::SpinLock>;

    template <MemPolicy MemoryPolicy = MemPolicy::Internal, TelPolicy TelemetryPolicy = TelPolicy::Disabled>
    using LocalTLSF = TLSF<MemoryPolicy, TelemetryPolicy, SafeModePolicy::Unsafe, mt::MTPolicy::NoMT>;

    template <MemPolicy MemoryPolicy = MemPolicy::Internal, TelPolicy TelemetryPolicy = TelPolicy::Disabled>
    using SafeLocalTLSF = TLSF<MemoryPolicy, TelemetryPolicy, SafeModePolicy::Safe, mt::MTPolicy::NoMT>;

} // namespace pmm

/** @} */
