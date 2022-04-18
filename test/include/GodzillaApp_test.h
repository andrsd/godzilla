#pragma once

#include "gtest/gtest.h"
#include "App.h"

using namespace godzilla;

class TestApp : public App {
public:
    TestApp() : App("godzilla", MPI_COMM_WORLD) {}

    virtual void
    check_integrity()
    {
        if (this->log.get_num_entries() > 0)
            this->log.print();
    }
};

class GodzillaAppTest : public ::testing::Test {
protected:
    void
    SetUp() override
    {
        this->app = new TestApp();
    }

    void
    TearDown() override
    {
        delete this->app;
        this->app = nullptr;
    }

    TestApp * app;
};
