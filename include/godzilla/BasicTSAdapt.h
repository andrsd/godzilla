// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/TimeSteppingAdaptor.h"

namespace godzilla {

/// Basic time stepping adaptivity
///
class BasicTSAdapt : public TimeSteppingAdaptor {
public:
    explicit BasicTSAdapt(const Parameters & pars);

    void create() override;

public:
    static Parameters parameters();
};

} // namespace godzilla
