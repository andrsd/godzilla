// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

namespace godzilla {

class Init {
public:
    /// Initialize runtime environment
    Init();

    /// Initialize runtime environment
    ///
    /// @param argc Number of arguments
    /// @param argv Arguments
    Init(int argc, char * argv[]);

    virtual ~Init();
};

} // namespace godzilla
