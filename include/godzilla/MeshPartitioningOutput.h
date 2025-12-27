// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/FileOutput.h"
#include "godzilla/String.h"

namespace godzilla {

/// Output mesh partitioning
///
class MeshPartitioningOutput : public FileOutput {
public:
    explicit MeshPartitioningOutput(const Parameters & pars);

    void output_step() override;

private:
    String get_file_ext() const override;

public:
    static Parameters parameters();
};

} // namespace godzilla
