#include "gmock/gmock.h"
#include "GodzillaConfig.h"
#include "Godzilla.h"
#include "PerfLog.h"
#include <string.h>
#include <unistd.h>

#ifdef GODZILLA_WITH_PERF_LOG

using namespace godzilla;

TEST(PerfLogTest, event)
{
    PerfLog::register_event("event1");
    auto event2_id = PerfLog::register_event("event2");

    for (int i = 0; i < 2; i++) {
        PerfLog::Event event("event1");
        event.log_flops(4.);
        usleep(50000);
    }

    {
        PerfLog::Event ev2(event2_id);
        ev2.log_flops(16.);
        usleep(200000);
    }

    PerfLog::EventInfo info1 = PerfLog::get_event_info("event1");
    EXPECT_DOUBLE_EQ(info1.get_flops(), 8.);
    EXPECT_EQ(info1.get_num_calls(), 2);
    EXPECT_NEAR(info1.get_time(), 0.1, 1.5e-2);

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
}

TEST(PerfLogTest, stage)
{
    PerfLog::register_event("event");

    PerfLog::register_stage("stage1");
    PetscLogStage stage2_id = PerfLog::register_stage("stage2");

    for (int i = 0; i < 3; i++) {
        PerfLog::Stage stage("stage1");
        PerfLog::Event ev("event");
        ev.log_flops(1.);
        usleep(50000);
    }

    {
        PerfLog::Stage stage2(stage2_id);
        PerfLog::Event ev("event");
        ev.log_flops(2.);
        usleep(100000);
    }

    PerfLog::Stage stage("stage1");
    EXPECT_EQ(stage.get_id(), 1);

    PerfLog::EventInfo info1 = PerfLog::get_event_info("event", "stage1");
    EXPECT_DOUBLE_EQ(info1.get_flops(), 3.);
    EXPECT_EQ(info1.get_num_calls(), 3);

    PerfLog::Event ev("event");
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

#endif
