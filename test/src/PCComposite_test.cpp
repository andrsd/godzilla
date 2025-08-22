#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/PCComposite.h"
#include "godzilla/PCJacobi.h"
#include "godzilla/PCFactor.h"

using namespace godzilla;
using namespace testing;

TEST(PCComposite, ctor_pc)
{
    TestApp app;
    Preconditioner pc;
    pc.create(app.get_comm());
    PCComposite composite(pc);
    composite.inc_reference();
    EXPECT_EQ(pc.get_type(), PCCOMPOSITE);
}

TEST(PCComposite, type)
{
    std::vector<PCComposite::Type> types = { PCComposite::ADDITIVE,
                                             PCComposite::MULTIPLICATIVE,
                                             PCComposite::SYMMETRIC_MULTIPLICATIVE,
                                             PCComposite::SPECIAL };
    std::vector<std::string> type_str = { "ADDITIVE",
                                          "MULTIPLICATIVE",
                                          "SYMMETRIC_MULTIPLICATIVE",
                                          "SPECIAL" };

    testing::internal::CaptureStdout();

    TestApp app;
    for (auto & t : types) {
        PCComposite pc;
        pc.create(app.get_comm());
        pc.set_type(t);
        EXPECT_EQ(pc.get_type(), t);
        pc.view();
    }

    auto o = testing::internal::GetCapturedStdout();
    for (auto & t : type_str)
        EXPECT_THAT(o, HasSubstr(t));
}

TEST(PCComposite, api)
{
    TestApp app;
    auto comm = app.get_comm();

    PCJacobi pc1;
    pc1.create(comm);

    PCFactor pc2;
    pc2.create(comm);
    pc2.set_type(PCFactor::ICC);

    PCComposite composite;
    composite.create(comm);
    composite.set_type(PCComposite::ADDITIVE);
    composite.add_pc(pc1);
    composite.add_pc(pc2);

    EXPECT_EQ(composite.get_number_pc(), 2);
    Preconditioner sub_pc0 = composite.get_pc(0);
    EXPECT_EQ(sub_pc0.get_type(), PCJACOBI);
    Preconditioner sub_pc1 = composite.get_pc(1);
    EXPECT_EQ(sub_pc1.get_type(), PCICC);
}

TEST(PCComposite, special_set_alpha)
{
    TestApp app;
    auto comm = app.get_comm();
    PCComposite composite;
    composite.create(comm);
    composite.set_type(PCComposite::SPECIAL);
    composite.special_set_alpha(2.);
    // TODO: how to test this was set?
}
