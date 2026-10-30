/**
 * @file AliasTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 30, 2026
 *
 * @brief Verify memory manager integration.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include <peregrine/Peregrine.h>

/// Test Setup
namespace
{
    using namespace pmm;

    namespace pool_tests
    {
        /// Verify pool aliases match the correct internal policies.
        /// MTPool
        static_assert(std::is_same_v<
                      MTPool<MemPolicy::Internal, TelPolicy::Disabled>,
                      Pool<MemPolicy::Internal, TelPolicy::Disabled, SafeModePolicy::Unsafe, mt::MTPolicy::SpinLock>>);

        static_assert(std::is_same_v<
                      MTPool<MemPolicy::External, TelPolicy::Disabled>,
                      Pool<MemPolicy::External, TelPolicy::Disabled, SafeModePolicy::Unsafe, mt::MTPolicy::SpinLock>>);

        static_assert(std::is_same_v<
                      MTPool<MemPolicy::Internal, TelPolicy::Enabled>,
                      Pool<MemPolicy::Internal, TelPolicy::Enabled, SafeModePolicy::Unsafe, mt::MTPolicy::SpinLock>>);

        static_assert(std::is_same_v<
                      MTPool<MemPolicy::External, TelPolicy::Enabled>,
                      Pool<MemPolicy::External, TelPolicy::Enabled, SafeModePolicy::Unsafe, mt::MTPolicy::SpinLock>>);


        /// SafeMTPool
        static_assert(std::is_same_v<
                      SafeMTPool<MemPolicy::Internal, TelPolicy::Disabled>,
                      Pool<MemPolicy::Internal, TelPolicy::Disabled, SafeModePolicy::Safe, mt::MTPolicy::SpinLock>>);

        static_assert(std::is_same_v<
                      SafeMTPool<MemPolicy::External, TelPolicy::Disabled>,
                      Pool<MemPolicy::External, TelPolicy::Disabled, SafeModePolicy::Safe, mt::MTPolicy::SpinLock>>);

        static_assert(std::is_same_v<
                      SafeMTPool<MemPolicy::Internal, TelPolicy::Enabled>,
                      Pool<MemPolicy::Internal, TelPolicy::Enabled, SafeModePolicy::Safe, mt::MTPolicy::SpinLock>>);

        static_assert(std::is_same_v<
                      SafeMTPool<MemPolicy::External, TelPolicy::Enabled>,
                      Pool<MemPolicy::External, TelPolicy::Enabled, SafeModePolicy::Safe, mt::MTPolicy::SpinLock>>);

        /// LocalPool
        static_assert(
            std::is_same_v<LocalPool<MemPolicy::Internal, TelPolicy::Disabled>,
                           Pool<MemPolicy::Internal, TelPolicy::Disabled, SafeModePolicy::Unsafe, mt::MTPolicy::NoMT>>);

        static_assert(
            std::is_same_v<LocalPool<MemPolicy::External, TelPolicy::Disabled>,
                           Pool<MemPolicy::External, TelPolicy::Disabled, SafeModePolicy::Unsafe, mt::MTPolicy::NoMT>>);

        static_assert(
            std::is_same_v<LocalPool<MemPolicy::Internal, TelPolicy::Enabled>,
                           Pool<MemPolicy::Internal, TelPolicy::Enabled, SafeModePolicy::Unsafe, mt::MTPolicy::NoMT>>);

        static_assert(
            std::is_same_v<LocalPool<MemPolicy::External, TelPolicy::Enabled>,
                           Pool<MemPolicy::External, TelPolicy::Enabled, SafeModePolicy::Unsafe, mt::MTPolicy::NoMT>>);

        /// SafeLocalPool
        static_assert(
            std::is_same_v<SafeLocalPool<MemPolicy::Internal, TelPolicy::Disabled>,
                           Pool<MemPolicy::Internal, TelPolicy::Disabled, SafeModePolicy::Safe, mt::MTPolicy::NoMT>>);

        static_assert(
            std::is_same_v<SafeLocalPool<MemPolicy::External, TelPolicy::Disabled>,
                           Pool<MemPolicy::External, TelPolicy::Disabled, SafeModePolicy::Safe, mt::MTPolicy::NoMT>>);

        static_assert(
            std::is_same_v<SafeLocalPool<MemPolicy::Internal, TelPolicy::Enabled>,
                           Pool<MemPolicy::Internal, TelPolicy::Enabled, SafeModePolicy::Safe, mt::MTPolicy::NoMT>>);

        static_assert(
            std::is_same_v<SafeLocalPool<MemPolicy::External, TelPolicy::Enabled>,
                           Pool<MemPolicy::External, TelPolicy::Enabled, SafeModePolicy::Safe, mt::MTPolicy::NoMT>>);


        /// @test Verify that safe all pool variants are internally managed and
        ///       has telemetry policy disabled by default.
        static_assert(std::is_same_v<
                      MTPool<>,
                      Pool<MemPolicy::Internal, TelPolicy::Disabled, SafeModePolicy::Unsafe, mt::MTPolicy::SpinLock>>);
        static_assert(std::is_same_v<
                      SafeMTPool<>,
                      Pool<MemPolicy::Internal, TelPolicy::Disabled, SafeModePolicy::Safe, mt::MTPolicy::SpinLock>>);
        static_assert(
            std::is_same_v<LocalPool<>,
                           Pool<MemPolicy::Internal, TelPolicy::Disabled, SafeModePolicy::Unsafe, mt::MTPolicy::NoMT>>);
        static_assert(
            std::is_same_v<SafeLocalPool<>,
                           Pool<MemPolicy::Internal, TelPolicy::Disabled, SafeModePolicy::Safe, mt::MTPolicy::NoMT>>);

    } // namespace pool_tests


    namespace arena_tests
    {
        /// Verify arena aliases match the correct internal policies.
        /// MTArena
        static_assert(std::is_same_v<
                      MTArena<MemPolicy::Internal, TelPolicy::Disabled>,
                      Arena<MemPolicy::Internal, TelPolicy::Disabled, SafeModePolicy::Unsafe, mt::MTPolicy::SpinLock>>);

        static_assert(std::is_same_v<
                      MTArena<MemPolicy::External, TelPolicy::Disabled>,
                      Arena<MemPolicy::External, TelPolicy::Disabled, SafeModePolicy::Unsafe, mt::MTPolicy::SpinLock>>);

        static_assert(std::is_same_v<
                      MTArena<MemPolicy::Internal, TelPolicy::Enabled>,
                      Arena<MemPolicy::Internal, TelPolicy::Enabled, SafeModePolicy::Unsafe, mt::MTPolicy::SpinLock>>);

        static_assert(std::is_same_v<
                      MTArena<MemPolicy::External, TelPolicy::Enabled>,
                      Arena<MemPolicy::External, TelPolicy::Enabled, SafeModePolicy::Unsafe, mt::MTPolicy::SpinLock>>);


        /// SafeMTArena
        static_assert(std::is_same_v<
                      SafeMTArena<MemPolicy::Internal, TelPolicy::Disabled>,
                      Arena<MemPolicy::Internal, TelPolicy::Disabled, SafeModePolicy::Safe, mt::MTPolicy::SpinLock>>);

        static_assert(std::is_same_v<
                      SafeMTArena<MemPolicy::External, TelPolicy::Disabled>,
                      Arena<MemPolicy::External, TelPolicy::Disabled, SafeModePolicy::Safe, mt::MTPolicy::SpinLock>>);

        static_assert(std::is_same_v<
                      SafeMTArena<MemPolicy::Internal, TelPolicy::Enabled>,
                      Arena<MemPolicy::Internal, TelPolicy::Enabled, SafeModePolicy::Safe, mt::MTPolicy::SpinLock>>);

        static_assert(std::is_same_v<
                      SafeMTArena<MemPolicy::External, TelPolicy::Enabled>,
                      Arena<MemPolicy::External, TelPolicy::Enabled, SafeModePolicy::Safe, mt::MTPolicy::SpinLock>>);

        /// LocalArena
        static_assert(std::is_same_v<
                      LocalArena<MemPolicy::Internal, TelPolicy::Disabled>,
                      Arena<MemPolicy::Internal, TelPolicy::Disabled, SafeModePolicy::Unsafe, mt::MTPolicy::NoMT>>);

        static_assert(std::is_same_v<
                      LocalArena<MemPolicy::External, TelPolicy::Disabled>,
                      Arena<MemPolicy::External, TelPolicy::Disabled, SafeModePolicy::Unsafe, mt::MTPolicy::NoMT>>);

        static_assert(
            std::is_same_v<LocalArena<MemPolicy::Internal, TelPolicy::Enabled>,
                           Arena<MemPolicy::Internal, TelPolicy::Enabled, SafeModePolicy::Unsafe, mt::MTPolicy::NoMT>>);

        static_assert(
            std::is_same_v<LocalArena<MemPolicy::External, TelPolicy::Enabled>,
                           Arena<MemPolicy::External, TelPolicy::Enabled, SafeModePolicy::Unsafe, mt::MTPolicy::NoMT>>);

        /// SafeLocalArena
        static_assert(
            std::is_same_v<SafeLocalArena<MemPolicy::Internal, TelPolicy::Disabled>,
                           Arena<MemPolicy::Internal, TelPolicy::Disabled, SafeModePolicy::Safe, mt::MTPolicy::NoMT>>);

        static_assert(
            std::is_same_v<SafeLocalArena<MemPolicy::External, TelPolicy::Disabled>,
                           Arena<MemPolicy::External, TelPolicy::Disabled, SafeModePolicy::Safe, mt::MTPolicy::NoMT>>);

        static_assert(
            std::is_same_v<SafeLocalArena<MemPolicy::Internal, TelPolicy::Enabled>,
                           Arena<MemPolicy::Internal, TelPolicy::Enabled, SafeModePolicy::Safe, mt::MTPolicy::NoMT>>);

        static_assert(
            std::is_same_v<SafeLocalArena<MemPolicy::External, TelPolicy::Enabled>,
                           Arena<MemPolicy::External, TelPolicy::Enabled, SafeModePolicy::Safe, mt::MTPolicy::NoMT>>);


        /// @test Verify that safe all arena variants are internally managed and
        ///       has telemetry policy disabled by default.
        static_assert(std::is_same_v<
                      MTArena<>,
                      Arena<MemPolicy::Internal, TelPolicy::Disabled, SafeModePolicy::Unsafe, mt::MTPolicy::SpinLock>>);
        static_assert(std::is_same_v<
                      SafeMTArena<>,
                      Arena<MemPolicy::Internal, TelPolicy::Disabled, SafeModePolicy::Safe, mt::MTPolicy::SpinLock>>);
        static_assert(std::is_same_v<
                      LocalArena<>,
                      Arena<MemPolicy::Internal, TelPolicy::Disabled, SafeModePolicy::Unsafe, mt::MTPolicy::NoMT>>);
        static_assert(
            std::is_same_v<SafeLocalArena<>,
                           Arena<MemPolicy::Internal, TelPolicy::Disabled, SafeModePolicy::Safe, mt::MTPolicy::NoMT>>);
    } // namespace arena_tests


    namespace stack_tests
    {
        /// Verify stack aliases match the correct internal policies.
        /// MTStack
        static_assert(std::is_same_v<MTStack<MemPolicy::Internal, TelPolicy::Disabled>,
                                     Stack<StackPolicy::Strict, MemPolicy::Internal, TelPolicy::Disabled,
                                           SafeModePolicy::Unsafe, mt::MTPolicy::SpinLock>>);

        static_assert(std::is_same_v<MTStack<MemPolicy::External, TelPolicy::Disabled>,
                                     Stack<StackPolicy::Strict, MemPolicy::External, TelPolicy::Disabled,
                                           SafeModePolicy::Unsafe, mt::MTPolicy::SpinLock>>);

        static_assert(std::is_same_v<MTStack<MemPolicy::Internal, TelPolicy::Enabled>,
                                     Stack<StackPolicy::Strict, MemPolicy::Internal, TelPolicy::Enabled,
                                           SafeModePolicy::Unsafe, mt::MTPolicy::SpinLock>>);

        static_assert(std::is_same_v<MTStack<MemPolicy::External, TelPolicy::Enabled>,
                                     Stack<StackPolicy::Strict, MemPolicy::External, TelPolicy::Enabled,
                                           SafeModePolicy::Unsafe, mt::MTPolicy::SpinLock>>);


        /// SafeMTStack
        static_assert(std::is_same_v<SafeMTStack<MemPolicy::Internal, TelPolicy::Disabled>,
                                     Stack<StackPolicy::Strict, MemPolicy::Internal, TelPolicy::Disabled,
                                           SafeModePolicy::Safe, mt::MTPolicy::SpinLock>>);

        static_assert(std::is_same_v<SafeMTStack<MemPolicy::External, TelPolicy::Disabled>,
                                     Stack<StackPolicy::Strict, MemPolicy::External, TelPolicy::Disabled,
                                           SafeModePolicy::Safe, mt::MTPolicy::SpinLock>>);

        static_assert(std::is_same_v<SafeMTStack<MemPolicy::Internal, TelPolicy::Enabled>,
                                     Stack<StackPolicy::Strict, MemPolicy::Internal, TelPolicy::Enabled,
                                           SafeModePolicy::Safe, mt::MTPolicy::SpinLock>>);

        static_assert(std::is_same_v<SafeMTStack<MemPolicy::External, TelPolicy::Enabled>,
                                     Stack<StackPolicy::Strict, MemPolicy::External, TelPolicy::Enabled,
                                           SafeModePolicy::Safe, mt::MTPolicy::SpinLock>>);

        /// LocalStack
        static_assert(std::is_same_v<LocalStack<MemPolicy::Internal, TelPolicy::Disabled>,
                                     Stack<StackPolicy::Strict, MemPolicy::Internal, TelPolicy::Disabled,
                                           SafeModePolicy::Unsafe, mt::MTPolicy::NoMT>>);

        static_assert(std::is_same_v<LocalStack<MemPolicy::External, TelPolicy::Disabled>,
                                     Stack<StackPolicy::Strict, MemPolicy::External, TelPolicy::Disabled,
                                           SafeModePolicy::Unsafe, mt::MTPolicy::NoMT>>);

        static_assert(std::is_same_v<LocalStack<MemPolicy::Internal, TelPolicy::Enabled>,
                                     Stack<StackPolicy::Strict, MemPolicy::Internal, TelPolicy::Enabled,
                                           SafeModePolicy::Unsafe, mt::MTPolicy::NoMT>>);

        static_assert(std::is_same_v<LocalStack<MemPolicy::External, TelPolicy::Enabled>,
                                     Stack<StackPolicy::Strict, MemPolicy::External, TelPolicy::Enabled,
                                           SafeModePolicy::Unsafe, mt::MTPolicy::NoMT>>);

        /// SafeLocalStack
        static_assert(std::is_same_v<SafeLocalStack<MemPolicy::Internal, TelPolicy::Disabled>,
                                     Stack<StackPolicy::Strict, MemPolicy::Internal, TelPolicy::Disabled,
                                           SafeModePolicy::Safe, mt::MTPolicy::NoMT>>);

        static_assert(std::is_same_v<SafeLocalStack<MemPolicy::External, TelPolicy::Disabled>,
                                     Stack<StackPolicy::Strict, MemPolicy::External, TelPolicy::Disabled,
                                           SafeModePolicy::Safe, mt::MTPolicy::NoMT>>);

        static_assert(std::is_same_v<SafeLocalStack<MemPolicy::Internal, TelPolicy::Enabled>,
                                     Stack<StackPolicy::Strict, MemPolicy::Internal, TelPolicy::Enabled,
                                           SafeModePolicy::Safe, mt::MTPolicy::NoMT>>);

        static_assert(std::is_same_v<SafeLocalStack<MemPolicy::External, TelPolicy::Enabled>,
                                     Stack<StackPolicy::Strict, MemPolicy::External, TelPolicy::Enabled,
                                           SafeModePolicy::Safe, mt::MTPolicy::NoMT>>);


        /// @test Verify that safe all stack variants are internally managed and
        ///       has telemetry policy disabled by default.
        static_assert(std::is_same_v<MTStack<>,
                                     Stack<StackPolicy::Strict, MemPolicy::Internal, TelPolicy::Disabled,
                                           SafeModePolicy::Unsafe, mt::MTPolicy::SpinLock>>);
        static_assert(std::is_same_v<SafeMTStack<>,
                                     Stack<StackPolicy::Strict, MemPolicy::Internal, TelPolicy::Disabled,
                                           SafeModePolicy::Safe, mt::MTPolicy::SpinLock>>);
        static_assert(std::is_same_v<LocalStack<>,
                                     Stack<StackPolicy::Strict, MemPolicy::Internal, TelPolicy::Disabled,
                                           SafeModePolicy::Unsafe, mt::MTPolicy::NoMT>>);
        static_assert(std::is_same_v<SafeLocalStack<>,
                                     Stack<StackPolicy::Strict, MemPolicy::Internal, TelPolicy::Disabled,
                                           SafeModePolicy::Safe, mt::MTPolicy::NoMT>>);
    } // namespace stack_tests


    namespace tlsf_tests
    {
        /// Verify tlsf aliases match the correct internal policies.
        /// MTTLSF
        static_assert(std::is_same_v<
                      MTTLSF<MemPolicy::Internal, TelPolicy::Disabled>,
                      TLSF<MemPolicy::Internal, TelPolicy::Disabled, SafeModePolicy::Unsafe, mt::MTPolicy::SpinLock>>);

        static_assert(std::is_same_v<
                      MTTLSF<MemPolicy::External, TelPolicy::Disabled>,
                      TLSF<MemPolicy::External, TelPolicy::Disabled, SafeModePolicy::Unsafe, mt::MTPolicy::SpinLock>>);

        static_assert(std::is_same_v<
                      MTTLSF<MemPolicy::Internal, TelPolicy::Enabled>,
                      TLSF<MemPolicy::Internal, TelPolicy::Enabled, SafeModePolicy::Unsafe, mt::MTPolicy::SpinLock>>);

        static_assert(std::is_same_v<
                      MTTLSF<MemPolicy::External, TelPolicy::Enabled>,
                      TLSF<MemPolicy::External, TelPolicy::Enabled, SafeModePolicy::Unsafe, mt::MTPolicy::SpinLock>>);


        /// SafeMTTLSF
        static_assert(std::is_same_v<
                      SafeMTTLSF<MemPolicy::Internal, TelPolicy::Disabled>,
                      TLSF<MemPolicy::Internal, TelPolicy::Disabled, SafeModePolicy::Safe, mt::MTPolicy::SpinLock>>);

        static_assert(std::is_same_v<
                      SafeMTTLSF<MemPolicy::External, TelPolicy::Disabled>,
                      TLSF<MemPolicy::External, TelPolicy::Disabled, SafeModePolicy::Safe, mt::MTPolicy::SpinLock>>);

        static_assert(std::is_same_v<
                      SafeMTTLSF<MemPolicy::Internal, TelPolicy::Enabled>,
                      TLSF<MemPolicy::Internal, TelPolicy::Enabled, SafeModePolicy::Safe, mt::MTPolicy::SpinLock>>);

        static_assert(std::is_same_v<
                      SafeMTTLSF<MemPolicy::External, TelPolicy::Enabled>,
                      TLSF<MemPolicy::External, TelPolicy::Enabled, SafeModePolicy::Safe, mt::MTPolicy::SpinLock>>);

        /// LocalTLSF
        static_assert(
            std::is_same_v<LocalTLSF<MemPolicy::Internal, TelPolicy::Disabled>,
                           TLSF<MemPolicy::Internal, TelPolicy::Disabled, SafeModePolicy::Unsafe, mt::MTPolicy::NoMT>>);

        static_assert(
            std::is_same_v<LocalTLSF<MemPolicy::External, TelPolicy::Disabled>,
                           TLSF<MemPolicy::External, TelPolicy::Disabled, SafeModePolicy::Unsafe, mt::MTPolicy::NoMT>>);

        static_assert(
            std::is_same_v<LocalTLSF<MemPolicy::Internal, TelPolicy::Enabled>,
                           TLSF<MemPolicy::Internal, TelPolicy::Enabled, SafeModePolicy::Unsafe, mt::MTPolicy::NoMT>>);

        static_assert(
            std::is_same_v<LocalTLSF<MemPolicy::External, TelPolicy::Enabled>,
                           TLSF<MemPolicy::External, TelPolicy::Enabled, SafeModePolicy::Unsafe, mt::MTPolicy::NoMT>>);

        /// SafeLocalTLSF
        static_assert(
            std::is_same_v<SafeLocalTLSF<MemPolicy::Internal, TelPolicy::Disabled>,
                           TLSF<MemPolicy::Internal, TelPolicy::Disabled, SafeModePolicy::Safe, mt::MTPolicy::NoMT>>);

        static_assert(
            std::is_same_v<SafeLocalTLSF<MemPolicy::External, TelPolicy::Disabled>,
                           TLSF<MemPolicy::External, TelPolicy::Disabled, SafeModePolicy::Safe, mt::MTPolicy::NoMT>>);

        static_assert(
            std::is_same_v<SafeLocalTLSF<MemPolicy::Internal, TelPolicy::Enabled>,
                           TLSF<MemPolicy::Internal, TelPolicy::Enabled, SafeModePolicy::Safe, mt::MTPolicy::NoMT>>);

        static_assert(
            std::is_same_v<SafeLocalTLSF<MemPolicy::External, TelPolicy::Enabled>,
                           TLSF<MemPolicy::External, TelPolicy::Enabled, SafeModePolicy::Safe, mt::MTPolicy::NoMT>>);


        /// @test Verify that safe all tlsf variants are internally managed and
        ///       has telemetry policy disabled by default.
        static_assert(std::is_same_v<
                      MTTLSF<>,
                      TLSF<MemPolicy::Internal, TelPolicy::Disabled, SafeModePolicy::Unsafe, mt::MTPolicy::SpinLock>>);
        static_assert(std::is_same_v<
                      SafeMTTLSF<>,
                      TLSF<MemPolicy::Internal, TelPolicy::Disabled, SafeModePolicy::Safe, mt::MTPolicy::SpinLock>>);
        static_assert(
            std::is_same_v<LocalTLSF<>,
                           TLSF<MemPolicy::Internal, TelPolicy::Disabled, SafeModePolicy::Unsafe, mt::MTPolicy::NoMT>>);
        static_assert(
            std::is_same_v<SafeLocalTLSF<>,
                           TLSF<MemPolicy::Internal, TelPolicy::Disabled, SafeModePolicy::Safe, mt::MTPolicy::NoMT>>);
    } // namespace tlsf_tests

} // namespace
