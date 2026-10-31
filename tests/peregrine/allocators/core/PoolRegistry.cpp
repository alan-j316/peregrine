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

#include <peregrine/Peregrine.h>


namespace
{

    /// +---------------------------------------------------------------------------------------+
    /// |                                 STATIC TESTS                                          |
    /// +---------------------------------------------------------------------------------------+
    namespace static_tests
    {
        constexpr size_t V4Size     = sizeof(Vec4);
        constexpr size_t BitSetSize = sizeof(pmm::RegistryBitset_t);
        constexpr size_t ByteSize   = sizeof(uint8_t);

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
