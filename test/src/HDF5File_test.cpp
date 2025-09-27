#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/HDF5File.h"
#include "godzilla/DenseVector.h"
#include "godzilla/DenseMatrix.h"
#include <array>

using namespace godzilla;

TEST(HDF5FileTest, write_read)
{
    TestApp app;

    {
        HDF5File f("file.h5", FileAccess::CREATE);

        f.write_attribute<int>("num", 432);
        f.write_attribute<std::string>("creator", "godzilla");

        f.write_dataset<int>("answer", 42);
        f.write_dataset<std::string>("greeting", "hello");

        std::vector<double> darr = { 4., 1., -9., 0. };
        f.write_dataset<std::vector<double>>("darr", darr);

        std::array<int, 6> fib = { 1, 1, 2, 3, 5, 8 };
        f.write_dataset<int>("fibonacci", 6, fib.data());

        DynDenseVector<Real> dvec1(3);
        dvec1.set_col(0, { 2, -1, 4 });
        f.write_dataset<DynDenseVector<Real>>("dyn_dense_vec_3", dvec1);

        DynDenseMatrix<Real> dmat1(3, 2);
        dmat1.set_row(0, { 1, 2 });
        dmat1.set_row(1, { 3, 4 });
        dmat1.set_row(2, { 5, 6 });
        f.write_dataset<DynDenseMatrix<Real>>("dyn_dense_matrix_3x2", dmat1);

        auto gr1 = f.create_group("group1");
        gr1.write_attribute<int>("num2", 234);

        gr1.write_dataset<int>("integer", 1);
        gr1.write_dataset<float>("float", 1876.);

        auto my_data = gr1.create_group("my_data");
        my_data.write_dataset<unsigned int>("uint", 12);
        my_data.write_dataset<double>("double", 987.69);
    }

    {
        HDF5File f("file.h5", FileAccess::READ);

        EXPECT_EQ(f.read_attribute<int>("num"), 432);
        EXPECT_EQ(f.read_attribute<std::string>("creator"), "godzilla");

        EXPECT_EQ(f.read_dataset<int>("answer"), 42);
        EXPECT_EQ(f.read_dataset<std::string>("greeting"), "hello");

        auto darr = f.read_dataset<std::vector<double>>("darr");
        EXPECT_THAT(darr, testing::ElementsAre(4., 1., -9., 0.));

        std::array<int, 6> fib;
        f.read_dataset<int>("fibonacci", 6, fib.data());
        EXPECT_THAT(fib, testing::ElementsAre(1, 1, 2, 3, 5, 8));

        auto dvec1 = f.read_dataset<DynDenseVector<Real>>("dyn_dense_vec_3");
        EXPECT_DOUBLE_EQ(dvec1(0), 2.);
        EXPECT_DOUBLE_EQ(dvec1(1), -1.);
        EXPECT_DOUBLE_EQ(dvec1(2), 4.);

        auto dmat1 = f.read_dataset<DynDenseMatrix<Real>>("dyn_dense_matrix_3x2");
        EXPECT_DOUBLE_EQ(dmat1(0, 0), 1.);
        EXPECT_DOUBLE_EQ(dmat1(0, 1), 2.);
        EXPECT_DOUBLE_EQ(dmat1(1, 0), 3.);
        EXPECT_DOUBLE_EQ(dmat1(1, 1), 4.);
        EXPECT_DOUBLE_EQ(dmat1(2, 0), 5.);
        EXPECT_DOUBLE_EQ(dmat1(2, 1), 6.);

        auto gr1 = f.open_group("group1");
        EXPECT_EQ(gr1.read_attribute<int>("num2"), 234);

        EXPECT_EQ(gr1.read_dataset<int>("integer"), 1);
        EXPECT_EQ(gr1.read_dataset<float>("float"), 1876.);

        auto my_data = gr1.open_group("my_data");
        EXPECT_EQ(my_data.read_dataset<unsigned int>("uint"), 12);
        EXPECT_EQ(my_data.read_dataset<double>("double"), 987.69);
    }
}
