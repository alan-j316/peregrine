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
        [[nodiscard]] constexpr Klass* create(Args&&... args)
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
        constexpr void free(Klass* klass)
        {
            // TODO:
            // Use pointer offset to calculate the index
            // Turn off the index in the bitmask
        }


        /**
         * @brief Get the number of elements in the registry.
         */
        [[nodiscard]] constexpr size_t size() { return 0; }


        /**
         * @brief Get the total number of elements the container is able to hold.
         */
        [[nodiscard]] constexpr size_t capacity() { return 0; }


        /**
         * @brief Returns whether the Registry has no elements.
         */
        [[nodiscard]] constexpr bool empty()
        {
            PEREGRINE_LOOP_UNROLL()
            // bool isEmpty = false;
            //     for (size_t i = 0; i < _BitsetArraySize; ++i)
            //     {
            //         retur
            //     }
            return false;
        }


    private:
        /**
         * @brief Returns the next free index.
         * @return
         */
        [[nodiscard]] constexpr size_t getNextIndex() const noexcept
        {
            return 0; // TODO:
        }

    private:
        static constexpr size_t _Bit = sizeof(RegistryBitset_t) * 8;
        /// Array size for storing the bitsets.
        static constexpr size_t _BitsetArraySize = (Count + _Bit - 1) / _Bit;

        alignas(Klass) uint8_t _data[sizeof(Klass) * Count]{}; /// Storage
        RegistryBitset_t _bitset[_BitsetArraySize]{};          /// Bitset
    };
} // namespace pmm

/** @} */
