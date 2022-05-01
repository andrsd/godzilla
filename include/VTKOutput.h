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
    VTKOutput(const InputParameters & params);

    virtual std::string get_file_ext() const override;
    virtual void create() override;
    virtual void check() override;
    virtual void output() override;

protected:
    void output_vtk_file(FILE * file);
    void output_header(FILE * file);
    void output_geometry(FILE * file);
    void output_variables(FILE * file);

public:
    static InputParameters valid_params();
};

} // namespace godzilla
