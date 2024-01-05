#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/PCFactor.h"

using namespace godzilla;
using namespace testing;

TEST(PCFactor, ctor_pc)
{
    TestApp app;
    Preconditioner pc;
    pc.create(app.get_comm());
    PCFactor factor(pc);
    factor.set_type(PCFactor::ILU);
    EXPECT_EQ(pc.get_type(), PCILU);
    pc.destroy();
}

TEST(PCFactor, type)
{
    std::vector<PCFactor::Type> factor_types = { PCFactor::ICC,
                                                 PCFactor::ILU,
                                                 PCFactor::LU,
                                                 PCFactor::CHOLESKY };
    std::vector<std::string> factor_type_str = { PCICC, PCILU, PCLU, PCCHOLESKY };

    testing::internal::CaptureStdout();

    TestApp app;
    for (auto & t : factor_types) {
        PCFactor pc;
        pc.create(app.get_comm());
        pc.set_type(t);
        EXPECT_EQ(pc.get_type(), t);
        pc.view();
        pc.destroy();
    }

    auto o = testing::internal::GetCapturedStdout();
    for (auto & t : factor_type_str)
        EXPECT_THAT(o, HasSubstr(t));
}

TEST(PCFactor, api)
{
    TestApp app;
    PCFactor pc;
    pc.create(app.get_comm());
    pc.set_type(PCFactor::ILU);

    pc.set_allow_diagonal_fill(true);
    EXPECT_TRUE(pc.get_allow_diagonal_fill());

    pc.set_levels(2);
    EXPECT_EQ(pc.get_levels(), 2);

    std::vector<PCFactor::MatShiftType> shift_type = { PCFactor::NONE,
                                                       PCFactor::NONZERO,
                                                       PCFactor::POSITIVE_DEFINITE,
                                                       PCFactor::INBLOCKS };
    for (auto & t : shift_type) {
        pc.set_shift_type(t);
        EXPECT_EQ(pc.get_shift_type(), t);
    }

    pc.set_shift_amount(0.1);
    EXPECT_EQ(pc.get_shift_amount(), 0.1);

    pc.set_use_in_place(true);
    EXPECT_TRUE(pc.get_use_in_place());

    pc.set_zero_pivot(0.1);
    EXPECT_DOUBLE_EQ(pc.get_zero_pivot(), 0.1);

    pc.destroy();
}

TEST(PCFactor, api2)
{
    testing::internal::CaptureStdout();

    TestApp app;
    auto comm = app.get_comm();
    PCFactor pc;
    pc.create(comm);
    pc.set_type(PCFactor::ILU);
    pc.set_mat_solver_type(PCFactor::PETSC);
    EXPECT_EQ(pc.get_mat_solver_type(), PCFactor::PETSC);

    auto A = Matrix::create_seq_aij(comm, 2, 2, 1);
    A.set_value(0, 0, 1.);
    A.set_value(1, 1, 1.);
    A.assemble();
    pc.set_operators(A, A);

    pc.reorder_for_nonzero_diagonal(1.e-8);
    pc.set_column_pivot(1.0);
    pc.set_drop_tolerance(0.1, 0.05, 2);
    pc.set_fill(2.5);
    pc.set_mat_ordering_type(PCFactor::NATURAL);
    pc.set_pivot_in_blocks(true);
    pc.set_reuse_fill(true);
    pc.set_reuse_ordering(true);
    pc.set_up_mat_solver_type();

    pc.view();

    pc.destroy();

    auto o = testing::internal::GetCapturedStdout();
    EXPECT_THAT(o, HasSubstr("Reusing fill from past factorization"));
    EXPECT_THAT(o, HasSubstr("Reusing reordering from past factorization"));
    EXPECT_THAT(o, HasSubstr("drop tolerance 0.1"));
    EXPECT_THAT(o, HasSubstr("column permutation tolerance 0.05"));
    EXPECT_THAT(o, HasSubstr("matrix ordering: natural"));
    EXPECT_THAT(o, HasSubstr("matrix solver type: petsc"));
}
