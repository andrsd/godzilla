#pragma once

#include "MeshFunction1D.h"
#include "AssemblyList.h"
#include "petsc.h"

namespace godzilla {

class Space;
class ShapeFunction1D;

/// Represents the solution of a PDE.
///
/// Solution1D represents the solution of a PDE. Given a space and a solution vector,
class Solution1D : public MeshFunction1D {
public:
    Solution1D(Mesh * mesh, uint num_components = 1);
    virtual ~Solution1D();

    virtual void set_fe_solution(Space * space, Vec vec);
    virtual void set_active_element(const Element1D * e) override;
    virtual void free() override;
    virtual void precalculate(const uint np, const QPoint1D * pt, uint mask) override;
    virtual uint get_order() override;

protected:
    Space * space;
    Vec vec;
    ShapeFunction1D * shfn;
    AssemblyList al;
};

} // namespace godzilla
