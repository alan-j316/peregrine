/**
 * @file PoolRegistry.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 31, 2026
 *
 * @brief Verifies internal functions of pool registry.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Utils.h"

#include <cstddef>
#include <gtest/gtest.h>
#include <peregrine/Peregrine.h>

namespace
{
    /// @brief Test fixture for @ref pmm::PoolRegistry.
    class PoolRegistryTests: public testing::Test
    {
    public:
        using T                              = Vec4;
        static constexpr size_t elementCount = 1024;
        pmm::PoolRegistry<Vec4, elementCount> vecPool{};
    };


    /// +---------------------------------------------------------------------------------------+
    /// |                                 STATIC TESTS                                          |
    /// +---------------------------------------------------------------------------------------+
    namespace static_tests
    {
        constexpr size_t V4Size     = sizeof(Vec4);
        constexpr size_t BitSetSize = sizeof(pmm::RegistryBitset_t);

        using namespace pmm;
        /// @test Verify that the pool registry only occupies the expected size.
        /// @note The test verifies that the internal data structure doesn't scale linearly with number of objects.
        // Zero sized arrays are not allowed so we have to have at least a 1 sized array, that combined
        // with the alignment of Vec4 will result in (24 bytes rounded to the next 16 byte boundary)
        static_assert(sizeof(PoolRegistry<Vec4, 0>) == 32);
        // NOTE: Since the Vec4 has an alignment of 16 bytes the registry will occupy 16 bytes even
        //       the real byte size is 24 (16 for Vec4 + 8 for Bitset)
        static_assert(sizeof(PoolRegistry<Vec4, 1>) == 32);
        static_assert(sizeof(PoolRegistry<Vec4, 3>) == 64);
        static_assert(sizeof(PoolRegistry<Vec4, 63>) == 63 * V4Size + 16);
        static_assert(sizeof(PoolRegistry<Vec4, 64>) == 64 * V4Size + 16);
        static_assert(sizeof(PoolRegistry<Vec4, 65>) == 65 * V4Size + 16);
        static_assert(sizeof(PoolRegistry<Vec4, 1023>) == 1023 * V4Size + 16 * sizeof(BitSetSize));
        static_assert(sizeof(PoolRegistry<Vec4, 1024>) == 1024 * V4Size + 16 * sizeof(BitSetSize));
        // The increase in bitfields is due to the alignment and not the space actually occupied by the Bitset.
        static_assert(sizeof(PoolRegistry<Vec4, 1025>) == 1025 * V4Size + 18 * sizeof(BitSetSize));
    } // namespace static_tests

} // namespace


// TODO: Add create and free empty tests as well



TEST_F(PoolRegistryTests, Create_GetsAValidObjectTillFullBlockIsExhausted)
{
    for (size_t i = 0; i < elementCount; ++i)
    {
        T* allocation = vecPool.create(1.0f, 2.0f, 3.0f, 4.0f);
        ASSERT_NE(nullptr, allocation);
    }
}


TEST_F(PoolRegistryTests, Create_InstantiatesObjectWithParameters)
{
    const T* vec = vecPool.create(1, 2, 3, 4);
    EXPECT_FLOAT_EQ(1.0f, vec->x);
    EXPECT_FLOAT_EQ(2.0f, vec->y);
    EXPECT_FLOAT_EQ(3.0f, vec->z);
    EXPECT_FLOAT_EQ(4.0f, vec->w);
}


TEST_F(PoolRegistryTests, Empty_ReturnsTrueForPoolWithoutAllocations) { ASSERT_TRUE(this->vecPool.empty()); }


TEST_F(PoolRegistryTests, Empty_ReturnsFalseAfterAllocation)
{
    [[maybe_unused]] const T* vec = vecPool.create(1, 2, 3, 4);
    EXPECT_FALSE(vecPool.empty());
}


TEST_F(PoolRegistryTests, Full_ReturnsFalseForPoolWithoutAllocations) { ASSERT_FALSE(this->vecPool.full()); }


TEST_F(PoolRegistryTests, Full_ReturnsFalseAfterSingleAllocation)
{
    [[maybe_unused]] const T* vec = vecPool.create(1, 2, 3, 4);
    EXPECT_FALSE(vecPool.full());
}


TEST_F(PoolRegistryTests, Full_ReturnsTrueAfterReachingMaxAllocations)
{
    for (size_t i = 0; i < elementCount; ++i)
    {
        static_cast<void>(vecPool.create(1.0f, 2.0f, 3.0f, 4.0f));
    }
    EXPECT_TRUE(vecPool.full());
}


TEST_F(PoolRegistryTests, Reset_ResetsThePool)
{
    // We can that reset is working by making a few allocations and then calling reset
    // and validating if full is false and empty is true.
    for (size_t i = 0; i < elementCount; ++i)
    {
        static_cast<void>(vecPool.create(1.0f, 2.0f, 3.0f, 4.0f));
    }

    // Reset the registry
    vecPool.reset();

    EXPECT_FALSE(vecPool.full());
    EXPECT_TRUE(vecPool.empty());
}


TEST_F(PoolRegistryTests, Capacity_ReturnsTheTotalCapacity) { EXPECT_EQ(elementCount, vecPool.capacity()); }
