#pragma once

#include "App.h"

using namespace godzilla;

class TestApp : public App {
public:
    TestApp() : App("godzilla", MPI_COMM_WORLD), problem(nullptr) {}

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
};