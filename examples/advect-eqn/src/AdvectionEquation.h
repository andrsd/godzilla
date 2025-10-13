#pragma once

#include "godzilla/ExplicitFVLinearProblem.h"

using namespace godzilla;

/// Solves advection equation using finite volume method
///
class AdvectionEquation : public ExplicitFVLinearProblem {
public:
    AdvectionEquation(const Parameters & parameters);
    void create() override;

protected:
    void set_up_fields() override;
    void set_up_weak_form() override;

    /// Method to compute flux across an edge
    ///
    /// @param x[in] Edge centroid
    /// @param n[in] Normal
    /// @param u_l[in] Solution on the "left" side
    /// @param u_r[in] Solution on the "right" side
    /// @param flux[out] Computed flux
    /// @return PETSc error code, zero means success
    void compute_flux(const Real x[],
                      const Real n[],
                      const Scalar u_l[],
                      const Scalar u_r[],
                      Scalar flux[]);

public:
    static Parameters parameters();
};
