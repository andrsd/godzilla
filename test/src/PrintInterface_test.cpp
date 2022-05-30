#include "gmock/gmock.h"
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
        explicit TestObject(const InputParameters & params) : Object(params), PrintInterface(this)
        {
        }

        virtual void
        create()
        {
            lprintf(0, "Print");
        }
    };

    InputParameters pars = TestObject::valid_params();
    pars.set<const App *>("_app") = &app;
    TestObject obj(pars);

    obj.create();
    EXPECT_THAT(testing::internal::GetCapturedStdout(), testing::HasSubstr("Print"));
}
