#pragma once

#include "Common.h"
#include "Error.h"

namespace godzilla {

/// AssemblyList is a simple container for the element assembly arrays idx, dof, coef.
/// These arrays are filled by Space::get_element_assembly_list() and used by the
/// assembling procedure and the Solution class. The arrays are allocated and deallocated
/// automatically by the class.
class AssemblyList {
public:
    /// Shape function indices
    uint * idx;
    /// DoF indices
    PetscInt * dof;
    /// Coefficient associated with the shape function
    PetscScalar * coef;

    uint cnt, cap;

    AssemblyList()
    {
        idx = nullptr;
        dof = nullptr;
        coef = nullptr;
        cnt = cap = 0;
    }

    ~AssemblyList()
    {
        free(idx);
        free(dof);
        free(coef);
    }

    void
    clear()
    {
        cnt = 0;
    }

    inline void
    add(uint idx, PetscInt dof, PetscScalar coef)
    {
        if (coef == 0.0)
            return;
        if (cnt >= cap)
            enlarge();

        this->idx[cnt] = idx;
        this->dof[cnt] = dof;
        this->coef[cnt] = coef;
        cnt++;
    }

protected:
    void
    enlarge()
    {
        cap = !cap ? 256 : cap * 2;

        idx = (uint *) realloc(idx, sizeof(uint) * cap);
        MEM_CHECK(idx);
        dof = (PetscInt *) realloc(dof, sizeof(PetscInt) * cap);
        MEM_CHECK(dof);
        coef = (PetscScalar *) realloc(coef, sizeof(PetscScalar) * cap);
        MEM_CHECK(coef);
    }
};

} // namespace godzilla
