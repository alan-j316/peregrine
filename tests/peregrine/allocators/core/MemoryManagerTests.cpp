/**
 * @file MemoryManagerTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 29, 2026
 *
 * @brief Verify memory manager integration.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include <gtest/gtest.h>
#include <peregrine/Peregrine.h>

// TODO: Add test for external and with stack as well heap based ctx variants
// TODO: Add death tests for inits
//
/// Test Setup
namespace
{

    class InternalMemoryManagerTests: public testing::Test
    {
    public:
    };

    /// +---------------------------------------------------------------------------------------+
    /// |                                STATIC TESTS                                           |
    /// +---------------------------------------------------------------------------------------+
    namespace static_tests
    {
        /// Verify that the @ref getContextSize returns the memory footprint of context.
        static_assert(sizeof(pmm::MemoryContext<pmm::TelPolicy::Enabled, pmm::DefaultMemoryManagerConfig>) ==
                      pmm::MemoryManager<pmm::TelPolicy::Enabled, pmm::DefaultMemoryManagerConfig>::getContextSize());
        static_assert(sizeof(pmm::MemoryContext<pmm::TelPolicy::Disabled, pmm::DefaultMemoryManagerConfig>) ==
                      pmm::MemoryManager<pmm::TelPolicy::Disabled, pmm::DefaultMemoryManagerConfig>::getContextSize());


    } // namespace static_tests
} // namespace


TEST_F(InternalMemoryManagerTests, GetContextSize_ReturnsTheSizeOfTheContextWhenTelemetryIsOff)
{
    EXPECT_EQ(sizeof(pmm::MemoryContext<pmm::TelPolicy::Disabled, pmm::DefaultMemoryManagerConfig>),
              (pmm::MemoryManager<pmm::TelPolicy::Disabled, pmm::DefaultMemoryManagerConfig>::getContextSize()));
}


TEST_F(InternalMemoryManagerTests, GetContextSize_ReturnsTheSizeOfTheContextWhenTelemetryIsOn)
{
    EXPECT_EQ(sizeof(pmm::MemoryContext<pmm::TelPolicy::Disabled, pmm::DefaultMemoryManagerConfig>),
              (pmm::MemoryManager<pmm::TelPolicy::Disabled, pmm::DefaultMemoryManagerConfig>::getContextSize()));
}

TEST_F(InternalMemoryManagerTests, Ctor_InitializesTheMemoryManager)
{
    // NOTE: There is no direct way to test successful initialization,
    //       but we can just ensure that the program doesn't crash.
    EXPECT_NO_FATAL_FAILURE(
        (pmm::MemoryManager<pmm::TelPolicy::Disabled, pmm::DefaultMemoryManagerConfig, pmm::MemPolicy::Internal>(500)));
}
