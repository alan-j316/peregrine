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


/**
 * @addtogroup PMM_Core
 * @{
 */

namespace pmm
{
    using RegistryBitset_t = uint64_t; ///< Type used for allocator registry's bitset.

    /**
     * @brief Fixed size container for storing objects with minimal fragmentation and O(1) lookups.
     *
     * @tparam Klass The stored element type.
     * @tparam Count The number of elements in the storage. Can be 0.
     */
    template <typename Klass, size_t Count>
    struct PoolRegistry
    {
        /**
         * @brief Reset the registry.
         */
        constexpr void reset() noexcept
        {
            // For a registry reset we can update the bitset to 0xFFFFFFFF
            // indicating all the "slots" are free.
            for (size_t i = 0; i < _BitsetArraySize; ++i)
            {
                _bitset[i] = static_cast<RegistryBitset_t>(~0ULL);
            }
        }

        PoolRegistry() { reset(); }
        /**
         * @brief Create and initialize an instance of @ref Klass type.
         *
         * @note If PMM_REGISTRY_UNSAFE is defined then release builds won't have
         *       empty checks and user will have to do the checks manually.
         * @note The call can trigger assertions if the registry is empty.
         *
         * @param args The arguments to the instance.
         *
         * @return A raw pointer to the class instance.
         */
        template <typename... Args>
        [[nodiscard]] constexpr Klass* create([[maybe_unused]] Args&&... args)
        {
#ifndef PMM_REGISTRY_UNSAFE
            // TODO: Only return a nullptr if the size is zero is no nullptr is
#endif
            return nullptr;
            // TODO:
        }


        /**
         * @brief Destroy the object allocate with @ref create and free the memory.
         * @note  The method will call the object's destructor.
         *
         * @param klass The class instance to free.
         */
        constexpr void free([[maybe_unused]] Klass* klass)
        {
            // TODO:
            // Use pointer offset to calculate the index
            // Turn off the index in the bitmask
        }


        /**
         * @brief Get the number of elements in the registry.
         */
        [[nodiscard]] constexpr size_t size() { return 0; } // TODO


        /**
         * @brief Get the total number of elements the container is able to hold.
         */
        [[nodiscard]] constexpr size_t capacity() { return 0; } // TODO


        /**
         * @brief Returns whether the Registry has no elements.
         */
        [[nodiscard]] constexpr bool empty()
        {
            bool isEmpty = false;
            PEREGRINE_LOOP_UNROLL()
            for (size_t i = 0; i < _BitsetArraySize; ++i)
            {
                isEmpty |= _bitset[i] == 0;
            }
            return isEmpty;
        }


    private:
        /**
         * @brief Returns the next free index.
         * @return
         */
        [[nodiscard]] constexpr size_t getNextIndex() const noexcept
        {
            constexpr size_t fullZero = sizeof(RegistryBitset_t) * 8; // 64 for a 64-bit integral indicating full zeros;
            size_t idx      = fullZero;

            // NOTE: Add idx as a iteration pass can create unpredictability and the compiler may not unroll
            //       the loop.
            PEREGRINE_LOOP_UNROLL()
            for (size_t i = 0; i < _BitsetArraySize; ++i)
            {
                idx = std::min(std::countr_zero(_bitset[i]), idx);
            }

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
