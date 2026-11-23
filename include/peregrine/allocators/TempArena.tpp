#pragma once
/**
 * @file TempArena.tpp
 * @author Alan Abraham P Kochumon
 * @date Created on: May 30, 2026
 *
 * @brief Implementation for Temporary Arena member functions defined in
 *        TempArena.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "peregrine/utils/Preprocessors.h"


namespace pmm
{
    /**************************************
     *          INITIALIZATIONS           *
     **************************************/

    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr TempArena<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::TempArena(
        Arena<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>* pArena) noexcept
        : targetArena(pArena), prevOffset(pArena->_prevOffset), currentOffset(pArena->_offset)
    {}


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE constexpr TempArena<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::~TempArena() noexcept
    {
        targetArena->_prevOffset = prevOffset;
        targetArena->_offset     = currentOffset;
    }


    /**************************************
     *            ALLOCATIONS             *
     **************************************/

    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    PMM_INLINE void* TempArena<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::malloc(
        const size_t bytes, const size_t alignment) const noexcept
    { return targetArena->malloc(bytes, alignment); }


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    template <typename T, typename... Args>
    PMM_INLINE constexpr T* TempArena<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::alloc(
        Args... args) noexcept
    { return targetArena->template alloc<T>(args...); }


    template <MemPolicy MemoryPolicy, TelPolicy TelemetryPolicy, SafeModePolicy SafeMode,
              mt::MTPolicy MultithreadingPolicy>
    template <typename T>
    PMM_INLINE constexpr std::span<T> TempArena<MemoryPolicy, TelemetryPolicy, SafeMode, MultithreadingPolicy>::allocV(
        const size_t count) noexcept
    { return targetArena->template allocV<T>(count); }


} // namespace pmm
