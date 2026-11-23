#pragma once
/**
 * @file StackTelemetry.h
 * @author Alan Abraham P Kochumon
 * @date Created on: August 03, 2026
 *
 * @brief Define structures and methods for pool allocator's telemetry collection.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "peregrine/allocators/Policy.h"

#include <cstddef>


namespace pmm
{

    /**
     * @addtogroup PMM_Telemetry
     * @{
     */

    struct StackTelemetry
    {

        /**
         * @brief Initialize a Stack Telemetry instance with the current stack size.
         *
         * @param[in] size The size of the Stack.
         */
        [[nodiscard]] explicit constexpr StackTelemetry(size_t size) noexcept;


        /**
         * @brief Increment the current stack usage.
         *
         * @note The function expects the size of current allocation, not the size of the stack's buffer,
         *       or the current offset.
         *
         * @param[in] size    The size of the allocated memory (exclusive of padding) in bytes.
         * @param[in] padding The padding used the by the allocation in bytes.
         *
         * @relatedalso decStackUsage()
         */
        constexpr void incStackUsage(size_t size, size_t padding) noexcept;


        /**
         * @brief Decrement the current stack usage.
         *
         * @note The function expects the size of current allocation, not the size of the stack's buffer,
         *       or the current offset.
         * @note This will not reset the recorded peak or minimum usage.
         *
         * @param[in] size    The size of the freed memory (exclusive of padding) in bytes.
         * @param[in] padding The padding used the by the allocation in bytes.
         *
         * @relatedalso incStackUsage()
         */
        constexpr void decStackUsage(size_t size, size_t padding) noexcept;


        /**
         * @brief Update the current minimum memory usage(non-padded) with @p usage
         *        if it is lower than the recorded minimum.
         *
         * @param[in] usage The new memory allocation size.
         *
         * @relatedalso updatePeakPaddingUsage()
         * @relatedalso updateMinPaddingUsage()
         * @relatedalso updatePeakMemoryUsage()
         */
        constexpr void updateMinMemoryUsage(size_t usage) noexcept;


        /**
         * @brief Update the current peak memory usage(non-padded) with @p usage if it is higher than the recorded peak.
         *
         * @param[in] usage The new memory allocation size.
         *
         * @relatedalso updatePeakPaddingUsage()
         * @relatedalso updateMinPaddingUsage()
         * @relatedalso updateMinMemoryUsage()
         */
        constexpr void updatePeakMemoryUsage(size_t usage) noexcept;


        /**
         * @brief Update the current minimum padding usage with @p usage if it is lower than the recorded minimum.
         *
         * @param[in] usage The new padding.
         *
         * @relatedalso updatePeakPaddingUsage()
         * @relatedalso updatePeakMemoryUsage()
         * @relatedalso updateMinMemoryUsage()
         */
        constexpr void updateMinPaddingUsage(size_t usage) noexcept;


        /**
         * @brief Update the current peak usage with @p usage if it is higher than the recorded peak.
         *
         * @param[in] usage The new padding.
         *
         * @relatedalso updateMinPaddingUsage()
         * @relatedalso updatePeakMemoryUsage()
         * @relatedalso updateMinMemoryUsage()
         */
        constexpr void updatePeakPaddingUsage(size_t usage) noexcept;


        /**
         * @brief Reset the current memory usage (including padding), while preserving the peak and minimum usage.
         *
         * @relatedalso resetTelemetry()
         */
        constexpr void resetCurrentUsage() noexcept;


        /**
         * @brief Update the telemetry usage statistics to zero.
         *
         * @relatedalso resetCurrentUsage()
         */
        constexpr void resetTelemetry() noexcept;


        /**
         * @brief Get the size of the stack.
         */
        [[nodiscard]] constexpr size_t getStackSize() const noexcept;


        /**
         * @brief Get the current memory usage (excluding padding) of the stack.
         *
         * @relatedalso getMinPadding()
         * @relatedalso getPeakPadding()
         * @relatedalso getCurrentPadding()
         *
         * @relatedalso getMinMemoryUsage()
         * @relatedalso getPeakMemoryUsage()
         * @relatedalso getTotalUsage()
         */
        [[nodiscard]] constexpr size_t getCurrentMemoryUsage() const noexcept;


        /**
         * @brief Get the all-time minimum memory usage (excluding padding) of the stack.
         *
         * @relatedalso getMinPadding()
         * @relatedalso getPeakPadding()
         * @relatedalso getCurrentPadding()
         *
         * @relatedalso getCurrentMemoryUsage()
         * @relatedalso getPeakMemoryUsage()
         * @relatedalso getTotalUsage()
         */
        [[nodiscard]] constexpr size_t getMinMemoryUsage() const noexcept;


        /**
         * @brief Get the all-time peak memory usage (excluding padding) of the stack.
         *
         * @relatedalso getMinPadding()
         * @relatedalso getPeakPadding()
         * @relatedalso getCurrentPadding()
         *
         * @relatedalso getCurrentMemoryUsage()
         * @relatedalso getMinMemoryUsage()
         * @relatedalso getTotalUsage()
         */
        [[nodiscard]] constexpr size_t getPeakMemoryUsage() const noexcept;


        /**
         * @brief Get the padding used the by the stack across all allocations.
         *
         * @note The value is not preserved if the stack is reset.
         *
         * @relatedalso getMinPadding()
         * @relatedalso getPeakPadding()
         *
         * @relatedalso getCurrentMemoryUsage()
         * @relatedalso getPeakMemoryUsage()
         * @relatedalso getMinMemoryUsage()
         * @relatedalso getTotalUsage()
         */
        [[nodiscard]] constexpr size_t getCurrentPadding() const noexcept;


        /**
         * @brief Get the maximum padding used across all allocations.
         *
         * @relatedalso getCurrentPadding()
         * @relatedalso getMinPadding()
         */
        [[nodiscard]] constexpr size_t getPeakPadding() const noexcept;


        /**
         * @brief Get the maximum padding used across all allocations.
         *
         * @relatedalso getCurrentPadding()
         * @relatedalso getPeakPadding()
         */
        [[nodiscard]] constexpr size_t getMinPadding() const noexcept;


        /**
         * @brief Get the total buffer usage inclusive of padding.
         */
        [[nodiscard]] constexpr size_t getTotalUsage() const noexcept;

    private:
        size_t _currentUsage{}, _minMemoryUsage{}, _peakMemoryUsage{};
        size_t _totalPadding{}, _minPaddingUsage{}, _peakPaddingUsage{};
        size_t _size{};
    };


    /**
     * @brief Dummy telemetry used when telemetry policy is set to @p pmm::telemetry::Disabled.
     */
    struct DummyStackTelemetry
    {

        [[nodiscard]] explicit constexpr DummyStackTelemetry(size_t) noexcept {}
        // NOLINTBEGIN
        constexpr void incStackUsage(size_t, size_t) noexcept {}
        constexpr void decStackUsage(size_t, size_t) noexcept {}
        constexpr void updateMinMemoryUsage(size_t) noexcept {}
        constexpr void updatePeakMemoryUsage(size_t) noexcept {}
        constexpr void updateMinPaddingUsage(size_t) noexcept {}
        constexpr void updatePeakPaddingUsage(size_t) noexcept {}
        constexpr void resetCurrentUsage() noexcept {}
        constexpr void resetTelemetry() noexcept {}
        [[nodiscard]] constexpr size_t getStackSize() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getCurrentMemoryUsage() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getMinMemoryUsage() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getPeakMemoryUsage() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getCurrentPadding() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getPeakPadding() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getMinPadding() const noexcept { return 0; }
        [[nodiscard]] constexpr size_t getTotalUsage() const noexcept { return 0; }
        // NOLINTEND
    };

    /**
     * @brief Define the type of stack telemetry based on the current telemetry policy.
     *
     * @tparam Policy Telemetry Policy used by the stack.
     */
    template <TelPolicy Policy>
    using StackTelemetryType = std::conditional_t<Policy == TelPolicy::Disabled, DummyStackTelemetry, StackTelemetry>;



    /**
     * @brief Get a telemetry instance depending on the telemetry policy in use by the target stack.
     *
     * @tparam Policy The Telemetry policy in use by the target stack.
     *
     * @param stackSize The size of the stack
     *
     * @return A stack telemetry instance suited for the telemetry policy.
     */
    template <TelPolicy Policy>
    constexpr StackTelemetryType<Policy> getStackTelemetryInstance(const size_t stackSize) noexcept
    {
        if constexpr (Policy == TelPolicy::Disabled)
        {
            return DummyStackTelemetry(stackSize);
        }
        else
        {
            return StackTelemetry(stackSize);
        }
    }

    /** @} */

} // namespace pmm


#include "StackTelemetry.tpp"
