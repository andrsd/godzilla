#include "gmock/gmock.h"
#include "godzilla/Section.h"
#include "godzilla/Range.h"

using namespace godzilla;

TEST(SectionTest, dofs)
{
    Section s;
    s.create(MPI_COMM_WORLD);
    s.set_num_fields(2);
    s.set_chart(make_range(10));
    for (Int i = 0; i < 10; i++) {
        s.set_dof(i, 4);
        s.set_field_dof(i, 0, 1);
        s.set_field_dof(i, 1, 2);
        s.add_field_dof(i, 1, 1);
    }
    s.set_up();

    EXPECT_EQ(s.get_num_fields(), 2);

    auto rng = s.get_chart();
    EXPECT_EQ(rng.first(), 0);
    EXPECT_EQ(rng.last(), 10);

    for (Int pt = 0; pt < 10; pt++) {
        EXPECT_EQ(s.get_dof(pt), 4);
        EXPECT_EQ(s.get_field_dof(pt, 0), 1);
        EXPECT_EQ(s.get_field_dof(pt, 1), 3);
    }

    s.destroy();
}

TEST(SectionTest, field_name)
{
    Section s;
    s.create(MPI_COMM_WORLD);
    s.set_num_fields(2);
    s.set_field_name(0, "fld1");
    s.set_field_name(1, "fld2");
    s.set_chart(0, 1);
    s.set_up();
    EXPECT_EQ(s.get_field_name(0), "fld1");
    EXPECT_EQ(s.get_field_name(1), "fld2");
    s.destroy();
}

TEST(SectionTest, get_max_dof)
{
    Section s;
    s.create(MPI_COMM_WORLD);
    s.set_num_fields(1);
    s.set_chart(0, 3);
    Int dofs[] = { 5, 3, 1 };
    for (Int i = 0; i < 3; i++)
        s.set_dof(i, dofs[i]);
    s.set_up();
    EXPECT_EQ(s.get_max_dof(), 5);
    s.destroy();
}

TEST(SectionTest, reset)
{
    Section s;
    s.create(MPI_COMM_WORLD);
    s.set_num_fields(1);
    s.set_chart(0, 1);
    s.set_dof(0, 10);
    s.set_up();
    s.reset();

    Int start, end;
    s.get_chart(start, end);
    EXPECT_EQ(start, -1);
    EXPECT_EQ(end, -1);

    s.destroy();
}

TEST(SectionTest, point_major)
{
    Section s;
    s.create(MPI_COMM_WORLD);
    s.set_num_fields(1);
    s.set_chart(0, 2);
    s.set_dof(0, 2);
    s.set_dof(1, 4);
    s.set_point_major();
    s.set_up();

    EXPECT_TRUE(s.get_point_major());

    s.destroy();
}

TEST(SectionTest, add_dof)
{
    Section s;
    s.create(MPI_COMM_WORLD);
    s.set_num_fields(1);
    s.set_chart(0, 2);
    s.set_dof(0, 1);
    s.set_dof(1, 1);
    s.add_dof(0, 10);
    s.add_dof(1, 20);
    s.set_up();
    EXPECT_EQ(s.get_dof(0), 11);
    EXPECT_EQ(s.get_dof(1), 21);
    s.destroy();
}

TEST(SectionTest, field_components)
{
    Section s;
    s.create(MPI_COMM_WORLD);
    s.set_num_fields(2);
    s.set_num_field_components(0, 2);
    s.set_component_name(0, 0, "f0c0");
    s.set_component_name(0, 1, "f0c1");
    s.set_num_field_components(1, 4);
    s.set_component_name(1, 0, "f1c1");
    s.set_component_name(1, 1, "f1c2");
    s.set_component_name(1, 2, "f1c3");
    s.set_component_name(1, 3, "f1c4");
    s.set_up();
    EXPECT_EQ(s.get_num_field_components(0), 2);
    EXPECT_EQ(s.get_component_name(0, 0), "f0c0");
    EXPECT_EQ(s.get_component_name(0, 1), "f0c1");
    EXPECT_EQ(s.get_num_field_components(1), 4);
    EXPECT_EQ(s.get_component_name(1, 0), "f1c1");
    EXPECT_EQ(s.get_component_name(1, 1), "f1c2");
    EXPECT_EQ(s.get_component_name(1, 2), "f1c3");
    EXPECT_EQ(s.get_component_name(1, 3), "f1c4");
    s.destroy();
}

TEST(SectionTest, get_offset)
{
    Section s;
    s.create(MPI_COMM_WORLD);
    s.set_num_fields(1);
    s.set_chart(0, 2);
    s.set_dof(0, 3);
    s.set_dof(1, 7);
    s.set_up();
    EXPECT_EQ(s.get_offset(1), 3);

    Int offst_start, offst_end;
    s.get_offset_range(offst_start, offst_end);
    EXPECT_EQ(offst_start, 0);
    EXPECT_EQ(offst_end, 10);

    s.destroy();
}

TEST(SectionTest, get_field_offset)
{
    Section s;
    s.create(MPI_COMM_WORLD);
    s.set_num_fields(2);
    s.set_chart(0, 2);

    s.set_dof(0, 3);
    s.set_field_dof(0, 0, 1);
    s.set_field_dof(0, 1, 2);

    s.set_dof(1, 6);
    s.set_field_dof(1, 0, 2);
    s.set_field_dof(1, 1, 4);

    s.set_up();

    EXPECT_EQ(s.get_field_offset(0, 0), 0);
    EXPECT_EQ(s.get_field_offset(0, 1), 1);
    EXPECT_EQ(s.get_field_offset(1, 0), 3);
    EXPECT_EQ(s.get_field_offset(1, 1), 5);

    EXPECT_EQ(s.get_field_point_offset(0, 0), 0);
    EXPECT_EQ(s.get_field_point_offset(0, 1), 1);
    EXPECT_EQ(s.get_field_point_offset(1, 0), 0);
    EXPECT_EQ(s.get_field_point_offset(1, 1), 2);

    s.destroy();
}

TEST(SectionTest, get_storage_size)
{
    Section s;
    s.create(MPI_COMM_WORLD);
    s.set_num_fields(2);
    s.set_chart(0, 2);
    s.set_dof(0, 3);
    s.set_dof(1, 6);
    s.set_up();

    EXPECT_EQ(s.get_storage_size(), 9);

    s.destroy();
}

TEST(SectionTest, ceds)
{
    Section s;
    s.create(MPI_COMM_WORLD);
    s.set_chart(0, 3);
    s.set_dof(0, 1);
    s.set_constraint_dof(0, 1);
    s.set_dof(1, 2);
    s.set_dof(2, 2);
    s.set_constraint_dof(2, 1);
    s.add_constraint_dof(2, 1);
    s.set_up();
    s.set_constraint_indices(0, { 0 });
    s.set_constraint_indices(2, { 0, 1 });

    EXPECT_TRUE(s.has_constraints());
    EXPECT_EQ(s.get_constraint_dof(0), 1);
    EXPECT_EQ(s.get_constraint_dof(1), 0);
    EXPECT_EQ(s.get_constraint_dof(2), 2);

    auto idx0 = s.get_constraint_indices(0);
    EXPECT_EQ(idx0[0], 0);

    auto idx2 = s.get_constraint_indices(2);
    EXPECT_EQ(idx2[0], 0);
    EXPECT_EQ(idx2[1], 1);

    s.destroy();
}

TEST(SectionTest, field_ceds)
{
    Section s;
    s.create(MPI_COMM_WORLD);
    s.set_num_fields(2);
    s.set_chart(0, 2);
    s.set_dof(0, 2);
    s.set_constraint_dof(0, 1);
    s.set_field_dof(0, 0, 1);
    s.set_field_constraint_dof(0, 0, 1);
    s.set_field_dof(0, 1, 1);

    s.set_dof(1, 2);
    s.set_constraint_dof(1, 1);
    s.set_field_dof(1, 0, 1);
    s.set_field_dof(1, 1, 1);
    s.add_field_constraint_dof(1, 1, 1);

    s.set_up();

    s.set_field_constraint_indices(0, 0, { 0 });
    s.set_field_constraint_indices(1, 1, { 0 });

    EXPECT_TRUE(s.has_constraints());
    EXPECT_EQ(s.get_field_constraint_dof(0, 0), 1);
    EXPECT_EQ(s.get_field_dof(0, 1), 1);
    EXPECT_EQ(s.get_field_dof(1, 0), 1);
    EXPECT_EQ(s.get_field_constraint_dof(1, 1), 1);

    auto fidx0 = s.get_field_constraint_indices(0, 1);
    EXPECT_EQ(fidx0[0], 0);

    auto fidx1 = s.get_field_constraint_indices(1, 1);
    EXPECT_EQ(fidx1[0], 0);

    s.destroy();
}

TEST(SectionTest, get_field)
{
    Section s;
    s.create(MPI_COMM_WORLD);
    s.set_num_fields(2);
    s.set_chart(0, 2);
    s.set_dof(0, 3);
    s.set_field_dof(0, 0, 2);
    s.set_field_dof(0, 1, 1);
    s.set_dof(1, 8);
    s.set_field_dof(1, 0, 3);
    s.set_field_dof(1, 1, 5);
    s.set_point_major(true);
    s.set_up();

    Section f1 = s.get_field(0);
    Int f1s, f1e;
    f1.get_offset_range(f1s, f1e);
    EXPECT_EQ(f1s, 0);
    EXPECT_EQ(f1e, 6);

    Section f2 = s.get_field(1);
    Int f2s, f2e;
    f2.get_offset_range(f2s, f2e);
    EXPECT_EQ(f2s, 2);
    EXPECT_EQ(f2e, 11);
}

TEST(SectionTest, view)
{
    testing::internal::CaptureStdout();

    Section s;
    s.create(MPI_COMM_WORLD);
    s.set_chart(0, 2);
    s.set_dof(0, 2);
    s.set_dof(1, 3);
    s.set_up();
    s.view();
    s.destroy();

    auto output = testing::internal::GetCapturedStdout();
    EXPECT_THAT(output, testing::HasSubstr("(   0) dim  2 offset   0"));
    EXPECT_THAT(output, testing::HasSubstr("(   1) dim  3 offset   2"));
}
