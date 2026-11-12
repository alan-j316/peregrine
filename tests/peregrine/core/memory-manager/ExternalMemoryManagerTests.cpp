/**
 * @file InternalMemoryManagerTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: November 12, 2026
 *
 * @brief Verify memory manager with external memory management policy.
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
    class ExternalMemoryManagerTests: public testing::Test
    {
    public:
        static constexpr size_t SIZE = 50_MB;
        uint8_t* managerBuffer       = new uint8_t[SIZE];
        pmm::MemoryManager<pmm::TelPolicy::Enabled, pmm::MemPolicy::External> manager{ managerBuffer, SIZE };

        ~ExternalMemoryManagerTests() override { delete[] managerBuffer; }
    };

} // namespace



/// +---------------------------------------------------------------------------------------+
/// |                               ARENA TESTS                                             |
/// +---------------------------------------------------------------------------------------+

TEST_F(ExternalMemoryManagerTests, CreateArena_CanAllocateBuffer)
{
    auto arena        = manager.createLocalArena(1_KB);
    const auto buffer = arena.malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(ExternalMemoryManagerTests, CreateSafeLocalArena_CanAllocateBuffer)
{
    auto arena        = manager.createSafeLocalArena(1_KB);
    const auto buffer = arena.malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(ExternalMemoryManagerTests, CreateMTArena_CanAllocateBuffer)
{
    auto arena        = manager.createMTArena(1_KB);
    const auto buffer = arena.malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(ExternalMemoryManagerTests, CreateSafeMTArena_CanAllocateBuffer)
{
    auto arena        = manager.createSafeMTArena(1_KB);
    const auto buffer = arena.malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(ExternalMemoryManagerTests, Free_FreesMemoryHeldByLocalArena)
{
    // For verifying free we allocate a large arena, free it
    // and try to allocate another arena that is near the memory buffer size.
    auto arena = manager.createLocalArena(40_MB);
    manager.free(arena);
    // Ensure that the allocation is successful. Set the value is returned by copy
    // we can try mallocing a small chunk and it shouldn't be a nulltpr.
    auto newArena  = manager.createLocalArena(45_MB);
    const auto ptr = newArena.malloc(5_MB);
    EXPECT_NE(nullptr, ptr);
}


TEST_F(ExternalMemoryManagerTests, Free_FreesMemoryHeldBySafeLocalArena)
{
    // For verifying free we allocate a large arena, free it
    // and try to allocate another arena that is near the memory buffer size.
    auto arena = manager.createSafeLocalArena(40_MB);
    manager.free(arena);
    // Ensure that the allocation is successful. Set the value is returned by copy
    // we can try mallocing a small chunk and it shouldn't be a nulltpr.
    auto newArena  = manager.createSafeLocalArena(45_MB);
    const auto ptr = newArena.malloc(5_MB);
    EXPECT_NE(nullptr, ptr);
}


TEST_F(ExternalMemoryManagerTests, Free_FreesMemoryHeldByMTArena)
{
    // For verifying free we allocate a large arena, free it
    // and try to allocate another arena that is near the memory buffer size.
    auto arena = manager.createMTArena(40_MB);
    manager.free(arena);
    // Ensure that the allocation is successful. Set the value is returned by copy
    // we can try mallocing a small chunk and it shouldn't be a nulltpr.
    auto newArena  = manager.createMTArena(45_MB);
    const auto ptr = newArena.malloc(5_MB);
    EXPECT_NE(nullptr, ptr);
}


TEST_F(ExternalMemoryManagerTests, Free_FreesMemoryHeldBySafeMTArena)
{
    // For verifying free we allocate a large arena, free it
    // and try to allocate another arena that is near the memory buffer size.
    auto arena = manager.createSafeMTArena(40_MB);
    manager.free(arena);
    // Ensure that the allocation is successful. Set the value is returned by copy
    // we can try mallocing a small chunk and it shouldn't be a nulltpr.
    auto newArena  = manager.createSafeMTArena(45_MB);
    const auto ptr = newArena.malloc(5_MB);
    EXPECT_NE(nullptr, ptr);
}


/// +---------------------------------------------------------------------------------------+
/// |                                  STACK TESTS                                          |
/// +---------------------------------------------------------------------------------------+

TEST_F(ExternalMemoryManagerTests, CreateStack_CanAllocateBuffer)
{
    auto stack        = manager.createLocalStack(1_KB);
    const auto buffer = stack.malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(ExternalMemoryManagerTests, CreateSafeLocalStack_CanAllocateBuffer)
{
    auto stack        = manager.createSafeLocalStack(1_KB);
    const auto buffer = stack.malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(ExternalMemoryManagerTests, CreateMTStack_CanAllocateBuffer)
{
    auto stack        = manager.createMTStack(1_KB);
    const auto buffer = stack.malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(ExternalMemoryManagerTests, CreateSafeMTStack_CanAllocateBuffer)
{
    auto stack        = manager.createSafeMTStack(1_KB);
    const auto buffer = stack.malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(ExternalMemoryManagerTests, Free_FreesMemoryHeldByLocalStack)
{
    // For verifying free we allocate a large stack, free it
    // and try to allocate another stack that is near the memory buffer size.
    auto stack = manager.createLocalStack(40_MB);
    manager.free(stack);
    // Ensure that the allocation is successful. Set the value is returned by copy
    // we can try mallocing a small chunk and it shouldn't be a nulltpr.
    auto newStack  = manager.createLocalStack(45_MB);
    const auto ptr = newStack.malloc(5_MB);
    EXPECT_NE(nullptr, ptr);
}


TEST_F(ExternalMemoryManagerTests, Free_FreesMemoryHeldBySafeLocalStack)
{
    // For verifying free we allocate a large stack, free it
    // and try to allocate another stack that is near the memory buffer size.
    auto stack = manager.createSafeLocalStack(40_MB);
    manager.free(stack);
    // Ensure that the allocation is successful. Set the value is returned by copy
    // we can try mallocing a small chunk and it shouldn't be a nulltpr.
    auto newStack  = manager.createSafeLocalStack(45_MB);
    const auto ptr = newStack.malloc(5_MB);
    EXPECT_NE(nullptr, ptr);
}


TEST_F(ExternalMemoryManagerTests, Free_FreesMemoryHeldByMTStack)
{
    // For verifying free we allocate a large stack, free it
    // and try to allocate another stack that is near the memory buffer size.
    auto stack = manager.createMTStack(40_MB);
    manager.free(stack);
    // Ensure that the allocation is successful. Set the value is returned by copy
    // we can try mallocing a small chunk and it shouldn't be a nulltpr.
    auto newStack  = manager.createMTStack(45_MB);
    const auto ptr = newStack.malloc(5_MB);
    EXPECT_NE(nullptr, ptr);
}


TEST_F(ExternalMemoryManagerTests, Free_FreesMemoryHeldBySafeMTStack)
{
    // For verifying free we allocate a large stack, free it
    // and try to allocate another stack that is near the memory buffer size.
    auto stack = manager.createSafeMTStack(40_MB);
    manager.free(stack);
    // Ensure that the allocation is successful. Set the value is returned by copy
    // we can try mallocing a small chunk and it shouldn't be a nulltpr.
    auto newStack  = manager.createSafeMTStack(45_MB);
    const auto ptr = newStack.malloc(5_MB);
    EXPECT_NE(nullptr, ptr);
}



/// +---------------------------------------------------------------------------------------+
/// |                                  POOL TESTS                                           |
/// +---------------------------------------------------------------------------------------+

TEST_F(ExternalMemoryManagerTests, CreateLocalPool_CanAllocateBuffer)
{
    auto pool         = manager.createLocalPool(100, sizeof(Vec4), alignof(Vec4));
    const auto buffer = pool.malloc();
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, sizeof(Vec4));
}


TEST_F(ExternalMemoryManagerTests, CreateLocalPool_CanAllocateMaximumNumberOfElements)
{
    constexpr auto maxAlloc = 100;
    auto pool               = manager.createLocalPool(maxAlloc, sizeof(Vec4), alignof(Vec4));
    for (size_t i = 0; i < maxAlloc; ++i)
    {
        const auto buffer = pool.malloc();
        EXPECT_NE(nullptr, buffer);
    }
}

TEST_F(ExternalMemoryManagerTests, CreateLocalPool_AllocationsAreAlignedToCustomAlignment)
{
    constexpr auto maxAlloc = 100;
    auto pool               = manager.createLocalPool(maxAlloc, sizeof(Vec4), alignof(Vec4));
    for (size_t i = 0; i < maxAlloc; ++i)
    {
        const auto buffer = static_cast<uint8_t*>(pool.malloc());
        EXPECT_EQ(0, reinterpret_cast<uintptr_t>(buffer) % alignof(Vec4));
    }
}


TEST_F(ExternalMemoryManagerTests, CreateSafeLocalPool_CanAllocateBuffer)
{
    auto pool         = manager.createSafeLocalPool(100, sizeof(Vec4), alignof(Vec4));
    const auto buffer = pool.malloc();
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, sizeof(Vec4));
}


TEST_F(ExternalMemoryManagerTests, CreateSafeLocalPool_CanAllocateMaximumNumberOfElements)
{
    constexpr auto maxAlloc = 100;
    auto pool               = manager.createSafeLocalPool(maxAlloc, sizeof(Vec4), alignof(Vec4));
    for (size_t i = 0; i < maxAlloc; ++i)
    {
        const auto buffer = pool.malloc();
        EXPECT_NE(nullptr, buffer);
    }
}


TEST_F(ExternalMemoryManagerTests, CreateSafeLocalPool_AllocationsAreAlignedToCustomAlignment)
{
    constexpr auto maxAlloc = 100;
    auto pool               = manager.createSafeLocalPool(maxAlloc, sizeof(Vec4), alignof(Vec4));
    for (size_t i = 0; i < maxAlloc; ++i)
    {
        const auto buffer = static_cast<uint8_t*>(pool.malloc());
        EXPECT_EQ(0, reinterpret_cast<uintptr_t>(buffer) % alignof(Vec4));
    }
}


TEST_F(ExternalMemoryManagerTests, CreateMTPool_CanAllocateBuffer)
{
    auto pool         = manager.createMTPool(100, sizeof(Vec4), alignof(Vec4));
    const auto buffer = pool.malloc();
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, sizeof(Vec4));
}


TEST_F(ExternalMemoryManagerTests, CreateMTPool_CanAllocateMaximumNumberOfElements)
{
    constexpr auto maxAlloc = 100;
    auto pool               = manager.createMTPool(maxAlloc, sizeof(Vec4), alignof(Vec4));
    for (size_t i = 0; i < maxAlloc; ++i)
    {
        const auto buffer = pool.malloc();
        EXPECT_NE(nullptr, buffer);
    }
}


TEST_F(ExternalMemoryManagerTests, CreateMTPool_AllocationsAreAlignedToCustomAlignment)
{
    constexpr auto maxAlloc = 100;
    auto pool               = manager.createMTPool(maxAlloc, sizeof(Vec4), alignof(Vec4));
    for (size_t i = 0; i < maxAlloc; ++i)
    {
        const auto buffer = static_cast<uint8_t*>(pool.malloc());
        EXPECT_EQ(0, reinterpret_cast<uintptr_t>(buffer) % alignof(Vec4));
    }
}


TEST_F(ExternalMemoryManagerTests, CreateSafeMTPool_CanAllocateBuffer)
{
    auto pool         = manager.createSafeMTPool(100, sizeof(Vec4), alignof(Vec4));
    const auto buffer = pool.malloc();
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, sizeof(Vec4));
}

TEST_F(ExternalMemoryManagerTests, CreateSafeMTPool_CanAllocateMaximumNumberOfElements)
{
    constexpr auto maxAlloc = 100;
    auto pool               = manager.createSafeMTPool(maxAlloc, sizeof(Vec4), alignof(Vec4));
    for (size_t i = 0; i < maxAlloc; ++i)
    {
        const auto buffer = pool.malloc();
        EXPECT_NE(nullptr, buffer);
    }
}


TEST_F(ExternalMemoryManagerTests, CreateSafeMTPool_AllocationsAreAlignedToCustomAlignment)
{
    constexpr auto maxAlloc = 100;
    auto pool               = manager.createSafeMTPool(maxAlloc, sizeof(Vec4), alignof(Vec4));
    for (size_t i = 0; i < maxAlloc; ++i)
    {
        const auto buffer = static_cast<uint8_t*>(pool.malloc());
        EXPECT_EQ(0, reinterpret_cast<uintptr_t>(buffer) % alignof(Vec4));
    }
}


TEST_F(ExternalMemoryManagerTests, Free_FreesMemoryHeldByLocalPool)
{
    // For verifying free we allocate a large pool, free it
    // and try to allocate another pool that is near the memory buffer size.
    auto pool = manager.createLocalPool(40, 1_MB);
    manager.free(pool);
    // Ensure that the allocation is successful. Set the value is returned by copy
    // we can try mallocing a small chunk and it shouldn't be a nulltpr.
    auto newPool   = manager.createLocalPool(45, 1_MB);
    const auto ptr = newPool.malloc();
    EXPECT_NE(nullptr, ptr);
}


TEST_F(ExternalMemoryManagerTests, Free_FreesMemoryHeldBySafeLocalPool)
{
    // For verifying free we allocate a large pool, free it
    // and try to allocate another pool that is near the memory buffer size.
    auto pool = manager.createSafeLocalPool(40, 1_MB);
    manager.free(pool);
    // Ensure that the allocation is successful. Set the value is returned by copy
    // we can try mallocing a small chunk and it shouldn't be a nulltpr.
    auto newPool   = manager.createSafeLocalPool(45, 1_MB);
    const auto ptr = newPool.malloc();
    EXPECT_NE(nullptr, ptr);
}


TEST_F(ExternalMemoryManagerTests, Free_FreesMemoryHeldByMTPool)
{
    // For verifying free we allocate a large pool, free it
    // and try to allocate another pool that is near the memory buffer size.
    auto pool = manager.createMTPool(40, 1_MB);
    manager.free(pool);
    // Ensure that the allocation is successful. Set the value is returned by copy
    // we can try mallocing a small chunk and it shouldn't be a nulltpr.
    auto newPool   = manager.createMTPool(45, 1_MB);
    const auto ptr = newPool.malloc();
    EXPECT_NE(nullptr, ptr);
}


TEST_F(ExternalMemoryManagerTests, Free_FreesMemoryHeldBySafeMTPool)
{
    // For verifying free we allocate a large pool, free it
    // and try to allocate another pool that is near the memory buffer size.
    auto pool = manager.createSafeMTPool(40, 1_MB);
    manager.free(pool);
    // Ensure that the allocation is successful. Set the value is returned by copy
    // we can try mallocing a small chunk and it shouldn't be a nulltpr.
    auto newPool   = manager.createSafeMTPool(45, 1_MB);
    const auto ptr = newPool.malloc();
    EXPECT_NE(nullptr, ptr);
}


/// +---------------------------------------------------------------------------------------+
/// |                                   TLSF TESTS                                          |
/// +---------------------------------------------------------------------------------------+

TEST_F(ExternalMemoryManagerTests, CreateTLSF_CanAllocateBuffer)
{
    auto tlsf         = manager.createLocalTLSF(1_KB);
    const auto buffer = tlsf.malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(ExternalMemoryManagerTests, CreateSafeLocalTLSF_CanAllocateBuffer)
{
    auto tlsf         = manager.createSafeLocalTLSF(1_KB);
    const auto buffer = tlsf.malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(ExternalMemoryManagerTests, CreateMTTLSF_CanAllocateBuffer)
{
    auto tlsf         = manager.createMTTLSF(1_KB);
    const auto buffer = tlsf.malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(ExternalMemoryManagerTests, CreateSafeMTTLSF_CanAllocateBuffer)
{
    auto tlsf         = manager.createSafeMTTLSF(1_KB);
    const auto buffer = tlsf.malloc(64);
    EXPECT_NE(nullptr, buffer);
    // Ensure the memory copy completes successfully without segfault.
    memset(buffer, 12, 64);
}


TEST_F(ExternalMemoryManagerTests, Free_FreesMemoryHeldByLocalTLSF)
{
    // For verifying free we allocate a large tlsf, free it
    // and try to allocate another tlsf that is near the memory buffer size.
    auto tlsf = manager.createLocalTLSF(40_MB);
    manager.free(tlsf);
    // Ensure that the allocation is successful. Set the value is returned by copy
    // we can try mallocing a small chunk and it shouldn't be a nulltpr.
    auto newTLSF   = manager.createLocalTLSF(45_MB);
    const auto ptr = newTLSF.malloc(5_MB);
    EXPECT_NE(nullptr, ptr);
}


TEST_F(ExternalMemoryManagerTests, Free_FreesMemoryHeldBySafeLocalTLSF)
{
    // For verifying free we allocate a large tlsf, free it
    // and try to allocate another tlsf that is near the memory buffer size.
    auto tlsf = manager.createSafeLocalTLSF(40_MB);
    manager.free(tlsf);
    // Ensure that the allocation is successful. Set the value is returned by copy
    // we can try mallocing a small chunk and it shouldn't be a nulltpr.
    auto newTLSF   = manager.createSafeLocalTLSF(45_MB);
    const auto ptr = newTLSF.malloc(5_MB);
    EXPECT_NE(nullptr, ptr);
}


TEST_F(ExternalMemoryManagerTests, Free_FreesMemoryHeldByMTTLSF)
{
    // For verifying free we allocate a large tlsf, free it
    // and try to allocate another tlsf that is near the memory buffer size.
    auto tlsf = manager.createMTTLSF(40_MB);
    manager.free(tlsf);
    // Ensure that the allocation is successful. Set the value is returned by copy
    // we can try mallocing a small chunk and it shouldn't be a nulltpr.
    auto newTLSF   = manager.createMTTLSF(45_MB);
    const auto ptr = newTLSF.malloc(5_MB);
    EXPECT_NE(nullptr, ptr);
}


TEST_F(ExternalMemoryManagerTests, Free_FreesMemoryHeldBySafeMTTLSF)
{
    // For verifying free we allocate a large tlsf, free it
    // and try to allocate another tlsf that is near the memory buffer size.
    auto tlsf = manager.createSafeMTTLSF(40_MB);
    manager.free(tlsf);
    // Ensure that the allocation is successful. Set the value is returned by copy
    // we can try mallocing a small chunk and it shouldn't be a nulltpr.
    auto newTLSF   = manager.createSafeMTTLSF(45_MB);
    const auto ptr = newTLSF.malloc(5_MB);
    EXPECT_NE(nullptr, ptr);
}


TEST_F(ExternalMemoryManagerTests, GetTelemetry_ReturnsATelemetryInstance)
{
    static_cast<void>(manager.createLocalArena(20_MB));
    const auto& tel = manager.getTelemetry();
    EXPECT_EQ(1, tel.getActiveAllocations());
    EXPECT_EQ(20_MB, tel.getCurrentPayloadUsage());
}
