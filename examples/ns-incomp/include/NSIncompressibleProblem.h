#pragma once

#include "ImplicitFENonlinearProblem.h"

using namespace godzilla;

/// Heat equation solver
///
class NSIncompressibleProblem : public ImplicitFENonlinearProblem {
public:
    NSIncompressibleProblem(const InputParameters & parameters);
    virtual ~NSIncompressibleProblem();

protected:
    virtual void set_up_fields() override;
    virtual void set_up_weak_form() override;
    virtual void set_up_matrix_properties() override;
    virtual void set_up_field_null_space(DM dm) override;
    virtual void set_up_preconditioning() override;
    void set_up_pc_fieldsplit(PC pc);
    void set_up_pc_lu(PC pc);

    /// Reynolds number
    const PetscReal & Re;

public:
    static InputParameters valid_params();

    static const PetscInt velocity_id = 0;
    static const PetscInt pressure_id = 1;

    static const PetscInt ffn_aid = 0;

    static const PetscInt Re_idx = 0;
};
