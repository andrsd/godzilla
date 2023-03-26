#pragma once

#include "App.h"

using namespace godzilla;

class TestApp : public App {
public:
    TestApp() : App("godzilla", mpi::Communicator(MPI_COMM_WORLD)), problem(nullptr) {}

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
        if (this->log->get_num_entries() > 0)
            this->log->print();
    }

    Problem * problem;
    std::string input_file_name;
};
