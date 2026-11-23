/**
 * @file MultithreadingTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: November 23, 2026
 *
 * @brief Verifies Pool Multithreading capabilities.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Utils.h"

#include <algorithm>
#include <gtest/gtest.h>
#include <peregrine/allocators/Pool.h>
#include <peregrine/utils/Constants.h>
#include <random>
#include <thread>
#include <vector>


/**
 * @addtogroup T_PMM_Pool
 * @{
 */

namespace
{
    using namespace pmm::constants;

    class InternallyManagedPool_MTTests: public testing::Test // NOLINT
    {
    public:
        static constexpr size_t SIZE       = 12_MB;
        static constexpr size_t CHUNK_SIZE = 1_KB;
        pmm::Pool<pmm::MemPolicy::Internal, pmm::TelPolicy::Enabled, pmm::SafeModePolicy::Unsafe,
                  pmm::mt::MTPolicy::SpinLock>
            pool{ SIZE, CHUNK_SIZE, 1 };
    };

    class ExternallyManagedPool_MTTests: public testing::Test // NOLINT
    {
    public:
        static constexpr size_t SIZE       = 12_MB;
        static constexpr size_t CHUNK_SIZE = 1_KB;
        uint8_t* mem                       = new uint8_t[SIZE];
        pmm::Pool<pmm::MemPolicy::External, pmm::TelPolicy::Enabled, pmm::SafeModePolicy::Unsafe,
                  pmm::mt::MTPolicy::SpinLock>
            pool{ mem, SIZE, CHUNK_SIZE, 1 };
        ~ExternallyManagedPool_MTTests() override { delete[] mem; }
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
///       INTERNALLY MANAGED Pool
/// +-----------------------------------+

TEST_F(InternallyManagedPool_MTTests, Malloc_CanAllocateConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    std::vector<int> threadIds;
    std::atomic_int counter;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            static_cast<void>(pool.malloc());
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


TEST_F(InternallyManagedPool_MTTests, Malloc_ConcurrentAllocationsDoNotCorruptInternalState)
{
    // To validate that concurrency doesn't corrupt internal state we create a random integer and
    // place it into the allocated region and assert the values are equal.
    // Since we cannot rely on indexing of any array, we can generate a random value and insert it into
    // an internal list along with allocated memory insertion.
    constexpr size_t numAllocations = 1420;
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
            auto alloc = static_cast<int*>(pool.malloc());

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


TEST_F(InternallyManagedPool_MTTests, Alloc_CanAllocateConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    std::vector<int> threadIds;
    std::atomic_int counter;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            // Inefficient use of storage as our pool chunk is 1KB.
            // But it can work for tests as the size of Vec4 is not used internally.
            static_cast<void>(pool.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f));
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


TEST_F(InternallyManagedPool_MTTests, Alloc_ConcurrentAllocationsDoNotCorruptInternalState)
{
    constexpr size_t numAllocations = 1420;
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
            // Inefficient use of storage as our pool chunk is 1KB.
            // But it can work for tests as the size of Vec4 is not used internally.
            auto alloc = pool.alloc<Vec4>(0.0f, 0.0f, 0.0f, 0.0f);

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


TEST_F(InternallyManagedPool_MTTests, MFree_ConcurrentFreesDoNotCorruptInternalState)
{
    // To verify that free doesn't corrupt the internal state we can concurrently free all allocations
    // and allocate a large block. A corruption of internal state will make it impossible to create a
    // near size allocation
    constexpr size_t numAllocations = 1420;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    std::mutex mutex;

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(pool.malloc());
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
            pool.mfree(block);
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

    // Make a near pool size allocation to ensure that all the blocks are freed
    const auto block = pool.malloc();

    // Ensure that the expected number of allocations match the counter
    EXPECT_NE(nullptr, block);
}

TEST_F(InternallyManagedPool_MTTests, Free_ConcurrentFreesDoNotCorruptInternalState)
{
    // To verify that free doesn't corrupt the internal state we can concurrently free all allocations
    // and allocate a large block. A corruption of internal state will make it impossible to create a
    // near size allocation
    constexpr size_t numAllocations = 1420;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    std::vector<Vec4*> allocations;

    std::mutex mutex;

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        // Inefficient use of storage as our pool chunk is 1KB.
        // But it can work for tests as the size of Vec4 is not used internally.
        allocations.push_back(pool.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f));
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
            pool.free(block);
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

    // Make a near pool size allocation to ensure that all the blocks are freed
    const auto block = pool.malloc();

    // Ensure that the expected number of allocations match the counter
    EXPECT_NE(nullptr, block);
}


TEST_F(InternallyManagedPool_MTTests, MoveCtor_MovesTheAllocator)
{
    pmm::Pool<pmm::MemPolicy::Internal, pmm::TelPolicy::Enabled, pmm::SafeModePolicy::Unsafe,
              pmm::mt::MTPolicy::SpinLock>
        pool2 = std::move(pool);
    // Without testing internal states, we can only allocate a block with the new allocator and check if its
    // non-null to ensure that the move worked.
    const auto allocation = pool2.malloc();
    EXPECT_NE(nullptr, allocation);
}


TEST_F(InternallyManagedPool_MTTests, MoveAssignment_MovesTheAllocator)
{
    constexpr size_t size2{ 1024 };
    constexpr size_t chunkSize2{ 128 };
    pmm::Pool<pmm::MemPolicy::Internal, pmm::TelPolicy::Enabled, pmm::SafeModePolicy::Unsafe,
              pmm::mt::MTPolicy::SpinLock>
        pool2{ size2, chunkSize2, 1 };

    pool2 = std::move(pool);
    // Without testing internal states, we can only allocate a block with the new allocator and check if its
    // non-null to ensure that the move worked.
    const auto allocation = pool2.malloc();
    EXPECT_NE(nullptr, allocation);
}


/// +-----------------------------------+
///       EXTERNALLY MANAGED Pool
/// +-----------------------------------+

TEST_F(ExternallyManagedPool_MTTests, Malloc_CanAllocateConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    std::vector<int> threadIds;
    std::atomic_int counter;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            static_cast<void>(pool.malloc());
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


TEST_F(ExternallyManagedPool_MTTests, Malloc_ConcurrentAllocationsDoNotCorruptInternalState)
{
    // To validate that concurrency doesn't corrupt internal state we create a random integer and
    // place it into the allocated region and assert the values are equal.
    // Since we cannot rely on indexing of any array, we can generate a random value and insert it into
    // an internal list along with allocated memory insertion.
    constexpr size_t numAllocations = 1420;
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
            auto alloc = static_cast<int*>(pool.malloc());

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


TEST_F(ExternallyManagedPool_MTTests, Alloc_CanAllocateConcurrently)
{
    constexpr size_t numAllocations = 1420;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    std::vector<int> threadIds;
    std::atomic_int counter;

    // Allocation Job
    const auto allocFn = [&]() {
        for (size_t j = 0; j < numAllocations; ++j)
        {
            // Inefficient use of storage as our pool chunk is 1KB.
            // But it can work for tests as the size of Vec4 is not used internally.
            static_cast<void>(pool.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f));
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


TEST_F(ExternallyManagedPool_MTTests, Alloc_ConcurrentAllocationsDoNotCorruptInternalState)
{
    constexpr size_t numAllocations = 1420;
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
            // Inefficient use of storage as our pool chunk is 1KB.
            // But it can work for tests as the size of Vec4 is not used internally.
            auto alloc = pool.alloc<Vec4>(0.0f, 0.0f, 0.0f, 0.0f);

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


TEST_F(ExternallyManagedPool_MTTests, MFree_ConcurrentFreesDoNotCorruptInternalState)
{
    // To verify that free doesn't corrupt the internal state we can concurrently free all allocations
    // and allocate a large block. A corruption of internal state will make it impossible to create a
    // near size allocation

    constexpr size_t numAllocations = 1420;
    constexpr size_t numThreads     = 8;

    std::vector<void*> allocations;
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    std::mutex mutex;

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        allocations.push_back(pool.malloc());
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
            pool.mfree(block);
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

    // Make a near pool size allocation to ensure that all the blocks are freed
    const auto block = pool.malloc();

    // Ensure that the expected number of allocations match the counter
    EXPECT_NE(nullptr, block);
}


TEST_F(ExternallyManagedPool_MTTests, Free_ConcurrentFreesDoNotCorruptInternalState)
{
    // To verify that free doesn't corrupt the internal state we can concurrently free all allocations
    // and allocate a large block. A corruption of internal state will make it impossible to create a
    // near size allocation
    constexpr size_t numAllocations = 1420;
    constexpr size_t numThreads     = 8;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    std::vector<Vec4*> allocations;

    std::mutex mutex;

    // Allocation is single threaded
    for (size_t j = 0; j < numAllocations * numThreads; ++j)
    {
        // Inefficient use of storage as our pool chunk is 1KB.
        // But it can work for tests as the size of Vec4 is not used internally.
        allocations.push_back(pool.alloc<Vec4>(1.0f, 2.0f, 3.0f, 4.0f));
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
            pool.free(block);
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

    // Make a near pool size allocation to ensure that all the blocks are freed
    const auto block = pool.malloc();

    // Ensure that the expected number of allocations match the counter
    EXPECT_NE(nullptr, block);
}


TEST_F(ExternallyManagedPool_MTTests, MoveCtor_MovesTheAllocator)
{
    pmm::Pool<pmm::MemPolicy::External, pmm::TelPolicy::Enabled, pmm::SafeModePolicy::Unsafe,
              pmm::mt::MTPolicy::SpinLock>
        pool2 = std::move(pool);
    // Without testing internal states, we can only allocate a block with the new allocator and check if its
    // non-null to ensure that the move worked.
    const auto allocation = pool2.malloc();
    EXPECT_NE(nullptr, allocation);
}


TEST_F(ExternallyManagedPool_MTTests, MoveAssignment_MovesTheAllocator)
{
    constexpr size_t size2{ 1024 };
    constexpr size_t chunkSize2{ 128 };
    const auto mem2 = new uint8_t[size2];
    pmm::Pool<pmm::MemPolicy::External, pmm::TelPolicy::Enabled, pmm::SafeModePolicy::Unsafe,
              pmm::mt::MTPolicy::SpinLock>
        pool2{ mem2, size2, chunkSize2, 1 };

    pool2 = std::move(pool);
    // Without testing internal states, we can only allocate a block with the new allocator and check if its
    // non-null to ensure that the move worked.
    const auto allocation = pool2.malloc();
    EXPECT_NE(nullptr, allocation);
    delete[] mem2;
}

/** @} */
