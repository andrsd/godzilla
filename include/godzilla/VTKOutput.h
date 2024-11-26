// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/FileOutput.h"

namespace godzilla {

/// VTK ASCII output
///
/// This saves the solution into an ASCII VTK file.
///
/// Input file example:
/// ```
/// output:
///   vtk:
///     type: VTKOutput
///     file: 'out.vtk'
/// ```
class VTKOutput : public FileOutput {
public:
    explicit VTKOutput(const Parameters & params);
    ~VTKOutput() override;

    std::string get_file_ext() const override;
    void create() override;
    void output_step() override;

private:
    /// Viewer for the output
    PetscViewer viewer;

public:
    static Parameters parameters();
};

} // namespace godzilla
