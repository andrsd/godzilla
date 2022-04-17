#pragma once

#include "Factory.h"
#include "gmock/gmock.h"
#include "GYMLFile.h"
#include "GodzillaApp_test.h"
#include "petsc.h"

using namespace godzilla;

class MockGYMLFile : public GYMLFile {
public:
    MockGYMLFile(const App & app) : GYMLFile(app) {}

    MOCK_METHOD(void, buildGrid, (), ());
    MOCK_METHOD(void, buildProblem, (), ());
};

class GYMLFileTest : public GodzillaAppTest {
protected:
    MockGYMLFile *
    gymlFile()
    {
        auto f = new MockGYMLFile(*this->app);
        return f;
    }
};
