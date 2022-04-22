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
    long unsigned int * idx;
    uint * dof;
    Scalar * coef;

    uint cnt, cap;

    AssemblyList()
    {
        idx = NULL;
        dof = NULL;
        coef = NULL;
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
    add(long unsigned int idx, uint dof, Scalar coef)
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

        idx = (long unsigned int *) realloc(idx, sizeof(long unsigned int) * cap);
        MEM_CHECK(idx);
        dof = (uint *) realloc(dof, sizeof(uint) * cap);
        MEM_CHECK(dof);
        coef = (Scalar *) realloc(coef, sizeof(Scalar) * cap);
        MEM_CHECK(coef);
    }
};

} // namespace godzilla
