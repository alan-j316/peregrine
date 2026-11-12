/**
 * @file MemoryManagerDeathTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: November 12, 2026
 *
 * @brief Validates memory managers assertions.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#ifdef ENABLE_PMM_DEATH_TESTS

namespace
{
    using namespace pmm::constants;
    class InternalMemoryManagerTests: public testing::Test
    {
    public:
        static constexpr size_t SIZE = 50_MB;
        pmm::MemoryManager<> manager{ SIZE };
    };
}


#endif