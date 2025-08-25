#pragma once

#include "godzilla/ImplicitFENonlinearProblem.h"

using namespace godzilla;

/// Heat equation solver
///
class HeatEquationProblem : public ImplicitFENonlinearProblem {
public:
    explicit HeatEquationProblem(const Parameters & parameters);

protected:
    void set_up_fields() override;
    void set_up_weak_form() override;

    Int temp_id;
    Int q_ppp_id;
    Int htc_aux_id;
    Int T_ambient_aux_id;
    /// Polynomial order of the FE space
    Order p_order;

public:
    static Parameters parameters();
};
