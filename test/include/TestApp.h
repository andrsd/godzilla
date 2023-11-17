#pragma once

#include "godzilla/App.h"

using namespace godzilla;

class TestApp : public App {
public:
    TestApp() : App(mpi::Communicator(MPI_COMM_WORLD), "godzilla"), problem(nullptr) {}

    const std::string &
    get_input_file_name() const
    {
        return this->input_file_name;
    }

    virtual Problem *
    get_problem() const
    {
        return this->problem;
    }

    virtual void
    check_integrity()
    {
        if (get_logger()->get_num_entries() > 0)
            get_logger()->print();
    }

    Problem * problem;
    std::string input_file_name;
};
