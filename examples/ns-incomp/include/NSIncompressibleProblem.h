#pragma once

#include "godzilla/ImplicitFENonlinearProblem.h"

using namespace godzilla;

/// Heat equation solver
///
class NSIncompressibleProblem : public ImplicitFENonlinearProblem {
public:
    explicit NSIncompressibleProblem(const Parameters & parameters);

    const PetscReal & get_reynolds_number() const;

protected:
    void set_up_fields() override;
    void set_up_weak_form() override;
    void set_up_matrix_properties() override;
    void set_up_field_null_space(DM dm) override;
    Preconditioner create_preconditioner(PC pc) override;

    PetscInt velocity_id;
    PetscInt pressure_id;
    PetscInt ffn_aid;
    /// Reynolds number
    const PetscReal & Re;

public:
    static Parameters parameters();
};
