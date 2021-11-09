#pragma once

#include "gtest/gtest.h"
#include "App.h"

using namespace godzilla;

class PrintInterfaceTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        this->app = new App("godzilla", MPI_COMM_WORLD);
    }

    void TearDown() override
    {
        delete this->app;
        this->app = nullptr;
    }

    App * app;
};
