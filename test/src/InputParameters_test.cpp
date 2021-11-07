#include "gtest/gtest.h"
#include "InputParameters.h"
#include "Factory.h"

using namespace godzilla;

// TEST(InputParameters, get)
// {
//     InputParameters params = Factory::getValidParams("Object");
//     EXPECT_DEATH(
//         params.get<int>("i"),
//         "No parameter 'i' found."
//     );
// }

TEST(InputParameters, empty_doc_str)
{
    InputParameters params = Factory::getValidParams("Object");

    EXPECT_EQ(
        params.getDocString("i"),
        std::string("")
    );
}
