#pragma once

#include "gmock/gmock.h"
#include "grids/GExodusIIMesh.h"
#include "base/GodzillaApp_test.h"


class MockGExodusIIMesh : public GExodusIIMesh
{
public:
    MockGExodusIIMesh(const InputParameters & params) : GExodusIIMesh(params) {}
};


class GExodusIIMeshTest : public GodzillaAppTest {
protected:
    std::shared_ptr<MockGExodusIIMesh>
    gExodusMesh(const std::string & file_name)
    {
        const std::string class_name = "MockGExodusIIMesh";
        InputParameters params = factory().getValidParams(class_name);
        params.set<std::string>("file") = file_name;
        return factory().create<MockGExodusIIMesh>(class_name, "obj", params);
    }
};
