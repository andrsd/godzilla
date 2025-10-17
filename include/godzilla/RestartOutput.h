// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "RestartInterface.h"
#include "godzilla/FileOutput.h"

namespace godzilla {

class RestartOutput : public FileOutput {
public:
    explicit RestartOutput(const Parameters & pars);

    void create() override;
    void output_step() override;

private:
    std::string get_file_ext() const override;

    /// Restart interface
    RestartInterface * ri;

public:
    static Parameters parameters();
};

} // namespace godzilla
