#include "gmock/gmock.h"
#include "grids/G2DRectangleMesh.h"

class MockG2DRectangleMesh : public G2DRectangleMesh
{
public:
    MockG2DRectangleMesh(const InputParameters & params) : G2DRectangleMesh(params) {}
};
