// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/FileMesh.h"

namespace godzilla {

/// Mesh loaded from a ExodusII file
///
class ExodusIIMesh : public FileMesh {
public:
    explicit ExodusIIMesh(const Parameters & pars);

public:
    static Parameters parameters();
};

} // namespace godzilla
