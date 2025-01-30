#pragma once

#include "godzilla/ImplicitFENonlinearProblem.h"
#include "godzilla/PCFieldSplit.h"

using namespace godzilla;

/// Heat equation solver
///
class NSIncompressibleProblem : public ImplicitFENonlinearProblem {
public:
    explicit NSIncompressibleProblem(const Parameters & parameters);

    const Real & get_reynolds_number() const;

protected:
    void set_up_fields() override;
    void set_up_weak_form() override;
    void set_up_matrix_properties() override;
    void set_up_field_null_space(DM dm) override;
    void create_preconditioner(PC pc) override;

    Int velocity_id;
    Int pressure_id;
    Int ffn_aid;
    /// Reynolds number
    const Real & Re;
    PCFieldSplit fsplit;

public:
    static Parameters parameters();
};
