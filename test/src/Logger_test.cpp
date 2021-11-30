#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Godzilla.h"
#include "Logger.h"

using namespace godzilla;

TEST(LoggerTest, ctor)
{
    Logger log;

    EXPECT_EQ(log.getNumEntries(), 0);
    EXPECT_EQ(log.getNumErrors(), 0);
    EXPECT_EQ(log.getNumWarnings(), 0);
}

TEST(LoggerTest, log_error)
{
    testing::internal::CaptureStderr();
    Logger log;

    log.error("error1");
    EXPECT_EQ(log.getNumEntries(), 1);
    EXPECT_EQ(log.getNumErrors(), 1);
    EXPECT_EQ(log.getNumWarnings(), 0);

    log.print();
    std::string output = testing::internal::GetCapturedStderr();
    EXPECT_THAT(output, testing::HasSubstr("error1"));
    EXPECT_THAT(output, testing::HasSubstr("1 error(s) found."));
}

TEST(LoggerTest, log_warning)
{
    testing::internal::CaptureStderr();
    Logger log;

    log.warning("warn1");
    EXPECT_EQ(log.getNumEntries(), 1);
    EXPECT_EQ(log.getNumErrors(), 0);
    EXPECT_EQ(log.getNumWarnings(), 1);

    log.print();
    std::string output = testing::internal::GetCapturedStderr();
    EXPECT_THAT(output, testing::HasSubstr("warn1"));
    EXPECT_THAT(output, testing::HasSubstr("1 warning(s) found."));
}

TEST(LoggerTest, log_err_warning)
{
    testing::internal::CaptureStderr();
    Logger log;

    log.error("error1");
    log.warning("warn1");
    EXPECT_EQ(log.getNumEntries(), 2);
    EXPECT_EQ(log.getNumErrors(), 1);
    EXPECT_EQ(log.getNumWarnings(), 1);

    log.print();
    std::string output = testing::internal::GetCapturedStderr();
    EXPECT_THAT(output, testing::HasSubstr("error1"));
    EXPECT_THAT(output, testing::HasSubstr("warn1"));
    EXPECT_THAT(output, testing::HasSubstr("1 error(s), 1 warning(s) found."));
}
