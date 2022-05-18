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
