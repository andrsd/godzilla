#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/PCJacobi.h"

using namespace godzilla;
using namespace testing;

TEST(PCJacobi, ctor_pc)
{
    TestApp app;
    Preconditioner pc;
    pc.create(app.get_comm());
    PCJacobi jacobi(pc);
    EXPECT_EQ(pc.get_type(), PCJACOBI);
    pc.destroy();
}

TEST(PCJacobi, type)
{
    std::vector<PCJacobi::Type> jacobi_types = { PCJacobi::DIAGONAL,
                                                 PCJacobi::ROWMAX,
                                                 PCJacobi::ROWSUM };
    std::vector<std::string> jacobi_type_str = { "DIAGONAL", "ROWMAX", "ROWSUM" };

    testing::internal::CaptureStdout();

    TestApp app;
    for (auto & t : jacobi_types) {
        PCJacobi pc;
        pc.create(app.get_comm());
        pc.set_type(t);
        EXPECT_EQ(pc.get_type(), t);
        pc.view();
        pc.destroy();
    }

    auto o = testing::internal::GetCapturedStdout();
    EXPECT_THAT(o, HasSubstr("type: jacobi"));
    for (auto & t : jacobi_type_str)
        EXPECT_THAT(o, HasSubstr(t));
}

TEST(PCJacobi, api)
{
    TestApp app;
    PCJacobi pc;
    pc.create(app.get_comm());

    pc.set_fix_diagonal(true);
    EXPECT_EQ(pc.get_fix_diagonal(), true);

    pc.set_use_abs(true);
    EXPECT_EQ(pc.get_use_abs(), true);

    pc.destroy();
}
