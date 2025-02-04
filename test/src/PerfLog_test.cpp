#include "gmock/gmock.h"
#include "godzilla/PerfLog.h"
#include "ExceptionTestMacros.h"
#include <time.h>

using namespace godzilla;

TEST(PerfLogTest, event)
{
    const std::string event1_name = "event1";
    const std::string event2_name = "event2";

    auto event2_id = perf_log::register_event(event2_name);

    for (int i = 0; i < 2; ++i) {
        struct timespec remaining, request = { 0, 50000000 };
        perf_log::ScopedEvent event(event1_name);
        perf_log::log_flops(4.);
        nanosleep(&request, &remaining);
    }

    {
        perf_log::Event ev2(event2_id);
        struct timespec remaining, request = { 0, 200000000 };
        ev2.begin();
        perf_log::log_flops(16.);
        nanosleep(&request, &remaining);
        ev2.end();
    }

    perf_log::EventInfo info1 = perf_log::get_event_info(event1_name);
    EXPECT_DOUBLE_EQ(info1.flops(), 8.);
    EXPECT_EQ(info1.num_calls(), 2);
    EXPECT_NEAR(info1.time(), 0.1, 0.3);

    perf_log::EventInfo info2 = perf_log::get_event_info(event2_id);
    EXPECT_DOUBLE_EQ(info2.flops(), 16.);
    EXPECT_EQ(info2.num_calls(), 1);
}

TEST(PerfLogTest, error_existing_event)
{
    perf_log::register_event("event_asdf");
    EXPECT_THROW_MSG(perf_log::register_event("event_asdf"),
                     "PerfLog event 'event_asdf' is already registered.");
}

TEST(PerfLogTest, non_existent_event_id)
{
    EXPECT_THROW_MSG(perf_log::get_event_id("event_none"),
                     "Event 'event_none' was not registered.");

    const std::string no_event = "event_none";
    EXPECT_THROW_MSG(perf_log::get_event_id(no_event), "Event 'event_none' was not registered.");
}

TEST(PerfLogTest, stage)
{
    const std::string event_name = "event";
    perf_log::register_event(event_name);

    const std::string stage_name = "stage1";
    perf_log::register_stage(stage_name);
    PetscLogStage stage2_id = perf_log::register_stage("stage2");

    for (int i = 0; i < 3; ++i) {
        struct timespec remaining, request = { 0, 50000000 };
        perf_log::Stage stage(stage_name);
        perf_log::ScopedEvent ev(event_name);
        perf_log::log_flops(1.);
        nanosleep(&request, &remaining);
    }

    {
        struct timespec remaining, request = { 0, 100000000 };
        perf_log::Stage stage2(stage2_id);
        perf_log::ScopedEvent ev("event");
        perf_log::log_flops(2.);
        nanosleep(&request, &remaining);
    }

    perf_log::Stage stage("stage1");
#if (PETSC_VERSION_GE(3, 18, 0)) && (PETSC_VERSION_LT(3, 20, 0))
    EXPECT_EQ(stage.get_id(), 2);
#else
    EXPECT_EQ(stage.get_id(), 1);
#endif

    perf_log::EventInfo info1 = perf_log::get_event_info(event_name, stage_name);
    EXPECT_DOUBLE_EQ(info1.flops(), 3.);
    EXPECT_EQ(info1.num_calls(), 3);

    perf_log::Event ev(event_name);
    perf_log::EventInfo info2 = perf_log::get_event_info(ev.get_id(), stage2_id);
    EXPECT_DOUBLE_EQ(info2.flops(), 2.);
    EXPECT_EQ(info2.num_calls(), 1);
}

TEST(PerfLogTest, error_existing_stage)
{
    perf_log::register_stage("stage_asdf");
    EXPECT_THROW_MSG(perf_log::register_stage("stage_asdf"),
                     "PerfLog stage 'stage_asdf' is already registered.");
}

TEST(PerfLogTest, non_existent_stage_id)
{
    EXPECT_THROW_MSG(perf_log::get_stage_id("stage_none"),
                     "Stage 'stage_none' was not registered.");
}

TEST(PerfLogTest, is_event_registered)
{
    EXPECT_FALSE(perf_log::is_event_registered("event3"));
    perf_log::register_event("event3");
    EXPECT_TRUE(perf_log::is_event_registered("event3"));
}
