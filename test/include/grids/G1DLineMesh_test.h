#include "gmock/gmock.h"
#include "grids/G1DLineMesh.h"

class MockG1DLineMesh : public G1DLineMesh
{
public:
    MockG1DLineMesh(const InputParameters & params) : G1DLineMesh(params) {}
};
