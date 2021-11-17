#pragma once

#include "Output.h"
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
class VTKOutput : public Output {
public:
    VTKOutput(const InputParameters & params);
    virtual ~VTKOutput();

    virtual const std::string & getFileName() const override;
    virtual void setFileName() override;
    virtual void setSequenceFileName(unsigned int step) override;
    virtual void output(DM dm, Vec vec) const override;

protected:
    /// Viewer for VTK output
    PetscViewer viewer;
    /// The file base of the output file
    const std::string file_base;
    /// The file name of the output file
    std::string file_name;

public:
    static InputParameters validParams();
};

} // namespace godzilla
