// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/ExodusIIMesh.h"
#include "godzilla/App.h"
#include "godzilla/CallStack.h"

namespace godzilla {

Parameters
ExodusIIMesh::parameters()
{
    Parameters params = FileMesh::parameters();
    return params;
}

ExodusIIMesh::ExodusIIMesh(const Parameters & parameters) : FileMesh(parameters)
{
    CALL_STACK_MSG();
    set_file_format(EXODUSII);
    deprecated("Use `FileMesh` instead.");
}

} // namespace godzilla
