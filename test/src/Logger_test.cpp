#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "godzilla/Logger.h"

using namespace godzilla;

TEST(LoggerTest, ctor)
{
    Logger log;

    EXPECT_EQ(log.get_num_entries(), 0);
    EXPECT_EQ(log.get_num_errors(), 0);
    EXPECT_EQ(log.get_num_warnings(), 0);
}

TEST(LoggerTest, log_error)
{
    testing::internal::CaptureStderr();
    Logger log;

    log.error("error1");
    EXPECT_EQ(log.get_num_entries(), 1);
    EXPECT_EQ(log.get_num_errors(), 1);
    EXPECT_EQ(log.get_num_warnings(), 0);

    log.print();
    String output = testing::internal::GetCapturedStderr();
    EXPECT_THAT(output, testing::HasSubstr("error1"));
    EXPECT_THAT(output, testing::HasSubstr("1 error(s) found."));
}

TEST(LoggerTest, log_warning)
{
    testing::internal::CaptureStderr();
    Logger log;

    log.warning("warn1");
    EXPECT_EQ(log.get_num_entries(), 1);
    EXPECT_EQ(log.get_num_errors(), 0);
    EXPECT_EQ(log.get_num_warnings(), 1);

    log.print();
    String output = testing::internal::GetCapturedStderr();
    EXPECT_THAT(output, testing::HasSubstr("warn1"));
    EXPECT_THAT(output, testing::HasSubstr("1 warning(s) found."));
}

TEST(LoggerTest, log_err_warning)
{
    testing::internal::CaptureStderr();
    Logger log;

    log.error("error1");
    log.warning("warn1");
    EXPECT_EQ(log.get_num_entries(), 2);
    EXPECT_EQ(log.get_num_errors(), 1);
    EXPECT_EQ(log.get_num_warnings(), 1);

    log.print();
    String output = testing::internal::GetCapturedStderr();
    EXPECT_THAT(output, testing::HasSubstr("error1"));
    EXPECT_THAT(output, testing::HasSubstr("warn1"));
    EXPECT_THAT(output, testing::HasSubstr("1 error(s), 1 warning(s) found."));
}
