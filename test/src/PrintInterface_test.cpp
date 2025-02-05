#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "godzilla/PrintInterface.h"

using namespace godzilla;

TEST(PrintInterfaceTest, error)
{
    class PrintTestApp : public TestApp {
    public:
        void
        test()
        {
            error("Error");
        }
    } app;

    EXPECT_DEATH(app.test(), "\\[ERROR\\] Error");
}

TEST(PrintInterfaceTest, lprint)
{
    testing::internal::CaptureStdout();

    TestApp app;

    class TestObject : public Object, public PrintInterface {
    public:
        explicit TestObject(const Parameters & params) : Object(params), PrintInterface(this) {}

        void
        create() override
        {
            lprintln(0, "Print");
        }
    };

    Parameters pars = TestObject::parameters();
    pars.set<App *>("_app") = &app;
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
        explicit TestObject(const Parameters & params) : Object(params), PrintInterface(this) {}

        void
        create() override
        {
            TIMED_EVENT(0, "Print", "Print {}", "text");
        }
    };

    Parameters pars = TestObject::parameters();
    pars.set<App *>("_app") = &app;
    TestObject obj(pars);

    obj.create();
    EXPECT_THAT(testing::internal::GetCapturedStdout(),
                testing::ContainsRegex("Print text... done \\[.+\\]"));
}
