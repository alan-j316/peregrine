/**
 * @file MultithreadingTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 12, 2026
 *
 * @brief Verifies TLSF Multithreading capabilities.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Utils.h"

#include <algorithm>
#include <gtest/gtest.h>
#include <peregrine/allocators/TLSF.h>
#include <peregrine/utils/Constants.h>
#include <random>
#include <thread>
#include <vector>


namespace
{
    using namespace pmm::constants;

    class InternallyManagedTLSF_MTTests: public testing::Test
    {
    public:
        static constexpr size_t SIZE = 1_MB;
        pmm::TLSF<pmm::MemPolicy::Internal, pmm::TelPolicy::Enabled, pmm::SafeModePolicy::Unsafe,
                  pmm::mt::MTPolicy::SpinLock>
            tlsf{ SIZE };
    };

    class ExternallyManagedTLSF_MTTests: public testing::Test
    {
    public:
        static constexpr size_t SIZE = 1_MB;
        uint8_t* mem                 = new uint8_t[SIZE];
        pmm::TLSF<pmm::MemPolicy::External, pmm::TelPolicy::Enabled, pmm::SafeModePolicy::Unsafe,
                  pmm::mt::MTPolicy::SpinLock>
            tlsf{ mem, SIZE };
        ~ExternallyManagedTLSF_MTTests() override { delete[] mem; }
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


/// +-----------------------------------+
///       INTERNALLY MANAGED TLSF
/// +-----------------------------------+

TEST_F(InternallyManagedTLSF_MTTests, Malloc_CanAllocateConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    std::vector<int> threadIds;
    std::atomic_int counter;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            static_cast<void>(tlsf.malloc(allocSize));
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

TEST_F(InternallyManagedTLSF_MTTests, Malloc_ConcurrentAllocationsDoNotCorruptInternalState)
{
    // To validate that concurrency doesn't corrupt internal state we create a random integer and
    // place it into the allocated region and assert the values are equal.
    // Since we cannot rely on indexing of any array, we can generate a random value and insert it into
    // an internal list along with allocated memory insertion.

    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    std::vector<int*> allocations;
    std::vector<int> ids;
    pmm::mt::SpinLock mutex;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            auto alloc = static_cast<int*>(tlsf.malloc(allocSize));

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

TEST_F(InternallyManagedTLSF_MTTests, Alloc_CanAllocateConcurrently)
{
    constexpr size_t numAllocations = 2700;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    std::vector<int> threadIds;
    std::atomic_int counter;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            static_cast<void>(tlsf.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f));
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

TEST_F(InternallyManagedTLSF_MTTests, Alloc_ConcurrentAllocationsDoNotCorruptInternalState)
{
    constexpr size_t numAllocations = 2700;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    std::vector<Vec4*> allocations;
    std::vector<Vec4> vecs;
    pmm::mt::SpinLock mutex;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            auto alloc = tlsf.alloc<Vec4>(0.0f, 0.0f, 0.0f, 0.0f);

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

TEST_F(InternallyManagedTLSF_MTTests, AllocV_CanAllocateConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t spanCount      = 16;
    using SpanType                  = int32_t;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    std::vector<int> threadIds;
    std::atomic_int counter;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            static_cast<void>(tlsf.allocV<SpanType>(spanCount));
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

TEST_F(InternallyManagedTLSF_MTTests, AllocV_ConcurrentAllocationsDoNotCorruptInternalState)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t spanCount      = 16;
    using SpanType                  = int32_t;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    std::vector<std::span<SpanType>> allocations;
    std::vector<std::array<SpanType, spanCount>> spans;
    pmm::mt::SpinLock mutex;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            auto alloc = tlsf.allocV<SpanType>(spanCount);

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

TEST_F(InternallyManagedTLSF_MTTests, MFree_ConcurrentFreesDoNotCorruptInternalState)
{
    // To verify that free doesn't corrupt the internal state we can concurrently free all allocations
    // and allocate a large block. A corruption of internal state will make it impossible to create a
    // near size allocation

    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    std::mutex mutex;

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(tlsf.malloc(allocSize));
    }


    const auto freeFn = [&] {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            // Pop off the final allocation
            // NOTE: Only apply mutex locks to the vector operations
            //       else the mfree itself won't be verifiable
            mutex.lock();
            const auto block = allocations.back();
            allocations.pop_back();
            mutex.unlock();

            // Free the block
            tlsf.mfree(block);
        }
    };

    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(freeFn);
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Make a near tlsf size allocation to ensure that all the blocks are freed
    const auto block = tlsf.malloc(SIZE - 4_KB);

    // Ensure that the expected number of allocations match the counter
    EXPECT_NE(nullptr, block);
}

TEST_F(InternallyManagedTLSF_MTTests, Free_ConcurrentFreesDoNotCorruptInternalState)
{
    // To verify that free doesn't corrupt the internal state we can concurrently free all allocations
    // and allocate a large block. A corruption of internal state will make it impossible to create a
    // near size allocation
    constexpr size_t numAllocations = 2700;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    std::vector<Vec4*> allocations;

    std::mutex mutex;

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(tlsf.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f));
    }


    const auto freeFn = [&] {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            // Pop off the final allocation
            // NOTE: Only apply mutex locks to the vector operations
            //       else the mfree itself won't be verifiable
            mutex.lock();
            const auto block = allocations.back();
            allocations.pop_back();
            mutex.unlock();

            // Free the block
            tlsf.free(block);
        }
    };

    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(freeFn);
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Make a near tlsf size allocation to ensure that all the blocks are freed
    const auto block = tlsf.malloc(SIZE - 4_KB);

    // Ensure that the expected number of allocations match the counter
    EXPECT_NE(nullptr, block);
}

TEST_F(InternallyManagedTLSF_MTTests, FreeV_ConcurrentFreesDoNotCorruptInternalState)
{
    // To verify that free doesn't corrupt the internal state we can concurrently free all allocations
    // and allocate a large block. A corruption of internal state will make it impossible to create a
    // near size allocation
    constexpr size_t numAllocations = 1420;
    constexpr size_t spanCount      = 16;
    using SpanType                  = int32_t;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    std::vector<std::span<SpanType>> allocations;
    std::vector<std::array<SpanType, spanCount>> spans;
    std::mutex mutex;

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(tlsf.allocV<SpanType>(spanCount));
    }

    const auto freeFn = [&] {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            // Pop off the final allocation
            // NOTE: Only apply mutex locks to the vector operations
            //       else the mfree itself won't be verifiable
            mutex.lock();
            const auto span = allocations.back();
            allocations.pop_back();
            mutex.unlock();

            // Free the block
            tlsf.freeV(span);
        }
    };

    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(freeFn);
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Make a near tlsf size allocation to ensure that all the blocks are freed
    const auto block = tlsf.malloc(SIZE - 4_KB);

    // Ensure that the expected number of allocations match the counter
    EXPECT_NE(nullptr, block);
}

TEST_F(InternallyManagedTLSF_MTTests, ResizeToSmallerSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t resizeSize     = 32;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(tlsf.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = tlsf.resize(allocations[j], allocSize, resizeSize);
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

TEST_F(InternallyManagedTLSF_MTTests, ResizeToSameSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(tlsf.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = tlsf.resize(allocations[j], allocSize, allocSize);
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

TEST_F(InternallyManagedTLSF_MTTests, ResizeToLargerSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 700;
    constexpr size_t allocSize      = 64;
    constexpr size_t resizeSize     = 128;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(tlsf.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = tlsf.resize(allocations[j], allocSize, resizeSize);
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


/// +-----------------------------------+
///       EXTERNALLY MANAGED TLSF
/// +-----------------------------------+

TEST_F(ExternallyManagedTLSF_MTTests, Malloc_CanAllocateConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    std::vector<int> threadIds;
    std::atomic_int counter;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            static_cast<void>(tlsf.malloc(allocSize));
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

TEST_F(ExternallyManagedTLSF_MTTests, Malloc_ConcurrentAllocationsDoNotCorruptInternalState)
{
    // To validate that concurrency doesn't corrupt internal state we create a random integer and
    // place it into the allocated region and assert the values are equal.
    // Since we cannot rely on indexing of any array, we can generate a random value and insert it into
    // an internal list along with allocated memory insertion.

    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    std::vector<int*> allocations;
    std::vector<int> ids;
    pmm::mt::SpinLock mutex;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            auto alloc = static_cast<int*>(tlsf.malloc(allocSize));

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

TEST_F(ExternallyManagedTLSF_MTTests, Alloc_CanAllocateConcurrently)
{
    constexpr size_t numAllocations = 2700;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    std::vector<int> threadIds;
    std::atomic_int counter;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            static_cast<void>(tlsf.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f));
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

TEST_F(ExternallyManagedTLSF_MTTests, Alloc_ConcurrentAllocationsDoNotCorruptInternalState)
{
    constexpr size_t numAllocations = 2700;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    std::vector<Vec4*> allocations;
    std::vector<Vec4> vecs;
    pmm::mt::SpinLock mutex;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            auto alloc = tlsf.alloc<Vec4>(0.0f, 0.0f, 0.0f, 0.0f);

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

TEST_F(ExternallyManagedTLSF_MTTests, AllocV_CanAllocateConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t spanCount      = 16;
    using SpanType                  = int32_t;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    std::vector<int> threadIds;
    std::atomic_int counter;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            static_cast<void>(tlsf.allocV<SpanType>(spanCount));
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

TEST_F(ExternallyManagedTLSF_MTTests, AllocV_ConcurrentAllocationsDoNotCorruptInternalState)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t spanCount      = 16;
    using SpanType                  = int32_t;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    std::vector<std::span<SpanType>> allocations;
    std::vector<std::array<SpanType, spanCount>> spans;
    pmm::mt::SpinLock mutex;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            auto alloc = tlsf.allocV<SpanType>(spanCount);

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

TEST_F(ExternallyManagedTLSF_MTTests, MFree_ConcurrentFreesDoNotCorruptInternalState)
{
    // To verify that free doesn't corrupt the internal state we can concurrently free all allocations
    // and allocate a large block. A corruption of internal state will make it impossible to create a
    // near size allocation

    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    std::mutex mutex;

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(tlsf.malloc(allocSize));
    }


    const auto freeFn = [&] {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            // Pop off the final allocation
            // NOTE: Only apply mutex locks to the vector operations
            //       else the mfree itself won't be verifiable
            mutex.lock();
            const auto block = allocations.back();
            allocations.pop_back();
            mutex.unlock();

            // Free the block
            tlsf.mfree(block);
        }
    };

    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(freeFn);
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Make a near tlsf size allocation to ensure that all the blocks are freed
    const auto block = tlsf.malloc(SIZE - 4_KB);

    // Ensure that the expected number of allocations match the counter
    EXPECT_NE(nullptr, block);
}

TEST_F(ExternallyManagedTLSF_MTTests, Free_ConcurrentFreesDoNotCorruptInternalState)
{
    // To verify that free doesn't corrupt the internal state we can concurrently free all allocations
    // and allocate a large block. A corruption of internal state will make it impossible to create a
    // near size allocation
    constexpr size_t numAllocations = 2700;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    std::vector<Vec4*> allocations;

    std::mutex mutex;

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(tlsf.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f));
    }


    const auto freeFn = [&] {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            // Pop off the final allocation
            // NOTE: Only apply mutex locks to the vector operations
            //       else the mfree itself won't be verifiable
            mutex.lock();
            const auto block = allocations.back();
            allocations.pop_back();
            mutex.unlock();

            // Free the block
            tlsf.free(block);
        }
    };

    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(freeFn);
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Make a near tlsf size allocation to ensure that all the blocks are freed
    const auto block = tlsf.malloc(SIZE - 4_KB);

    // Ensure that the expected number of allocations match the counter
    EXPECT_NE(nullptr, block);
}

TEST_F(ExternallyManagedTLSF_MTTests, FreeV_ConcurrentFreesDoNotCorruptInternalState)
{
    // To verify that free doesn't corrupt the internal state we can concurrently free all allocations
    // and allocate a large block. A corruption of internal state will make it impossible to create a
    // near size allocation
    constexpr size_t numAllocations = 1420;
    constexpr size_t spanCount      = 16;
    using SpanType                  = int32_t;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    std::vector<std::span<SpanType>> allocations;
    std::vector<std::array<SpanType, spanCount>> spans;
    std::mutex mutex;

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(tlsf.allocV<SpanType>(spanCount));
    }

    const auto freeFn = [&] {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            // Pop off the final allocation
            // NOTE: Only apply mutex locks to the vector operations
            //       else the mfree itself won't be verifiable
            mutex.lock();
            const auto span = allocations.back();
            allocations.pop_back();
            mutex.unlock();

            // Free the block
            tlsf.freeV(span);
        }
    };

    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(freeFn);
    }

    // Join all the threads
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Make a near tlsf size allocation to ensure that all the blocks are freed
    const auto block = tlsf.malloc(SIZE - 4_KB);

    // Ensure that the expected number of allocations match the counter
    EXPECT_NE(nullptr, block);
}

TEST_F(ExternallyManagedTLSF_MTTests, ResizeToSmallerSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t resizeSize     = 32;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(tlsf.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = tlsf.resize(allocations[j], allocSize, resizeSize);
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

TEST_F(ExternallyManagedTLSF_MTTests, ResizeToSameSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t allocSize      = 64;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(tlsf.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = tlsf.resize(allocations[j], allocSize, allocSize);
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

TEST_F(ExternallyManagedTLSF_MTTests, ResizeToLargerSize_CanRunConcurrently)
{
    constexpr size_t numAllocations = 700;
    constexpr size_t allocSize      = 64;
    constexpr size_t resizeSize     = 128;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(tlsf.malloc(allocSize));
    }


    // Dispatch the job
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, threadIdx = i]() {
            const size_t startIdx = threadIdx * numAllocations;
            const size_t endIdx   = startIdx + numAllocations;

            for (size_t j = startIdx; j < endIdx; ++j)
            {
                allocations[j] = tlsf.resize(allocations[j], allocSize, resizeSize);
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
