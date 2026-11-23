/**
 * @file MultithreadingTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: November 23, 2026
 *
 * @brief Verifies Stack Multithreading capabilities.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Utils.h"

#include <algorithm>
#include <array>
#include <gtest/gtest.h>
#include <peregrine/allocators/Stack.h>
#include <peregrine/utils/Constants.h>
#include <random>
#include <thread>
#include <vector>


/**
 * @addtogroup T_PMM_Stack_Core
 * @{
 */

namespace
{
    using namespace pmm::constants;

    class InternallyManagedStrictStack_MTTests: public testing::Test // NOLINT
    {
    public:
        static constexpr size_t SIZE = 2_MB;
        pmm::Stack<pmm::StackPolicy::Strict, pmm::MemPolicy::Internal, pmm::TelPolicy::Enabled,
                   pmm::SafeModePolicy::Unsafe, pmm::mt::MTPolicy::SpinLock>
            stack{ SIZE };
    };

    class ExternallyManagedStrictStack_MTTests: public testing::Test // NOLINT
    {
    public:
        static constexpr size_t SIZE = 2_MB;
        uint8_t* mem                 = new uint8_t[SIZE];
        pmm::Stack<pmm::StackPolicy::Strict, pmm::MemPolicy::External, pmm::TelPolicy::Enabled,
                   pmm::SafeModePolicy::Unsafe, pmm::mt::MTPolicy::SpinLock>
            stack{ mem, SIZE };
        ~ExternallyManagedStrictStack_MTTests() override { delete[] mem; }
    };


    class InternallyManagedLooseStack_MTTests: public testing::Test // NOLINT
    {
    public:
        static constexpr size_t SIZE = 2_MB;
        pmm::Stack<pmm::StackPolicy::Loose, pmm::MemPolicy::Internal, pmm::TelPolicy::Enabled,
                   pmm::SafeModePolicy::Unsafe, pmm::mt::MTPolicy::SpinLock>
            stack{ SIZE };
    };

    class ExternallyManagedLooseStack_MTTests: public testing::Test // NOLINT
    {
    public:
        static constexpr size_t SIZE = 2_MB;
        uint8_t* mem                 = new uint8_t[SIZE];
        pmm::Stack<pmm::StackPolicy::Loose, pmm::MemPolicy::External, pmm::TelPolicy::Enabled,
                   pmm::SafeModePolicy::Unsafe, pmm::mt::MTPolicy::SpinLock>
            stack{ mem, SIZE };
        ~ExternallyManagedLooseStack_MTTests() override { delete[] mem; }
    };


    // Random number generation helper
    int genRandom()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution dist;
        return dist(gen);
    }
} // namespace



/// +---------------------------------------------------------------------------------------+
/// |                                STRICT STACK                                           |
/// +---------------------------------------------------------------------------------------+

/// +-----------------------------------+
///       INTERNALLY MANAGED Stack
/// +-----------------------------------+

TEST_F(InternallyManagedStrictStack_MTTests, Malloc_CanAllocateConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    std::vector<int> threadIds;
    std::atomic_int counter;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            static_cast<void>(stack.malloc(allocSize));
            ++counter;
        }
    };

    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(allocFn);
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Ensure that the expected number of allocations match the counter
    EXPECT_EQ(numAllocations * numThreads, counter.load());
}


TEST_F(InternallyManagedStrictStack_MTTests, Malloc_ConcurrentAllocationsDoNotCorruptInternalState)
{
    // To validate that concurrency doesn't corrupt internal state we create a random integer and
    // place it into the allocated region and assert the values are equal.
    // Since we cannot rely on indexing of any array, we can generate a random value and insert it into
    // an internal list along with allocated memory insertion.

    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    std::vector<int*> allocations;
    std::vector<int> ids;
    pmm::mt::SpinLock mutex;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            auto alloc = static_cast<int*>(stack.malloc(allocSize));

            // Lock the critical region to ensure that vector inserts are uncorrupted.
            mutex.lock();
            const int id = genRandom();
            *alloc       = id;
            ids.push_back(id);
            allocations.push_back(alloc);
            mutex.unlock();
        }
    };

    // Dispatch
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(allocFn);
    }

    // Join
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Assert
    // The number of allocations and ids must match, else there is a sync issue in the test code.
    EXPECT_EQ(allocations.size(), ids.size()) << "Test variables out-of-sync!";
    for (size_t i = 0; i < allocations.size(); ++i)
    {
        // Ensure that the assigned id matches the id at the specific allocation
        EXPECT_EQ(ids[i], *allocations[i]);
    }
}


TEST_F(InternallyManagedStrictStack_MTTests, Alloc_CanAllocateConcurrently)
{
    constexpr size_t numAllocations = 2700;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    std::vector<int> threadIds;
    std::atomic_int counter;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            static_cast<void>(stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f));
            ++counter;
        }
    };

    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(allocFn);
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Ensure that the expected number of allocations match the counter
    EXPECT_EQ(numAllocations * numThreads, counter.load());
}


TEST_F(InternallyManagedStrictStack_MTTests, Alloc_ConcurrentAllocationsDoNotCorruptInternalState)
{
    constexpr size_t numAllocations = 2700;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    std::vector<Vec4*> allocations;
    std::vector<Vec4> vecs;
    pmm::mt::SpinLock mutex;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            auto alloc = stack.alloc<Vec4>(0.0f, 0.0f, 0.0f, 0.0f);

            // Lock the critical region to ensure that vector inserts are uncorrupted.
            mutex.lock();
            const Vec4 vec(genRandom(), genRandom(), genRandom(), genRandom());
            alloc->set(vec.x, vec.y, vec.z, vec.w);
            vecs.push_back(vec);
            allocations.push_back(alloc);
            mutex.unlock();
        }
    };

    // Dispatch
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(allocFn);
    }

    // Join
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Assert
    // The number of allocations and vecs must match, else there is a sync issue in the test code.
    EXPECT_EQ(allocations.size(), vecs.size()) << "Test variables out-of-sync!";
    for (size_t i = 0; i < allocations.size(); ++i)
    {
        // Ensure that the assigned id matches the id at the specific allocation
        EXPECT_FLOAT_EQ(vecs[i].x, allocations[i]->x);
        EXPECT_FLOAT_EQ(vecs[i].y, allocations[i]->y);
        EXPECT_FLOAT_EQ(vecs[i].z, allocations[i]->z);
        EXPECT_FLOAT_EQ(vecs[i].w, allocations[i]->w);
    }
}


TEST_F(InternallyManagedStrictStack_MTTests, AllocV_CanAllocateConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t spanCount      = 16;
    using SpanType                  = int32_t;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    std::vector<int> threadIds;
    std::atomic_int counter;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            static_cast<void>(stack.allocV<SpanType>(spanCount));
            ++counter;
        }
    };

    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(allocFn);
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Ensure that the expected number of allocations match the counter
    EXPECT_EQ(numAllocations * numThreads, counter.load());
}


TEST_F(InternallyManagedStrictStack_MTTests, AllocV_ConcurrentAllocationsDoNotCorruptInternalState)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t spanCount      = 16;
    using SpanType                  = int32_t;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    std::vector<std::span<SpanType>> allocations;
    std::vector<std::array<SpanType, spanCount>> spans;
    pmm::mt::SpinLock mutex;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            auto alloc = stack.allocV<SpanType>(spanCount);

            // Lock the critical region to ensure that vector inserts are uncorrupted.
            mutex.lock();
            // Insert elements to both the span used for tracking and the allocated span.
            std::array<SpanType, spanCount> span;
            for (size_t i = 0; i < spanCount; ++i)
            {
                const auto val = genRandom();
                alloc[i]       = val;
                span[i]        = val;
            }
            spans.emplace_back(span);
            allocations.emplace_back(alloc);
            mutex.unlock();
        }
    };

    // Dispatch
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(allocFn);
    }

    // Join
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Assert
    // The number of allocations and spans must match, else there is a sync issue in the test code.
    EXPECT_EQ(allocations.size(), spans.size()) << "Test variables out-of-sync!";
    for (size_t i = 0; i < allocations.size(); ++i)
    {
        EXPECT_EQ(spanCount, spans[i].size())
            << "Test variables out-of-sync. Expected: " << spanCount << " Got: " << spans[i].size() << '\n';
        EXPECT_EQ(spanCount, allocations[i].size())
            << "Test variables out-of-sync. Expected: " << spanCount << " Got: " << allocations[i].size() << '\n';

        EXPECT_TRUE(std::equal(spans[i].begin(), spans[i].end(), allocations[i].begin(), allocations[i].end()));
    }
}


TEST_F(InternallyManagedStrictStack_MTTests, Resize_ToSmallerSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t resizeSize     = 32;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(stack.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = stack.resize(allocations[j], allocSize, resizeSize);
            }
        });
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // To ensure all the resizes work, we can make sure that the resized pointer are non-null
    for (const auto allocation : allocations)
    {
        EXPECT_NE(nullptr, allocation);
    }
}

TEST_F(InternallyManagedStrictStack_MTTests, Resize_ToSameSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(stack.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = stack.resize(allocations[j], allocSize, allocSize);
            }
        });
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // To ensure all the resizes work, we can make sure that the resized pointer are non-null
    for (const auto allocation : allocations)
    {
        EXPECT_NE(nullptr, allocation);
    }
}


TEST_F(InternallyManagedStrictStack_MTTests, Resize_ToLargerSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 700;
    constexpr size_t allocSize      = 64;
    constexpr size_t resizeSize     = 128;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(stack.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = stack.resize(allocations[j], allocSize, resizeSize);
            }
        });
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // To ensure all the resizes work, we can make sure that the resized pointer are non-null
    for (const auto allocation : allocations)
    {
        EXPECT_NE(nullptr, allocation);
    }
}


TEST_F(InternallyManagedStrictStack_MTTests, ResizeFast_ToSmallerSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t resizeSize     = 32;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(stack.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = stack.resizeFast(allocations[j], allocSize, resizeSize);
            }
        });
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // To ensure all the resizes work, we can make sure that the resized pointer are non-null
    for (const auto allocation : allocations)
    {
        EXPECT_NE(nullptr, allocation);
    }
}


TEST_F(InternallyManagedStrictStack_MTTests, ResizeFast_ToSameSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(stack.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = stack.resizeFast(allocations[j], allocSize, allocSize);
            }
        });
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // To ensure all the resizes work, we can make sure that the resized pointer are non-null
    for (const auto allocation : allocations)
    {
        EXPECT_NE(nullptr, allocation);
    }
}


TEST_F(InternallyManagedStrictStack_MTTests, ResizeFast_ToLargerSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 700;
    constexpr size_t allocSize      = 64;
    constexpr size_t resizeSize     = 128;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(stack.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = stack.resizeFast(allocations[j], allocSize, resizeSize);
            }
        });
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // To ensure all the resizes work, we can make sure that the resized pointer are non-null
    for (const auto allocation : allocations)
    {
        EXPECT_NE(nullptr, allocation);
    }
}


TEST_F(InternallyManagedStrictStack_MTTests, MoveCtor_MovesTheAllocator)
{
    pmm::Stack<pmm::StackPolicy::Strict, pmm::MemPolicy::Internal, pmm::TelPolicy::Enabled, pmm::SafeModePolicy::Unsafe,
               pmm::mt::MTPolicy::SpinLock>
        stack2 = std::move(stack);
    // Without testing internal states, we can only allocate a block with the new allocator and check if its
    // non-null to ensure that the move worked.
    const auto allocation = stack2.malloc(950_KB);
    EXPECT_NE(nullptr, allocation);
}


TEST_F(InternallyManagedStrictStack_MTTests, MoveAssignment_MovesTheAllocator)
{
    constexpr size_t size2{ 1024 };
    pmm::Stack<pmm::StackPolicy::Strict, pmm::MemPolicy::Internal, pmm::TelPolicy::Enabled, pmm::SafeModePolicy::Unsafe,
               pmm::mt::MTPolicy::SpinLock>
        stack2{ size2 };

    stack2 = std::move(stack);
    // Without testing internal states, we can only allocate a block with the new allocator and check if its
    // non-null to ensure that the move worked.
    const auto allocation = stack2.malloc(950_KB);
    EXPECT_NE(nullptr, allocation);
}


/// +-----------------------------------+
///       EXTERNALLY MANAGED Stack
/// +-----------------------------------+

TEST_F(ExternallyManagedStrictStack_MTTests, Malloc_CanAllocateConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    std::vector<int> threadIds;
    std::atomic_int counter;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            static_cast<void>(stack.malloc(allocSize));
            ++counter;
        }
    };

    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(allocFn);
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Ensure that the expected number of allocations match the counter
    EXPECT_EQ(numAllocations * numThreads, counter.load());
}

TEST_F(ExternallyManagedStrictStack_MTTests, Malloc_ConcurrentAllocationsDoNotCorruptInternalState)
{
    // To validate that concurrency doesn't corrupt internal state we create a random integer and
    // place it into the allocated region and assert the values are equal.
    // Since we cannot rely on indexing of any array, we can generate a random value and insert it into
    // an internal list along with allocated memory insertion.

    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    std::vector<int*> allocations;
    std::vector<int> ids;
    pmm::mt::SpinLock mutex;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            auto alloc = static_cast<int*>(stack.malloc(allocSize));

            // Lock the critical region to ensure that vector inserts are uncorrupted.
            mutex.lock();
            const int id = genRandom();
            *alloc       = id;
            ids.push_back(id);
            allocations.push_back(alloc);
            mutex.unlock();
        }
    };

    // Dispatch
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(allocFn);
    }

    // Join
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Assert
    // The number of allocations and ids must match, else there is a sync issue in the test code.
    EXPECT_EQ(allocations.size(), ids.size()) << "Test variables out-of-sync!";
    for (size_t i = 0; i < allocations.size(); ++i)
    {
        // Ensure that the assigned id matches the id at the specific allocation
        EXPECT_EQ(ids[i], *allocations[i]);
    }
}

TEST_F(ExternallyManagedStrictStack_MTTests, Alloc_CanAllocateConcurrently)
{
    constexpr size_t numAllocations = 2700;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    std::vector<int> threadIds;
    std::atomic_int counter;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            static_cast<void>(stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f));
            ++counter;
        }
    };

    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(allocFn);
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Ensure that the expected number of allocations match the counter
    EXPECT_EQ(numAllocations * numThreads, counter.load());
}

TEST_F(ExternallyManagedStrictStack_MTTests, Alloc_ConcurrentAllocationsDoNotCorruptInternalState)
{
    constexpr size_t numAllocations = 2700;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    std::vector<Vec4*> allocations;
    std::vector<Vec4> vecs;
    pmm::mt::SpinLock mutex;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            auto alloc = stack.alloc<Vec4>(0.0f, 0.0f, 0.0f, 0.0f);

            // Lock the critical region to ensure that vector inserts are uncorrupted.
            mutex.lock();
            const Vec4 vec(genRandom(), genRandom(), genRandom(), genRandom());
            alloc->set(vec.x, vec.y, vec.z, vec.w);
            vecs.push_back(vec);
            allocations.push_back(alloc);
            mutex.unlock();
        }
    };

    // Dispatch
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(allocFn);
    }

    // Join
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Assert
    // The number of allocations and vecs must match, else there is a sync issue in the test code.
    EXPECT_EQ(allocations.size(), vecs.size()) << "Test variables out-of-sync!";
    for (size_t i = 0; i < allocations.size(); ++i)
    {
        // Ensure that the assigned id matches the id at the specific allocation
        EXPECT_FLOAT_EQ(vecs[i].x, allocations[i]->x);
        EXPECT_FLOAT_EQ(vecs[i].y, allocations[i]->y);
        EXPECT_FLOAT_EQ(vecs[i].z, allocations[i]->z);
        EXPECT_FLOAT_EQ(vecs[i].w, allocations[i]->w);
    }
}

TEST_F(ExternallyManagedStrictStack_MTTests, AllocV_CanAllocateConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t spanCount      = 16;
    using SpanType                  = int32_t;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    std::vector<int> threadIds;
    std::atomic_int counter;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            static_cast<void>(stack.allocV<SpanType>(spanCount));
            ++counter;
        }
    };

    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(allocFn);
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Ensure that the expected number of allocations match the counter
    EXPECT_EQ(numAllocations * numThreads, counter.load());
}

TEST_F(ExternallyManagedStrictStack_MTTests, AllocV_ConcurrentAllocationsDoNotCorruptInternalState)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t spanCount      = 16;
    using SpanType                  = int32_t;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    std::vector<std::span<SpanType>> allocations;
    std::vector<std::array<SpanType, spanCount>> spans;
    pmm::mt::SpinLock mutex;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            auto alloc = stack.allocV<SpanType>(spanCount);

            // Lock the critical region to ensure that vector inserts are uncorrupted.
            mutex.lock();
            // Insert elements to both the span used for tracking and the allocated span.
            std::array<SpanType, spanCount> span;
            for (size_t i = 0; i < spanCount; ++i)
            {
                const auto val = genRandom();
                alloc[i]       = val;
                span[i]        = val;
            }
            spans.emplace_back(span);
            allocations.emplace_back(alloc);
            mutex.unlock();
        }
    };

    // Dispatch
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(allocFn);
    }

    // Join
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Assert
    // The number of allocations and spans must match, else there is a sync issue in the test code.
    EXPECT_EQ(allocations.size(), spans.size()) << "Test variables out-of-sync!";
    for (size_t i = 0; i < allocations.size(); ++i)
    {
        EXPECT_EQ(spanCount, spans[i].size())
            << "Test variables out-of-sync. Expected: " << spanCount << " Got: " << spans[i].size() << '\n';
        EXPECT_EQ(spanCount, allocations[i].size())
            << "Test variables out-of-sync. Expected: " << spanCount << " Got: " << allocations[i].size() << '\n';

        EXPECT_TRUE(std::equal(spans[i].begin(), spans[i].end(), allocations[i].begin(), allocations[i].end()));
    }
}


TEST_F(ExternallyManagedStrictStack_MTTests, Resize_ToSmallerSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t resizeSize     = 32;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(stack.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = stack.resize(allocations[j], allocSize, resizeSize);
            }
        });
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // To ensure all the resizes work, we can make sure that the resized pointer are non-null
    for (const auto allocation : allocations)
    {
        EXPECT_NE(nullptr, allocation);
    }
}

TEST_F(ExternallyManagedStrictStack_MTTests, Resize_ToSameSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(stack.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = stack.resize(allocations[j], allocSize, allocSize);
            }
        });
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // To ensure all the resizes work, we can make sure that the resized pointer are non-null
    for (const auto allocation : allocations)
    {
        EXPECT_NE(nullptr, allocation);
    }
}

TEST_F(ExternallyManagedStrictStack_MTTests, Resize_ToLargerSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 700;
    constexpr size_t allocSize      = 64;
    constexpr size_t resizeSize     = 128;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(stack.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = stack.resize(allocations[j], allocSize, resizeSize);
            }
        });
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // To ensure all the resizes work, we can make sure that the resized pointer are non-null
    for (const auto allocation : allocations)
    {
        EXPECT_NE(nullptr, allocation);
    }
}


TEST_F(ExternallyManagedStrictStack_MTTests, ResizeFast_ToSmallerSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t resizeSize     = 32;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(stack.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = stack.resizeFast(allocations[j], allocSize, resizeSize);
            }
        });
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // To ensure all the resizes work, we can make sure that the resized pointer are non-null
    for (const auto allocation : allocations)
    {
        EXPECT_NE(nullptr, allocation);
    }
}


TEST_F(ExternallyManagedStrictStack_MTTests, ResizeFast_ToSameSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(stack.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = stack.resizeFast(allocations[j], allocSize, allocSize);
            }
        });
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // To ensure all the resizes work, we can make sure that the resized pointer are non-null
    for (const auto allocation : allocations)
    {
        EXPECT_NE(nullptr, allocation);
    }
}


TEST_F(ExternallyManagedStrictStack_MTTests, ResizeFast_ToLargerSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 700;
    constexpr size_t allocSize      = 64;
    constexpr size_t resizeSize     = 128;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(stack.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = stack.resizeFast(allocations[j], allocSize, resizeSize);
            }
        });
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // To ensure all the resizes work, we can make sure that the resized pointer are non-null
    for (const auto allocation : allocations)
    {
        EXPECT_NE(nullptr, allocation);
    }
}


TEST_F(ExternallyManagedStrictStack_MTTests, MoveCtor_MovesTheAllocator)
{
    pmm::Stack<pmm::StackPolicy::Strict, pmm::MemPolicy::External, pmm::TelPolicy::Enabled, pmm::SafeModePolicy::Unsafe,
               pmm::mt::MTPolicy::SpinLock>
        stack2 = std::move(stack);
    // Without testing internal states, we can only allocate a block with the new allocator and check if its
    // non-null to ensure that the move worked.
    const auto allocation = stack2.malloc(950_KB);
    EXPECT_NE(nullptr, allocation);
}


TEST_F(ExternallyManagedStrictStack_MTTests, MoveAssignment_MovesTheAllocator)
{
    constexpr size_t size2{ 1024 };
    const auto mem2 = new uint8_t[size2];
    pmm::Stack<pmm::StackPolicy::Strict, pmm::MemPolicy::External, pmm::TelPolicy::Enabled, pmm::SafeModePolicy::Unsafe,
               pmm::mt::MTPolicy::SpinLock>
        stack2{ mem2, size2 };

    stack2 = std::move(stack);
    // Without testing internal states, we can only allocate a block with the new allocator and check if its
    // non-null to ensure that the move worked.
    const auto allocation = stack2.malloc(950_KB);
    EXPECT_NE(nullptr, allocation);
    delete[] mem2;
}




/// +---------------------------------------------------------------------------------------+
/// |                                 LOOSE STACK                                           |
/// +---------------------------------------------------------------------------------------+

/// +-----------------------------------+
///       INTERNALLY MANAGED Stack
/// +-----------------------------------+

TEST_F(InternallyManagedLooseStack_MTTests, Malloc_CanAllocateConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    std::vector<int> threadIds;
    std::atomic_int counter;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            static_cast<void>(stack.malloc(allocSize));
            ++counter;
        }
    };

    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(allocFn);
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Ensure that the expected number of allocations match the counter
    EXPECT_EQ(numAllocations * numThreads, counter.load());
}


TEST_F(InternallyManagedLooseStack_MTTests, Malloc_ConcurrentAllocationsDoNotCorruptInternalState)
{
    // To validate that concurrency doesn't corrupt internal state we create a random integer and
    // place it into the allocated region and assert the values are equal.
    // Since we cannot rely on indexing of any array, we can generate a random value and insert it into
    // an internal list along with allocated memory insertion.

    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    std::vector<int*> allocations;
    std::vector<int> ids;
    pmm::mt::SpinLock mutex;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            auto alloc = static_cast<int*>(stack.malloc(allocSize));

            // Lock the critical region to ensure that vector inserts are uncorrupted.
            mutex.lock();
            const int id = genRandom();
            *alloc       = id;
            ids.push_back(id);
            allocations.push_back(alloc);
            mutex.unlock();
        }
    };

    // Dispatch
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(allocFn);
    }

    // Join
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Assert
    // The number of allocations and ids must match, else there is a sync issue in the test code.
    EXPECT_EQ(allocations.size(), ids.size()) << "Test variables out-of-sync!";
    for (size_t i = 0; i < allocations.size(); ++i)
    {
        // Ensure that the assigned id matches the id at the specific allocation
        EXPECT_EQ(ids[i], *allocations[i]);
    }
}


TEST_F(InternallyManagedLooseStack_MTTests, Alloc_CanAllocateConcurrently)
{
    constexpr size_t numAllocations = 2700;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    std::vector<int> threadIds;
    std::atomic_int counter;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            static_cast<void>(stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f));
            ++counter;
        }
    };

    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(allocFn);
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Ensure that the expected number of allocations match the counter
    EXPECT_EQ(numAllocations * numThreads, counter.load());
}


TEST_F(InternallyManagedLooseStack_MTTests, Alloc_ConcurrentAllocationsDoNotCorruptInternalState)
{
    constexpr size_t numAllocations = 2700;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    std::vector<Vec4*> allocations;
    std::vector<Vec4> vecs;
    pmm::mt::SpinLock mutex;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            auto alloc = stack.alloc<Vec4>(0.0f, 0.0f, 0.0f, 0.0f);

            // Lock the critical region to ensure that vector inserts are uncorrupted.
            mutex.lock();
            const Vec4 vec(genRandom(), genRandom(), genRandom(), genRandom());
            alloc->set(vec.x, vec.y, vec.z, vec.w);
            vecs.push_back(vec);
            allocations.push_back(alloc);
            mutex.unlock();
        }
    };

    // Dispatch
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(allocFn);
    }

    // Join
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Assert
    // The number of allocations and vecs must match, else there is a sync issue in the test code.
    EXPECT_EQ(allocations.size(), vecs.size()) << "Test variables out-of-sync!";
    for (size_t i = 0; i < allocations.size(); ++i)
    {
        // Ensure that the assigned id matches the id at the specific allocation
        EXPECT_FLOAT_EQ(vecs[i].x, allocations[i]->x);
        EXPECT_FLOAT_EQ(vecs[i].y, allocations[i]->y);
        EXPECT_FLOAT_EQ(vecs[i].z, allocations[i]->z);
        EXPECT_FLOAT_EQ(vecs[i].w, allocations[i]->w);
    }
}


TEST_F(InternallyManagedLooseStack_MTTests, AllocV_CanAllocateConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t spanCount      = 16;
    using SpanType                  = int32_t;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    std::vector<int> threadIds;
    std::atomic_int counter;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            static_cast<void>(stack.allocV<SpanType>(spanCount));
            ++counter;
        }
    };

    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(allocFn);
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Ensure that the expected number of allocations match the counter
    EXPECT_EQ(numAllocations * numThreads, counter.load());
}


TEST_F(InternallyManagedLooseStack_MTTests, AllocV_ConcurrentAllocationsDoNotCorruptInternalState)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t spanCount      = 16;
    using SpanType                  = int32_t;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    std::vector<std::span<SpanType>> allocations;
    std::vector<std::array<SpanType, spanCount>> spans;
    pmm::mt::SpinLock mutex;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            auto alloc = stack.allocV<SpanType>(spanCount);

            // Lock the critical region to ensure that vector inserts are uncorrupted.
            mutex.lock();
            // Insert elements to both the span used for tracking and the allocated span.
            std::array<SpanType, spanCount> span;
            for (size_t i = 0; i < spanCount; ++i)
            {
                const auto val = genRandom();
                alloc[i]       = val;
                span[i]        = val;
            }
            spans.emplace_back(span);
            allocations.emplace_back(alloc);
            mutex.unlock();
        }
    };

    // Dispatch
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(allocFn);
    }

    // Join
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Assert
    // The number of allocations and spans must match, else there is a sync issue in the test code.
    EXPECT_EQ(allocations.size(), spans.size()) << "Test variables out-of-sync!";
    for (size_t i = 0; i < allocations.size(); ++i)
    {
        EXPECT_EQ(spanCount, spans[i].size())
            << "Test variables out-of-sync. Expected: " << spanCount << " Got: " << spans[i].size() << '\n';
        EXPECT_EQ(spanCount, allocations[i].size())
            << "Test variables out-of-sync. Expected: " << spanCount << " Got: " << allocations[i].size() << '\n';

        EXPECT_TRUE(std::equal(spans[i].begin(), spans[i].end(), allocations[i].begin(), allocations[i].end()));
    }
}


TEST_F(InternallyManagedLooseStack_MTTests, Resize_ToSmallerSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t resizeSize     = 32;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(stack.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = stack.resize(allocations[j], allocSize, resizeSize);
            }
        });
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // To ensure all the resizes work, we can make sure that the resized pointer are non-null
    for (const auto allocation : allocations)
    {
        EXPECT_NE(nullptr, allocation);
    }
}

TEST_F(InternallyManagedLooseStack_MTTests, Resize_ToSameSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(stack.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = stack.resize(allocations[j], allocSize, allocSize);
            }
        });
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // To ensure all the resizes work, we can make sure that the resized pointer are non-null
    for (const auto allocation : allocations)
    {
        EXPECT_NE(nullptr, allocation);
    }
}


TEST_F(InternallyManagedLooseStack_MTTests, Resize_ToLargerSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 700;
    constexpr size_t allocSize      = 64;
    constexpr size_t resizeSize     = 128;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(stack.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = stack.resize(allocations[j], allocSize, resizeSize);
            }
        });
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // To ensure all the resizes work, we can make sure that the resized pointer are non-null
    for (const auto allocation : allocations)
    {
        EXPECT_NE(nullptr, allocation);
    }
}


TEST_F(InternallyManagedLooseStack_MTTests, ResizeFast_ToSmallerSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t resizeSize     = 32;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(stack.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = stack.resizeFast(allocations[j], allocSize, resizeSize);
            }
        });
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // To ensure all the resizes work, we can make sure that the resized pointer are non-null
    for (const auto allocation : allocations)
    {
        EXPECT_NE(nullptr, allocation);
    }
}


TEST_F(InternallyManagedLooseStack_MTTests, ResizeFast_ToSameSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(stack.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = stack.resizeFast(allocations[j], allocSize, allocSize);
            }
        });
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // To ensure all the resizes work, we can make sure that the resized pointer are non-null
    for (const auto allocation : allocations)
    {
        EXPECT_NE(nullptr, allocation);
    }
}


TEST_F(InternallyManagedLooseStack_MTTests, ResizeFast_ToLargerSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 700;
    constexpr size_t allocSize      = 64;
    constexpr size_t resizeSize     = 128;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(stack.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = stack.resizeFast(allocations[j], allocSize, resizeSize);
            }
        });
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // To ensure all the resizes work, we can make sure that the resized pointer are non-null
    for (const auto allocation : allocations)
    {
        EXPECT_NE(nullptr, allocation);
    }
}


TEST_F(InternallyManagedLooseStack_MTTests, MoveCtor_MovesTheAllocator)
{
    pmm::Stack<pmm::StackPolicy::Loose, pmm::MemPolicy::Internal, pmm::TelPolicy::Enabled, pmm::SafeModePolicy::Unsafe,
               pmm::mt::MTPolicy::SpinLock>
        stack2 = std::move(stack);
    // Without testing internal states, we can only allocate a block with the new allocator and check if its
    // non-null to ensure that the move worked.
    const auto allocation = stack2.malloc(950_KB);
    EXPECT_NE(nullptr, allocation);
}


TEST_F(InternallyManagedLooseStack_MTTests, MoveAssignment_MovesTheAllocator)
{
    constexpr size_t size2{ 1024 };
    pmm::Stack<pmm::StackPolicy::Loose, pmm::MemPolicy::Internal, pmm::TelPolicy::Enabled, pmm::SafeModePolicy::Unsafe,
               pmm::mt::MTPolicy::SpinLock>
        stack2{ size2 };

    stack2 = std::move(stack);
    // Without testing internal states, we can only allocate a block with the new allocator and check if its
    // non-null to ensure that the move worked.
    const auto allocation = stack2.malloc(950_KB);
    EXPECT_NE(nullptr, allocation);
}


/// +-----------------------------------+
///       EXTERNALLY MANAGED Stack
/// +-----------------------------------+

TEST_F(ExternallyManagedLooseStack_MTTests, Malloc_CanAllocateConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    std::vector<int> threadIds;
    std::atomic_int counter;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            static_cast<void>(stack.malloc(allocSize));
            ++counter;
        }
    };

    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(allocFn);
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Ensure that the expected number of allocations match the counter
    EXPECT_EQ(numAllocations * numThreads, counter.load());
}

TEST_F(ExternallyManagedLooseStack_MTTests, Malloc_ConcurrentAllocationsDoNotCorruptInternalState)
{
    // To validate that concurrency doesn't corrupt internal state we create a random integer and
    // place it into the allocated region and assert the values are equal.
    // Since we cannot rely on indexing of any array, we can generate a random value and insert it into
    // an internal list along with allocated memory insertion.

    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    std::vector<int*> allocations;
    std::vector<int> ids;
    pmm::mt::SpinLock mutex;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            auto alloc = static_cast<int*>(stack.malloc(allocSize));

            // Lock the critical region to ensure that vector inserts are uncorrupted.
            mutex.lock();
            const int id = genRandom();
            *alloc       = id;
            ids.push_back(id);
            allocations.push_back(alloc);
            mutex.unlock();
        }
    };

    // Dispatch
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(allocFn);
    }

    // Join
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Assert
    // The number of allocations and ids must match, else there is a sync issue in the test code.
    EXPECT_EQ(allocations.size(), ids.size()) << "Test variables out-of-sync!";
    for (size_t i = 0; i < allocations.size(); ++i)
    {
        // Ensure that the assigned id matches the id at the specific allocation
        EXPECT_EQ(ids[i], *allocations[i]);
    }
}

TEST_F(ExternallyManagedLooseStack_MTTests, Alloc_CanAllocateConcurrently)
{
    constexpr size_t numAllocations = 2700;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    std::vector<int> threadIds;
    std::atomic_int counter;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            static_cast<void>(stack.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f));
            ++counter;
        }
    };

    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(allocFn);
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Ensure that the expected number of allocations match the counter
    EXPECT_EQ(numAllocations * numThreads, counter.load());
}

TEST_F(ExternallyManagedLooseStack_MTTests, Alloc_ConcurrentAllocationsDoNotCorruptInternalState)
{
    constexpr size_t numAllocations = 2700;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    std::vector<Vec4*> allocations;
    std::vector<Vec4> vecs;
    pmm::mt::SpinLock mutex;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            auto alloc = stack.alloc<Vec4>(0.0f, 0.0f, 0.0f, 0.0f);

            // Lock the critical region to ensure that vector inserts are uncorrupted.
            mutex.lock();
            const Vec4 vec(genRandom(), genRandom(), genRandom(), genRandom());
            alloc->set(vec.x, vec.y, vec.z, vec.w);
            vecs.push_back(vec);
            allocations.push_back(alloc);
            mutex.unlock();
        }
    };

    // Dispatch
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(allocFn);
    }

    // Join
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Assert
    // The number of allocations and vecs must match, else there is a sync issue in the test code.
    EXPECT_EQ(allocations.size(), vecs.size()) << "Test variables out-of-sync!";
    for (size_t i = 0; i < allocations.size(); ++i)
    {
        // Ensure that the assigned id matches the id at the specific allocation
        EXPECT_FLOAT_EQ(vecs[i].x, allocations[i]->x);
        EXPECT_FLOAT_EQ(vecs[i].y, allocations[i]->y);
        EXPECT_FLOAT_EQ(vecs[i].z, allocations[i]->z);
        EXPECT_FLOAT_EQ(vecs[i].w, allocations[i]->w);
    }
}

TEST_F(ExternallyManagedLooseStack_MTTests, AllocV_CanAllocateConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t spanCount      = 16;
    using SpanType                  = int32_t;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    std::vector<int> threadIds;
    std::atomic_int counter;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            static_cast<void>(stack.allocV<SpanType>(spanCount));
            ++counter;
        }
    };

    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(allocFn);
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Ensure that the expected number of allocations match the counter
    EXPECT_EQ(numAllocations * numThreads, counter.load());
}

TEST_F(ExternallyManagedLooseStack_MTTests, AllocV_ConcurrentAllocationsDoNotCorruptInternalState)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t spanCount      = 16;
    using SpanType                  = int32_t;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    std::vector<std::span<SpanType>> allocations;
    std::vector<std::array<SpanType, spanCount>> spans;
    pmm::mt::SpinLock mutex;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            auto alloc = stack.allocV<SpanType>(spanCount);

            // Lock the critical region to ensure that vector inserts are uncorrupted.
            mutex.lock();
            // Insert elements to both the span used for tracking and the allocated span.
            std::array<SpanType, spanCount> span;
            for (size_t i = 0; i < spanCount; ++i)
            {
                const auto val = genRandom();
                alloc[i]       = val;
                span[i]        = val;
            }
            spans.emplace_back(span);
            allocations.emplace_back(alloc);
            mutex.unlock();
        }
    };

    // Dispatch
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(allocFn);
    }

    // Join
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Assert
    // The number of allocations and spans must match, else there is a sync issue in the test code.
    EXPECT_EQ(allocations.size(), spans.size()) << "Test variables out-of-sync!";
    for (size_t i = 0; i < allocations.size(); ++i)
    {
        EXPECT_EQ(spanCount, spans[i].size())
            << "Test variables out-of-sync. Expected: " << spanCount << " Got: " << spans[i].size() << '\n';
        EXPECT_EQ(spanCount, allocations[i].size())
            << "Test variables out-of-sync. Expected: " << spanCount << " Got: " << allocations[i].size() << '\n';

        EXPECT_TRUE(std::equal(spans[i].begin(), spans[i].end(), allocations[i].begin(), allocations[i].end()));
    }
}


TEST_F(ExternallyManagedLooseStack_MTTests, Resize_ToSmallerSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t resizeSize     = 32;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(stack.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = stack.resize(allocations[j], allocSize, resizeSize);
            }
        });
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // To ensure all the resizes work, we can make sure that the resized pointer are non-null
    for (const auto allocation : allocations)
    {
        EXPECT_NE(nullptr, allocation);
    }
}

TEST_F(ExternallyManagedLooseStack_MTTests, Resize_ToSameSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(stack.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = stack.resize(allocations[j], allocSize, allocSize);
            }
        });
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // To ensure all the resizes work, we can make sure that the resized pointer are non-null
    for (const auto allocation : allocations)
    {
        EXPECT_NE(nullptr, allocation);
    }
}

TEST_F(ExternallyManagedLooseStack_MTTests, Resize_ToLargerSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 700;
    constexpr size_t allocSize      = 64;
    constexpr size_t resizeSize     = 128;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(stack.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = stack.resize(allocations[j], allocSize, resizeSize);
            }
        });
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // To ensure all the resizes work, we can make sure that the resized pointer are non-null
    for (const auto allocation : allocations)
    {
        EXPECT_NE(nullptr, allocation);
    }
}


TEST_F(ExternallyManagedLooseStack_MTTests, ResizeFast_ToSmallerSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t resizeSize     = 32;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(stack.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = stack.resizeFast(allocations[j], allocSize, resizeSize);
            }
        });
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // To ensure all the resizes work, we can make sure that the resized pointer are non-null
    for (const auto allocation : allocations)
    {
        EXPECT_NE(nullptr, allocation);
    }
}


TEST_F(ExternallyManagedLooseStack_MTTests, ResizeFast_ToSameSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(stack.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = stack.resizeFast(allocations[j], allocSize, allocSize);
            }
        });
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // To ensure all the resizes work, we can make sure that the resized pointer are non-null
    for (const auto allocation : allocations)
    {
        EXPECT_NE(nullptr, allocation);
    }
}


TEST_F(ExternallyManagedLooseStack_MTTests, ResizeFast_ToLargerSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 700;
    constexpr size_t allocSize      = 64;
    constexpr size_t resizeSize     = 128;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(stack.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = stack.resizeFast(allocations[j], allocSize, resizeSize);
            }
        });
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // To ensure all the resizes work, we can make sure that the resized pointer are non-null
    for (const auto allocation : allocations)
    {
        EXPECT_NE(nullptr, allocation);
    }
}


TEST_F(ExternallyManagedLooseStack_MTTests, MoveCtor_MovesTheAllocator)
{
    pmm::Stack<pmm::StackPolicy::Loose, pmm::MemPolicy::External, pmm::TelPolicy::Enabled, pmm::SafeModePolicy::Unsafe,
               pmm::mt::MTPolicy::SpinLock>
        stack2 = std::move(stack);
    // Without testing internal states, we can only allocate a block with the new allocator and check if its
    // non-null to ensure that the move worked.
    const auto allocation = stack2.malloc(950_KB);
    EXPECT_NE(nullptr, allocation);
}


TEST_F(ExternallyManagedLooseStack_MTTests, MoveAssignment_MovesTheAllocator)
{
    constexpr size_t size2{ 1024 };
    const auto mem2 = new uint8_t[size2];
    pmm::Stack<pmm::StackPolicy::Loose, pmm::MemPolicy::External, pmm::TelPolicy::Enabled, pmm::SafeModePolicy::Unsafe,
               pmm::mt::MTPolicy::SpinLock>
        stack2{ mem2, size2 };

    stack2 = std::move(stack);
    // Without testing internal states, we can only allocate a block with the new allocator and check if its
    // non-null to ensure that the move worked.
    const auto allocation = stack2.malloc(950_KB);
    EXPECT_NE(nullptr, allocation);
    delete[] mem2;
}


/** @} */
