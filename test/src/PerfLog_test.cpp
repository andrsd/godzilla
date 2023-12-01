#include "gmock/gmock.h"
#include "godzilla/PerfLog.h"
#include <time.h>

#ifdef GODZILLA_WITH_PERF_LOG

using namespace godzilla;

TEST(PerfLogTest, event)
{
    const std::string event1_name = "event1";
    const std::string event2_name = "event2";

    PerfLog::register_event(event1_name);
    auto event2_id = PerfLog::register_event(event2_name);

    for (int i = 0; i < 2; i++) {
        struct timespec remaining, request = { 0, 50000000 };
        PerfLog::Event event(event1_name);
        event.log_flops(4.);
        nanosleep(&request, &remaining);
    }

    {
        struct timespec remaining, request = { 0, 200000000 };
        PerfLog::Event ev2(event2_id);
        ev2.log_flops(16.);
        nanosleep(&request, &remaining);
    }

    PerfLog::EventInfo info1 = PerfLog::get_event_info(event1_name);
    EXPECT_DOUBLE_EQ(info1.get_flops(), 8.);
    EXPECT_EQ(info1.get_num_calls(), 2);
    EXPECT_NEAR(info1.get_time(), 0.1, 0.3);

    PerfLog::EventInfo info2 = PerfLog::get_event_info(event2_id);
    EXPECT_DOUBLE_EQ(info2.get_flops(), 16.);
    EXPECT_EQ(info2.get_num_calls(), 1);
}

TEST(PerfLogTest, error_existing_event)
{
    PerfLog::register_event("event_asdf");
    EXPECT_DEATH(PerfLog::register_event("event_asdf"),
                 "PerfLog event 'event_asdf' is already registered.");
}

TEST(PerfLogTest, non_existent_event_id)
{
    EXPECT_DEATH(PerfLog::get_event_id("event_none"), "Event 'event_none' was not registered.");

    const std::string no_event = "event_none";
    EXPECT_DEATH(PerfLog::get_event_id(no_event), "Event 'event_none' was not registered.");
}

TEST(PerfLogTest, stage)
{
    const std::string event_name = "event";
    PerfLog::register_event(event_name);

    const std::string stage_name = "stage1";
    PerfLog::register_stage(stage_name);
    PetscLogStage stage2_id = PerfLog::register_stage("stage2");

    for (int i = 0; i < 3; i++) {
        struct timespec remaining, request = { 0, 50000000 };
        PerfLog::Stage stage(stage_name);
        PerfLog::Event ev(event_name);
        ev.log_flops(1.);
        nanosleep(&request, &remaining);
    }

    {
        struct timespec remaining, request = { 0, 100000000 };
        PerfLog::Stage stage2(stage2_id);
        PerfLog::Event ev("event");
        ev.log_flops(2.);
        nanosleep(&request, &remaining);
    }

    PerfLog::Stage stage("stage1");
    #if PETSC_VERSION_GE(3, 18, 0)
    EXPECT_EQ(stage.get_id(), 2);
    #else
    EXPECT_EQ(stage.get_id(), 1);
    #endif

    PerfLog::EventInfo info1 = PerfLog::get_event_info(event_name, stage_name);
    EXPECT_DOUBLE_EQ(info1.get_flops(), 3.);
    EXPECT_EQ(info1.get_num_calls(), 3);

    PerfLog::Event ev(event_name);
    PerfLog::EventInfo info2 = PerfLog::get_event_info(ev.get_id(), stage2_id);
    EXPECT_DOUBLE_EQ(info2.get_flops(), 2.);
    EXPECT_EQ(info2.get_num_calls(), 1);
}

TEST(PerfLogTest, error_existing_stage)
{
    PerfLog::register_stage("stage_asdf");
    EXPECT_DEATH(PerfLog::register_stage("stage_asdf"),
                 "PerfLog stage 'stage_asdf' is already registered.");
}

TEST(PerfLogTest, non_existent_stage_id)
{
    EXPECT_DEATH(PerfLog::get_stage_id("stage_none"), "Stage 'stage_none' was not registered.");
}

TEST(PerfLogTest, is_event_registered)
{
    EXPECT_FALSE(PerfLog::is_event_registered("event3"));
    PerfLog::register_event("event3");
    EXPECT_TRUE(PerfLog::is_event_registered("event3"));
}

#endif
