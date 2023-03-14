#include "gmock/gmock.h"
#include "mpi/Communicator.h"
#include "mpi/Wait.h"
#include "mpi/Operation.h"

using namespace godzilla;

TEST(MPITest, size_rank)
{
    int sz, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    mpi::Communicator comm;
    EXPECT_EQ(comm.size(), sz);
    EXPECT_EQ(comm.rank(), rank);
}

TEST(MPITest, barrier)
{
    mpi::Communicator comm;
    comm.barrier();
}

TEST(MPITest, send_recv_int)
{
    mpi::Communicator comm;
    int n_mpis = comm.size();
    if (n_mpis == 1)
        return;

    int tag = 0;
    if (comm.rank() == 0) {
        for (int i = 1; i < n_mpis; i++) {
            int val;
            comm.recv(i, tag, val);
            EXPECT_EQ(val, i * 4);
        }
    }
    else {
        int number = 4 * comm.rank();
        comm.send(0, tag, number);
    }
}

TEST(MPITest, send_recv_bool)
{
    mpi::Communicator comm;
    int n_mpis = comm.size();
    if (n_mpis == 1)
        return;

    int tag = 0;
    if (comm.rank() == 0) {
        for (int i = 1; i < n_mpis; i++) {
            bool val;
            comm.recv(i, tag, val);
            EXPECT_EQ(val, i % 2 == 0);
        }
    }
    else {
        bool val = comm.rank() % 2 == 0;
        comm.send(0, tag, val);
    }
}

TEST(MPITest, send_recv)
{
    mpi::Communicator comm;
    int n_mpis = comm.size();
    if (n_mpis == 1)
        return;

    int tag = 0;
    if (comm.rank() == 0) {
        for (int i = 1; i < n_mpis; i++) {
            comm.recv(i, tag);
        }
    }
    else {
        comm.send(0, tag);
    }
}

TEST(MPITest, send_recv_arr_int)
{
    mpi::Communicator comm;
    int n_mpis = comm.size();
    if (n_mpis == 1)
        return;

    int tag = 0;
    if (comm.rank() == 0) {
        for (int i = 1; i < n_mpis; i++) {
            std::vector<int> arr;
            comm.recv(i, tag, arr);
            auto sz = arr.size();
            EXPECT_EQ(sz, i);
            for (int j = 0; j < sz; j++)
                EXPECT_EQ(arr[j], 2 * j);
        }
    }
    else {
        int n = comm.rank();
        std::vector<int> arr;
        arr.resize(n);
        for (int i = 0; i < n; i++)
            arr[i] = 2 * i;
        comm.send(0, tag, arr);
    }
}

TEST(MPITest, broadcast)
{
    mpi::Communicator comm;
    if (comm.size() == 1)
        return;

    int number = 0;
    if (comm.rank() == 0)
        number = 1234;
    comm.broadcast(number, 0);
    if (comm.rank() != 0)
        EXPECT_EQ(number, 1234);
}

TEST(MPITest, broadcast_arr)
{
    mpi::Communicator comm;
    if (comm.size() == 1)
        return;

    std::vector<int> nums;
    nums.resize(10);
    if (comm.rank() == 0) {
        for (std::size_t i = 0; i < 10; i++)
            nums[i] = 3 * i;
    }
    comm.broadcast(nums.data(), 10, 0);
    if (comm.rank() != 0) {
        for (std::size_t i = 0; i < 10; i++)
            EXPECT_EQ(nums[i], 3 * i);
    }
}

TEST(MPITest, gather)
{
    mpi::Communicator comm;
    if (comm.size() == 1)
        return;

    int number = comm.rank() * 5;
    std::vector<int> vals;
    comm.gather(number, vals, 0);
    if (comm.rank() == 0) {
        for (std::size_t i = 0; i < comm.size(); i++)
            EXPECT_EQ(vals[i], i * 5);
    }
}

TEST(MPITest, gather_n)
{
    mpi::Communicator comm;
    if (comm.size() == 1)
        return;

    int number[2] = { comm.rank() * 5, comm.rank() * 7 };
    std::vector<int> vals;
    comm.gather(number, 2, vals, 0);
    if (comm.rank() == 0) {
        for (std::size_t i = 0; i < comm.size(); i++) {
            EXPECT_EQ(vals[2 * i], i * 5);
            EXPECT_EQ(vals[2 * i + 1], i * 7);
        }
    }
}

TEST(MPITest, scatter)
{
    mpi::Communicator comm;
    if (comm.size() == 1)
        return;

    std::vector<int> vals;
    if (comm.rank() == 0) {
        vals.resize(comm.size());
        for (std::size_t i = 0; i < comm.size(); i++)
            vals[i] = (i + 1) * 5;
    }
    int number = -1;
    comm.scatter(vals, number, 0);

    EXPECT_EQ(number, (comm.rank() + 1) * 5);
}

TEST(MPITest, scatter_n)
{
    mpi::Communicator comm;
    if (comm.size() == 1)
        return;

    std::vector<int> vals;
    if (comm.rank() == 0) {
        vals.resize(comm.size() * 2);
        for (std::size_t i = 0; i < comm.size(); i++) {
            vals[2 * i] = (i + 1) * 5;
            vals[2 * i + 1] = (i + 1) * 7;
        }
    }
    int number[2] = { -1, -1 };
    comm.scatter(vals, number, 2, 0);

    EXPECT_EQ(number[0], (comm.rank() + 1) * 5);
    EXPECT_EQ(number[1], (comm.rank() + 1) * 7);
}

TEST(MPITest, reduce_sum)
{
    mpi::Communicator comm;
    if (comm.size() == 1)
        return;

    int n = comm.size();
    double loc_sum = (comm.rank() + 1) * 3;
    double glob_sum;
    comm.reduce(loc_sum, glob_sum, mpi::op::sum<double>(), 0);

    if (comm.rank() == 0) {
        double gold = 3. * (n * (1 + n) / 2.);
        EXPECT_EQ(glob_sum, gold);
    }
}

TEST(MPITest, reduce_all_sum)
{
    mpi::Communicator comm;
    if (comm.size() == 1)
        return;

    int n = comm.size();
    int loc_sum = (comm.rank() + 1) * 3;
    int glob_sum;
    comm.all_reduce(loc_sum, glob_sum, mpi::op::sum<int>());
    comm.all_reduce(loc_sum, mpi::op::sum<int>());

    int gold = 3 * (n * (1 + n) / 2);
    EXPECT_EQ(loc_sum, gold);
    EXPECT_EQ(glob_sum, gold);
}

TEST(MPITest, reduce_all_prod)
{
    mpi::Communicator comm;
    if (comm.size() == 1)
        return;

    int loc_prod = (comm.rank() + 1) * 3;
    int glob_prod;
    comm.all_reduce(loc_prod, glob_prod, mpi::op::prod<int>());
    comm.all_reduce(loc_prod, mpi::op::prod<int>());

    int gold = 1;
    for (int i = 1; i <= comm.size(); i++)
        gold *= 3. * i;

    EXPECT_EQ(loc_prod, gold);
    EXPECT_EQ(glob_prod, gold);
}

TEST(MPITest, reduce_all_min)
{
    mpi::Communicator comm;
    if (comm.size() == 1)
        return;

    int loc = (comm.rank() + 1) * 3;
    int glob;
    comm.all_reduce(loc, glob, mpi::op::min<int>());
    comm.all_reduce(loc, mpi::op::min<int>());

    int gold = 3;
    EXPECT_EQ(loc, gold);
    EXPECT_EQ(glob, gold);
}

TEST(MPITest, reduce_all_max)
{
    mpi::Communicator comm;
    if (comm.size() == 1)
        return;

    int loc = (comm.rank() + 1) * 3;
    int glob;
    comm.all_reduce(loc, glob, mpi::op::max<int>());
    comm.all_reduce(loc, mpi::op::max<int>());

    int gold = comm.size() * 3;
    EXPECT_EQ(loc, gold);
    EXPECT_EQ(glob, gold);
}

TEST(MPITest, isend_irecv_wait)
{
    mpi::Communicator comm;
    if (comm.size() == 1)
        return;

    int tag = 1;
    if (comm.rank() == 0) {
        for (int i = 1; i < comm.size(); i++) {
            int num = i * 5;
            mpi::Request req = comm.isend(i, tag, num);
            mpi::wait(req);
        }
    }
    else {
        int val;
        mpi::Request req = comm.irecv(0, tag, val);
        mpi::wait(req);
        EXPECT_EQ(val, comm.rank() * 5);
    }
}

TEST(MPITest, isend_irecv_waitall)
{
    mpi::Communicator comm;
    if (comm.size() == 1)
        return;

    int tag = 1;
    if (comm.rank() == 0) {
        int n = comm.size() - 1;
        std::vector<int> vals;
        vals.resize(n);
        std::vector<mpi::Request> reqs;
        reqs.resize(n);
        for (int i = 0; i < n; i++)
            reqs[i] = comm.irecv(i + 1, tag, vals[i]);
        mpi::wait_all(reqs);
        for (int i = 0; i < n; i++)
            EXPECT_EQ(vals[i], (i + 1) * 7);
    }
    else {
        int num = comm.rank() * 7;
        mpi::Request req = comm.isend(0, tag, num);
        mpi::wait(req);
    }
}

TEST(MPITest, isend_irecv_waitany)
{
    mpi::Communicator comm;
    if (comm.size() == 1)
        return;

    int tag = 1;
    if (comm.rank() == 0) {
        int n = comm.size() - 1;
        std::vector<int> vals;
        vals.resize(n);
        std::vector<mpi::Request> reqs;
        reqs.resize(n);
        for (int i = 0; i < n; i++)
            reqs[i] = comm.irecv(i + 1, tag, vals[i]);

        auto idx = mpi::wait_any(reqs);
        EXPECT_EQ(vals[idx], (idx + 1) * 7);

        for (int i = 0; i < n; i++)
            if (i != idx)
                mpi::wait(reqs[i]);
    }
    else {
        int num = comm.rank() * 7;
        mpi::Request req = comm.isend(0, tag, num);
        mpi::wait(req);
    }
}
