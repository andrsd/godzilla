#include "gmock/gmock.h"
#include "TestApp.h"
#include "GTestFENonlinearProblem.h"
#include "godzilla/PCFieldSplit.h"
#include "godzilla/LineMesh.h"

using namespace godzilla;
using namespace testing;

TEST(PCFieldSplit, ctor_pc)
{
    TestApp app;
    Preconditioner pc;
    pc.create(app.get_comm());
    PCFieldSplit fs(pc);
    EXPECT_EQ(pc.get_type(), PCFIELDSPLIT);
    pc.destroy();
}

TEST(PCFieldSplit, type)
{
    std::vector<PCFieldSplit::Type> types = { PCFieldSplit::ADDITIVE,
                                              PCFieldSplit::COMPOSITE_MULTIPLICATIVE,
                                              PCFieldSplit::SYMMETRIC_MULTIPLICATIVE,
                                              PCFieldSplit::SPECIAL,
                                              PCFieldSplit::SCHUR };
    std::vector<std::string> type_str = { "ADDITIVE",
                                          "MULTIPLICATIVE",
                                          "SYMMETRIC_MULTIPLICATIVE",
                                          "SPECIAL",
                                          "Schur" };

    testing::internal::CaptureStdout();

    TestApp app;
    for (auto & t : types) {
        PCFieldSplit pc;
        pc.create(app.get_comm());
        pc.set_type(t);
        EXPECT_EQ(pc.get_type(), t);
        pc.view();
        pc.destroy();
    }

    auto o = testing::internal::GetCapturedStdout();
    for (auto & t : type_str)
        EXPECT_THAT(o, HasSubstr(t));
}

TEST(PCFieldSplit, schur_fact_type)
{
    std::vector<PCFieldSplit::SchurFactType> types = { PCFieldSplit::SCHUR_FACT_DIAG,
                                                       PCFieldSplit::SCHUR_FACT_LOWER,
                                                       PCFieldSplit::SCHUR_FACT_UPPER,
                                                       PCFieldSplit::SCHUR_FACT_FULL };
    std::vector<std::string> type_str = { "DIAG", "LOWER", "UPPER", "FULL" };

    testing::internal::CaptureStdout();

    TestApp app;
    PCFieldSplit pc;
    pc.create(app.get_comm());
    pc.set_type(PCFieldSplit::SCHUR);
    for (auto & t : types) {
        pc.set_schur_fact_type(t);
        pc.view();
    }
    pc.destroy();

    auto o = testing::internal::GetCapturedStdout();
    for (auto & t : type_str)
        EXPECT_THAT(o, HasSubstr(t));
}

TEST(PCFieldSplit, schur_pre_type)
{
    testing::internal::CaptureStdout();

    std::vector<PCFieldSplit::SchurPreType> types = { PCFieldSplit::SCHUR_PRE_SELF,
                                                      PCFieldSplit::SCHUR_PRE_A11,
                                                      PCFieldSplit::SCHUR_PRE_USER,
                                                      PCFieldSplit::SCHUR_PRE_FULL };
    std::vector<std::string> type_str = { "S itself",
                                          "A11",
                                          "user provided matrix",
                                          "the exact Schur complement" };

    TestApp app;
    auto comm = app.get_comm();
    PCFieldSplit pc;
    pc.create(comm);
    pc.set_type(PCFieldSplit::SCHUR);
    for (auto & t : types) {
        Matrix pre = Matrix::create_seq_aij(comm, 3, 3, 1);
        pc.set_schur_pre(t, pre);
        pc.view();
        pre.destroy();
    }
    pc.destroy();

    auto o = testing::internal::GetCapturedStdout();
    for (auto & t : type_str)
        EXPECT_THAT(o, HasSubstr(t));
}

TEST(PCFieldSplit, schur_scale)
{
    TestApp app;
    auto comm = app.get_comm();
    PCFieldSplit pc;
    pc.create(comm);
    pc.set_type(PCFieldSplit::SCHUR);
    pc.set_schur_scale(2.);
    // NOTE: There is no easy way to check this, unless a system is solved
    pc.destroy();
}

TEST(PCFieldSplit, detect_saddle_point)
{
    TestApp app;
    PCFieldSplit pc;
    pc.create(app.get_comm());
    pc.set_type(PCFieldSplit::SCHUR);
    pc.set_detect_saddle_point(true);
    EXPECT_TRUE(pc.get_detect_saddle_point());
    pc.destroy();
}

TEST(PCFieldSplit, block_size)
{
    testing::internal::CaptureStdout();

    TestApp app;
    PCFieldSplit pc;
    pc.create(app.get_comm());
    pc.set_block_size(2);
    pc.view();
    pc.destroy();

    auto o = testing::internal::GetCapturedStdout();
    EXPECT_THAT(o, HasSubstr("blocksize = 2"));
}

TEST(PCFieldSplit, diag_use_amat)
{
    TestApp app;
    PCFieldSplit pc;
    pc.create(app.get_comm());
    pc.set_diag_use_amat(true);
    EXPECT_TRUE(pc.get_diag_use_amat());
    pc.destroy();
}

TEST(PCFieldSplit, off_diag_use_amat)
{
    TestApp app;
    PCFieldSplit pc;
    pc.create(app.get_comm());
    pc.set_off_diag_use_amat(true);
    EXPECT_TRUE(pc.get_off_diag_use_amat());
    pc.destroy();
}

TEST(PCFieldSplit, schur)
{
    TestApp app;
    auto comm = app.get_comm();

    Parameters * mesh_pars = app.get_parameters("LineMesh");
    mesh_pars->set<Int>("nx") = 2;
    auto mesh = app.build_object<LineMesh>("LineMesh", "mesh", mesh_pars);

    Parameters * prob_pars = app.get_parameters("GTest2FieldsFENonlinearProblem");
    prob_pars->set<MeshObject *>("_mesh_obj") = mesh;
    auto prob =
        app.build_object<FENonlinearProblem>("GTest2FieldsFENonlinearProblem", "prob", prob_pars);

    mesh->create();
    prob->create();

    auto fdecomp = prob->create_field_decomposition();

    auto ksp = prob->get_ksp();
    PC pc;
    KSPGetPC(ksp, &pc);

    Matrix pre = Matrix::create_seq_aij(comm, 3, 3, 1);
    for (Int i = 0; i < 3; i++)
        pre.set_value(i, i, 2.);
    pre.assemble();

    PCFieldSplit fs(pc);
    fs.set_type(PCFieldSplit::SCHUR);
    fs.set_schur_fact_type(PCFieldSplit::SCHUR_FACT_FULL);
    fs.set_schur_pre(PCFieldSplit::SCHUR_PRE_USER, pre);
    fs.set_dm_splits(true);
    EXPECT_TRUE(fs.get_dm_splits());
    std::vector<std::string> fld_name = { "split0", "split1" };
    for (PetscInt i = 0; i < fdecomp.get_num_fields(); i++)
        fs.set_is(fld_name[i], fdecomp.is[i]);

    auto is0 = fs.get_is("split0");
    is0.get_indices();
    EXPECT_THAT(is0.to_std_vector(), ElementsAre(0, 2, 4));
    is0.restore_indices();
    auto is1 = fs.get_is("split1");
    is1.get_indices();
    EXPECT_THAT(is1.to_std_vector(), ElementsAre(1, 3, 5));
    is1.restore_indices();

    auto is_idx0 = fs.get_is_by_index(0);
    is_idx0.get_indices();
    EXPECT_THAT(is_idx0.to_std_vector(), ElementsAre(0, 2, 4));
    is_idx0.restore_indices();
    auto is_idx1 = fs.get_is_by_index(1);
    is_idx1.get_indices();
    EXPECT_THAT(is_idx1.to_std_vector(), ElementsAre(1, 3, 5));
    is_idx1.restore_indices();

    auto J = prob->get_jacobian();
    fs.set_operators(J, J);
    fs.set_up();

    auto sub_ksp = fs.get_sub_ksp();
    ASSERT_EQ(sub_ksp.size(), 2);

    auto schur_sub_ksp = fs.schur_get_sub_ksp();
    ASSERT_EQ(schur_sub_ksp.size(), 2);

    auto schur_blks = fs.get_schur_blocks();
    EXPECT_EQ(schur_blks.A00.get_n_cols(), 3);
    EXPECT_EQ(schur_blks.A00.get_n_rows(), 3);

    EXPECT_EQ(schur_blks.A01.get_n_cols(), 3);
    EXPECT_EQ(schur_blks.A01.get_n_rows(), 3);

    EXPECT_EQ(schur_blks.A10.get_n_cols(), 3);
    EXPECT_EQ(schur_blks.A10.get_n_rows(), 3);

    EXPECT_EQ(schur_blks.A11.get_n_cols(), 3);
    EXPECT_EQ(schur_blks.A11.get_n_rows(), 3);

    auto schur_pc = fs.get_schur_pre();
    EXPECT_EQ(schur_pc.type, PCFieldSplit::SCHUR_PRE_USER);
    for (Int i = 0; i < 3; i++)
        EXPECT_DOUBLE_EQ(schur_pc.matrix.get_value(i, i), 2.);

    auto s = fs.schur_get_s();
    EXPECT_EQ(s.get_type(), MATSCHURCOMPLEMENT);
    fs.schur_restore_s(s);

    fdecomp.destroy();
}

TEST(PCFieldSplit, gbk)
{
    TestApp app;
    PCFieldSplit pc;
    pc.create(app.get_comm());
    pc.set_type(PCFieldSplit::GBK);
    pc.set_gkb_delay(2);
    pc.set_gkb_maxit(3);
    pc.set_gkb_nu(0.1);
    pc.set_gkb_tol(1e-5);
    // NOTE: pc.view() causes a segfault
    // pc.view();
    pc.destroy();
}
