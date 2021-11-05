#pragma once

#include "base/Factory.h"
#include "gmock/gmock.h"
#include "grids/GExodusIIMesh.h"
#include "base/GodzillaApp_test.h"


using namespace godzilla;

class MockGExodusIIMesh : public GExodusIIMesh
{
public:
    MockGExodusIIMesh(const InputParameters & params) : GExodusIIMesh(params) {}
};


class GExodusIIMeshTest : public GodzillaAppTest {
protected:
    MockGExodusIIMesh *
    gExodusMesh(const std::string & file_name)
    {
        const std::string class_name = "MockGExodusIIMesh";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<std::string>("file") = file_name;
        return Factory::create<MockGExodusIIMesh>(class_name, "obj", params);
    }
};
