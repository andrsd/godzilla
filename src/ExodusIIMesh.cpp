// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/ExodusIIMesh.h"
#include "godzilla/App.h"
#include "godzilla/CallStack.h"

namespace godzilla {

REGISTER_OBJECT(ExodusIIMesh);

Parameters
ExodusIIMesh::parameters()
{
    Parameters params = FileMesh::parameters();
    return params;
}

ExodusIIMesh::ExodusIIMesh(const Parameters & parameters) : FileMesh(parameters)
{
    _F_;
    set_file_format(EXODUSII);
}

} // namespace godzilla
