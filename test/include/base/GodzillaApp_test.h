#pragma once

#include "gtest/gtest.h"
#include "base/GodzillaApp.h"
#include "base/AppFactory.h"

class GodzillaAppTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        this->app = AppFactory::createAppShared("GodzillaApp", 0, NULL);
    }

    void TearDown() override
    {
        this->app.reset();
    }

    Factory & factory()
    {
        return this->app->getFactory();
    }

    std::shared_ptr<MooseApp> app;
};
