#pragma once

#include "GOutput.h"
#include "petscviewer.h"

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
class VTKOutput : public GOutput {
public:
    VTKOutput(const InputParameters & params);
    virtual ~VTKOutput();

    virtual void output() const override;

protected:
    /// Viewer for VTK output
    PetscViewer viewer;
    ///
    const std::string file_name;

public:
    static InputParameters validParams();
};

} // godzilla
