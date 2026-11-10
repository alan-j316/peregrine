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

} // namespace



/// +---------------------------------------------------------------------------------------+
/// |                               ARENA TESTS                                             |
/// +---------------------------------------------------------------------------------------+

// TODO: Add death tests for maximum memory usage
// TODO: Add death test for allocating more maximum possible arena.

TEST_F(InternalMemoryManagerTests, CreateArena_CanAllocateBuffer)
{
    auto arena        = manager.createLocalArena(1_KB);
    const auto buffer = arena.malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(InternalMemoryManagerTests, CreateSafeLocalArena_CanAllocateBuffer)
{
    auto arena        = manager.createSafeLocalArena(1_KB);
    const auto buffer = arena.malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(InternalMemoryManagerTests, CreateMTArena_CanAllocateBuffer)
{
    auto arena        = manager.createMTArena(1_KB);
    const auto buffer = arena.malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(InternalMemoryManagerTests, CreateSafeMTArena_CanAllocateBuffer)
{
    auto arena        = manager.createSafeMTArena(1_KB);
    const auto buffer = arena.malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}



/// +---------------------------------------------------------------------------------------+
/// |                                  STACK TESTS                                          |
/// +---------------------------------------------------------------------------------------+

// TODO: Add death tests for maximum memory usage
// TODO: Add death test for allocating more maximum possible stack.

TEST_F(InternalMemoryManagerTests, CreateStack_CanAllocateBuffer)
{
    auto stack        = manager.createLocalStack(1_KB);
    const auto buffer = stack.malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(InternalMemoryManagerTests, CreateSafeLocalStack_CanAllocateBuffer)
{
    auto stack        = manager.createSafeLocalStack(1_KB);
    const auto buffer = stack.malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(InternalMemoryManagerTests, CreateMTStack_CanAllocateBuffer)
{
    auto stack        = manager.createMTStack(1_KB);
    const auto buffer = stack.malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(InternalMemoryManagerTests, CreateSafeMTStack_CanAllocateBuffer)
{
    auto stack        = manager.createSafeMTStack(1_KB);
    const auto buffer = stack.malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}
