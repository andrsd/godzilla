#pragma once

#include "FileOutput.h"

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
    virtual ~VTKOutput();

    std::string get_file_ext() const override;
    void create() override;
    void check() override;
    void output_step() override;

protected:
    /// Viewer for the output
    PetscViewer viewer;

public:
    static Parameters parameters();
};

} // namespace godzilla
