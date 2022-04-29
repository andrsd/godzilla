#pragma once

#include "Common.h"
#include "Quadrature1D.h"
#include "RefMap1D.h"
#include "RealVector.h"

namespace godzilla {

class FELinearProblem;
class AssemblyList;
class PetscMatrix;
class PetscVector;

// Template for functions computed on elements at points
template <typename T>
class Fn1DTempl {
public:
    Fn1DTempl() : val(nullptr) {}
    virtual ~Fn1DTempl() { delete this->val; }

    virtual void
    allocate(uint np)
    {
        delete[] this->val;
        this->val = new T[np];
        MEM_CHECK(this->val);
    }

    const T &
    operator[](uint idx) const
    {
        return this->val[idx];
    }

    const T *
    get_values() const
    {
        return this->val;
    }

protected:
    /// Values
    T * val;
};

class Fn1D : public Fn1DTempl<Real> {
public:
    /// For values of a shape function
    ///
    /// @param[in] np Number of quadrature points
    /// @param[in] shfn Shape function
    void
    set(uint np, ShapeFunction1D * shfn)
    {
        allocate(np);
        Real * fn = shfn->get_fn_values();
        for (uint i = 0; i < np; i++) {
            this->val[i] = fn[i];
        }
    }

    void
    set(uint np, Fn1D *fn)
    {
        allocate(np);
        for (uint i = 0; i < np; i++)
            this->val[i] = (*fn)[i];
    }
};

class VecFn1D : public Fn1DTempl<RealVector1D> {
public:
};

class Gradient1D : public VecFn1D {
public:
    /// For a gradient of a shape function
    ///
    /// @param[in] np Number of quadrature points
    /// @param[in] shfn Shape function
    /// @param[in] m Inverse reference map
    void
    set(uint np, ShapeFunction1D * shfn, Real1x1 * m)
    {
        allocate(np);
        Real * dx = shfn->get_dx_values();
        for (uint i = 0; i < np; i++)
            this->val[i][0] = dx[i] * m[i][0][0];
    }

    void
    set(uint np, Gradient1D *grad)
    {
        allocate(np);
        for (uint i = 0; i < np; i++)
            this->val[i] = (*grad)[i];
    }
};

/// Base class for weak form statements
///
class FormBase {
public:
    FormBase(const FELinearProblem * fep);

    /// Return the order of the weak statement
    uint get_order();

    /// Precalculate values needed for evaluation
    virtual void precalculate() = 0;

protected:
    virtual Scalar integrate() = 0;

    /// FELinearProblem that we feed values into
    const FELinearProblem * fep;

    uint np;
};

class BilinearForm : public FormBase {
public:
    BilinearForm(const FELinearProblem * fep, uint m, uint n);

    ///
    virtual void precalculate();

    /// Assemble the bilinear form
    virtual void assemble(PetscMatrix * matrix, PetscVector * rhs);

protected:
    virtual Scalar integrate();

    uint m, n;

    const Real * jxw;
    const RealVector1D * grad_u;
    const RealVector1D * grad_v;
};

class LinearForm : public FormBase {
public:
    LinearForm(const FELinearProblem * fep, uint m);

    virtual void precalculate();

    /// Assemble the bilinear form
    virtual void assemble(PetscVector * rhs);

protected:
    virtual Scalar integrate();

    uint m;
    const Real * jxw;
    const Real * v;
};

} // namespace godzilla
