#pragma once

#include "gmock/gmock.h"
#include "Factory.h"
#include "ExodusIIMesh.h"
#include "GodzillaApp_test.h"

using namespace godzilla;

class MockExodusIIMesh : public ExodusIIMesh {
public:
    MockExodusIIMesh(const InputParameters & params) : ExodusIIMesh(params) {}
};

class ExodusIIMeshTest : public GodzillaAppTest {
protected:
    MockExodusIIMesh *
    gExodusMesh(const std::string & file_name)
    {
        const std::string class_name = "MockExodusIIMesh";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<std::string>("file") = file_name;
        return this->app->buildObject<MockExodusIIMesh>(class_name, "obj", params);
    }
};
