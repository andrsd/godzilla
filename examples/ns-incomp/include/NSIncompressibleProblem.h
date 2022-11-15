#pragma once

#include "ImplicitFENonlinearProblem.h"

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
    void set_up_preconditioning() override;

    /// Reynolds number
    const PetscReal & Re;

public:
    static Parameters parameters();

    static const PetscInt velocity_id = 0;
    static const PetscInt pressure_id = 1;

    static const PetscInt ffn_aid = 0;
};
