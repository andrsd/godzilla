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
    String get_file_ext() const override;
    String create_file_name() const override;

    /// Restart interface
    RestartInterface * ri;
    /// The file base of the output file
    String file_base;

public:
    static Parameters parameters();
};

} // namespace godzilla
