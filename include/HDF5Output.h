#pragma once

#include "FileOutput.h"

namespace godzilla {

/// HDF5 output
///
/// This saves the solution into an HDF5 file.
///
/// Input file example:
/// ```
/// output:
///   hdf5:
///     type: HDF5Output
///     file: 'out'
/// ```
///
/// To visualize, you need to create xdmf file first:
/// ```
/// petsc_gen_xdmf.py <output.h5>
/// ```
/// Then, open the produced `.xmf` file in paraview using the `XDMF reader`
///
/// `petsc_gen_xdmf.py` should be included in your PETSc installation under
/// `$PETSC_DIR/lib/petsc/bin`.
///
class HDF5Output : public FileOutput {
public:
    explicit HDF5Output(const Parameters & params);
    virtual ~HDF5Output();

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
