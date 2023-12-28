#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/Preconditioner.h"
#include "godzilla/Matrix.h"

using namespace godzilla;
using namespace testing;

TEST(Preconditioner, type)
{
    testing::internal::CaptureStdout();

    TestApp app;

    Preconditioner pc;
    pc.create(app.get_comm());
    pc.set_type(PCICC);
    EXPECT_EQ(pc.get_type(), PCICC);
    pc.view();
    auto o = testing::internal::GetCapturedStdout();
    EXPECT_THAT(o, HasSubstr("type: icc"));
    pc.destroy();
}

TEST(Preconditioner, set_up)
{
    testing::internal::CaptureStdout();

    TestApp app;
    auto comm = app.get_comm();

    Preconditioner pc;
    pc.create(comm);
    pc.set_type(PCILU);
    Matrix A = Matrix::create_seq_aij(comm, 3, 3, 1);
    A.set_value(0, 0, 1.);
    A.set_value(1, 1, 1.);
    A.set_value(2, 2, 1.);
    A.assemble();
    pc.set_operators(A, A);
    pc.set_up();
    pc.view();

    auto o = testing::internal::GetCapturedStdout();
    EXPECT_THAT(o, HasSubstr("linear system matrix = precond matrix:"));

    pc.destroy();
}

TEST(Preconditioner, reset)
{
    testing::internal::CaptureStdout();

    TestApp app;
    auto comm = app.get_comm();

    Preconditioner pc;
    pc.create(comm);
    pc.set_type(PCILU);
    Matrix A = Matrix::create_seq_aij(comm, 3, 3, 1);
    A.set_value(0, 0, 1.);
    A.set_value(1, 1, 1.);
    A.set_value(2, 2, 1.);
    A.assemble();
    pc.set_operators(A, A);
    pc.set_up();
    pc.reset();
    pc.view();

    auto o = testing::internal::GetCapturedStdout();
    EXPECT_THAT(o, HasSubstr("PC has not been set up so information may be incomplete"));

    pc.destroy();
}

TEST(Preconditioner, oper_pc)
{
    TestApp app;

    Preconditioner pc;
    pc.create(app.get_comm());
    PCSetType(pc, PCICC);
    EXPECT_EQ(pc.get_type(), PCICC);
    pc.destroy();
}
