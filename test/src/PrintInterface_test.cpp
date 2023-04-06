#include "gmock/gmock.h"
#include "GodzillaConfig.h"
#include "GodzillaApp_test.h"
#include "PrintInterface.h"

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

TEST(PrintInterfaceTest, lprintf)
{
    testing::internal::CaptureStdout();

    TestApp app;

    class TestObject : public Object, public PrintInterface {
    public:
        explicit TestObject(const Parameters & params) : Object(params), PrintInterface(this) {}

        virtual void
        create()
        {
            lprintf(0, "Print");
        }
    };

    Parameters pars = TestObject::parameters();
    pars.set<const App *>("_app") = &app;
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

        virtual void
        create()
        {
            TIMED_EVENT(0, "Print", "Print {}", "text");
        }
    };

    Parameters pars = TestObject::parameters();
    pars.set<const App *>("_app") = &app;
    TestObject obj(pars);

    obj.create();
#ifdef GODZILLA_WITH_PERF_LOG
    EXPECT_THAT(testing::internal::GetCapturedStdout(),
                testing::ContainsRegex("Print text... done \\[.+\\]"));
#else
    EXPECT_THAT(testing::internal::GetCapturedStdout(), testing::ContainsRegex("Print text"));
#endif
}
