// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "petsc.h"

namespace godzilla {

class Init {
public:
    Init(int argc, char * argv[]);
    virtual ~Init();
};

} // namespace godzilla
