#pragma once

#include "gtest/gtest.h"
#include "App.h"

using namespace godzilla;

class TestApp : public App {
public:
    TestApp() : App("godzilla", MPI_COMM_WORLD), problem(nullptr) {}

    virtual Problem *
    getProblem() const
    {
        return this->problem;
    }

    virtual void
    checkIntegrity()
    {
        if (this->log.getNumEntries() > 0)
            this->log.print();
    }

    Problem * problem;
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
