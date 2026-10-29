#pragma once
/**
 * @file Peregrine.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 12, 2026
 *
 * @brief Aggregate header for consuming interfaces.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "allocators/Arena.h"
#include "allocators/Policy.h"
#include "allocators/Pool.h"
#include "allocators/Stack.h"
#include "allocators/TLSF.h"
#include "allocators/TempArena.h"
#include "core/MemoryManager.h"
#include "memory/Memory.h"
#include "telemetry/ArenaTelemetry.h"
#include "telemetry/PoolTelemetry.h"
#include "telemetry/StackTelemetry.h"
#include "telemetry/TLSFTelemetry.h"
#include "utils/Bit.h"
#include "utils/Constants.h"
#include "utils/Helpers.h"
#include "utils/Multithreading.h"
