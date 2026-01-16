#include "gmock/gmock.h"
#include "godzilla/Logger.h"
#include "godzilla/LoggingInterface.h"

using namespace godzilla;

namespace {

class TestLogging : public LoggingInterface {
public:
    TestLogging(Logger & logger) : LoggingInterface(&logger) {}

    void
    log(String msg)
    {
        info("{}", msg);
    }
};

} // namespace

TEST(LoggerTest, ctor)
{
    Logger log;
}

TEST(LoggerTest, log_into_a_file)
{
    Logger logger;
    TestLogging obj(logger);

    logger.set_log_file_name("test_log.txt");
    logger.set_format_string("%Y-%m-%d %H:%M:%S %n %l: %v");
    obj.log("Test a log line");
}
