#include "gmock/gmock.h"
#include <chrono>
#include <thread>
#include "GodzillaApp_test.h"
#include "godzilla/PrintInterface.h"

using namespace godzilla;

TEST(PrintInterfaceTest, lprint)
{
    testing::internal::CaptureStdout();

    TestApp app;

    class TestObject : public Object, public PrintInterface {
    public:
        explicit TestObject(const Parameters & pars) : Object(pars), PrintInterface(this) {}

        void
        create() override
        {
            lprintln(0, "Print");
        }
    };

    auto pars = TestObject::parameters();
    pars.set<App *>("app", &app);
    TestObject obj(pars);

    obj.create();
    EXPECT_THAT(testing::internal::GetCapturedStdout(), testing::HasSubstr("Print"));
}

TEST(PrintInterfaceTest, timed_event)
{
    testing::internal::CaptureStdout();

    TestApp app;

    class TestObject : public Object, public PrintInterface {
    public:
        explicit TestObject(const Parameters & pars) : Object(pars), PrintInterface(this) {}

        void
        create() override
        {
            TIMED_EVENT(0, "Print", "Print {}", "text");
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    };

    auto pars = TestObject::parameters();
    pars.set<App *>("app", &app);
    TestObject obj(pars);

    obj.create();
    EXPECT_THAT(testing::internal::GetCapturedStdout(),
                testing::ContainsRegex("Print text... took .+"));
}
