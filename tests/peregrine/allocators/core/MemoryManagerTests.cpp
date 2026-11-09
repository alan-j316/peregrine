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

    using namespace pmm::constants;
    class InternalMemoryManagerTests: public testing::Test
    {
    public:
        static constexpr size_t SIZE = 50_MB;
        pmm::MemoryManager<> manager{ SIZE };
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



/// +---------------------------------------------------------------------------------------+
/// |                               ARENA TESTS                                             |
/// +---------------------------------------------------------------------------------------+

// TODO: Add death tests for maximum memory usage
// TODO: Add death test for allocating more maximum possible arena.
TEST_F(InternalMemoryManagerTests, CreateArena_CreatesAnArena)
{
    const auto arena = manager.createLocalArena(1_KB);
    EXPECT_NE(nullptr, arena);
}


TEST_F(InternalMemoryManagerTests, CreateArena_CanAllocateBuffer)
{
    const auto arena  = manager.createLocalArena(1_KB);
    const auto buffer = arena->malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(InternalMemoryManagerTests, CreateArena_CanCreateSpecifiedNumberOfArenas)
{
    constexpr auto maxArenaCount = pmm::DefaultMemoryManagerConfig.localArenaCount;
    for (size_t i{ 0 }; i < maxArenaCount; ++i)
    {
        const auto arena = manager.createLocalArena(1_KB);
        EXPECT_NE(nullptr, arena);
    }
}


TEST_F(InternalMemoryManagerTests, CreateSafeLocalArena_CreatesAnArena)
{
    const auto arena = manager.createSafeLocalArena(1_KB);
    EXPECT_NE(nullptr, arena);
}


TEST_F(InternalMemoryManagerTests, CreateSafeLocalArena_CanAllocateBuffer)
{
    const auto arena  = manager.createSafeLocalArena(1_KB);
    const auto buffer = arena->malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(InternalMemoryManagerTests, CreateSafeLocalArena_CanCreateSpecifiedNumberOfArenas)
{
    constexpr auto maxArenaCount = pmm::DefaultMemoryManagerConfig.safeLocalArenaCount;
    for (size_t i{ 0 }; i < maxArenaCount; ++i)
    {
        const auto arena = manager.createSafeLocalArena(1_KB);
        EXPECT_NE(nullptr, arena);
    }
}


TEST_F(InternalMemoryManagerTests, CreateMTArena_CreatesAnArena)
{
    const auto arena = manager.createMTArena(1_KB);
    EXPECT_NE(nullptr, arena);
}


TEST_F(InternalMemoryManagerTests, CreateMTArena_CanAllocateBuffer)
{
    const auto arena  = manager.createMTArena(1_KB);
    const auto buffer = arena->malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(InternalMemoryManagerTests, CreateMTArena_CanCreateSpecifiedNumberOfArenas)
{
    constexpr auto maxArenaCount = pmm::DefaultMemoryManagerConfig.mtArenaCount;
    for (size_t i{ 0 }; i < maxArenaCount; ++i)
    {
        const auto arena = manager.createMTArena(1_KB);
        EXPECT_NE(nullptr, arena);
    }
}


TEST_F(InternalMemoryManagerTests, CreateSafeMTArena_CreatesAnArena)
{
    const auto arena = manager.createSafeMTArena(1_KB);
    EXPECT_NE(nullptr, arena);
}


TEST_F(InternalMemoryManagerTests, CreateSafeMTArena_CanAllocateBuffer)
{
    auto arena        = manager.createSafeMTArena(1_KB);
    const auto buffer = arena->malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(InternalMemoryManagerTests, CreateSafeMTArena_CanCreateSpecifiedNumberOfArenas)
{
    constexpr auto maxArenaCount = pmm::DefaultMemoryManagerConfig.safeMtArenaCount;
    for (size_t i{ 0 }; i < maxArenaCount; ++i)
    {
        const auto arena = manager.createSafeMTArena(1_KB);
        EXPECT_NE(nullptr, arena);
    }
}




/// +---------------------------------------------------------------------------------------+
/// |                                  STACK TESTS                                          |
/// +---------------------------------------------------------------------------------------+

// TODO: Add death tests for maximum memory usage
// TODO: Add death test for allocating more maximum possible stack.
TEST_F(InternalMemoryManagerTests, CreateStack_CreatesAStack)
{
    const auto stack = manager.createLocalStack(1_KB);
    EXPECT_NE(nullptr, stack);
}


TEST_F(InternalMemoryManagerTests, CreateStack_CanAllocateBuffer)
{
    const auto stack  = manager.createLocalStack(1_KB);
    const auto buffer = stack->malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(InternalMemoryManagerTests, CreateStack_CanCreateSpecifiedNumberOfStacks)
{
    constexpr auto maxStackCount = pmm::DefaultMemoryManagerConfig.localStackCount;
    for (size_t i{ 0 }; i < maxStackCount; ++i)
    {
        const auto stack = manager.createLocalStack(1_KB);
        EXPECT_NE(nullptr, stack);
    }
}


TEST_F(InternalMemoryManagerTests, CreateSafeLocalStack_CreatesAnStack)
{
    const auto stack = manager.createSafeLocalStack(1_KB);
    EXPECT_NE(nullptr, stack);
}


TEST_F(InternalMemoryManagerTests, CreateSafeLocalStack_CanAllocateBuffer)
{
    const auto stack  = manager.createSafeLocalStack(1_KB);
    const auto buffer = stack->malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(InternalMemoryManagerTests, CreateSafeLocalStack_CanCreateSpecifiedNumberOfStacks)
{
    constexpr auto maxStackCount = pmm::DefaultMemoryManagerConfig.safeLocalStackCount;
    for (size_t i{ 0 }; i < maxStackCount; ++i)
    {
        const auto stack = manager.createSafeLocalStack(1_KB);
        EXPECT_NE(nullptr, stack);
    }
}


TEST_F(InternalMemoryManagerTests, CreateMTStack_CreatesAnStack)
{
    const auto stack = manager.createMTStack(1_KB);
    EXPECT_NE(nullptr, stack);
}


TEST_F(InternalMemoryManagerTests, CreateMTStack_CanAllocateBuffer)
{
    const auto stack  = manager.createMTStack(1_KB);
    const auto buffer = stack->malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(InternalMemoryManagerTests, CreateMTStack_CanCreateSpecifiedNumberOfStacks)
{
    constexpr auto maxStackCount = pmm::DefaultMemoryManagerConfig.mtStackCount;
    for (size_t i{ 0 }; i < maxStackCount; ++i)
    {
        const auto stack = manager.createMTStack(1_KB);
        EXPECT_NE(nullptr, stack);
    }
}


TEST_F(InternalMemoryManagerTests, CreateSafeMTStack_CreatesAnStack)
{
    const auto stack = manager.createSafeMTStack(1_KB);
    EXPECT_NE(nullptr, stack);
}


TEST_F(InternalMemoryManagerTests, CreateSafeMTStack_CanAllocateBuffer)
{
    auto stack        = manager.createSafeMTStack(1_KB);
    const auto buffer = stack->malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(InternalMemoryManagerTests, CreateSafeMTStack_CanCreateSpecifiedNumberOfStacks)
{
    constexpr auto maxStackCount = pmm::DefaultMemoryManagerConfig.safeMtStackCount;
    for (size_t i{ 0 }; i < maxStackCount; ++i)
    {
        const auto stack = manager.createSafeMTStack(1_KB);
        EXPECT_NE(nullptr, stack);
    }
}
