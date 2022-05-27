#pragma once

#include "gtest/gtest.h"
#include "TestApp.h"

using namespace godzilla;

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
