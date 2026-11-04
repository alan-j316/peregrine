#pragma once
/**
 * @file PoolRegistry.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 31, 2026
 *
 * @brief Fixed size container for storing objects with minimal fragmentation and O(1) lookups.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include <cstdint>
#include <new>


/**
 * @addtogroup PMM_Core
 * @{
 */

namespace pmm
{
    using RegistryBitset_t = uint64_t; ///< Type used for allocator registry's bitset.

    /**
     * @brief Fixed size container for storing objects with minimal fragmentation and near O(1) lookups.
     *
     * @tparam Klass The stored element type.
     * @tparam Count The number of elements in the storage.
     *               Can be 0 but will have a memory footprint.
     *
     * @note The structure must be externally synchronized.
     */
    template <typename Klass, size_t Count>
    struct PoolRegistry
    {
        /**
         * @brief Reset the registry and clear out all data.
         */
        constexpr void reset() noexcept
        {
            // For a registry reset we can update the bitset to 0xFFFFFFFF
            // indicating all the "slots" are free.
            for (size_t i = 0; i < _BitsetArraySize; ++i)
            {
                _bitset[i] = static_cast<RegistryBitset_t>(~0ULL);
                std::memset(_data, 0, sizeof(Klass) * Count);
            }
        }

        PoolRegistry() noexcept { reset(); }

        /**
         * @brief Create and initialize an instance of @ref Klass type.
         *
         * @note If PMM_REGISTRY_UNSAFE is defined then release builds won't have
         *       empty checks and user will have to do the checks manually.
         * @note The call can trigger assertions if the registry is full.
         *
         * @param args The arguments to the instance.
         *
         * @return A raw pointer to the class instance.
         */
        template <typename... Args>
        [[nodiscard]] constexpr Klass* create(Args&&... args) noexcept
        {
            PMM_ASSERT_MSG(!full(), "[Pool Registry]: No free space left in pool for allocations");
#ifndef PMM_REGISTRY_UNSAFE
            // Only return a nullptr if the size is zero is no nullptr is
            if (full())
            {
                return nullptr;
            }
#endif
            // Get the index of the next free block
            const auto blockIdx = getNextIndex();
            // Map pure block index to a proper memory location index.
            const auto trueIdx = blockIdx * sizeof(Klass);
            // Example:
            // For a Registry of 8 blocks we'll have indices from 0 to 7.
            // Assume Klass is 4 bytes; then, our stride is 4.
            // Therefore, the indices into the block will be 0, 4, 8, and so on or blockIndex * sizeof(Klass).
            return new (&_data[trueIdx]) Klass(std::forward<Args>(args)...);
        }


        /**
         * @brief Destroy the object allocate with @ref create and free the memory.
         * @note  The method will call the object's destructor.
         *
         * @param klass The class instance to free.
         */
        constexpr void free([[maybe_unused]] Klass* klass) noexcept
        {
            // TODO:
            // Use pointer offset to calculate the index
            // Turn off the index in the bitmask
        }


        /**
         * @brief Get the number of elements in the registry.
         */
        [[nodiscard]] constexpr size_t size() const noexcept { return 0; } // TODO


        /**
         * @brief Get the total number of elements the container is able to hold.
         */
        [[nodiscard]] constexpr size_t capacity() const noexcept { return Count; }


        /**
         * @brief Returns whether the Registry has no elements.
         */
        [[nodiscard]] constexpr bool empty() const noexcept
        {
            bool isEmpty = true;
            PEREGRINE_LOOP_UNROLL()
            for (size_t i = 0; i < _BitsetArraySize; ++i)
            {
                isEmpty = isEmpty && _bitset[i] == static_cast<RegistryBitset_t>(~0ULL);
            }
            return isEmpty;
        }


        /**
         * @brief Returns whether the registry has no storage left.
         */
        [[nodiscard]] constexpr bool full() const noexcept
        {
            bool isFull = true;
            PEREGRINE_LOOP_UNROLL()
            for (size_t i = 0; i < _BitsetArraySize; ++i)
            {
                isFull = isFull && _bitset[i] == 0;
            }
            return isFull;
        }


    private:
        /**
         * @brief Returns the next free index and mark it as used.
         * @return The next free block index; not the pure offset for memory location.
         */
        [[nodiscard]] constexpr size_t getNextIndex() noexcept
        {
            constexpr size_t fullZero = sizeof(RegistryBitset_t) * 8; // 64 for a 64-bit integral indicating full zeros;
            size_t idx                = fullZero;

            // NOTE: Add idx as a iteration pass can create unpredictability and the compiler may not unroll
            //       the loop.
            // NOTE: Bitset's LSB is the 0th bit of the value at zeroth index.
            size_t i = 0;
            PEREGRINE_LOOP_UNROLL()
            for (; i < _BitsetArraySize && idx == fullZero; ++i)
            {
                // Get the minimum index from the bit set block where there is a 1(free slot)
                idx = std::min<size_t>(std::countl_zero(_bitset[i]), idx);
                // Invert the mask to mark the slot as used
                _bitset[i] &= ~(0x8000000000000000ULL >> idx);
            }

            // Offset the index by the block size.
            idx += sizeof(RegistryBitset_t) * 8 * (i - 1);

            return idx;
        }

    private:
        static constexpr size_t _Bit = sizeof(RegistryBitset_t) * 8;
        /// Count clamped to a minimum of 1 to prevent zero-sized array in struct/union error.
        static constexpr size_t MinCount = (Count == 0) ? 1 : Count;
        /// Array size for storing the bitsets.
        static constexpr size_t _BitsetArraySize = (MinCount + _Bit - 1) / _Bit;

        alignas(Klass) uint8_t _data[sizeof(Klass) * MinCount]{}; /// Storage
        RegistryBitset_t _bitset[_BitsetArraySize]{};             /// Bitset
    };
} // namespace pmm

/** @} */
