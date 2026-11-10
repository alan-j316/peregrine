/**
 * @file MemoryManagerTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 29, 2026
 *
 * @brief Verify memory manager integration.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Utils.h"

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


/// +---------------------------------------------------------------------------------------+
/// |                                  POOL TESTS                                           |
/// +---------------------------------------------------------------------------------------+

// TODO: Add death tests for maximum memory usage

TEST_F(InternalMemoryManagerTests, CreateLocalPool_CanAllocateBuffer)
{
    auto pool         = manager.createLocalPool(100, sizeof(Vec4), alignof(Vec4));
    const auto buffer = pool.malloc();
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, sizeof(Vec4));
}


TEST_F(InternalMemoryManagerTests, CreateLocalPool_CanAllocateMaximumNumberOfElements)
{
    constexpr auto maxAlloc = 100;
    auto pool               = manager.createLocalPool(maxAlloc, sizeof(Vec4), alignof(Vec4));
    for (size_t i = 0; i < maxAlloc; ++i)
    {
        const auto buffer = pool.malloc();
        EXPECT_NE(nullptr, buffer);
    }
}

TEST_F(InternalMemoryManagerTests, CreateLocalPool_AllocationsAreAlignedToCustomAlignment)
{
    constexpr auto maxAlloc = 100;
    auto pool               = manager.createLocalPool(maxAlloc, sizeof(Vec4), alignof(Vec4));
    for (size_t i = 0; i < maxAlloc; ++i)
    {
        const auto buffer = static_cast<uint8_t*>(pool.malloc());
        EXPECT_EQ(0, reinterpret_cast<uintptr_t>(buffer) % alignof(Vec4));
    }
}


TEST_F(InternalMemoryManagerTests, CreateSafeLocalPool_CanAllocateBuffer)
{
    auto pool         = manager.createSafeLocalPool(100, sizeof(Vec4), alignof(Vec4));
    const auto buffer = pool.malloc();
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, sizeof(Vec4));
}


TEST_F(InternalMemoryManagerTests, CreateSafeLocalPool_CanAllocateMaximumNumberOfElements)
{
    constexpr auto maxAlloc = 100;
    auto pool               = manager.createSafeLocalPool(maxAlloc, sizeof(Vec4), alignof(Vec4));
    for (size_t i = 0; i < maxAlloc; ++i)
    {
        const auto buffer = pool.malloc();
        EXPECT_NE(nullptr, buffer);
    }
}


TEST_F(InternalMemoryManagerTests, CreateSafeLocalPool_AllocationsAreAlignedToCustomAlignment)
{
    constexpr auto maxAlloc = 100;
    auto pool               = manager.createSafeLocalPool(maxAlloc, sizeof(Vec4), alignof(Vec4));
    for (size_t i = 0; i < maxAlloc; ++i)
    {
        const auto buffer = static_cast<uint8_t*>(pool.malloc());
        EXPECT_EQ(0, reinterpret_cast<uintptr_t>(buffer) % alignof(Vec4));
    }
}


TEST_F(InternalMemoryManagerTests, CreateMTPool_CanAllocateBuffer)
{
    auto pool         = manager.createMTPool(100, sizeof(Vec4), alignof(Vec4));
    const auto buffer = pool.malloc();
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, sizeof(Vec4));
}


TEST_F(InternalMemoryManagerTests, CreateMTPool_CanAllocateMaximumNumberOfElements)
{
    constexpr auto maxAlloc = 100;
    auto pool               = manager.createMTPool(maxAlloc, sizeof(Vec4), alignof(Vec4));
    for (size_t i = 0; i < maxAlloc; ++i)
    {
        const auto buffer = pool.malloc();
        EXPECT_NE(nullptr, buffer);
    }
}


TEST_F(InternalMemoryManagerTests, CreateMTPool_AllocationsAreAlignedToCustomAlignment)
{
    constexpr auto maxAlloc = 100;
    auto pool               = manager.createMTPool(maxAlloc, sizeof(Vec4), alignof(Vec4));
    for (size_t i = 0; i < maxAlloc; ++i)
    {
        const auto buffer = static_cast<uint8_t*>(pool.malloc());
        EXPECT_EQ(0, reinterpret_cast<uintptr_t>(buffer) % alignof(Vec4));
    }
}


TEST_F(InternalMemoryManagerTests, CreateSafeMTPool_CanAllocateBuffer)
{
    auto pool         = manager.createSafeMTPool(100, sizeof(Vec4), alignof(Vec4));
    const auto buffer = pool.malloc();
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, sizeof(Vec4));
}

TEST_F(InternalMemoryManagerTests, CreateSafeMTPool_CanAllocateMaximumNumberOfElements)
{
    constexpr auto maxAlloc = 100;
    auto pool               = manager.createSafeMTPool(maxAlloc, sizeof(Vec4), alignof(Vec4));
    for (size_t i = 0; i < maxAlloc; ++i)
    {
        const auto buffer = pool.malloc();
        EXPECT_NE(nullptr, buffer);
    }
}


TEST_F(InternalMemoryManagerTests, CreateSafeMTPool_AllocationsAreAlignedToCustomAlignment)
{
    constexpr auto maxAlloc = 100;
    auto pool               = manager.createSafeMTPool(maxAlloc, sizeof(Vec4), alignof(Vec4));
    for (size_t i = 0; i < maxAlloc; ++i)
    {
        const auto buffer = static_cast<uint8_t*>(pool.malloc());
        EXPECT_EQ(0, reinterpret_cast<uintptr_t>(buffer) % alignof(Vec4));
    }
}



/// +---------------------------------------------------------------------------------------+
/// |                                   TLSF TESTS                                          |
/// +---------------------------------------------------------------------------------------+

// TODO: Add death tests for maximum memory usage

TEST_F(InternalMemoryManagerTests, CreateTLSF_CanAllocateBuffer)
{
    auto tlsf         = manager.createLocalTLSF(1_KB);
    const auto buffer = tlsf.malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(InternalMemoryManagerTests, CreateSafeLocalTLSF_CanAllocateBuffer)
{
    auto tlsf         = manager.createSafeLocalTLSF(1_KB);
    const auto buffer = tlsf.malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(InternalMemoryManagerTests, CreateMTTLSF_CanAllocateBuffer)
{
    auto tlsf         = manager.createMTTLSF(1_KB);
    const auto buffer = tlsf.malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(InternalMemoryManagerTests, CreateSafeMTTLSF_CanAllocateBuffer)
{
    auto tlsf         = manager.createSafeMTTLSF(1_KB);
    const auto buffer = tlsf.malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}
