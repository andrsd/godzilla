// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Validation.h"
#include "godzilla/UnstructuredMesh.h"

namespace godzilla {

std::tuple<bool, Int>
check_element_type(Ref<const UnstructuredMesh> mesh, PolytopeType expected_type)
{
    CALL_STACK_MSG();
    for (const auto & cell : mesh->get_cell_range()) {
        const auto ct = mesh->get_cell_type(cell);
        if (ct != expected_type)
            return { false, cell };
    }
    return { true, -1 };
}

} // namespace godzilla
