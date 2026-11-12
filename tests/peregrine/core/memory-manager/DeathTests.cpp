/**
 * @file DeathTests.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: November 12, 2026
 *
 * @brief Validates memory managers assertions.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#ifdef ENABLE_PMM_DEATH_TESTS

    #include <gtest/gtest.h>
    #include <peregrine/Peregrine.h>

namespace
{
    using namespace pmm::constants;
    class InternalMemoryManagerDeathTests: public testing::Test
    {
    public:
        static constexpr size_t SIZE = 50_MB;
        pmm::MemoryManager<> manager{ SIZE };
    };

    class ExternalMemoryManagerDeathTests: public testing::Test
    {
    public:
        static constexpr size_t SIZE = 50_MB;
        uint8_t* managerBuffer       = new uint8_t[SIZE];
        pmm::MemoryManager<pmm::TelPolicy::Enabled, pmm::MemPolicy::External> manager{ managerBuffer, SIZE };

        ~ExternalMemoryManagerDeathTests() override { delete[] managerBuffer; }
    };

} // namespace



/// +--------------------------------------------------------------------------------------------------+
/// |                               INTERNAL MEMORY MANAGED TESTS                                      |
/// +--------------------------------------------------------------------------------------------------+

/// +---------------------------------------------------------------------------------------+
/// |                               ARENA TESTS                                             |
/// +---------------------------------------------------------------------------------------+

TEST_F(InternalMemoryManagerDeathTests, CreateArena_TriggersAssertion_WhenCreatingAnArenaLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createLocalArena(51_MB)), ""); }


TEST_F(InternalMemoryManagerDeathTests, CreateSafeLocalArena_TriggersAssertion_WhenCreatingAnArenaLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createSafeLocalArena(51_MB)), ""); }


TEST_F(InternalMemoryManagerDeathTests, CreateMTArena_TriggersAssertion_WhenCreatingAnArenaLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createMTArena(51_MB)), ""); }


TEST_F(InternalMemoryManagerDeathTests, CreateSafeMTArena_TriggersAssertion_WhenCreatingAnArenaLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createSafeMTArena(51_MB)), ""); }



/// +---------------------------------------------------------------------------------------+
/// |                                  STACK TESTS                                          |
/// +---------------------------------------------------------------------------------------+

TEST_F(InternalMemoryManagerDeathTests, CreateStack_TriggersAssertion_WhenCreatingAnStackLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createLocalStack(51_MB)), ""); }


TEST_F(InternalMemoryManagerDeathTests, CreateSafeLocalStack_TriggersAssertion_WhenCreatingAnStackLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createSafeLocalStack(51_MB)), ""); }


TEST_F(InternalMemoryManagerDeathTests, CreateMTStack_TriggersAssertion_WhenCreatingAnStackLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createMTStack(51_MB)), ""); }


TEST_F(InternalMemoryManagerDeathTests, CreateSafeMTStack_TriggersAssertion_WhenCreatingAnStackLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createSafeMTStack(51_MB)), ""); }



/// +---------------------------------------------------------------------------------------+
/// |                                  POOL TESTS                                           |
/// +---------------------------------------------------------------------------------------+

TEST_F(InternalMemoryManagerDeathTests, CreatePool_TriggersAssertion_WhenCreatingAnPoolLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createLocalPool(51, 1_MB)), ""); }


TEST_F(InternalMemoryManagerDeathTests, CreateSafeLocalPool_TriggersAssertion_WhenCreatingAnPoolLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createSafeLocalPool(51, 1_MB)), ""); }


TEST_F(InternalMemoryManagerDeathTests, CreateMTPool_TriggersAssertion_WhenCreatingAnPoolLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createMTPool(51, 1_MB)), ""); }


TEST_F(InternalMemoryManagerDeathTests, CreateSafeMTPool_TriggersAssertion_WhenCreatingAnPoolLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createSafeMTPool(51, 1_MB)), ""); }



/// +---------------------------------------------------------------------------------------+
/// |                                   TLSF TESTS                                          |
/// +---------------------------------------------------------------------------------------+

TEST_F(InternalMemoryManagerDeathTests, CreateTLSF_TriggersAssertion_WhenCreatingAnTLSFLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createLocalTLSF(51_MB)), ""); }


TEST_F(InternalMemoryManagerDeathTests, CreateSafeLocalTLSF_TriggersAssertion_WhenCreatingAnTLSFLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createSafeLocalTLSF(51_MB)), ""); }


TEST_F(InternalMemoryManagerDeathTests, CreateMTTLSF_TriggersAssertion_WhenCreatingAnTLSFLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createMTTLSF(51_MB)), ""); }


TEST_F(InternalMemoryManagerDeathTests, CreateSafeMTTLSF_TriggersAssertion_WhenCreatingAnTLSFLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createSafeMTTLSF(51_MB)), ""); }




/// +--------------------------------------------------------------------------------------------------+
/// |                               INTERNAL MEMORY MANAGED TESTS                                      |
/// +--------------------------------------------------------------------------------------------------+

/// +---------------------------------------------------------------------------------------+
/// |                               ARENA TESTS                                             |
/// +---------------------------------------------------------------------------------------+

TEST_F(ExternalMemoryManagerDeathTests, CreateArena_TriggersAssertion_WhenCreatingAnArenaLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createLocalArena(51_MB)), ""); }


TEST_F(ExternalMemoryManagerDeathTests, CreateSafeLocalArena_TriggersAssertion_WhenCreatingAnArenaLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createSafeLocalArena(51_MB)), ""); }


TEST_F(ExternalMemoryManagerDeathTests, CreateMTArena_TriggersAssertion_WhenCreatingAnArenaLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createMTArena(51_MB)), ""); }


TEST_F(ExternalMemoryManagerDeathTests, CreateSafeMTArena_TriggersAssertion_WhenCreatingAnArenaLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createSafeMTArena(51_MB)), ""); }



/// +---------------------------------------------------------------------------------------+
/// |                                  STACK TESTS                                          |
/// +---------------------------------------------------------------------------------------+

TEST_F(ExternalMemoryManagerDeathTests, CreateStack_TriggersAssertion_WhenCreatingAnStackLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createLocalStack(51_MB)), ""); }


TEST_F(ExternalMemoryManagerDeathTests, CreateSafeLocalStack_TriggersAssertion_WhenCreatingAnStackLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createSafeLocalStack(51_MB)), ""); }


TEST_F(ExternalMemoryManagerDeathTests, CreateMTStack_TriggersAssertion_WhenCreatingAnStackLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createMTStack(51_MB)), ""); }


TEST_F(ExternalMemoryManagerDeathTests, CreateSafeMTStack_TriggersAssertion_WhenCreatingAnStackLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createSafeMTStack(51_MB)), ""); }



/// +---------------------------------------------------------------------------------------+
/// |                                  POOL TESTS                                           |
/// +---------------------------------------------------------------------------------------+

TEST_F(ExternalMemoryManagerDeathTests, CreatePool_TriggersAssertion_WhenCreatingAnPoolLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createLocalPool(51, 1_MB)), ""); }


TEST_F(ExternalMemoryManagerDeathTests, CreateSafeLocalPool_TriggersAssertion_WhenCreatingAnPoolLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createSafeLocalPool(51, 1_MB)), ""); }


TEST_F(ExternalMemoryManagerDeathTests, CreateMTPool_TriggersAssertion_WhenCreatingAnPoolLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createMTPool(51, 1_MB)), ""); }


TEST_F(ExternalMemoryManagerDeathTests, CreateSafeMTPool_TriggersAssertion_WhenCreatingAnPoolLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createSafeMTPool(51, 1_MB)), ""); }



/// +---------------------------------------------------------------------------------------+
/// |                                   TLSF TESTS                                          |
/// +---------------------------------------------------------------------------------------+

TEST_F(ExternalMemoryManagerDeathTests, CreateTLSF_TriggersAssertion_WhenCreatingAnTLSFLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createLocalTLSF(51_MB)), ""); }


TEST_F(ExternalMemoryManagerDeathTests, CreateSafeLocalTLSF_TriggersAssertion_WhenCreatingAnTLSFLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createSafeLocalTLSF(51_MB)), ""); }


TEST_F(ExternalMemoryManagerDeathTests, CreateMTTLSF_TriggersAssertion_WhenCreatingAnTLSFLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createMTTLSF(51_MB)), ""); }


TEST_F(ExternalMemoryManagerDeathTests, CreateSafeMTTLSF_TriggersAssertion_WhenCreatingAnTLSFLargerThanMemorySize)
{ EXPECT_DEATH(static_cast<void>(manager.createSafeMTTLSF(51_MB)), ""); }

#endif
