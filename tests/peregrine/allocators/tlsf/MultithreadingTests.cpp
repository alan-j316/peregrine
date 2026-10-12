/**
 * @file MultithreadingTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: October 12, 2026
 *
 * @brief Verifies TLSF Multithreading capabilities.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

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
} // namespace


int genRandom()
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> dist;

    return dist(gen);
}

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
    std::vector<int> threadIds;
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
